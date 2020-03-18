#include "base.h"


void HelloTriangleApplication::createDescriptorSetLayout()
{
	vk::DescriptorSetLayoutBinding ubo_layout_binding =
	{
		0,vk::DescriptorType::eUniformBuffer,1,vk::ShaderStageFlagBits::eVertex
	};
	vk::DescriptorSetLayoutBinding sample_layout_binding =
	{
		1,vk::DescriptorType::eCombinedImageSampler,1,vk::ShaderStageFlagBits::eFragment
	};
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sample_layout_binding };
	vk::DescriptorSetLayoutCreateInfo create_info =
	{
		{},static_cast<uint32_t>(bindings.size()),bindings.data()
	};

	descriptor_set_layout = device.createDescriptorSetLayout(create_info);
}

void HelloTriangleApplication::createUniformBuffers()
{
	vk::DeviceSize buffer_size = sizeof(UBO);
	uniform_buffers.resize(swapChainImages.size());
	uniform_memories.resize(swapChainImages.size());
	for (int i=0;i<swapChainImages.size();i++)
	{
		buffer_process.createBuffer(device, physicalDevice, uniform_buffers[i], uniform_memories[i], buffer_size,
			vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
	}
	
}

void HelloTriangleApplication::updateUniformBuffer(uint32_t currentImage)
{
	static  auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UBO ubo;
	//ubo.model = glm::mat4(1.0f);
	ubo.model = glm::rotate(glm::mat4(1.0f), -time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//camera.LookAt(glm::vec3(0.5, 2, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	//camera.UpdataViewMatrix();
	ubo.Light = glm::vec4(1, 1, 1, 0.01);
	ubo.light_pos = glm::vec3(1, 1, 1);
	ubo.mirror_light = glm::vec4(camera.Position, 0.8f);
	camera.UpdataCameraPosition();
	ubo.view = camera.GetViewMatrix();
	ubo.proj = glm::perspective(glm::radians(camera.fov), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 100.0f);
	ubo.proj[1][1] *= -1;
	//ubo.proj = camera.GetProjMatrix4x4();
	void* data;
	device.mapMemory(uniform_memories[currentImage], 0, sizeof(ubo), {}, &data);
	memcpy(data, &ubo, sizeof(ubo));
	device.unmapMemory(uniform_memories[currentImage]);
	
}

void HelloTriangleApplication::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> pool_sizes;
	pool_sizes[0].setDescriptorCount(static_cast<uint32_t>(swapChainImages.size())).setType(vk::DescriptorType::eUniformBuffer);
	pool_sizes[1].setDescriptorCount(static_cast<uint32_t>(swapChainImages.size())).setType(vk::DescriptorType::eCombinedImageSampler);
	vk::DescriptorPoolCreateInfo create_info =
	{
		{},static_cast<uint32_t>(swapChainImages.size()),static_cast<uint32_t>(pool_sizes.size()),pool_sizes.data()
	};

	descriptor_pool = device.createDescriptorPool(create_info);
}

void HelloTriangleApplication::createDescriptorSets()
{
	std::vector<vk::DescriptorSetLayout> layouts(swapChainImages.size(), descriptor_set_layout);
	vk::DescriptorSetAllocateInfo allocate_info =
	{
		descriptor_pool,static_cast<uint32_t>(swapChainImages.size()),layouts.data()
	};
	descriptor_sets = device.allocateDescriptorSets(allocate_info);

	for (int i=0;i<swapChainImages.size();i++)
	{
		vk::DescriptorBufferInfo buffer_info =
		{
			uniform_buffers[i],0,sizeof(UBO)
		};
		vk::DescriptorImageInfo image_info =
		{
			texture_sampler,texture_image_view,vk::ImageLayout::eShaderReadOnlyOptimal
		};
		
		std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].dstSet = descriptor_sets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = vk::DescriptorType::eUniformBuffer;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &buffer_info;

		descriptorWrites[1].dstSet = descriptor_sets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = vk::DescriptorType::eCombinedImageSampler;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &image_info;

		device.updateDescriptorSets(descriptorWrites, nullptr);
	}
}

void HelloTriangleApplication::createTextureImgae()
{
	texture.loadTexture("D:/vulkan/tu/1.bmp", device, physicalDevice, buffer_process, commandPool, graphicsQueue, texture_image,
		texture_memory);
}

void HelloTriangleApplication::createTextureImgaeView()
{
	texture_image_view = texture.createImageView(device, texture_image, vk::Format::eR8G8B8A8Srgb,vk::ImageAspectFlagBits::eColor);
}

void HelloTriangleApplication::createVertexBuffer()
{
	vk::DeviceSize size = sizeof(meshes.vertices[0]) * meshes.vertices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	buffer_process.createBuffer(device, physicalDevice, stagingBuffer, stagingBufferMemory, size, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data;
	device.mapMemory(stagingBufferMemory, 0, size,{},&data);
	memcpy(data, vertices.data(), size);
	device.unmapMemory(stagingBufferMemory);
	
	buffer_process.createBuffer(device, physicalDevice, vertex_buffer, vertex_memory, size, vk::BufferUsageFlagBits::eVertexBuffer |
		vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
	buffer_process.copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, vertex_buffer,size);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void HelloTriangleApplication::createTextureSampler()
{
	vk::SamplerCreateInfo create_info =
	{
		{},vk::Filter::eLinear,vk::Filter::eLinear,vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eRepeat,vk::SamplerAddressMode::eRepeat,vk::SamplerAddressMode::eRepeat,
		0.0f,VK_TRUE,16,VK_FALSE,vk::CompareOp::eAlways,0.0f,0.0f,
		vk::BorderColor::eIntOpaqueBlack,VK_FALSE
	};
	texture_sampler = device.createSampler(create_info);
}

void HelloTriangleApplication::createIndexBuffer()
{
	vk::DeviceSize size = sizeof(meshes.indices[0]) * meshes.indices.size();

	vk::Buffer stagingBuffer;
	vk::DeviceMemory stagingBufferMemory;
	buffer_process.createBuffer(device, physicalDevice, stagingBuffer, stagingBufferMemory, size, vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	void* data;
	device.mapMemory(stagingBufferMemory, 0, size, {}, &data);
	memcpy(data, indices.data(), size);
	device.unmapMemory(stagingBufferMemory);

	buffer_process.createBuffer(device, physicalDevice, index_buffer, index_memory, size, vk::BufferUsageFlagBits::eIndexBuffer |
		vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
	buffer_process.copyBuffer(device, commandPool, graphicsQueue, stagingBuffer, index_buffer, size);

	device.destroyBuffer(stagingBuffer);
	device.freeMemory(stagingBufferMemory);
}

void HelloTriangleApplication::createDepthResources()
{
	auto format = texture.find_supported_format(physicalDevice, { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
	texture.createImage(device, physicalDevice, swapChainExtent.width, swapChainExtent.height, format, vk::ImageTiling::eOptimal,
		vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depth_image, depth_memory);
	depth_image_view = texture.createImageView(device, depth_image, format, vk::ImageAspectFlagBits::eDepth);	
}

void HelloTriangleApplication::load_my_model()
{
	Model model;
    meshes= model.loadModel("D:/vulkan/model/chalet.obj");
}
