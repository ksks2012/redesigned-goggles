#ifndef EDITOR_COMMANDS_H
#define EDITOR_COMMANDS_H

#include "EditorInterfaces.h"
#include "EditorServices.h"
#include <memory>
#include <vector>
#include <algorithm>

namespace Editor {

/**
 * Base command handler implementation
 * Follows Template Method Pattern and Single Responsibility Principle (SRP)
 */
class BaseCommandHandler : public ICommandHandler {
protected:
    std::shared_ptr<IUserInterface> ui_;
    std::shared_ptr<IDataService> dataService_;
    std::shared_ptr<IGameStateService> gameStateService_;
    
public:
    BaseCommandHandler(std::shared_ptr<IUserInterface> ui,
                      std::shared_ptr<IDataService> dataService,
                      std::shared_ptr<IGameStateService> gameStateService)
        : ui_(ui), dataService_(dataService), gameStateService_(gameStateService) {}
    
    virtual ~BaseCommandHandler() = default;
};

/**
 * Material operations command handler
 * Follows Single Responsibility Principle (SRP)
 */
class MaterialCommandHandler : public BaseCommandHandler, public IMenuHandler {
public:
    MaterialCommandHandler(std::shared_ptr<IUserInterface> ui,
                          std::shared_ptr<IDataService> dataService,
                          std::shared_ptr<IGameStateService> gameStateService)
        : BaseCommandHandler(ui, dataService, gameStateService) {}
    
    std::string getCommandName() const override { return "material"; }
    std::vector<std::string> getAliases() const override { return {"mat", "m"}; }
    std::string getDescription() const override { return "Material operations"; }
    
    void execute() override {
        ui_->displayMessage("\n=== MATERIAL OPERATIONS ===");
        auto options = getMenuOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ui_->displayMessage(std::to_string(i + 1) + ". " + options[i]);
        }
        ui_->displayMessage("0. Back to main menu");
        
        int choice = ui_->getUserChoice("Choose option", 0, static_cast<int>(options.size()));
        if (choice > 0) {
            handleMenuChoice(choice - 1);
        }
    }
    
    std::string getMenuTitle() const override { return "Material Operations"; }
    
    std::vector<std::string> getMenuOptions() const override {
        return {
            "List materials",
            "Show material details",
            "Create material",
            "Edit material",
            "Delete material"
        };
    }
    
    void handleMenuChoice(int choice) override {
        switch (choice) {
            case 0: listMaterials(); break;
            case 1: showMaterialDetails(); break;
            case 2: createMaterial(); break;
            case 3: editMaterial(); break;
            case 4: deleteMaterial(); break;
            default: ui_->displayError("Invalid choice"); break;
        }
    }
    
private:
    void listMaterials() {
        const auto& materials = dataService_->getMaterials();
        
        if (materials.empty()) {
            ui_->displayMessage("No materials found.");
            return;
        }
        
        std::vector<std::vector<std::string>> data;
        std::vector<std::string> headers = {"Name", "Type", "Rarity", "Quantity"};
        
        for (const auto& material : materials) {
            data.push_back({
                material.name,
                std::to_string(static_cast<int>(material.type)),
                std::to_string(material.rarity),
                std::to_string(material.baseQuantity)
            });
        }
        
        ui_->displayMessage("\n=== MATERIALS (" + std::to_string(materials.size()) + ") ===");
        ui_->displayTable(data, headers);
    }
    
    void showMaterialDetails() {
        std::string name = ui_->getUserInput("Material name");
        int rarity = ui_->getUserChoice("Material rarity", 1, 5);
        
        const auto* material = dataService_->findMaterial(name, rarity);
        if (!material) {
            ui_->displayError("Material not found: " + name + " (rarity " + std::to_string(rarity) + ")");
            return;
        }
        
        ui_->displayMessage("\n=== MATERIAL DETAILS ===");
        ui_->displayMessage("Name: " + material->name);
        ui_->displayMessage("Type: " + std::to_string(static_cast<int>(material->type)));
        ui_->displayMessage("Rarity: " + std::to_string(material->rarity));
        ui_->displayMessage("Base Quantity: " + std::to_string(material->baseQuantity));
        
        if (!material->attributes.empty()) {
            ui_->displayMessage("Attributes:");
            for (const auto& attr : material->attributes) {
                ui_->displayMessage("  " + std::to_string(static_cast<int>(attr.first)) + ": " + 
                                   std::to_string(attr.second));
            }
        }
    }
    
    void createMaterial() {
        DataManagement::MaterialData material;
        
        ui_->displayMessage("\n=== CREATE MATERIAL ===");
        material.name = ui_->getUserInput("Material Name");
        material.rarity = ui_->getUserChoice("Rarity", 1, 5);
        
        // Check if material already exists
        if (dataService_->findMaterial(material.name, material.rarity)) {
            ui_->displayError("Material '" + material.name + "' with rarity " + 
                             std::to_string(material.rarity) + " already exists!");
            return;
        }
        
        // Choose type
        ui_->displayMessage("\nCard Types:");
        ui_->displayMessage("0. Metal    1. Weapon   2. Armor    3. Herb");
        ui_->displayMessage("4. Food     5. Fuel     6. Building 7. Misc");
        int typeChoice = ui_->getUserChoice("Choose type", 0, 7);
        material.type = static_cast<CardType>(typeChoice);
        
        material.baseQuantity = ui_->getUserChoice("Base quantity", 1, 100);
        
        if (dataService_->addMaterial(material)) {
            ui_->displaySuccess("Material '" + material.name + "' created successfully!");
        } else {
            ui_->displayError("Failed to create material");
        }
    }
    
