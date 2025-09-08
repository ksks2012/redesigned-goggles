#pragma once
// 
// DEPRECATED: This file is kept for backwards compatibility.
// Please use the new separated constant files:
// - UIConstants.h for UI-related constants
// - GameConstants.h for game logic constants  
// - CommonConstants.h for shared constants
//

#include "UIConstants.h"
#include "GameConstants.h"
#include "CommonConstants.h"

// Backwards compatibility - re-export all constants under Constants namespace
namespace Constants {
// Re-export UI constants
using namespace UIConstants;

// Re-export Game constants
using namespace GameConstants;

// Re-export Common constants  
using namespace CommonConstants;
}