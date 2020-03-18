#pragma once
#include "buffer.h"
#include "base.h"
#include "model.h"
#include "texture.h"

class Engine
{
public:
private:
	std::vector<uint16_t> object_indexs;
	std::shared_ptr<vk::Device> device;
	std::vector<std::shared_ptr<vk::Buffer>> buffers;
	std::vector<std::shared_ptr<vk::DeviceMemory>> memories;
 };
