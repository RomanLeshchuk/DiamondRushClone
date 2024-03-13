#pragma once

#include "raylib.h"

#include <string>
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
		std::string texturePath;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
	};

	using SimpleTextureData = std::string;
	using ImageData = std::pair<std::string, Pair<float>>;
	using SimpleImageData = std::string;

	struct AnimationData
	{
		std::string animationPath;
		std::vector<int> sequece;
		Pair<float> stretch;
		Pair<float> offset;
		Pair<bool> flip;
		int duration;
		int totalFrames;
	};

	Photos();
	Photos(
		const std::unordered_map<std::string, TextureData>* texturesData,
		const std::unordered_map<std::string, SimpleTextureData>* simpleTexturesData,
		const std::unordered_map<std::string, ImageData>* imagesData,
		const std::unordered_map<std::string, SimpleImageData>* simpleImagesData,
		const std::unordered_map<std::string, AnimationData>* animationsData);

	const PreloadedTexture* getTexture(const std::string& key);
	const PreloadedSimpleTexture* getSimpleTexture(const std::string& key);

	const PreloadedImage* getImage(const std::string& key);
	const PreloadedSimpleImage* getSimpleImage(const std::string& key);

	const PreloadedAnimation* getAnimation(const std::string& key);

	static bool isSimpleTextureDrawable(const PreloadedSimpleTexture* texture);
	static bool isSimpleImageDrawable(const PreloadedSimpleImage* image);

	static bool equalAnimations(const PreloadedAnimation* firstAnimation, const PreloadedAnimation* secondAnimation);

	~Photos();

private:
	const std::unordered_map<std::string, TextureData>* m_texturesData;
	const std::unordered_map<std::string, std::string>* m_simpleTexturesData;

	const std::unordered_map<std::string, ImageData>* m_imagesData;
	const std::unordered_map<std::string, std::string>* m_simpleImagesData;

	const std::unordered_map<std::string, AnimationData>* m_animationsData;

	std::unordered_map<std::string, PreloadedTexture> m_preloadedTextures{};
	std::unordered_map<std::string, Texture> m_preloadedSimpleTextures{};

	std::unordered_map<std::string, PreloadedImage> m_preloadedImages{};
	std::unordered_map<std::string, Image> m_preloadedSimpleImages{};

	std::unordered_map<std::string, Texture> m_preloadedAnimationsTextures{};
	std::unordered_map<std::string, PreloadedAnimation> m_preloadedAnimations{};
};