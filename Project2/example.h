#pragma once
#include "base.h"

class Example
{
public:
	void run();
private:
	vk_demo::DemoBase demo_base_;
	vk_demo::DemoProcess demo_process_;

	vk::Instance instance;
	vk::Device device;
	vk::SurfaceKHR surface;
	vk::PhysicalDevice physicalDevice;
	vk::SwapchainKHR swapchain;
	std::vector<vk::Image> swapchain_images;
	std::vector<vk::ImageView> swapchain_imageViews;
	std::vector<vk::Framebuffer> frameBuffers;
	vk::Pipeline graphicsPipeline;
	vk::RenderPass render_pass;
	vk::PipelineLayout pipeline_layout;
	vk::CommandPool command_pool;
	std::vector<vk::CommandBuffer> command_buffers;
	vk::Semaphore image_semaphore;
	vk::Semaphore render_semaphore;
	vk::Fence fence;
	vk::Queue graphics_queue;
	vk::Queue present_queue;
	
	SDL_Window* window;
	void init_vk();
	void init_SDL();
	void mainLoop();
	void start_command_buffers();
	void create_semphores();
	void drawFrames();
	void destroy();
};


