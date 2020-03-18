#include "base.h"

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

vk::Instance vk_demo::DemoBase::get_vulkan_instance(SDL_Window* window)
{
	vk::ApplicationInfo appInfo;
	appInfo.setApiVersion(VK_API_VERSION_1_0).setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
		.setEngineVersion(VK_MAKE_VERSION(1, 0, 0)).setPEngineName("no engine").setPApplicationName("vulkan");
	std::vector<const char*> extensionNames;

	extensionNames = getRequiredExtensions(window);
	
	vk::InstanceCreateInfo instanceInfo
	{
		{},&appInfo,0,{},(uint32_t)extensionNames.size(),extensionNames.size()>0? extensionNames.data():nullptr
	};
	
	auto instance = vk::createInstance(instanceInfo);
	return instance;
}

vk::Device vk_demo::DemoBase::get_vulkan_device(const vk::SurfaceKHR& surface)
{
	vk::DeviceCreateInfo deviceInfo;
	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	auto queueProperties = physicalDevice.getQueueFamilyProperties();
	for (size_t i = 0; i <queueProperties.size(); i++)
	{
		if (queueProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			this->graphicsQueueIndex = i;
		}

		auto support= physicalDevice.getSurfaceSupportKHR(i, surface);
		if (support)
		{
			this->presentQueueIndex = i;
		}

		if (presentQueueIndex != NULL && graphicsQueueIndex !=NULL)
			break;
	}
	presentQueueIndex++;
	int indexs[] = { presentQueueIndex,graphicsQueueIndex };
	for (size_t i = 0; i < 2; i++)
	{
		vk::DeviceQueueCreateInfo createInfo;
		float priority = 1.0f;
		createInfo.setPQueuePriorities(&priority)
			.setQueueCount(1).setQueueFamilyIndex(indexs[i]);
		queueInfos.push_back(createInfo);
	}

	vk::PhysicalDeviceFeatures features = {};
	deviceInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueInfos.size()))
		.setPQueueCreateInfos(queueInfos.data()).setPEnabledFeatures(&features);
	std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	deviceInfo.setEnabledExtensionCount((uint32_t)deviceExtensions.size())
		.setPpEnabledExtensionNames(deviceExtensions.size() > 0 ? deviceExtensions.data() : nullptr);

	auto device = physicalDevice.createDevice(deviceInfo);

	return device;
}

vk::Device vk_demo::DemoBase::get_vulkan_device(const vk::SurfaceKHR& surface, std::vector<const char*> more_extensions,
	vk::PhysicalDeviceFeatures features)
{
	vk::DeviceCreateInfo deviceInfo;
	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	auto queueProperties = physicalDevice.getQueueFamilyProperties();
	for (size_t i = 0; i < queueProperties.size(); i++)
	{
		if (queueProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			this->graphicsQueueIndex = i;
		}

		auto support = physicalDevice.getSurfaceSupportKHR(i, surface);
		if (support)
		{
			this->presentQueueIndex = i;
		}

		if (presentQueueIndex != NULL && graphicsQueueIndex != NULL)
			break;
	}
	presentQueueIndex++;
	int indexs[] = { presentQueueIndex,graphicsQueueIndex };
	for (size_t i = 0; i < 2; i++)
	{
		vk::DeviceQueueCreateInfo createInfo;
		float priority = 1.0f;
		createInfo.setPQueuePriorities(&priority)
			.setQueueCount(1).setQueueFamilyIndex(indexs[i]);
		queueInfos.push_back(createInfo);
	}

	deviceInfo.setQueueCreateInfoCount(static_cast<uint32_t>(queueInfos.size()))
		.setPQueueCreateInfos(queueInfos.data()).setPEnabledFeatures(&features);
	std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	std::vector<const char*> extensions;
	extensions.insert(extensions.end(), deviceExtensions.begin(), deviceExtensions.end());
	extensions.insert(extensions.end(), more_extensions.begin(), more_extensions.end());
	
	deviceInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()))
		.setPpEnabledExtensionNames(extensions.size() > 0 ? extensions.data() : nullptr);

	auto device = physicalDevice.createDevice(deviceInfo);

	return device;
}

vk::SurfaceKHR vk_demo::DemoBase::get_window_surface(SDL_Window* window, const vk::Instance& instance)
{
	VkSurfaceKHR gsurface;
	if (SDL_Vulkan_CreateSurface(window, (VkInstance)instance, &gsurface) != VK_SUCCESS)
	{
		std::cerr<< SDL_GetError();
	};
	vk::SurfaceKHR surface = gsurface;
	return surface;
}

