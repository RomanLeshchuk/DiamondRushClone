#include "Photos.h"

Photos::Photos() = default;

Photos::Photos(
	const std::unordered_map<std::string, TextureData>* texturesData,
	const std::unordered_map<std::string, std::string>* simpleTexturesData,
	const std::unordered_map<std::string, ImageData>* imagesData,
	const std::unordered_map<std::string, std::string>* simpleImagesData,
	const std::unordered_map<std::string, AnimationData>* animationsData) :
	m_texturesData{ texturesData },
	m_simpleTexturesData{ simpleTexturesData },
	m_imagesData{ imagesData },
	m_simpleImagesData{ simpleImagesData },
	m_animationsData{ animationsData }
{
}

const Photos::PreloadedTexture* Photos::getTexture(const std::string& key)
{
	std::unordered_map<std::string, PreloadedTexture>::iterator preloadedTextureIt = m_preloadedTextures.find(key);

	if (preloadedTextureIt != m_preloadedTextures.end())
	{
		return &preloadedTextureIt->second;
	}

	std::unordered_map<std::string, TextureData>::const_iterator texturePathIt = m_texturesData->find(key);

	if (texturePathIt != m_texturesData->end())
	{
		return &(m_preloadedTextures[key] = {
			LoadTexture(texturePathIt->second.texturePath.c_str()),
			texturePathIt->second.stretch,
			texturePathIt->second.offset,
			texturePathIt->second.flip
		});
	}

	return nullptr;
}

const Photos::PreloadedSimpleTexture* Photos::getSimpleTexture(const std::string& key)
{
	std::unordered_map<std::string, Texture>::iterator preloadedSimpleTextureIt = m_preloadedSimpleTextures.find(key);

	if (preloadedSimpleTextureIt != m_preloadedSimpleTextures.end())
	{
		return &preloadedSimpleTextureIt->second;
	}

	std::unordered_map<std::string, std::string>::const_iterator texturePathIt = m_simpleTexturesData->find(key);

	if (texturePathIt != m_simpleTexturesData->end())
	{
		return &(m_preloadedSimpleTextures[key] = LoadTexture(texturePathIt->second.c_str()));
	}

	return nullptr;
}

const Photos::PreloadedImage* Photos::getImage(const std::string& key)
{
	std::unordered_map<std::string, PreloadedImage>::iterator preloadedImageIt = m_preloadedImages.find(key);

	if (preloadedImageIt != m_preloadedImages.end())
	{
		return &preloadedImageIt->second;
	}

	std::unordered_map<std::string, ImageData>::const_iterator imagePathIt = m_imagesData->find(key);

	if (imagePathIt != m_imagesData->end())
	{
		return &(m_preloadedImages[key] = {
			LoadImage(imagePathIt->second.first.c_str()),
			imagePathIt->second.second
		});
	}

	return nullptr;
}

const Photos::PreloadedSimpleImage* Photos::getSimpleImage(const std::string& key)
{
	std::unordered_map<std::string, Image>::iterator preloadedSimpleTextureIt = m_preloadedSimpleImages.find(key);

	if (preloadedSimpleTextureIt != m_preloadedSimpleImages.end())
	{
		return &preloadedSimpleTextureIt->second;
	}

	std::unordered_map<std::string, std::string>::const_iterator imagePathIt = m_simpleImagesData->find(key);

	if (imagePathIt != m_simpleImagesData->end())
	{
		return &(m_preloadedSimpleImages[key] = LoadImage(imagePathIt->second.c_str()));
	}

	return nullptr;
}

const Photos::PreloadedAnimation* Photos::getAnimation(const std::string& key)
{
	std::unordered_map<std::string, PreloadedAnimation>::iterator preloadedAnimationIt = m_preloadedAnimations.find(key);

	if (preloadedAnimationIt != m_preloadedAnimations.end())
	{
		return &preloadedAnimationIt->second;
	}

	std::unordered_map<std::string, AnimationData>::const_iterator animationPathIt = m_animationsData->find(key);

	if (animationPathIt != m_animationsData->end())
	{
		Photos::PreloadedAnimation* preloadedAnimation = &(m_preloadedAnimations[key] = {
			LoadTexture(animationPathIt->second.animationPath.c_str()),
			animationPathIt->second.sequece,
			animationPathIt->second.stretch,
			animationPathIt->second.offset,
			animationPathIt->second.flip,
			animationPathIt->second.duration,
			0
		});

		preloadedAnimation->frameWidth = preloadedAnimation->animation.width / animationPathIt->second.totalFrames;

		return preloadedAnimation;
	}

	return nullptr;
}

bool Photos::equalAnimations(const Photos::PreloadedAnimation* firstAnimation, const Photos::PreloadedAnimation* secondAnimation)
{
	return firstAnimation->animation.id == secondAnimation->animation.id;
}


Photos::~Photos()
{
	for (const std::pair<const std::string, PreloadedTexture>& texture : m_preloadedTextures)
	{
		UnloadTexture(texture.second.texture);
	}

	for (const std::pair<const std::string, Texture>& simpleTexture : m_preloadedSimpleTextures)
	{
		UnloadTexture(simpleTexture.second);
	}

	for (const std::pair<const std::string, PreloadedImage>& image : m_preloadedImages)
	{
		UnloadImage(image.second.first);
	}

	for (const std::pair<const std::string, Image>& simpleImage : m_preloadedSimpleImages)
	{
		UnloadImage(simpleImage.second);
	}

	for (const std::pair<const std::string, Texture>& animationTexture : m_preloadedAnimationsTextures)
	{
		UnloadTexture(animationTexture.second);
	}
}