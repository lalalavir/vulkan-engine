#include "bufferTool.h"

void BufferTool::createBuffer(Transfer& transfer,BufferMemoryMixed& bufferMemoryMixed,vk::BufferUsageFlags usages
	, vk::SharingMode sharingMode,vk::MemoryPropertyFlags properties,vk::DeviceSize size)
{
	vk::BufferCreateInfo bufferInfo =
	{
		{},size,usages,sharingMode
	};

	bufferMemoryMixed.buffer = transfer.device.createBuffer(bufferInfo);

	auto memRequirements = transfer.device.getBufferMemoryRequirements(bufferMemoryMixed.buffer);
	auto memProperties = transfer.physicalDevice.getMemoryProperties();

	vk::MemoryAllocateInfo allocInfo =
	{
		size
	};
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			allocInfo.memoryTypeIndex = i;
		}
	}
	
	bufferMemoryMixed.bufferMemory = transfer.device.allocateMemory(allocInfo);

	transfer.device.bindBufferMemory(bufferMemoryMixed.buffer, bufferMemoryMixed.bufferMemory, 0);
	
}

void BufferTool::copyBuffer(Transfer& transfer, vk::Buffer srcBuffer, vk::Buffer dstBuffer, VkDeviceSize size)
{
	vk::CommandBufferAllocateInfo allocInfo =
	{
		transfer.commandPool,vk::CommandBufferLevel::ePrimary,1
	};

	auto commandBuffer = transfer.device.allocateCommandBuffers(allocInfo);

	vk::CommandBufferBeginInfo beginInfo =
	{
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	};

	commandBuffer[0].begin(beginInfo);


}
