#pragma once

#include <iostream>

#include "data_types.h"

namespace Options
{
	constexpr Coords WorldSize = { 792, 792 }; // 792
	constexpr int SidebarWidth = 300;
	constexpr Coords ViewportSize = { 5, 5 };
	constexpr Coords UpdateRectSize = { 17, 17 };
	constexpr Coords MaxPlayerShift = { 2, 2 };
	constexpr int FPS = 120; // 120
	constexpr int MovesPerSecond = 10;

	constexpr int FramesPerMove = FPS / MovesPerSecond;
}