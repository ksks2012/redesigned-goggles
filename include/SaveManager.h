#pragma once
#include <string>
#include <memory>
#include <fstream>
#include "Inventory.h"
#include "../lib/nlohmann/json.hpp"

/**
 * The SaveManager class handles game saving and loading functionality.
 * Uses RAII principles to manage file resources.
 * Stores game data in JSON format.
 */
class SaveManager {
public:
    SaveManager(const std::string& saveFilePath = "save.json");
    ~SaveManager() = default;

    // Save functionality
    bool saveGame(const Inventory& inventory);
    
    // Load functionality  
    bool loadGame(Inventory& inventory);
    
    // Check if the save file exists
    bool saveFileExists() const;
    
    // Get the save file path
    const std::string& getSaveFilePath() const;

private:
    std::string saveFilePath;
    
    // RAII file handler class
    class FileHandler {
    public:
        FileHandler(const std::string& filePath, std::ios::openmode mode);
        ~FileHandler();
        
        bool isOpen() const;
        std::fstream& getStream();
        
    private:
        std::unique_ptr<std::fstream> fileStream;
    };
    
    // JSON conversion functions
    nlohmann::json cardToJson(const Card& card) const;
    nlohmann::json inventoryToJson(const Inventory& inventory) const;
    
    Card jsonToCard(const nlohmann::json& cardJson) const;
    void jsonToInventory(const nlohmann::json& json, Inventory& inventory) const;
    
    // Error handling
    void logError(const std::string& message) const;
};
