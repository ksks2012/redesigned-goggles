# UI Component Framework

This document describes the refactored UI component framework that abstracts UI elements into reusable, RAII-managed components.

## Architecture Overview

The framework is built around a base `UIComponent` class that provides common functionality for all UI elements. Specific components inherit from this base class and implement their own rendering logic.

### Class Hierarchy

```
UIComponent (Abstract Base)
├── UIButton
├── UICard
├── UITooltip
└── UICraftingPanel
    └── UIRecipeItem
```

## Core Components

### UIComponent (Base Class)
- **Purpose**: Abstract base class providing common UI functionality
- **Features**:
  - RAII resource management for SDL textures and surfaces
  - Common rendering functions (background, border, text)
  - Position and size management
  - Hit testing for mouse interactions

### UIButton
- **Purpose**: Interactive button component
- **Features**:
  - Customizable appearance (colors, text)
  - Click event handling with callbacks
  - Enabled/disabled states
  - Automatic size calculation based on text

### UICard
- **Purpose**: Displays inventory cards with visual feedback
- **Features**:
  - Rarity-based color coding
  - Selection highlighting
  - Drag rendering support
  - Card type and quantity display

### UITooltip
- **Purpose**: Shows detailed card information on hover
- **Features**:
  - Dynamic content generation
  - Smart positioning to avoid screen edges
  - Multi-line text rendering
  - Automatic size calculation

### UICraftingPanel
- **Purpose**: Complete crafting interface with recipe management
- **Features**:
  - Recipe list with availability checking
  - Click handling for recipe selection
  - Visual feedback for craftable/locked recipes
  - Background overlay management

## Usage Examples

### Basic Button Creation
```cpp
auto button = std::make_unique<UIButton>(
    "Click Me",                    // Button text
    100, 50,                       // Position (x, y)
    120, 30,                       // Min width, height
    sdlManager,                    // SDL manager reference
    []() { std::cout << "Clicked!"; } // Click callback
);

button->render();
```

### Card Display
```cpp
auto uiCard = std::make_unique<UICard>(
    gameCard,                      // Card data
    200, 100,                      // Position
    sdlManager                     // SDL manager reference
);

uiCard->setSelected(true);        // Highlight as selected
uiCard->render();
```

### Tooltip Usage
```cpp
auto tooltip = std::make_unique<UITooltip>(sdlManager);

// Show tooltip for a card
tooltip->showForCard(card, mouseX, mouseY);
tooltip->render();

// Hide tooltip
tooltip->hide();
```

### Complete UI Setup (ViewRefactored)
```cpp
ViewRefactored view(sdlManager);

// Set up button callbacks
view.setButtonCallbacks(
    []() { /* Add card */ },
    []() { /* Remove card */ },
    []() { /* Explore */ },
    []() { /* Toggle crafting */ }
);

// Set up crafting callback
view.setCraftingCallback(
    [](const Recipe& recipe) { /* Handle crafting */ }
);

// Render everything
view.render(inventory, selectedCard, mouseX, mouseY, 
           showCraftingPanel, craftingSystem);
```

## Benefits of the Refactored Design

### 1. **Code Reusability**
- Common UI patterns abstracted into reusable components
- Consistent rendering behavior across all UI elements
- Easy to create new UI components by inheriting from UIComponent

### 2. **RAII Resource Management**
- Automatic cleanup of SDL textures and surfaces
- Exception-safe resource handling
- No memory leaks from improper SDL resource cleanup

### 3. **Separation of Concerns**
- UI logic separated from game logic
- Each component responsible for its own rendering
- Clear interfaces between components

### 4. **Maintainability**
- Easier to modify individual components without affecting others
- Centralized styling through Constants.h
- Consistent code patterns across all UI elements

### 5. **Extensibility**
- Easy to add new UI components
- Event handling abstracted through callbacks
- Flexible positioning and sizing system

## Migration from Old View Class

The old `View` class had several issues:
- Monolithic design with all rendering logic in one class
- Repeated code for background/border rendering
- Manual resource management prone to leaks
- Difficult to extend with new UI elements

The new framework addresses these issues:
- **Before**: All rendering in View::render()
- **After**: Each component handles its own rendering

- **Before**: Manual SDL texture management
- **After**: Automatic RAII cleanup in UIComponent

- **Before**: Hardcoded UI positioning
- **After**: Flexible component-based positioning

## Implementation Details

### RAII in UIComponent
```cpp
void UIComponent::renderText(const std::string& text, int offsetX, int offsetY, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Solid(sdlManager_.getFont(), text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlManager_.getRenderer(), surface);
    if (!texture) {
        SDL_FreeSurface(surface);  // Cleanup on error
        return;
    }
    
    // Use texture...
    
    // Automatic cleanup (RAII)
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
```

### Event Handling Pattern
```cpp
class UIButton : public UIComponent {
    std::function<void()> onClick_;
    
public:
    void handleClick(int mouseX, int mouseY) {
        if (isPointInside(mouseX, mouseY) && onClick_) {
            onClick_();  // Invoke callback
        }
    }
};
```

## Future Enhancements

Potential improvements to the framework:
1. **Animation System**: Add support for animated UI elements
2. **Layout Managers**: Automatic positioning and sizing
3. **Theme System**: Runtime theme switching
4. **Input Validation**: Form input components with validation
5. **Accessibility**: Screen reader support and keyboard navigation

## Conclusion

The refactored UI framework provides a solid foundation for the game's user interface. It follows modern C++ best practices, ensures proper resource management, and creates a maintainable codebase that can be easily extended with new features.
