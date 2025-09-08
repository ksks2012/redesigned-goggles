#pragma once

// Common constants that might be used by both UI and game logic
namespace CommonConstants {
// Version information
constexpr const char* VERSION = "1.0.0";
constexpr const char* BUILD_DATE = __DATE__;

// Debug settings
#ifndef NDEBUG
constexpr bool DEBUG_MODE = true;
#else
constexpr bool DEBUG_MODE = false;
#endif

// General application settings
constexpr int MAX_FPS = 60;
constexpr bool VSYNC_ENABLED = true;
}
