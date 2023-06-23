#pragma once

#include "raylib.h"

#include <iostream>
#include <vector>
#include <numeric>

template <typename T>
struct Pair
{
	T x;
	T y;

	template <typename U>
	constexpr Pair operator+(const Pair<U>& pair) const
	{
		return {
			x + pair.x,
			y + pair.y
		};
	}

	template <typename U>
	constexpr Pair operator-(const Pair<U>& pair) const
	{
		return {
			x - pair.x,
			y - pair.y
		};
	}

	template <typename U>
	constexpr Pair operator*(const Pair<U>& pair) const
	{
		return {
			x * pair.x,
			y * pair.y
		};
	}

	template <typename U>
	constexpr Pair operator/(const Pair<U>& pair) const
	{
		return {
			x / pair.x,
			y / pair.y
		};
	}

	template <typename U>
	constexpr Pair operator%(const Pair<U>& pair) const
	{
		return {
			x % pair.x,
			y % pair.y
		};
	}

	template <typename U>
	constexpr Pair operator+(U scalar) const
	{
		return {
			x + scalar,
			y + scalar
		};
	}

	template <typename U>
	constexpr Pair operator-(U scalar) const
	{
		return {
			x - scalar,
			y - scalar
		};
	}

	template <typename U>
	constexpr Pair operator*(U scalar) const
	{
		return {
			x * scalar,
			y * scalar
		};
	}

	template <typename U>
	constexpr Pair operator/(U scalar) const
	{
		return {
			x / scalar,
			y / scalar
		};
	}

	template <typename U>
	constexpr Pair operator%(U scalar) const
	{
		return {
			x % scalar,
			y % scalar
		};
	}

	template <typename U>
	Pair& operator+=(const Pair<U>& pair)
	{
		x += pair.x;
		y += pair.y;

		return *this;
	}

	template <typename U>
	Pair& operator-=(const Pair<U>& pair)
	{
		x -= pair.x;
		y -= pair.y;

		return *this;
	}

	template <typename U>
	Pair& operator*=(const Pair<U>& pair)
	{
		x *= pair.x;
		y *= pair.y;

		return *this;
	}

	template <typename U>
	Pair& operator/=(const Pair<U>& pair)
	{
		x /= pair.x;
		y /= pair.y;

		return *this;
	}

	template <typename U>
	Pair& operator%=(const Pair<U>& pair)
	{
		x %= pair.x;
		y %= pair.y;

		return *this;
	}

	template <typename U>
	Pair& operator+=(U scalar)
	{
		x += scalar;
		y += scalar;

		return *this;
	}

	template <typename U>
	Pair& operator-=(U scalar)
	{
		x -= scalar;
		y -= scalar;

		return *this;
	}

	template <typename U>
	Pair& operator*=(U scalar)
	{
		x *= scalar;
		y *= scalar;

		return *this;
	}

	template <typename U>
	Pair& operator/=(U scalar)
	{
		x /= scalar;
		y /= scalar;

		return *this;
	}

	template <typename U>
	Pair& operator%=(U scalar)
	{
		x %= scalar;
		y %= scalar;

		return *this;
	}

	constexpr Pair operator-() const
	{
		return {
			-x,
			-y
		};
	}

	template <typename U>
	constexpr bool operator==(const Pair<U>& pair) const
	{
		return x == pair.x && y == pair.y;
	}

	constexpr operator bool() const
	{
		return x || y;
	}

	Pair& modThis()
	{
		x = std::abs(x);
		y = std::abs(y);

		return *this;
	}

	constexpr Pair modCopy() const
	{
		return {
			std::abs(x),
			std::abs(y)
		};
	}

	template <typename U>
	constexpr bool isCovering(const Pair<U>& pair) const
	{
		const Pair<U> sameSignVec = *this - pair;
		return (sameSignVec.x ? (x / sameSignVec.x > 0) : true) && (sameSignVec.y ? (y / sameSignVec.y > 0) : true);
	}
};

template <typename T>
Rectangle operator+(const Rectangle& rect, const Pair<T>& pair)
{
	return {
		rect.x + pair.x,
		rect.y + pair.y,
		rect.width,
		rect.height
	};
}

template <typename T>
Rectangle operator+(const Pair<T>& pair, const Rectangle& rect)
{
	return rect + pair;
}

template <typename T>
Rectangle operator-(const Rectangle& rect, const Pair<T>& pair)
{
	return {
		rect.x - pair.x,
		rect.y - pair.y,
		rect.width,
		rect.height
	};
}

template <typename T>
Rectangle operator-(const Pair<T>& pair, const Rectangle& rect)
{
	return rect - pair;
}

template <typename T>
std::ostream& operator<<(std::ostream& out, Pair<T> pair)
{
	out << "X: " << pair.x << ", Y: " << pair.y;
	return out;
}

using Coords = Pair<int>;

template <int scalar>
class Movement
{
public:
	inline static constexpr Coords UP{ 0, -scalar };
	inline static constexpr Coords DOWN{ 0, scalar };
	inline static constexpr Coords LEFT{ -scalar, 0 };
	inline static constexpr Coords RIGHT{ scalar, 0 };
	inline static constexpr Coords NONE{ 0, 0 };
};

constexpr float ToRadians = PI / 180.0f;
constexpr float ToDegreees = 180.0f / PI;

template <typename T>
std::vector<T> operator+(const std::vector<T>& firstVector, const std::vector<T>& secondVector)
{
	std::vector<T> sum{};

	sum.insert(sum.end(), firstVector.begin(), firstVector.end());
	sum.insert(sum.end(), secondVector.begin(), secondVector.end());

	return sum;
}

inline std::vector<int> generateSequence(int begin, int end)
{
	std::vector<int> sequence(end - begin + 1);
	std::iota(sequence.begin(), sequence.end(), begin);

	return sequence;
}

inline bool operator==(Color firstColor, Color secondColor)
{
	return firstColor.r == secondColor.r && firstColor.g == secondColor.g && firstColor.b == secondColor.b && firstColor.a == secondColor.a;
}