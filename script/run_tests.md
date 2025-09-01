# Catch2 Testing Framework for MVC Model Components

## Overview
This project now includes comprehensive unit testing for the MVC architecture's model components using the Catch2 testing framework. The tests focus on validating the core business logic and data management functionality.

## Test Structure

### Test Files
- `tests/main.cpp` - Main test runner with Catch2 configuration
- `tests/test_card.cpp` - Tests for the Card model class
- `tests/test_inventory.cpp` - Tests for the Inventory model class  
- `tests/test_crafting_system.cpp` - Tests for the CraftingSystem model class

### Tested Components

#### 1. Card Model (`test_card.cpp`)
**Purpose**: Tests the fundamental game item representation
**Test Categories**:
- **Basic Construction**: Verifies card creation with different constructors
- **Attribute Management**: Tests setting and retrieving card attributes
- **Type System**: Validates card type enumeration functionality
- **Edge Cases**: Tests with zero/negative quantities, empty names, high rarity values

**Key Test Cases**:
```cpp
// Basic card creation
Card metalCard("Iron", 1, CardType::METAL, 5);

// Attribute management
weapon.setAttribute(AttributeType::ATTACK, 15.5f);
REQUIRE(weapon.getAttribute(AttributeType::ATTACK) == 15.5f);

// Backward compatibility
Card legacyCard("Wood", 2, 3); // Old constructor
```

#### 2. Inventory Model (`test_inventory.cpp`)
**Purpose**: Tests the inventory management system
**Test Categories**:
- **Basic Operations**: Adding, removing, updating cards
- **Quantity Management**: Handling card stacking and depletion
- **Thread Safety**: Concurrent access testing with multiple threads
- **Edge Cases**: Empty inventory, large collections, stress testing

**Key Test Cases**:
```cpp
// Adding duplicate cards increases quantity
inventory.addCard(Card("Iron Ore", 1, CardType::METAL, 5));
inventory.addCard(Card("Iron Ore", 1, CardType::METAL, 3));
// Result: Single card with quantity 8

// Thread safety testing
std::vector<std::thread> threads;
for (int t = 0; t < numThreads; ++t) {
    threads.emplace_back([&inventory, t]() {
        // Concurrent card additions
    });
}
```

#### 3. CraftingSystem Model (`test_crafting_system.cpp`)
**Purpose**: Tests the recipe and crafting mechanics
**Test Categories**:
- **Recipe Management**: Creation, validation, unlocking
- **Crafting Logic**: Material checking, success/failure handling
- **Resource Consumption**: Proper material deduction
- **Edge Cases**: Zero ingredients, insufficient materials, RNG handling

**Key Test Cases**:
```cpp
// Recipe availability checking
Card medicine("Medicine", 2, CardType::HERB, 5);
inventory.addCard(medicine);
const Recipe* medkitRecipe = craftingSystem.getRecipe("medkit");
REQUIRE(craftingSystem.canCraft(*medkitRecipe, inventory) == true);

// Crafting execution with RNG
CraftingResult result = craftingSystem.craftItem(*wallRecipe, inventory);
// Tests handle randomness by multiple attempts
```

## Test Categories and Coverage

### 1. Functional Testing
- **Data Integrity**: Ensures data remains consistent across operations
- **Business Logic**: Validates game rules and mechanics
- **API Contracts**: Tests public interface behavior

### 2. Thread Safety Testing
- **Concurrent Access**: Multiple threads accessing inventory simultaneously
- **Race Condition Prevention**: Mutex protection validation
- **Data Consistency**: Ensuring no corruption during concurrent operations

### 3. Edge Case Testing
- **Boundary Values**: Zero quantities, maximum values, empty collections
- **Invalid Input**: Non-existent items, negative values
- **Resource Limits**: Large collections, stress testing

### 4. Integration Testing
- **Component Interaction**: How Card, Inventory, and CraftingSystem work together
- **Real Game Scenarios**: Using actual game data and recipes
- **Cross-Component Dependencies**: Recipe ingredients matching inventory items

## Build and Execution

### Building Tests
```bash
cd /home/hong/code/C++/survive
mkdir build && cd build
cmake ..
make
```

### Running Tests
```bash
# Run all tests
make test_run

# Run specific test executable directly
./SurviveTests

# Run with Catch2 filters
./SurviveTests "[Card]"           # Only Card tests
./SurviveTests "[threading]"      # Only threading tests
./SurviveTests --list-tests       # List all available tests
```

### Test Output Interpretation
```
===============================================================================
All tests passed (264 assertions in 10 test cases)
```
- **264 assertions**: Individual test conditions verified
- **10 test cases**: Grouped test scenarios
- **All tests passed**: Full validation success

## Technical Implementation

### Catch2 Integration
- **Header-Only**: Uses single-header Catch2 v2.13.10
- **Location**: `lib/catch2/catch.hpp`
- **CMake Integration**: Automatic detection and test target creation

### Test Architecture
```
tests/
├── main.cpp                 # Test runner entry point
├── test_card.cpp           # Card model tests
├── test_inventory.cpp      # Inventory model tests
└── test_crafting_system.cpp # CraftingSystem model tests
```

### Library Structure
- **SurviveLib**: Static library containing all game logic (excludes main.cpp)
- **SurviveTests**: Test executable linking to SurviveLib
- **Separate Compilation**: Tests compile independently from main game

## Test Data and Scenarios

### Real Game Integration
Tests use actual game data from the live system:
- **Card Factory**: Uses `Constants::CardFactory` for realistic cards
- **Recipe System**: Tests against real recipes (medkit, wall, fuel, etc.)
- **Attribute System**: Validates actual game attributes (attack, durability, weight)

### Threading Scenarios
```cpp
// 4 threads adding 25 cards each concurrently
const int numThreads = 4;
const int cardsPerThread = 25;
// Tests mutex protection and data consistency
```

### Stress Testing
```cpp
// Large inventory with 1000 items
const int numCards = 1000;
// Tests performance and memory management
```

## Continuous Integration Support

### CMake Integration
```cmake
# Automatic test detection
if(CATCH2_AVAILABLE)
    enable_testing()
    add_executable(SurviveTests ${TEST_SOURCES})
    target_link_libraries(SurviveTests SurviveLib)
endif()
```

### Custom Test Target
```cmake
add_custom_target(test_run
    COMMAND SurviveTests
    DEPENDS SurviveTests
    COMMENT "Running Catch2 tests"
)
```

## Benefits and Value

### 1. Code Quality Assurance
- **Regression Prevention**: Catches breaking changes early
- **Refactoring Safety**: Validates behavior during code changes
- **Documentation**: Tests serve as executable specifications

### 2. Development Productivity
- **Rapid Feedback**: Immediate validation of changes
- **Debugging Aid**: Isolated testing of components
- **Confidence**: Safe to modify with comprehensive test coverage

### 3. Maintenance Benefits
- **Behavioral Documentation**: Tests describe expected functionality
- **Change Impact**: Quickly identify affected components
- **Quality Metrics**: Measurable code reliability

## Future Enhancements

### Additional Test Coverage
- **View Component Testing**: UI component validation
- **Controller Testing**: User interaction logic
- **Integration Testing**: Full game flow scenarios
- **Performance Testing**: Benchmarking critical paths

### Advanced Testing Features
- **Property-Based Testing**: Generate random test data
- **Mutation Testing**: Validate test effectiveness
- **Coverage Analysis**: Measure test completeness
- **Benchmark Testing**: Performance regression detection

This comprehensive testing framework provides a solid foundation for maintaining code quality and ensuring reliable game functionality as the project evolves.
