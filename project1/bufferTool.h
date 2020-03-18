#pragma once

#include "base.h"

class BufferTool
{
public:
	struct BufferMemoryMixed
	{
		vk::Buffer buffer;
		vk::DeviceMemory bufferMemory;
	};

	void createBuffer(Transfer& transfer,BufferMemoryMixed& bufferMemory, vk::BufferUsageFlags usages
		,vk::SharingMode sharingMode, vk::MemoryPropertyFlags properties,vk::DeviceSize size);
	void copyBuffer(Transfer& transfer,vk::Buffer srcBuffer, vk::Buffer dstBuffer, VkDeviceSize size);
private:
};