#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Texture::loadTexture(const std::string path, const vk::Device& device, const vk::PhysicalDevice& physical_device,
BufferProcess& buffer_process, const vk::CommandPool& command_pool, const vk::Queue& transfer_queue, vk::Image& texture_image, vk::DeviceMemory& texture_memory)
{
	int channels;
	auto* ui = stbi_load(path.c_str(), &texture_width, &texture_height, &channels, STBI_rgb_alpha);
	vk::DeviceSize size = texture_width * texture_height * 4;

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingMemory;

	buffer_process.createBuffer(device, physical_device, stagingBuffer, stagingMemory, size, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);

	void* data;
	device.mapMemory(stagingMemory, 0, size, {}, &data);
	memcpy(data, ui, static_cast<uint32_t>(size));
	device.unmapMemory(stagingMemory);
	
	stbi_image_free(ui);

	createImage(device, physical_device, texture_width, texture_height, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal,
		texture_image, texture_memory);
	Transfer_image_settings firstSettings = {vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal};
	transitionImageLayout(device, command_pool, texture_image, vk::Format::eR8G8B8A8Srgb, firstSettings, buffer_process, transfer_queue);
	copyBufferToImage(device, command_pool, texture_image, stagingBuffer, buffer_process, transfer_queue);
	Transfer_image_settings second_settings = {vk::ImageLayout::eTransferDstOptimal,vk::ImageLayout::eShaderReadOnlyOptimal};
	transitionImageLayout(device, command_pool, texture_image, vk::Format::eR8G8B8A8Srgb, second_settings, buffer_process, transfer_queue);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingMemory);
}

vk::ImageView Texture::createImageView(const vk::Device& device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags)
{
	vk::ImageSubresourceRange subresource =
	{
		aspect_flags,0,1,0,1
	};
	vk::ImageViewCreateInfo create_info =
	{
		{},image,vk::ImageViewType::e2D,format,{},subresource
	};

	return device.createImageView(create_info);
}

uint32_t Texture::findMemoryType(const vk::PhysicalDevice& physical_device, uint32_t typeFilter,
                                 vk::MemoryPropertyFlags property_flags)
{
	auto memProperties = physical_device.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void Texture::createImage(const vk::Device& device, const vk::PhysicalDevice& physical_device, uint32_t width,
	uint32_t height, vk::Format format, vk::ImageTiling image_tiling, vk::ImageUsageFlags usage,
	vk::MemoryPropertyFlags property_flags, vk::Image& image, vk::DeviceMemory& image_memory)
{
	vk::Extent3D extent =
	{
		static_cast<uint32_t>(width),static_cast<uint32_t>(height),1
	};
	vk::ImageCreateInfo create_info =
	{
		{},vk::ImageType::e2D,format,extent,1,1,vk::SampleCountFlagBits::e1,
		image_tiling,usage,vk::SharingMode::eExclusive,0,nullptr,vk::ImageLayout::eUndefined
	};

	image = device.createImage(create_info);
	auto memRequirements = device.getImageMemoryRequirements(image);

	vk::MemoryAllocateInfo allocate_info =
	{
		memRequirements.size,findMemoryType(physical_device,memRequirements.memoryTypeBits,property_flags)
	};

	image_memory = device.allocateMemory(allocate_info);

	device.bindImageMemory(image, image_memory, 0);
}

void Texture::transitionImageLayout(const vk::Device& device, const vk::CommandPool& command_pool, vk::Image image, vk::Format format, const Transfer_image_settings& settings, BufferProcess& buffer_process, const vk::Queue& transfer_queue)
{
	auto command_buffer = buffer_process.beginCommandBuffer(device, command_pool);
	vk::ImageMemoryBarrier memory_barrier =
	{
		vk::AccessFlags(0),vk::AccessFlags(0),settings.old_image_layout,settings.new_image_layout,
		VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,image
	};
	memory_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	memory_barrier.subresourceRange.baseArrayLayer = 0;
	memory_barrier.subresourceRange.baseMipLevel = 0;
	memory_barrier.subresourceRange.levelCount = 1;
	memory_barrier.subresourceRange.layerCount = 1;

	vk::PipelineStageFlags source_stage_flags;
	vk::PipelineStageFlags dst_stage_flags;
	if (settings.old_image_layout == vk::ImageLayout::eUndefined && settings.new_image_layout == vk::ImageLayout::eTransferDstOptimal)
	{
		memory_barrier.srcAccessMask = vk::AccessFlags(0);
		memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		source_stage_flags = vk::PipelineStageFlagBits::eTopOfPipe;
		dst_stage_flags = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (settings.old_image_layout == vk::ImageLayout::eTransferDstOptimal && settings.new_image_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		memory_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		source_stage_flags = vk::PipelineStageFlagBits::eTransfer;
		dst_stage_flags = vk::PipelineStageFlagBits::eFragmentShader;
	}
	
	else
		throw std::runtime_error("unsupported layout transition!(bind texture barrier)");
	command_buffer.pipelineBarrier(source_stage_flags, dst_stage_flags, {}, 0, nullptr, 0, nullptr, 1
		, &memory_barrier);
	
	buffer_process.endSingleTimeCommands(command_buffer, device, command_pool, transfer_queue);

}

void Texture::copyBufferToImage(const vk::Device& device, const vk::CommandPool& command_pool, vk::Image image,
	vk::Buffer buffer, BufferProcess& buffer_process, const vk::Queue& transfer_queue)
{
	auto commandBuffer = buffer_process.beginCommandBuffer(device, command_pool);

	vk::ImageSubresourceLayers subresource =
	{
		vk::ImageAspectFlagBits::eColor,0,0,1
	};
	vk::BufferImageCopy image_copy =
	{
		0,0,0,subresource,{0,0,0},
		{static_cast<uint32_t>(texture_width),static_cast<uint32_t>(texture_height),1}
	};
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, image_copy);
	buffer_process.endSingleTimeCommands(commandBuffer, device, command_pool, transfer_queue);
}

vk::Format Texture::find_supported_format(const vk::PhysicalDevice physical_device,const std::vector<vk::Format>& candidates, vk::ImageTiling tiling,
	vk::FormatFeatureFlags features)
{
	for (auto format:candidates)
	{
		auto props = physical_device.getFormatProperties(format);
		if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}
