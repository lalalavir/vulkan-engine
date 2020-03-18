#pragma once
#include "base.h"

class TextureTool
{
public:
	TextureTool(){}
	~TextureTool(){}

	struct Texture
	{
		vk::Sampler sampler;
		vk::Image image;
		vk::ImageView imageView;
		vk::DeviceMemory deviceMemory;
		vk::ImageLayout imageLayout;
	};

	void loadTexture(Texture& texture,const Transfer& transfer);
private:

};
