#pragma once
#include <type_traits>

#pragma region ENUM

// Defines operators & and | for enum class types
#define GENERATE_ENUM_CLASS_BITWISE_OPERATORS(EnumType) \
inline constexpr EnumType operator&(EnumType x, EnumType y) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) & static_cast<std::underlying_type_t<EnumType>>(y)); \
} \
inline constexpr EnumType operator|(EnumType x, EnumType y) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) | static_cast<std::underlying_type_t<EnumType>>(y)); \
}

// Defines operators ++(prefix) and < for enum class types
#define GENERATE_ENUM_CLASS_ITERATION_OPERATORS(EnumType) \
inline constexpr EnumType& operator++(EnumType& x) { \
	return x = static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) + 1); \
} \
inline constexpr bool operator<(EnumType x, EnumType y) { \
	return static_cast<std::underlying_type_t<EnumType>>(x) < static_cast<std::underlying_type_t<EnumType>>(y); \
}

// Defines binary operator + for enum class types
#define GENERATE_ENUM_CLASS_ARITHMETIC_OPERATORS(EnumType) \
inline constexpr EnumType operator+(EnumType x, int offset) { \
	return static_cast<EnumType>(static_cast<std::underlying_type_t<EnumType>>(x) + offset); \
}

#pragma endregion // ENUM
