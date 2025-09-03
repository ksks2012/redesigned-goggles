// Debug card click detection
#include <iostream>
#include "Core/View.h"
#include "Systems/SDLManager.h"
#include "Core/Inventory.h"
#include "Systems/CraftingSystem.h"
#include "Core/Card.h"
#include "Constants.h"

int main() {
    std::cout << "Debugging card click detection..." << std::endl;
    
    try {
        // Initialize SDL
        SDLManager sdlManager;
        
        // Create game components
        Inventory inventory;
        inventory.addCard(Card("Wood", 1, CardType::BUILDING, 5));
        inventory.addCard(Card("Stone", 2, CardType::BUILDING, 3));
        
        View view(sdlManager);
        
        std::cout << "Card constants:" << std::endl;
        std::cout << "CARD_X: " << Constants::CARD_X << std::endl;
        std::cout << "CARD_WIDTH: " << Constants::CARD_WIDTH << std::endl;
        std::cout << "CARD_HEIGHT: " << Constants::CARD_HEIGHT << std::endl;
        std::cout << "CARD_SPACING: " << Constants::CARD_SPACING << std::endl;
        
        // Test card positions
        std::cout << "\nFirst card should be at: (" << Constants::CARD_X << ", " << Constants::CARD_X << ")" << std::endl;
        std::cout << "First card bounds: (" << Constants::CARD_X << ", " << Constants::CARD_X << ") to (" 
                 << (Constants::CARD_X + Constants::CARD_WIDTH) << ", " 
                 << (Constants::CARD_X + Constants::CARD_HEIGHT) << ")" << std::endl;
        
        // Test various click positions
        std::vector<std::pair<int, int>> testPositions = {
            {Constants::CARD_X + 10, Constants::CARD_X + 10},  // Inside first card
            {Constants::CARD_X + Constants::CARD_WIDTH/2, Constants::CARD_X + Constants::CARD_HEIGHT/2},  // Center of first card
            {Constants::CARD_X + 10, Constants::CARD_X + Constants::CARD_SPACING + 10},  // Inside second card
            {10, 10},  // Outside cards
            {Constants::CARD_X - 5, Constants::CARD_X + 10}  // Just outside first card
        };
        
        std::cout << "\nInventory has " << inventory.getCards().size() << " cards:" << std::endl;
        for (const auto& card : inventory.getCards()) {
            std::cout << "- " << card.name << std::endl;
        }
        
        for (const auto& pos : testPositions) {
            const Card* hoveredCard = view.getHoveredCard(inventory, pos.first, pos.second);
            std::cout << "Click at (" << pos.first << ", " << pos.second << "): " 
                     << (hoveredCard ? hoveredCard->name : "No card") << std::endl;
        }
        
        std::cout << "\n✅ Card detection debug completed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
