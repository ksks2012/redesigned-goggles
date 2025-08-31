#pragma once
#include <string>

#include <sstream>
#include <iomanip>

#if __cplusplus >= 202002L
#include <format>
inline std::string to_string_with_precision(double value, int precision = 2) {
    return std::format("{:.{}f}", value, precision);
}
#else
inline std::string to_string_with_precision(double value, int precision = 2) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << value;
    return oss.str();
}
#endif