vk::PhysicalDevice vk_demo::DemoBase::get_physical_device(const vk::Instance& instance)
{
	auto Devices = instance.enumeratePhysicalDevices();
	physicalDevice = Devices[0];
	return Devices[0];
}

vk::SwapchainKHR vk_demo::DemoProcess::getVulkanSwapChain(vk::Device& device, vk::PhysicalDevice& physicalDevice, vk::SurfaceKHR& surface)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice,surface);
	
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;


	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkSwapchainKHR swapChain;
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
	return swapChain;
}

std::vector<vk::Image> vk_demo::DemoProcess::getSwapchainImages(vk::Device& device, vk::SwapchainKHR& sp)
{
	auto images = device.getSwapchainImagesKHR(sp);
	return images;
}

vk::ImageView vk_demo::DemoProcess::get_image_view(vk::Device& device, vk::Image& image)
{
	vk::ComponentMapping mapping =
	{
		vk::ComponentSwizzle::eIdentity,vk::ComponentSwizzle::eIdentity,vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity
	};
	vk::ImageSubresourceRange range =
	{
		vk::ImageAspectFlagBits::eColor,0,1,0,1
	};
	vk::ImageViewCreateInfo createInfo =
	{
		{},image,vk::ImageViewType::e2D,vk::Format::eR8G8B8A8Srgb,mapping,range
	};

	vk::ImageView image_view;
	image_view = device.createImageView(createInfo);
	return image_view;
}

vk::Pipeline vk_demo::DemoProcess::get_graphics_pipeline(vk::Device& device, vk::PipelineLayout& pipeline_layout, ShaderPasses shader_passes,
	vk::RenderPass& render_pass, Other_things* other_things)
{
	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;
	auto vertShaderCode = readFile(shader_passes.vertPass);
	auto fragShaderCode = readFile(shader_passes.fragmentPass);
	vk::ShaderModule vertShaderModule, fragShaderModule;
	if(vertShaderCode.size()!=0)
	{
		vertShaderModule = createShaderModule(device, vertShaderCode);
		vk::PipelineShaderStageCreateInfo create_info =
		{
			{},vk::ShaderStageFlagBits::eVertex,vertShaderModule,"main"
		};
		shader_stages.push_back(create_info);
	}

	if (fragShaderCode.size() != 0)
	{
		fragShaderModule = createShaderModule(device, fragShaderCode);
		vk::PipelineShaderStageCreateInfo create_info =
		{
			{},vk::ShaderStageFlagBits::eFragment,fragShaderModule,"main"
		};
		shader_stages.push_back(create_info);
	}

	vk::PipelineVertexInputStateCreateInfo vertex_input;
	vk::PipelineInputAssemblyStateCreateInfo input_assembly_state =
	{
		{},vk::PrimitiveTopology::eTriangleList,VK_FALSE
	};

	vk::Viewport viewport =
	{
		0.0f,0.0f,800.0f,600.0f,0.0f,1.0f
	};

	vk::Rect2D scisssor =
	{
		{0,0},{800,600}
	};
	vk::PipelineViewportStateCreateInfo viewport_state =
	{
		{},1,&viewport,1,&scisssor
	};

	vk::PipelineRasterizationStateCreateInfo rasterization_state =
	{
		{},VK_FALSE,VK_FALSE,vk::PolygonMode::eFill,vk::CullModeFlagBits::eBack,
		vk::FrontFace::eClockwise,VK_FALSE
	};

	vk::PipelineMultisampleStateCreateInfo multisample_state =
	{
		{},vk::SampleCountFlagBits::e1,VK_FALSE
	};

	vk::PipelineColorBlendAttachmentState color_blend_attachment =
	{
		VK_FALSE
	};
	color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eG |
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eB;
	vk::PipelineColorBlendStateCreateInfo color_blend_state =
	{
		{},VK_FALSE,vk::LogicOp::eCopy,1,&color_blend_attachment
	};

	std::array<vk::DynamicState, 2> dynamic_states =
	{
		vk::DynamicState::eViewport,vk::DynamicState::eLineWidth
	};

	vk::PipelineDynamicStateCreateInfo pipeline_dynamic_state =
	{
		{},static_cast<uint32_t>(dynamic_states.size()),dynamic_states.data()
	};
	
	vk::GraphicsPipelineCreateInfo create_info =
	{
		{},static_cast<uint32_t>(shader_stages.size()),shader_stages.data(),&vertex_input,
		&input_assembly_state,nullptr,&viewport_state,&rasterization_state,&multisample_state,nullptr,
		&color_blend_state,&pipeline_dynamic_state,pipeline_layout,render_pass,0
	};
	if(other_things!=nullptr)
	{
		create_info.setBasePipelineIndex(other_things->base_pipeline_index)
			.setBasePipelineHandle(other_things->base_pipeline);
	}
	
	auto pipeline = device.createGraphicsPipeline(nullptr, create_info);

	if (is_destroy_object(vertShaderModule))
		device.destroyShaderModule(vertShaderModule);
	if (is_destroy_object(fragShaderModule))
		device.destroyShaderModule(fragShaderModule);
	return pipeline;
}


