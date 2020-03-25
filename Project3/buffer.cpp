#include "buffer.h"

void BufferProcess::createBuffer(const vk::Device& device, const vk::PhysicalDevice& physical_device,
    vk::Buffer& buffer, vk::DeviceMemory& bufferMemory, vk::DeviceSize size, vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags property_flags)
{
    vk::BufferCreateInfo buffer_create_info =
    {
        {},size,usage,vk::SharingMode::eExclusive
    };
    buffer = device.createBuffer(buffer_create_info);

    auto memRequirements = device.getBufferMemoryRequirements(buffer);
    vk::MemoryAllocateInfo allocate_info =
    {
         memRequirements.size,findMemoryType(physical_device,memRequirements.memoryTypeBits,property_flags)
    };

    bufferMemory = device.allocateMemory(allocate_info);
    device.bindBufferMemory(buffer, bufferMemory,0);
}

void BufferProcess::copyBuffer(const vk::Device& device, const vk::CommandPool& commandPool, const vk::Queue& transfer_queue,
    vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer command_buffer = beginCommandBuffer(device, commandPool);

    vk::BufferCopy buffer_copy =
    {
    	0,0,size
    };
    command_buffer.copyBuffer(srcBuffer,dstBuffer,buffer_copy);

    endSingleTimeCommands(command_buffer, device, commandPool, transfer_queue);
}

void BufferProcess::createBuffer(const vk::Device& device, const vk::PhysicalDevice& physical_device,
	vk::UniqueBuffer& buffer, vk::UniqueDeviceMemory& bufferMemory, vk::DeviceSize size, vk::BufferUsageFlags usage,
	vk::MemoryPropertyFlags property_flags)
{
    vk::BufferCreateInfo buffer_create_info =
    {
        {},size,usage,vk::SharingMode::eExclusive
    };
    buffer = device.createBufferUnique(buffer_create_info);

    auto memRequirements = device.getBufferMemoryRequirements(*buffer);
    vk::MemoryAllocateInfo allocate_info =
    {
         memRequirements.size,findMemoryType(physical_device,memRequirements.memoryTypeBits,property_flags)
    };

    bufferMemory = device.allocateMemoryUnique(allocate_info);
    device.bindBufferMemory(*buffer, *bufferMemory, 0);
}

void BufferProcess::copyBuffer(const vk::Device& device, const vk::CommandPool& commandPool,
	const vk::Queue& transfer_queue, vk::UniqueBuffer srcBuffer, vk::UniqueBuffer dstBuffer, vk::DeviceSize size)
{
    vk::CommandBuffer command_buffer = beginCommandBuffer(device, commandPool);

    vk::BufferCopy buffer_copy =
    {
        0,0,size
    };
    command_buffer.copyBuffer(*srcBuffer, *dstBuffer, buffer_copy);

    endSingleTimeCommands(command_buffer, device, commandPool, transfer_queue);
}

vk::CommandBuffer BufferProcess::beginCommandBuffer(const vk::Device& device, const vk::CommandPool& command_pool)
{
    vk::CommandBufferAllocateInfo allocate_info =
    {
        command_pool,vk::CommandBufferLevel::ePrimary,1
    };

    auto commandbuffer = device.allocateCommandBuffers(allocate_info);
    vk::CommandBufferBeginInfo begin_info =
    {
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    };

    commandbuffer[0].begin(begin_info);
    return commandbuffer[0];
}

void BufferProcess::endSingleTimeCommands(vk::CommandBuffer command_buffer, const vk::Device& device,
	const vk::CommandPool& command_pool,const vk::Queue& transfer_queue)
{
    command_buffer.end();
    vk::SubmitInfo submit_info;
    submit_info.setCommandBufferCount(1).setPCommandBuffers(&command_buffer);

    transfer_queue.submit(1, &submit_info, nullptr);
    transfer_queue.waitIdle();
    device.freeCommandBuffers(command_pool, command_buffer);
}

uint32_t BufferProcess::findMemoryType(const vk::PhysicalDevice& physical_device, uint32_t typeFilter,
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
