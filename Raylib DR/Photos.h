#pragma once

#include "raylib.h"

#include <string_view>
#include <string>
#include <initializer_list>
#include <unordered_map>
#include <algorithm>

#include "data_types.h"

class Photos
{
public:
	struct PreloadedTexture
	{
		Texture texture;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
	};

	using PreloadedSimpleTexture = Texture;
	using PreloadedImage = std::pair<Image, Pair<float>>;
	using PreloadedSimpleImage = Image;

	struct PreloadedAnimation
	{
		Texture animation;
		std::vector<int> sequence;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
		int duration;
		int frameWidth;
	};

	struct TextureData
	{
		std::string_view texturePath;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
	};

	using SimpleTextureData = std::string_view;
	using ImageData = std::pair<std::string_view, Pair<float>>;
	using SimpleImageData = std::string_view;

	struct AnimationData
	{
		std::string_view animationPath;
		std::vector<int> sequece;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
		int duration;
		int totalFrames;
	};

	Photos();
	Photos(
		const std::unordered_map<std::string_view, TextureData>* texturesData,
		const std::unordered_map<std::string_view, SimpleTextureData>* simpleTexturesData,
		const std::unordered_map<std::string_view, ImageData>* imagesData,
		const std::unordered_map<std::string_view, SimpleImageData>* simpleImagesData,
		const std::unordered_map<std::string_view, AnimationData>* animationsData);

	const PreloadedTexture* getTexture(const std::string_view& key);
	const PreloadedSimpleTexture* getSimpleTexture(const std::string_view& key);

	const PreloadedImage* getImage(const std::string_view& key);
	const PreloadedSimpleImage* getSimpleImage(const std::string_view& key);

	const PreloadedAnimation* getAnimation(const std::string_view& key);

	static bool isSimpleTextureDrawable(const PreloadedSimpleTexture* texture);
	static bool isSimpleImageDrawable(const PreloadedSimpleImage* image);

	static bool equalAnimations(const PreloadedAnimation* firstAnimation, const PreloadedAnimation* secondAnimation);

	~Photos();

private:
	const std::unordered_map<std::string_view, TextureData>* m_texturesData;
	const std::unordered_map<std::string_view, std::string_view>* m_simpleTexturesData;

	const std::unordered_map<std::string_view, ImageData>* m_imagesData;
	const std::unordered_map<std::string_view, std::string_view>* m_simpleImagesData;

	const std::unordered_map<std::string_view, AnimationData>* m_animationsData;

	std::unordered_map<std::string_view, PreloadedTexture> m_preloadedTextures{};
	std::unordered_map<std::string_view, Texture> m_preloadedSimpleTextures{};

	std::unordered_map<std::string_view, PreloadedImage> m_preloadedImages{};
	std::unordered_map<std::string_view, Image> m_preloadedSimpleImages{};

	std::unordered_map<std::string_view, Texture> m_preloadedAnimationsTextures{};
	std::unordered_map<std::string_view, PreloadedAnimation> m_preloadedAnimations{};
};