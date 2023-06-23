#include "Photos.h"

Photos::Photos() = default;

Photos::Photos(
	const std::unordered_map<std::string_view, TextureData>* texturesData,
	const std::unordered_map<std::string_view, std::string_view>* simpleTexturesData,
	const std::unordered_map<std::string_view, ImageData>* imagesData,
	const std::unordered_map<std::string_view, std::string_view>* simpleImagesData,
	const std::unordered_map<std::string_view, AnimationData>* animationsData) :
	m_texturesData{ texturesData },
	m_simpleTexturesData{ simpleTexturesData },
	m_imagesData{ imagesData },
	m_simpleImagesData{ simpleImagesData },
	m_animationsData{ animationsData }
{
}

const Photos::PreloadedTexture* Photos::getTexture(const std::string_view& key)
{
	std::unordered_map<std::string_view, PreloadedTexture>::iterator preloadedTextureIt = m_preloadedTextures.find(key);

	if (preloadedTextureIt != m_preloadedTextures.end())
	{
		return &preloadedTextureIt->second;
	}

	std::unordered_map<std::string_view, TextureData>::const_iterator texturePathIt = m_texturesData->find(key);

	if (texturePathIt != m_texturesData->end())
	{
		return &(m_preloadedTextures[key] = {
			LoadTexture(texturePathIt->second.texturePath.data()),
			texturePathIt->second.stretch,
			texturePathIt->second.offset,
			texturePathIt->second.flip
		});
	}

	return nullptr;
}

const Photos::PreloadedSimpleTexture* Photos::getSimpleTexture(const std::string_view& key)
{
	std::unordered_map<std::string_view, Texture>::iterator preloadedSimpleTextureIt = m_preloadedSimpleTextures.find(key);

	if (preloadedSimpleTextureIt != m_preloadedSimpleTextures.end())
	{
		return &preloadedSimpleTextureIt->second;
	}

	std::unordered_map<std::string_view, std::string_view>::const_iterator texturePathIt = m_simpleTexturesData->find(key);

	if (texturePathIt != m_simpleTexturesData->end())
	{
		return &(m_preloadedSimpleTextures[key] = LoadTexture(texturePathIt->second.data()));
	}

	return nullptr;
}

const Photos::PreloadedImage* Photos::getImage(const std::string_view& key)
{
	std::unordered_map<std::string_view, PreloadedImage>::iterator preloadedImageIt = m_preloadedImages.find(key);

	if (preloadedImageIt != m_preloadedImages.end())
	{
		return &preloadedImageIt->second;
	}

	std::unordered_map<std::string_view, ImageData>::const_iterator imagePathIt = m_imagesData->find(key);

	if (imagePathIt != m_imagesData->end())
	{
		return &(m_preloadedImages[key] = {
			LoadImage(imagePathIt->second.first.data()),
			imagePathIt->second.second
		});
	}

	return nullptr;
}

const Photos::PreloadedSimpleImage* Photos::getSimpleImage(const std::string_view& key)
{
	std::unordered_map<std::string_view, Image>::iterator preloadedSimpleTextureIt = m_preloadedSimpleImages.find(key);

	if (preloadedSimpleTextureIt != m_preloadedSimpleImages.end())
	{
		return &preloadedSimpleTextureIt->second;
	}

	std::unordered_map<std::string_view, std::string_view>::const_iterator imagePathIt = m_simpleImagesData->find(key);

	if (imagePathIt != m_simpleImagesData->end())
	{
		return &(m_preloadedSimpleImages[key] = LoadImage(imagePathIt->second.data()));
	}

	return nullptr;
}

const Photos::PreloadedAnimation* Photos::getAnimation(const std::string_view& key)
{
	std::unordered_map<std::string_view, PreloadedAnimation>::iterator preloadedAnimationIt = m_preloadedAnimations.find(key);

	if (preloadedAnimationIt != m_preloadedAnimations.end())
	{
		return &preloadedAnimationIt->second;
	}

	std::unordered_map<std::string_view, AnimationData>::const_iterator animationPathIt = m_animationsData->find(key);

	if (animationPathIt != m_animationsData->end())
	{
		Photos::PreloadedAnimation* preloadedAnimation = &(m_preloadedAnimations[key] = {
			LoadTexture(animationPathIt->second.animationPath.data()),
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

bool Photos::isSimpleTextureDrawable(const PreloadedSimpleTexture* texture)
{
	if (!texture)
	{
		return false;
	}

	return texture->width && texture->height;
}

bool Photos::isSimpleImageDrawable(const PreloadedSimpleImage* image)
{
	if (!image)
	{
		return false;
	}

	return image->width && image->height;
}

bool Photos::equalAnimations(const Photos::PreloadedAnimation* firstAnimation, const Photos::PreloadedAnimation* secondAnimation)
{
	return firstAnimation->animation.id == secondAnimation->animation.id;
}


Photos::~Photos()
{
	for (const std::pair<const std::string_view, PreloadedTexture>& texture : m_preloadedTextures)
	{
		UnloadTexture(texture.second.texture);
	}

	for (const std::pair<const std::string_view, Texture>& simpleTexture : m_preloadedSimpleTextures)
	{
		UnloadTexture(simpleTexture.second);
	}

	for (const std::pair<const std::string_view, PreloadedImage>& image : m_preloadedImages)
	{
		UnloadImage(image.second.first);
	}

	for (const std::pair<const std::string_view, Image>& simpleImage : m_preloadedSimpleImages)
	{
		UnloadImage(simpleImage.second);
	}

	for (const std::pair<const std::string_view, Texture>& animationTexture : m_preloadedAnimationsTextures)
	{
		UnloadTexture(animationTexture.second);
	}
}