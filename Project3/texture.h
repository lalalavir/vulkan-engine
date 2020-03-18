
#pragma once
#include "buffer.h"

class Texture
{
public:
	struct Transfer_image_settings
	{
		vk::ImageLayout old_image_layout;
		vk::ImageLayout new_image_layout;
	};
	
	void loadTexture(const std::string path, const vk::Device& device, const vk::PhysicalDevice& physical_device,
BufferProcess& buffer_process, const vk::CommandPool& command_pool , const vk::Queue& transfer_queue, vk::Image& texture_image, vk::DeviceMemory& texture_memory);
	vk::ImageView createImageView(const vk::Device& device, vk::Image image, vk::Format format,vk::ImageAspectFlags aspect_flags);
	uint32_t findMemoryType(const vk::PhysicalDevice& physical_device, uint32_t typeFilter, vk::MemoryPropertyFlags property_flags);
	void createImage(const vk::Device& device, const vk::PhysicalDevice& physical_device, uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling image_tiling, vk::ImageUsageFlags usage,
		vk::MemoryPropertyFlags flags, vk::Image& image, vk::DeviceMemory& image_memory);
	void transitionImageLayout(const vk::Device& device, const vk::CommandPool& command_pool, vk::Image image, vk::Format format, const Transfer_image_settings& settings, BufferProcess& buffer_process, const vk::Queue& transfer_queue);
	void copyBufferToImage(const vk::Device& device, const vk::CommandPool& command_pool, vk::Image image, vk::Buffer buffer, BufferProcess& buffer_process, const vk::Queue& transfer_queue);
	vk::Format find_supported_format(const vk::PhysicalDevice physical_device, const std::vector<vk::Format>& candidates,vk::ImageTiling tiling,vk::FormatFeatureFlags feature);
private:
	
	int texture_width, texture_height;
};


