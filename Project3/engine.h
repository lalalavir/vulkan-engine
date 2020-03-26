#pragma once
#include "buffer.h"
#include "model.h"
#include "texture.h"
#include <map>

struct Engine_value
{
	vk::Device device;
	vk::PhysicalDevice physical_device;
	vk::CommandPool command_pool;
	size_t swapChain_image_size;
};
class Engine
{
public:
	Engine& operator=(Engine_value& triangle)
	{
		device = triangle.device;
		physical_device = triangle.physical_device;
		transfer_queue = std::make_unique<vk::Queue>(device.getQueue(0, 0));
		command_pool = triangle.command_pool;
		swapChain_image_size = triangle.swapChain_image_size;
		createCommonSampler();
		return *this;
	}

	enum class ObjectType
	{
		triangle,
		cube,
		ball,
		round,
		index
	};
	~Engine()
	{
		for (auto& i:uniform_buffer_map)
		{
			for (size_t j=0;j<swapChain_image_size;j++)
			{
				device.destroyBuffer(i.second[j]);
			}
		}
		for (auto& i : uniform_memory_map)
		{
			for (size_t j = 0; j < swapChain_image_size; j++)
			{
				device.freeMemory(i.second[j]);
			}
		}
		for (auto& i : descriptor_sets_ubo_map)
				device.freeDescriptorSets(descriptor_pool,i.second);
		for (auto& i : descriptor_sets_texture_map)
				device.freeDescriptorSets(descriptor_pool, i.second);
		if (triangle_buffer.has_value == true)
			triangle_buffer.freeBuffer(device);
		device.destroy(descriptor_pool);
		device.destroy(descriptor_set_layout);
	}
	uint16_t createObject(ObjectType type, glm::vec3 init_pos);
	void draw(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);
	void increaseTexture(std::string path, uint16_t index);

	struct UBO
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
		glm::vec4 Light;
		glm::vec3 light_pos;
		glm::vec4 mirror_light;
	};
private:
	std::vector<uint16_t> object_indexs;
	vk::Device device;
	vk::PhysicalDevice physical_device;
	std::unique_ptr<vk::Queue> transfer_queue;
	vk::CommandPool command_pool;

	std::vector<std::shared_ptr<vk::Buffer>> buffers;
	std::vector<std::shared_ptr<vk::DeviceMemory>> memories;
	std::map<uint16_t, ObjectType> index_types;
	std::map<uint16_t, std::vector<vk::Buffer>> uniform_buffer_map;
	std::map<uint16_t, std::vector<vk::DeviceMemory>> uniform_memory_map;
	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<uint32_t>> indices;
	std::map<uint16_t, std::tuple<vk::Image, vk::ImageView, vk::DeviceMemory>> image_tuples;
	vk::UniqueSampler common_sampler;
	BufferProcess buffer_process;
	Texture texture;
	size_t swapChain_image_size;
	vk::DescriptorPool descriptor_pool;
	vk::DescriptorSetLayout descriptor_set_layout;
	std::map<uint16_t, std::vector<vk::DescriptorSet>> descriptor_sets_ubo_map;
	std::map<uint16_t, std::vector<vk::DescriptorSet>> descriptor_sets_texture_map;
	struct Buffer
	{
		vk::Buffer vertex_buffer;
		vk::Buffer index_buffer;
		vk::DeviceMemory vertex_memory;
		vk::DeviceMemory index_memory;
		void freeBuffer(const vk::Device device)
		{
			device.destroyBuffer(vertex_buffer);
			device.destroyBuffer(index_buffer);
			device.freeMemory(vertex_memory);
			device.freeMemory(index_memory);
		}
		bool has_value = false;
	};
	Buffer triangle_buffer;
	void createTriangle(glm::vec3 init_pos);
	void createCube(glm::vec3 init_pos);
	void createIndex(glm::vec3 init_pos);
	void createBall(glm::vec3 init_pos);
	void createRound(glm::vec3 init_pos);

	void drawTriangle(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);
	void drawRound(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);
	void drawCube(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);
	void drawIndex(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);
	void drawBall(vk::CommandBuffer command_buffer, uint16_t index, uint16_t commandbuffer_index);

	void createDescriptorPool();
	void createDescriptorLayout();
	void createDescriptorUBOSets(uint16_t index);
	void createDescriptorTextureSets(uint16_t index);
	void createCommonSampler();

	std::map<uint16_t, bool> texture_enables;
};