    void editMaterial() {
        std::string name = ui_->getUserInput("Material name to edit");
        int rarity = ui_->getUserChoice("Material rarity", 1, 5);
        
        auto* material = dataService_->findMaterial(name, rarity);
        if (!material) {
            ui_->displayError("Material not found: " + name + " (rarity " + std::to_string(rarity) + ")");
            return;
        }
        
        ui_->displayMessage("\nEditing material: " + material->name);
        ui_->displayMessage("1. Change name");
        ui_->displayMessage("2. Change type");
        ui_->displayMessage("3. Change rarity");
        ui_->displayMessage("4. Change base quantity");
        ui_->displayMessage("0. Cancel");
        
        int choice = ui_->getUserChoice("Choose option", 0, 4);
        switch (choice) {
            case 1: material->name = ui_->getUserInput("New name"); break;
            case 2: {
                ui_->displayMessage("0. Metal  1. Weapon  2. Armor  3. Herb");
                ui_->displayMessage("4. Food   5. Fuel    6. Building  7. Misc");
                int typeChoice = ui_->getUserChoice("Choose type", 0, 7);
                material->type = static_cast<CardType>(typeChoice);
                break;
            }
            case 3: material->rarity = ui_->getUserChoice("New rarity", 1, 5); break;
            case 4: material->baseQuantity = ui_->getUserChoice("New base quantity", 1, 100); break;
            case 0: return;
        }
        
        ui_->displaySuccess("Material updated successfully!");
    }
    
    void deleteMaterial() {
        std::string name = ui_->getUserInput("Material name to delete");
        int rarity = ui_->getUserChoice("Material rarity", 1, 5);
        
        if (!dataService_->findMaterial(name, rarity)) {
            ui_->displayError("Material not found: " + name + " (rarity " + std::to_string(rarity) + ")");
            return;
        }
        
        if (ui_->confirmAction("delete material '" + name + "'")) {
            if (dataService_->removeMaterial(name, rarity)) {
                ui_->displaySuccess("Material '" + name + "' deleted successfully!");
            } else {
                ui_->displayError("Failed to delete material");
            }
        }
    }
};

/**
 * Recipe operations command handler
 * Follows Single Responsibility Principle (SRP)
 */
class RecipeCommandHandler : public BaseCommandHandler, public IMenuHandler {
public:
    RecipeCommandHandler(std::shared_ptr<IUserInterface> ui,
                        std::shared_ptr<IDataService> dataService,
                        std::shared_ptr<IGameStateService> gameStateService)
        : BaseCommandHandler(ui, dataService, gameStateService) {}
    
    std::string getCommandName() const override { return "recipe"; }
    std::vector<std::string> getAliases() const override { return {"rec", "r"}; }
    std::string getDescription() const override { return "Recipe operations"; }
    
    void execute() override {
        ui_->displayMessage("\n=== RECIPE OPERATIONS ===");
        auto options = getMenuOptions();
        for (size_t i = 0; i < options.size(); ++i) {
            ui_->displayMessage(std::to_string(i + 1) + ". " + options[i]);
        }
        ui_->displayMessage("0. Back to main menu");
        
        int choice = ui_->getUserChoice("Choose option", 0, static_cast<int>(options.size()));
        if (choice > 0) {
            handleMenuChoice(choice - 1);
        }
    }
    
    std::string getMenuTitle() const override { return "Recipe Operations"; }
    
    std::vector<std::string> getMenuOptions() const override {
        return {
            "List recipes",
            "Show recipe details"
        };
    }
    
    void handleMenuChoice(int choice) override {
        switch (choice) {
            case 0: listRecipes(); break;
            case 1: showRecipeDetails(); break;
            default: ui_->displayError("Invalid choice"); break;
        }
    }
    
private:
    void listRecipes() {
        const auto& recipes = dataService_->getRecipes();
        
        if (recipes.empty()) {
            ui_->displayMessage("No recipes found.");
            return;
        }
        
        std::vector<std::vector<std::string>> data;
        std::vector<std::string> headers = {"ID", "Name", "Result", "Success %"};
        
        for (const auto& recipe : recipes) {
            data.push_back({
                recipe.id,
                recipe.name,
                recipe.resultMaterial,
                std::to_string(static_cast<int>(recipe.successRate * 100)) + "%"
            });
        }
        
        ui_->displayMessage("\n=== RECIPES (" + std::to_string(recipes.size()) + ") ===");
        ui_->displayTable(data, headers);
    }
    
    void showRecipeDetails() {
        std::string id = ui_->getUserInput("Recipe ID");
        
        const auto* recipe = dataService_->findRecipe(id);
        if (!recipe) {
            ui_->displayError("Recipe not found: " + id);
            return;
        }
        
        ui_->displayMessage("\n=== RECIPE DETAILS ===");
        ui_->displayMessage("ID: " + recipe->id);
        ui_->displayMessage("Name: " + recipe->name);
        ui_->displayMessage("Description: " + recipe->description);
        ui_->displayMessage("Result: " + recipe->resultMaterial);
        ui_->displayMessage("Success Rate: " + std::to_string(static_cast<int>(recipe->successRate * 100)) + "%");
        ui_->displayMessage("Unlock Level: " + std::to_string(recipe->unlockLevel));
        ui_->displayMessage("Unlocked: " + std::string(recipe->isUnlocked ? "Yes" : "No"));
        
        ui_->displayMessage("Ingredients:");
        for (const auto& ingredient : recipe->ingredients) {
            ui_->displayMessage("  " + ingredient.first + " x" + std::to_string(ingredient.second));
        }
    }
};

} // namespace Editor

#endif // EDITOR_COMMANDS_H
