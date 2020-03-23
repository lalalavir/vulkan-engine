#pragma once
#include "buffer.h"
#include "base.h"
#include "model.h"
#include "texture.h"
#include <map>
class Engine
{
public:
	Engine& operator=(HelloTriangleApplication& triangle)
	{
		device = triangle.device;
		physical_device = triangle.physicalDevice;
		transfer_queue = std::make_unique<vk::Queue>(device.getQueue(0,0));
		command_pool = triangle.commandPool;
		swapChain_image_size = triangle.swapChainImages.size();
	}
	enum class ObjectType
	{
		triangle,
		cube,
		ball,
		round,
		index
	};
	uint16_t createObject(ObjectType type, glm::vec3 init_pos);
	void draw(vk::CommandBuffer command_buffer, uint16_t index);
	void increaseTexture(std::string path, uint16_t index);
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
	BufferProcess buffer_process;
	Texture texture;
	size_t swapChain_image_size;
	vk::DescriptorPool descriptor_pool;
	struct Buffer
	{
		std::unique_ptr<vk::Buffer> triangle_vertex_buffer;
		std::unique_ptr<vk::Buffer> triangle_index_buffer;
	};

	struct Memory
	{
		std::unique_ptr<vk::DeviceMemory> triangle_vertex_memory;
		std::unique_ptr<vk::DeviceMemory> triangle_index_memory;
		
	};
	Buffer buffer;
	Memory memory;
	void createTriangle(glm::vec3 init_pos);
	void createCube(glm::vec3 init_pos);
	void createIndex(glm::vec3 init_pos);
	void createBall(glm::vec3 init_pos);
	void createRound(glm::vec3 init_pos);

	void drawTriangle(vk::CommandBuffer command_buffer, uint16_t index);
	void drawRound(vk::CommandBuffer command_buffer, uint16_t index);
	void drawCube(vk::CommandBuffer command_buffer, uint16_t index);
	void drawIndex(vk::CommandBuffer command_buffer, uint16_t index);
	void drawBall(vk::CommandBuffer command_buffer, uint16_t index);

	void createDescriptorPool();
	
 };
