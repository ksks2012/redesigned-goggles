#ifndef EDITOR_INTERFACES_H
#define EDITOR_INTERFACES_H

#include "DataManager.h"
#include <string>
#include <vector>
#include <memory>

// Forward declarations
class Game;
class Inventory;
class CraftingSystem;
class Controller;

namespace Editor {

/**
 * Interface for user input/output operations
 * Follows Interface Segregation Principle (ISP)
 */
class IUserInterface {
public:
    virtual ~IUserInterface() = default;
    
    // Output operations
    virtual void displayMessage(const std::string& message) = 0;
    virtual void displayError(const std::string& error) = 0;
    virtual void displayWarning(const std::string& warning) = 0;
    virtual void displaySuccess(const std::string& success) = 0;
    virtual void displayTable(const std::vector<std::vector<std::string>>& data,
                             const std::vector<std::string>& headers) = 0;
    
    // Input operations
    virtual std::string getUserInput(const std::string& prompt) = 0;
    virtual int getUserChoice(const std::string& prompt, int min, int max) = 0;
    virtual float getUserFloat(const std::string& prompt) = 0;
    virtual bool confirmAction(const std::string& action) = 0;
};

/**
 * Interface for data operations
 * Follows Dependency Inversion Principle (DIP)
 */
class IDataService {
public:
    virtual ~IDataService() = default;
    
    // Material operations
    virtual const std::vector<DataManagement::MaterialData>& getMaterials() const = 0;
    virtual DataManagement::MaterialData* findMaterial(const std::string& name, int rarity) = 0;
    virtual bool addMaterial(const DataManagement::MaterialData& material) = 0;
    virtual bool updateMaterial(const std::string& name, int rarity, const DataManagement::MaterialData& material) = 0;
    virtual bool removeMaterial(const std::string& name, int rarity) = 0;
    
    // Recipe operations
    virtual const std::vector<DataManagement::RecipeData>& getRecipes() const = 0;
    virtual DataManagement::RecipeData* findRecipe(const std::string& id) = 0;
    virtual bool addRecipe(const DataManagement::RecipeData& recipe) = 0;
    virtual bool updateRecipe(const std::string& id, const DataManagement::RecipeData& recipe) = 0;
    virtual bool removeRecipe(const std::string& id) = 0;
    
    // Event operations
    virtual const std::vector<DataManagement::EventData>& getEvents() const = 0;
    virtual bool addEvent(const DataManagement::EventData& event) = 0;
    virtual bool removeEvent(const std::string& name) = 0;
    
    // Data validation and persistence
    virtual DataManagement::ValidationResult validateAll() const = 0;
    virtual bool saveAll() = 0;
    virtual bool loadAll() = 0;
    virtual bool exportData(const std::string& directory) = 0;
};

/**
 * Interface for game state operations
 * Follows Interface Segregation Principle (ISP)
 */
class IGameStateService {
public:
    virtual ~IGameStateService() = default;
    
    virtual bool isGameConnected() const = 0;
    virtual std::vector<std::string> getInventoryItems() const = 0;
    virtual std::vector<std::string> getUnlockedRecipes() const = 0;
    virtual bool syncFromGame() = 0;
    virtual bool syncToGame() = 0;
    virtual std::string getGameStatusSummary() const = 0;
};

/**
 * Interface for command handling
 * Follows Command Pattern and Single Responsibility Principle (SRP)
 */
class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;
    
    virtual std::string getCommandName() const = 0;
    virtual std::vector<std::string> getAliases() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void execute() = 0;
};

/**
 * Interface for menu operations
 * Follows Single Responsibility Principle (SRP)
 */
class IMenuHandler {
public:
    virtual ~IMenuHandler() = default;
    
    virtual std::string getMenuTitle() const = 0;
    virtual std::vector<std::string> getMenuOptions() const = 0;
    virtual void handleMenuChoice(int choice) = 0;
};

} // namespace Editor

#endif // EDITOR_INTERFACES_H
