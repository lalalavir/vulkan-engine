#pragma once
#include <vulkan/vulkan.hpp>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <iostream>
#include <fstream>

template<typename  T>
bool is_destroy_object(T& obj)
{
	if(static_cast<bool>(obj)!=VK_NULL_HANDLE)
	{
		return true;
	}
	return false;
}

namespace vk_demo
{
	struct DemoBase
	{
		vk::Instance get_vulkan_instance(SDL_Window* window);
		vk::Device get_vulkan_device(const vk::SurfaceKHR& surface);
		vk::Device get_vulkan_device(const vk::SurfaceKHR& surface,std::vector<const char*> more_extensions,
			vk::PhysicalDeviceFeatures features);
		vk::SurfaceKHR get_window_surface(SDL_Window* window,const vk::Instance& instance);
		vk::PhysicalDevice get_physical_device(const vk::Instance& instance);
		int graphicsQueueIndex;
		int presentQueueIndex;

	protected:
		vk::PhysicalDevice physicalDevice;
		std::vector<const char*> getRequiredExtensions(SDL_Window* window);
	};

	struct DemoProcess
	{
		struct ShaderPasses
		{
			std::string vertPass;
			std::string fragmentPass;
		};

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		struct Other_things
		{
			vk::Pipeline base_pipeline;
			uint32_t base_pipeline_index;
		};
		vk::SwapchainKHR getVulkanSwapChain(vk::Device& device,vk::PhysicalDevice& physicalDevice,vk::SurfaceKHR& surface);
		std::vector<vk::Image> inline getSwapchainImages(vk::Device& device,vk::SwapchainKHR& sp);
		vk::ImageView get_image_view(vk::Device&,vk::Image&);
		vk::Pipeline get_graphics_pipeline(vk::Device& device,vk::PipelineLayout& pipeline_layout,ShaderPasses shader_passes,
			vk::RenderPass& render_pass,Other_things* other_things);
		vk::Pipeline get_compute_pipeline(vk::Device& device, vk::PipelineLayout pipeline_layout);
		vk::RenderPass get_render_pass(vk::Device& device);
		vk::PipelineLayout inline get_pipeline_layout(vk::Device& device, vk::PipelineLayoutCreateInfo create_info);
		std::vector<vk::Framebuffer> get_framebuffers(vk::Device& device, vk::RenderPass& render_pass,std::vector<vk::ImageView>& image_views);
		vk::CommandPool get_command_pool(vk::Device& device, DemoBase& demo_base);
		/*must be called before get_graphics_pipeline*/
		vk::GraphicsPipelineCreateInfo& get_graphics_pipeline_create_info();
	protected:
		VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
					return availableFormat;
				}
			}

			return availableFormats[0];
		}

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
			for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR) {
					return availablePresentMode;
				}
			}
		}
		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
			if (capabilities.currentExtent.width != UINT32_MAX) {
				return capabilities.currentExtent;
			}
			else {
				VkExtent2D actualExtent = { 800, 600 };

				actualExtent.width =800;
				actualExtent.height = 600;

				return actualExtent;
			}
		}
		SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

			if (formatCount != 0) {
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

			if (presentModeCount != 0) {
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}
		vk::GraphicsPipelineCreateInfo graphics_pipeline_create_info_;
		static std::vector<char> readFile(const std::string& filename);
		vk::ShaderModule createShaderModule(const vk::Device& device,const std::vector<char>& code);
	};

	inline vk::PipelineLayout DemoProcess::get_pipeline_layout(vk::Device& device,
		vk::PipelineLayoutCreateInfo create_info)
	{
		return device.createPipelineLayout(create_info);
	}

	struct DemoBufferProcess
	{
	public:
		struct Bufferdata
		{
			vk::Buffer buffer;
			vk::DeviceMemory buffer_memory;
		};

		Bufferdata create_buffer(vk::Device& device,vk::PhysicalDevice& physical_device, vk::DeviceSize size,
			vk::BufferUsageFlags usage,vk::MemoryPropertyFlags property_flags);
	private:
		
	};
}