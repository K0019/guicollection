#pragma once
#include <type_traits>
#include <memory>
#include <random>
#include <chrono>
#include <string>
#include <cstdio>


// Platform-dependent usages
#if defined(_WIN32) || defined(_WIN64)
#define STRNCPY(dst, src, size) strncpy_s(dst, src, size)
#elif (defined(__linux__) || defined(__APPLE__)) && defined(HAVE_STRLCPY)
#define STRNCPY(dst, src, size) strlcpy(dst, src, size)
#else
#define STRNCPY(dst, src, size) strncpy(dst, src, size - 1); (dst)[size - 1] = '\0'
#endif


namespace util {

	namespace internal {

		// Used by ReturnType_t
		// No definition because we're only using this for templating the received function.
		template <typename ReturnType, typename ...Args>
		ReturnType FunctionTypeSplitter(ReturnType(*)(Args...));

	}

	// Gets the return type of a function pointer
	template <auto Func>
	using ReturnType_t = decltype(internal::FunctionTypeSplitter(Func));

	// Determines whether a container's data is stored as a C array. Requires that the container has methods size() and data().
	template <typename ContType>
	concept ConvertibleToCArray = requires(ContType container)
	{
		std::size(container);
		std::data(container);
	};

	inline uint64_t Hash(const std::string& s)
	{
		// https://stackoverflow.com/questions/66764096/calculating-stdhash-using-different-compilers

		constexpr uint64_t fnv_prime{ 1099511628211ULL };
		constexpr uint64_t fnv_offset_basis{ 14695981039346656037ULL };

		uint64_t hash{ fnv_offset_basis };

		for (auto c : s)
		{
			hash ^= c;
			hash *= fnv_prime;
		}

		return hash;
	}

	inline uint64_t CombineHashes(uint64_t a, uint64_t b)
	{
		// boost::hash_combine
		uint64_t x{ a + 0x9e3779b9 + b };
		constexpr uint64_t m{ 0xe9846af9b1a615d };
		x ^= x >> 32;
		x *= m;
		x ^= x >> 32;
		x *= m;
		x ^= x >> 28;
		return x;
	}

	template <typename... Args>
	std::string sprintf(const char* fmt, const Args&... args)
	{
		int size{ std::snprintf(nullptr, 0, fmt, args...) };
		if (size <= 0)
			return std::string{};

		std::string buffer( size, '\0' );
		std::snprintf(buffer.data(), size + 1, fmt, args...);
		return buffer;
	}

}

// Defines unary + operator to decay an enum class to its underlying type (int, char, etc)
template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, std::underlying_type_t<T>> operator+(T e)
{
	return static_cast<std::underlying_type_t<T>>(e);
}
