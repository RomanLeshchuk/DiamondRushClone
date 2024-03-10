#pragma once

#include "data_types.h"

#include "raylib.h"

#include <string>

class Text
{
public:
	Text(const std::string& text, const Coords& coords, int fontSize, Color color);

	void draw() const;

	std::string text;
	Coords coords;
	int fontSize;
	Color color;
};

class Counter
{
public:
	Counter(const std::string& text, const int* valuePtr, const Coords& coords, int fontSize, Color color);

	void draw() const;

	std::string text;
	const int* valuePtr;
	Coords coords;
	int fontSize;
	Color color;
};