vk::Pipeline vk_demo::DemoProcess::get_compute_pipeline(vk::Device& device, vk::PipelineLayout pipeline_layout)
{
	vk::ComputePipelineCreateInfo create_info =
	{
	
	};
	auto pipeline = device.createComputePipeline(nullptr,create_info);
	return pipeline;
}

vk::RenderPass vk_demo::DemoProcess::get_render_pass(vk::Device& device)
{
	std::array<vk::AttachmentDescription, 1> descriptions;
	descriptions[0].setFormat(vk::Format::eR8G8B8A8Srgb)
		.setLoadOp(vk::AttachmentLoadOp::eClear).setStoreOp(vk::AttachmentStoreOp::eStore)
		.setSamples(vk::SampleCountFlagBits::e1).setInitialLayout(vk::ImageLayout::eUndefined)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	std::array<vk::SubpassDescription, 1> subpass_descriptions;

	vk::AttachmentReference reference =
	{
		0,vk::ImageLayout::eColorAttachmentOptimal
	};
	subpass_descriptions[0].setColorAttachmentCount(1).setPColorAttachments(&reference)
		.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
	vk::SubpassDependency subpass_dependency =
	{
		VK_SUBPASS_EXTERNAL,0,vk::PipelineStageFlagBits::eColorAttachmentOutput,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,(vk::AccessFlags)0,
		vk::AccessFlagBits::eColorAttachmentRead|vk::AccessFlagBits::eColorAttachmentWrite
	};
	vk::RenderPassCreateInfo create_info =
	{
		{},static_cast<uint32_t>(descriptions.size()),descriptions.data(), static_cast<uint32_t>(subpass_descriptions.size()),
		subpass_descriptions.data(),1,& subpass_dependency
	};
	auto renderpass = device.createRenderPass(create_info);
	return renderpass;
}

std::vector<vk::Framebuffer> vk_demo::DemoProcess::get_framebuffers(vk::Device& device, vk::RenderPass& render_pass, std::vector<vk::ImageView>& image_views)
{
	std::vector<vk::Framebuffer> framebuffers;
	for (size_t i=0;i<image_views.size();i++)
	{
		vk::FramebufferCreateInfo create_info=
		{
			{},render_pass,1,&image_views[i],800,600,1
		};
		auto framebuffer = device.createFramebuffer(create_info);
		framebuffers.push_back(framebuffer);
	}

	return framebuffers;
}

vk::CommandPool vk_demo::DemoProcess::get_command_pool(vk::Device& device, DemoBase& demo_base)
{
	vk::CommandPoolCreateInfo create_info =
	{
		{},static_cast<uint32_t>( demo_base.graphicsQueueIndex)
	};

	return device.createCommandPool(create_info);
}

vk::GraphicsPipelineCreateInfo& vk_demo::DemoProcess::get_graphics_pipeline_create_info()
{
	return  graphics_pipeline_create_info_;
}

std::vector<char> vk_demo::DemoProcess::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

vk::ShaderModule vk_demo::DemoProcess::createShaderModule(const vk::Device& device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

std::vector<const char*> vk_demo::DemoBase::getRequiredExtensions(SDL_Window* window)
{
	uint32_t SDL_ExtensionCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window,&SDL_ExtensionCount,nullptr);

	std::vector<const char*> extensions;

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	const size_t additional_extension_count = extensions.size();
	extensions.resize(SDL_ExtensionCount+additional_extension_count);

	SDL_Vulkan_GetInstanceExtensions(window, &SDL_ExtensionCount, extensions.data() + additional_extension_count);

	return extensions;
}
