#include "Systems/SaveManager.h"
#include <iostream>
#include <filesystem>

SaveManager::SaveManager(const std::string& saveFilePath) 
    : saveFilePath(saveFilePath) {
}

bool SaveManager::saveGame(const Inventory& inventory) {
    try {
        // Use RAII to manage file
        FileHandler fileHandler(saveFilePath, std::ios::out | std::ios::trunc);
        
        if (!fileHandler.isOpen()) {
            logError("Unable to open save file for writing: " + saveFilePath);
            return false;
        }
        
        // Convert to JSON
        nlohmann::json gameData;
        gameData["version"] = "1.0";
        gameData["timestamp"] = std::time(nullptr);
        gameData["inventory"] = inventoryToJson(inventory);
        
        // Write to file
        fileHandler.getStream() << gameData.dump(4); // Pretty print with 4 spaces
        
        std::cout << "Game successfully saved to: " << saveFilePath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        logError("Error occurred while saving: " + std::string(e.what()));
        return false;
    }
}

bool SaveManager::loadGame(Inventory& inventory) {
    try {
        if (!saveFileExists()) {
            logError("Save file does not exist: " + saveFilePath);
            return false;
        }
        
        // Use RAII to manage file
        FileHandler fileHandler(saveFilePath, std::ios::in);
        
        if (!fileHandler.isOpen()) {
            logError("Unable to open save file for reading: " + saveFilePath);
            return false;
        }
        
        // Read file content
        std::string fileContent;
        std::string line;
        while (std::getline(fileHandler.getStream(), line)) {
            fileContent += line + "\n";
        }
        
        if (fileContent.empty()) {
            logError("Save file is empty");
            return false;
        }
        
        // Parse JSON
        nlohmann::json gameData = nlohmann::json::parse(fileContent);
        
        // Check version
        if (gameData.contains("version")) {
            std::string version = gameData["version"];
            std::cout << "Loaded save version: " << version << std::endl;
        }
        
        // Load inventory
        if (gameData.contains("inventory")) {
            jsonToInventory(gameData["inventory"], inventory);
            std::cout << "Game successfully loaded from: " << saveFilePath << std::endl;
            return true;
        } else {
            logError("Save file format error: missing inventory data");
            return false;
        }
        
    } catch (const nlohmann::json::parse_error& e) {
        logError("JSON parse error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        logError("Error occurred while loading: " + std::string(e.what()));
        return false;
    }
}

bool SaveManager::saveFileExists() const {
    return std::filesystem::exists(saveFilePath);
}

const std::string& SaveManager::getSaveFilePath() const {
    return saveFilePath;
}

// RAII file handler implementation
SaveManager::FileHandler::FileHandler(const std::string& filePath, std::ios::openmode mode)
    : fileStream(std::make_unique<std::fstream>(filePath, mode)) {
}

SaveManager::FileHandler::~FileHandler() {
    if (fileStream && fileStream->is_open()) {
        fileStream->close();
    }
}

bool SaveManager::FileHandler::isOpen() const {
    return fileStream && fileStream->is_open();
}

std::fstream& SaveManager::FileHandler::getStream() {
    return *fileStream;
}

// JSON conversion functions
nlohmann::json SaveManager::cardToJson(const Card& card) const {
    nlohmann::json cardJson;
    cardJson["name"] = card.name;
    cardJson["rarity"] = card.rarity;
    cardJson["quantity"] = card.quantity;
    cardJson["type"] = static_cast<int>(card.type);
    
    // Serialize attributes
    cardJson["attributes"] = nlohmann::json::object();
    for (const auto& attr : card.attributes) {
        cardJson["attributes"][std::to_string(static_cast<int>(attr.first))] = attr.second;
    }
    
    return cardJson;
}

nlohmann::json SaveManager::inventoryToJson(const Inventory& inventory) const {
    nlohmann::json inventoryJson;
    inventoryJson["cards"] = nlohmann::json::array();
    
    const auto& cards = inventory.getCards();
    for (const auto& card : cards) {
        inventoryJson["cards"].push_back(cardToJson(card));
    }
    
    return inventoryJson;
}

Card SaveManager::jsonToCard(const nlohmann::json& cardJson) const {
    std::string name = cardJson["name"];
    int rarity = cardJson["rarity"];
    int quantity = cardJson["quantity"];
    
    // Handle type (backward compatibility)
    CardType type = CardType::MISC;
    if (cardJson.contains("type")) {
        type = static_cast<CardType>(cardJson["type"]);
    }
    
    Card card(name, rarity, type, quantity);

    // Load attributes (backward compatibility)
    if (cardJson.contains("attributes") && cardJson["attributes"].is_object()) {
        for (const auto& attr : cardJson["attributes"].items()) {
            AttributeType attrType = static_cast<AttributeType>(std::stoi(attr.key()));
            float value = attr.value();
            card.setAttribute(attrType, value);
        }
    }
    
    return card;
}

void SaveManager::jsonToInventory(const nlohmann::json& json, Inventory& inventory) const {
    // Clear existing inventory
    std::vector<Card> emptyCards;
    inventory.updateCards(emptyCards);
    
    // Load cards
    if (json.contains("cards") && json["cards"].is_array()) {
        for (const auto& cardJson : json["cards"]) {
            Card card = jsonToCard(cardJson);
            inventory.addCard(card);
        }
    }
}

void SaveManager::logError(const std::string& message) const {
    std::cerr << "[SaveManager Error] " << message << std::endl;
}
