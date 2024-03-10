#include "Text.h"

Text::Text(const std::string& text, const Coords& coords, int fontSize, Color color) :
	text{ text }, coords{ coords }, fontSize{ fontSize }, color{ color }
{
}

void Text::draw() const
{
	DrawText(text.c_str(), coords.x - MeasureText(text.c_str(), fontSize) / 2, coords.y - fontSize / 2, fontSize, color);
}

Counter::Counter(const std::string& text, const int* valuePtr, const Coords& coords, int fontSize, Color color) :
	text{ text }, valuePtr{ valuePtr }, coords{ coords }, fontSize{ fontSize }, color{ color }
{
}

void Counter::draw() const
{
	std::string currentText = text + ": " + std::to_string(*valuePtr);
	DrawText(currentText.c_str(), coords.x - MeasureText(currentText.c_str(), fontSize) / 2, coords.y - fontSize / 2, fontSize, color);
}