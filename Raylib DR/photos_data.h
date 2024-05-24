#pragma once

#include "Photos.h"

namespace TexturesLayouts
{
	struct Layout
	{
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
	};

	constexpr Layout Default = {
		{ 1.0f, 1.0f },
		{ 0.0f, 0.0f },
		{ false, false }
	};

	constexpr Layout Player = {
		{ 26.0f / 24.0f, 26.0f / 24.0f },
		{ -(1.0f - (26.0f / 24.0f)) / 2.0f, 1.0f - (26.0f / 24.0f) },
		{ false, false }
	};

	constexpr Layout PlayerClimb = {
		{ 26.0f / 24.0f, 28.0f / 24.0f },
		{ 0.0f, -4.0f/24 },
		{ false, false }
	};
}

namespace Textures
{
	using namespace TexturesLayouts;

	namespace Themes
	{
		std::unordered_map<std::string, Photos::TextureData> Jungle
		{
			{ "wall", { "textures/wall.png", Default.stretch, Default.offset, Default.flip } },
			{ "chest", { "textures/chest.png", Default.stretch, Default.offset, Default.flip } },
			{ "chest_opened", { "textures/chest_opened.png", Default.stretch, Default.offset, Default.flip } },
			{ "bush", { "textures/bush.png", Default.stretch, Default.offset, Default.flip } },
			{ "rock", { "textures/rock.png", Default.stretch, Default.offset, Default.flip } },
			{ "wall_way", { "textures/wall_way.png", Default.stretch, Default.offset, Default.flip } },
			{ "diamond", { "textures/diamond.png", Default.stretch, Default.offset, Default.flip } },
			{ "finish", { "textures/finish.png", Default.stretch, Default.offset, Default.flip } }
		};
	}
}

namespace SimpleTextures
{
	std::unordered_map<std::string, Photos::SimpleTextureData> SimpleTexturesDatas
	{
		{ "background", "textures/background.png" },
		{ "sidebar", "textures/sidebar.png" },
		{ "menu", "textures/menu.png" }
	};
}

namespace Images
{
	std::unordered_map<std::string, Photos::ImageData> ImagesDatas
	{
	};
}

namespace SimpleImages
{
	namespace Levels
	{
		std::unordered_map<std::string, Photos::SimpleImageData> StartMenu
		{
		};

		std::unordered_map<std::string, Photos::SimpleImageData> Level1
		{
			{ "map", "textures/map.png" }
		};
	}
}

namespace Animations
{
	using namespace TexturesLayouts;

	std::unordered_map<std::string, Photos::AnimationData> Jungle
	{
		{ "player_calm", { "textures/player/calm.png", std::vector(39, 1) + std::vector{ 2 }, Player.stretch, Player.offset, Player.flip, 20, 2 } },
		{ "player_push", { "textures/player/push.png", { 1, 2 }, Player.stretch, Player.offset + Pair<float>{ 0.1f, 0.0f }, Player.flip, 2, 2 } },
		{ "player_hold", { "textures/player/hold.png", { 1, 2 }, Player.stretch, Player.offset, Player.flip, 8, 2 } },
		{ "player_climb", { "textures/player/climb.png", { 1, 2, 3, 2, 1, 4, 5, 4 }, PlayerClimb.stretch, PlayerClimb.offset, PlayerClimb.flip, 4, 5 } },
		{ "player_calm_up", { "textures/player/climb.png", { 1 }, PlayerClimb.stretch, PlayerClimb.offset, PlayerClimb.flip, 1, 5 } },
		{ "player_descent", { "textures/player/descent.png", { 1, 2, 3, 2, 1, 4, 5, 4 }, PlayerClimb.stretch, PlayerClimb.offset, PlayerClimb.flip, 4, 5 } },
		{ "player_calm_down", { "textures/player/descent.png", { 1 }, PlayerClimb.stretch, PlayerClimb.offset, PlayerClimb.flip, 1, 5 } },
		{ "bush_particles", { "textures/bush_particles.png", generateSequence(1, 12), Default.stretch, Default.offset, Default.flip, 8, 12 } },
		{ "diamond_particles", { "textures/diamond_particles.png", generateSequence(1, 3), Default.stretch, Default.offset, Default.flip, 1, 3 } }
	};
}

std::array<Photos, 3> LevelsPhotos
{
	Photos(
		&Textures::Themes::Jungle,
		&SimpleTextures::SimpleTexturesDatas,
		&Images::ImagesDatas,
		&SimpleImages::Levels::StartMenu,
		&Animations::Jungle
	),
	Photos(
		&Textures::Themes::Jungle,
		&SimpleTextures::SimpleTexturesDatas,
		&Images::ImagesDatas,
		&SimpleImages::Levels::Level1,
		&Animations::Jungle
	),
	Photos(
		&Textures::Themes::Jungle,
		&SimpleTextures::SimpleTexturesDatas,
		&Images::ImagesDatas,
		&SimpleImages::Levels::Level1,
		&Animations::Jungle
	)
};