#include  "engine.h"

uint16_t Engine::createObject(ObjectType type, glm::vec3 init_pos)
{
	auto index = object_indexs.size();
	switch (type)
	{
	case ObjectType::triangle:
		createTriangle(init_pos);
		index_types.insert({index, ObjectType::triangle});
		break;
	case ObjectType::cube:
		createCube(init_pos);
		index_types.insert({index, ObjectType::cube});
		break;
	case ObjectType::index:
		createIndex(init_pos);
		index_types.insert({index, ObjectType::index});
		break;
	case ObjectType::ball:
		createBall(init_pos);
		index_types.insert({index, ObjectType::ball});
		break;
	case ObjectType::round:
		createRound(init_pos);
		index_types.insert({index, ObjectType::round});
		break;
	}
	createDescriptorUBOSets(index);
	object_indexs.push_back(index);
	return index;
}

void Engine::draw(vk::CommandBuffer command_buffer, uint16_t index , uint16_t commandbuffer_index)
{
	auto iter = index_types.find(index);
	if (iter == index_types.end())
	{
		std::cout << "此索引不存在\n";
		return;
	}

	switch (iter->second)
	{
	case ObjectType::triangle:
		drawTriangle(command_buffer, index, commandbuffer_index);
		break;
	case ObjectType::index:
		drawIndex(command_buffer, index, commandbuffer_index);
		break;
	case ObjectType::cube:
		drawCube(command_buffer, index, commandbuffer_index);
		break;
	case ObjectType::ball:
		drawBall(command_buffer, index, commandbuffer_index);
		break;
	case ObjectType::round:
		drawRound(command_buffer, index, commandbuffer_index);
		break;
	}
}

