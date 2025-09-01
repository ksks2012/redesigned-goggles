# Game Editor System Documentation

## Overview

The Game Editor System provides a comprehensive in-game editing interface for modifying game content including materials, crafting recipes, and events. The system is designed with a modular architecture that separates data management from UI presentation.

## Architecture

### Core Components

1. **GameDataManager** - Central data management and validation
2. **GameEditor** - Main editor interface coordinator  
3. **Editor Panels** - Specialized editing interfaces for different content types
4. **ImGuiManager** - Integration layer for ImGui UI library

### Data Structures

#### MaterialTemplate
Represents editable card/material templates:
```cpp
struct MaterialTemplate {
    std::string id;           // Unique identifier
    std::string name;         // Display name
    CardType type;           // Material type (FOOD, WEAPON, etc.)
    int rarity;              // Rarity level (1-3)
    std::map<AttributeType, float> attributes; // Material properties
    std::string description; // Description text
};
```

#### EventTemplate
Represents game events with conditions and effects:
```cpp
struct EventTemplate {
    std::string id;
    std::string name;
    std::string description;
    std::vector<EventCondition> conditions;  // Trigger conditions
    std::vector<EventEffect> effects;        // Event results
    bool isRepeatable;
    int priority;
};
```

## Features

### 1. Material Editor
- **Add/Remove/Edit Materials**: Full CRUD operations for game materials
- **Attribute Management**: Edit weight, attack, defense, durability, etc.
- **Type Classification**: Organize materials by type (FOOD, WEAPON, ARMOR, etc.)
- **Rarity System**: Set rarity levels affecting material properties

### 2. Recipe Editor  
- **Recipe Management**: Create and modify crafting recipes
- **Ingredient System**: Drag-and-drop ingredient selection
- **Success Rates**: Configure crafting success probabilities
- **Batch Operations**: Bulk modify multiple recipes

### 3. Event System Editor
- **Event Creation**: Define game events with complex trigger conditions
- **Condition Trees**: Visual condition editing with logical operators
- **Effect Chains**: Configure multiple effects per event
- **Event Testing**: Preview and test event execution

### 4. Debug Tools
- **Data Validation**: Real-time validation of all game data
- **Reference Checking**: Ensure all references are valid
- **Performance Monitoring**: Track data access patterns
- **Export/Import**: JSON-based data exchange

### 5. Undo/Redo System
- **State Management**: Automatic state tracking for all changes
- **Operation History**: Navigate through edit history
- **Selective Undo**: Target specific types of changes
- **Change Notifications**: Real-time change tracking

## Usage

### Basic Editor Integration

```cpp
// Initialize editor system
ImGuiManager imguiManager;
GameEditor gameEditor;

if (imguiManager.initialize(window, renderer)) {
    gameEditor.initialize(imguiManager);
}

// In main game loop
if (imguiManager.isEditorMode()) {
    imguiManager.beginFrame(window);
    gameEditor.update();
    gameEditor.render();
    imguiManager.endFrame();
}
imguiManager.render();
```

### Material Management

```cpp
// Create new material
MaterialTemplate material;
material.id = "iron_sword";
material.name = "Iron Sword";
material.type = CardType::WEAPON;
material.rarity = 2;
material.attributes[AttributeType::ATTACK] = 25.0f;
material.attributes[AttributeType::DURABILITY] = 150.0f;

gameEditor.getDataManager().addMaterial(material);
```

### Recipe Creation

```cpp
// Define ingredients
std::vector<std::pair<Card, int>> ingredients = {
    {Card("Iron", 1, CardType::METAL), 3},
    {Card("Wood", 1, CardType::MISC), 1}
};

// Create recipe
Recipe recipe("iron_sword_recipe", "Forge Iron Sword", 
              "Create an iron sword from metal and wood",
              ingredients, 
              Card("Iron Sword", 2, CardType::WEAPON),
              0.85f,  // 85% success rate
              0);     // Unlock level

gameEditor.getDataManager().addRecipe(recipe);
```

### Event System

```cpp
// Create event with conditions and effects
EventTemplate event;
event.id = "random_encounter";
event.name = "Wild Animal Encounter";
event.description = "Player encounters a wild animal while exploring";

// Add condition: 20% chance when exploring
EventCondition condition;
condition.type = ConditionType::RANDOM_CHANCE;
condition.value = 0.2f;
event.conditions.push_back(condition);

// Add effect: Take damage
EventEffect effect;
effect.type = EffectType::DAMAGE_PLAYER;
effect.value = 10.0f;
event.effects.push_back(effect);

gameEditor.getDataManager().addEvent(event);
```

## Keyboard Shortcuts

- **F1**: Toggle editor mode on/off
- **Ctrl+S**: Save current project
- **Ctrl+O**: Open project file
- **Ctrl+N**: New project
- **Ctrl+Z**: Undo last action
- **Ctrl+Y**: Redo last undone action
- **Ctrl+Shift+E**: Export data to JSON

## Data Validation

The system performs comprehensive validation:

### Material Validation
- Unique ID checking
- Required field validation  
- Attribute range validation
- Type consistency checking

### Recipe Validation
- Ingredient reference validation
- Circular dependency detection
- Success rate bounds checking
- Material existence verification

### Event Validation
- Condition syntax validation
- Effect target validation
- Priority conflict detection
- Trigger condition feasibility

## File Format

The editor uses JSON format for data persistence:

```json
{
    "materials": [
        {
            "id": "iron_sword",
            "name": "Iron Sword",
            "type": "WEAPON",
            "rarity": 2,
            "attributes": {
                "ATTACK": 25.0,
                "DURABILITY": 150.0,
                "WEIGHT": 3.5
            },
            "description": "A sturdy iron sword"
        }
    ],
    "recipes": [...],
    "events": [...]
}
```

## Extension Points

The editor system is designed for extensibility:

### Custom Panels
```cpp
class CustomEditorPanel : public EditorPanel {
public:
    CustomEditorPanel() : EditorPanel("Custom Panel") {}
    
    void render() override {
        // Custom ImGui rendering code
    }
};
```

### Data Validators
```cpp
class CustomValidator {
public:
    ValidationResult validateCustomData(const GameDataManager& data) {
        // Custom validation logic
    }
};
```

### Export Formats
```cpp
class CustomExporter {
public:
    bool exportToFormat(const GameDataManager& data, const std::string& filename) {
        // Custom export implementation
    }
};
```

## Performance Considerations

- **Lazy Loading**: Large datasets are loaded on-demand
- **Change Tracking**: Only modified data is processed
- **Memory Management**: Automatic cleanup of unused data
- **Caching**: Validation results are cached until data changes

## Future Enhancements

1. **Visual Scripting**: Node-based event and condition editing
2. **Asset Integration**: Direct asset file management 
3. **Multiplayer Support**: Collaborative editing features
4. **Plugin System**: Third-party extension support
5. **Version Control**: Built-in version tracking and branching

## Troubleshooting

### Common Issues

1. **ImGui Not Available**: The system falls back to console-based editing
2. **Validation Errors**: Check the Debug panel for detailed error messages
3. **Performance Issues**: Use the profiling tools in Debug panel
4. **File Access**: Ensure write permissions for save directory

### Debug Output

Enable verbose logging by setting:
```cpp
gameEditor.setVerboseLogging(true);
```

This will output detailed information about all editor operations to the console.
