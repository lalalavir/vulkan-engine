#pragma once
#include <vulkan/vulkan.hpp>

struct BufferProcess
{
	void createBuffer(const vk::Device& device, const vk::PhysicalDevice& physical_device,
		vk::Buffer& buffer, vk::DeviceMemory& bufferMemory,
		vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property_flags);
	void copyBuffer(const vk::Device& device,const vk::CommandPool& commandPool,const vk::Queue& transfer_queue,
		vk::Buffer srcBuffer,vk::Buffer dstBuffer,vk::DeviceSize size);
	void createBuffer(const vk::Device& device, const vk::PhysicalDevice& physical_device,
		vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory,
		vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property_flags);
	void copyBuffer(const vk::Device& device, const vk::CommandPool& commandPool, const vk::Queue& transfer_queue,
		vk::UniqueBuffer srcBuffer, vk::UniqueBuffer dstBuffer, vk::DeviceSize size);
	vk::CommandBuffer beginCommandBuffer(const vk::Device& device,const vk::CommandPool& command_pool);
	
	void endSingleTimeCommands(vk::CommandBuffer command_buffer, const vk::Device& device, const vk::CommandPool& command_pool,const vk::Queue& transfer_queue);
private:

	uint32_t findMemoryType(const vk::PhysicalDevice& physical_device, uint32_t typeFilter, vk::MemoryPropertyFlags property_flags);
};