void Engine::increaseTexture(std::string path, uint16_t index)
{
	vk::Image image;
	vk::DeviceMemory image_memory;
	vk::ImageView image_view;
	texture.loadTexture(path, device, physical_device, buffer_process, command_pool, *transfer_queue, image,
	                    image_memory);
	image_view = texture.createImageView(device, image, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
	auto tuple = std::make_tuple(image, image_view, image_memory);
	image_tuples.insert({index, tuple});
	texture_enables.find(index)->second = true;
	createDescriptorTextureSets(index);
}

void Engine::createTriangle(glm::vec3 pos)
{
	std::vector<Vertex> vertex =
	{
		{{pos.x, pos.y - 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, //上
		{{pos.x - 0.5f, pos.y + 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
		{{pos.x + 0.5f, pos.y + 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}
	};
	std::vector<uint32_t> index =
	{
		0, 1, 2
	};
	if (&triangle_buffer.vertex_buffer == nullptr)
	{
		vk::DeviceSize size = sizeof(vertex[0]) * vertex.size();

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		buffer_process.createBuffer(device, physical_device, stagingBuffer, stagingBufferMemory, size,
		                            vk::BufferUsageFlagBits::eTransferSrc,
		                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::
		                            eHostCoherent);

		void* data;
		device.mapMemory(stagingBufferMemory, 0, size, {}, &data);
		memcpy(data, vertex.data(), size);
		device.unmapMemory(stagingBufferMemory);

		buffer_process.createBuffer(device, physical_device, triangle_buffer.vertex_buffer,
		                            triangle_buffer.vertex_memory, size, vk::BufferUsageFlagBits::eVertexBuffer |
		                            vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
		buffer_process.copyBuffer(device, command_pool, *transfer_queue, stagingBuffer, triangle_buffer.vertex_buffer,
		                          size);

		device.destroyBuffer(stagingBuffer);
		device.freeMemory(stagingBufferMemory);
	}
	if (&triangle_buffer.index_buffer == nullptr)
	{
		vk::DeviceSize size = sizeof(index[0]) * index.size();

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		buffer_process.createBuffer(device, physical_device, stagingBuffer, stagingBufferMemory, size,
		                            vk::BufferUsageFlagBits::eTransferSrc,
		                            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::
		                            eHostCoherent);

		void* data;
		device.mapMemory(stagingBufferMemory, 0, size, {}, &data);
		memcpy(data, index.data(), size);
		device.unmapMemory(stagingBufferMemory);

		buffer_process.createBuffer(device, physical_device, triangle_buffer.index_buffer,
		                            triangle_buffer.index_memory, size, vk::BufferUsageFlagBits::eIndexBuffer |
		                            vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
		buffer_process.copyBuffer(device, command_pool, *transfer_queue, stagingBuffer, triangle_buffer.index_buffer,
		                          size);

		device.destroyBuffer(stagingBuffer);
		device.freeMemory(stagingBufferMemory);
	}
	triangle_buffer.has_value = true;
	
	std::vector<vk::Buffer> uniform_buffers;
	std::vector<vk::DeviceMemory> uniform_memories;
	uniform_buffers.resize(swapChain_image_size);
	uniform_memories.resize(swapChain_image_size);
	for (int i = 0; i < swapChain_image_size; i++)
	{
		buffer_process.createBuffer(device, physical_device, uniform_buffers[i], uniform_memories[i], sizeof(UBO),
		                            vk::BufferUsageFlagBits::eUniformBuffer,
		                            vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::
		                            eHostVisible);
	}
	uniform_buffer_map.insert({object_indexs.size(), uniform_buffers});
	uniform_memory_map.insert({object_indexs.size(), uniform_memories});
	vertices.push_back(vertex);
	indices.push_back(index);
	texture_enables.insert({ object_indexs.size(),false });
}

void Engine::createIndex(glm::vec3 pos)
{
}

void Engine::createRound(glm::vec3 pos)
{
}

void Engine::createCube(glm::vec3 pos)
{
}

void Engine::createBall(glm::vec3 pos)
{
}

void Engine::drawTriangle(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index)
{
	vk::DeviceSize offset = 0;
	command_buffer.bindVertexBuffers(0, triangle_buffer.vertex_buffer, offset);
	command_buffer.bindIndexBuffer(triangle_buffer.index_buffer, offset, vk::IndexType::eUint32);
	if(texture_enables.find(index)->second==false)
	{
		
	}
	else
	{
		
	}
	command_buffer.drawIndexed(static_cast<uint32_t>(indices[index].size()), 1, 0, offset, 0);
}

void Engine::drawIndex(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index)
{
}

void Engine::drawCube(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index)
{
}

void Engine::drawRound(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index)
{
}

void Engine::drawBall(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index)
{
}

void Engine::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> pool_sizes;
	pool_sizes[0].setDescriptorCount(swapChain_image_size).setType(vk::DescriptorType::eUniformBuffer);
	pool_sizes[1].setDescriptorCount(swapChain_image_size).setType(vk::DescriptorType::eCombinedImageSampler);
	vk::DescriptorPoolCreateInfo create_info =
	{
		{}, static_cast<uint32_t>(swapChain_image_size), static_cast<uint32_t>(pool_sizes.size()), pool_sizes.data()
	};

	descriptor_pool = device.createDescriptorPool(create_info);
}

void Engine::createDescriptorLayout()
{
	vk::DescriptorSetLayoutBinding ubo_binding =
	{
		0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex
	};
	vk::DescriptorSetLayoutBinding image_binding =
	{
		1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment
	};
	std::array<vk::DescriptorSetLayoutBinding, 2> bindings = {ubo_binding, image_binding};

	vk::DescriptorSetLayoutCreateInfo create_info =
	{
		{}, static_cast<uint32_t>(bindings.size()), bindings.data()
	};

	descriptor_set_layout = device.createDescriptorSetLayout(create_info);
}

void Engine::createDescriptorUBOSets(uint16_t index)
{
	std::vector<vk::DescriptorSetLayout> layouts(swapChain_image_size, descriptor_set_layout);
	vk::DescriptorSetAllocateInfo allocate_info =
	{
		descriptor_pool, static_cast<uint32_t>(swapChain_image_size), layouts.data()
	};
	auto descriptorSets = device.allocateDescriptorSets(allocate_info);
	for (size_t i=0;i<descriptorSets.size();i++)
	{
		vk::DescriptorBufferInfo buffer_info =
		{
			uniform_buffer_map.find(index)->second[i],0,sizeof(UBO)
		};
		vk::WriteDescriptorSet write =
		{
			descriptorSets[i],0,0,1,vk::DescriptorType::eUniformBuffer,nullptr,
			&buffer_info
		};
		device.updateDescriptorSets(write, nullptr);
	}
	descriptor_sets_ubo_map.insert({ index,descriptorSets });
}

void Engine::createDescriptorTextureSets(uint16_t index)
{
	std::vector<vk::DescriptorSetLayout> layouts(swapChain_image_size, descriptor_set_layout);
	vk::DescriptorSetAllocateInfo allocate_info =
	{
		descriptor_pool, static_cast<uint32_t>(swapChain_image_size), layouts.data()
	};
	auto image_view = std::get<1>(image_tuples.find(index)->second);
	auto descriptorSets = device.allocateDescriptorSets(allocate_info);
	for (size_t i = 0; i < descriptorSets.size(); i++)
	{
		vk::DescriptorBufferInfo buffer_info =
		{
			uniform_buffer_map.find(index)->second[i],0,sizeof(UBO)
		};
		vk::DescriptorImageInfo image_info =
		{
			*common_sampler,image_view,vk::ImageLayout::eShaderReadOnlyOptimal
		};

		vk::WriteDescriptorSet write_buffer =
		{
			descriptorSets[i],0,0,1,vk::DescriptorType::eUniformBuffer,nullptr,
			&buffer_info
		};
		vk::WriteDescriptorSet write_image =
		{
			descriptorSets[i],1,0,1,vk::DescriptorType::eCombinedImageSampler,&image_info
		};

		device.updateDescriptorSets({ write_buffer,write_image }, nullptr);
	}
	descriptor_sets_texture_map.insert({ index,descriptorSets });
}

void Engine::createCommonSampler()
{
	const vk::SamplerCreateInfo create_info =
	{
		{},vk::Filter::eLinear,vk::Filter::eLinear,vk::SamplerMipmapMode::eLinear,
		vk::SamplerAddressMode::eRepeat,vk::SamplerAddressMode::eRepeat,vk::SamplerAddressMode::eRepeat,
		0.0f,VK_TRUE,16,VK_FALSE,vk::CompareOp::eAlways,0.0f,0.0f,
		vk::BorderColor::eIntOpaqueBlack,VK_FALSE
	};
	common_sampler = device.createSamplerUnique(create_info);
}
