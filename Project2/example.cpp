#include  "example.h"

void Example::run()
{
	init_SDL();
	init_vk();
	start_command_buffers();
	create_semphores();
	mainLoop();
	destroy();
}

void Example::init_vk()
{
	instance = demo_base_.get_vulkan_instance(window);
	physicalDevice = demo_base_.get_physical_device(instance);
	surface = demo_base_.get_window_surface(window, instance);
	device = demo_base_.get_vulkan_device(surface);
	swapchain = demo_process_.getVulkanSwapChain(device, physicalDevice, surface);
	swapchain_images = device.getSwapchainImagesKHR(swapchain);
    for (int i=0;i<swapchain_images.size();i++)
    {
		swapchain_imageViews.push_back(demo_process_.get_image_view(device, swapchain_images[i]));
    }
	render_pass = demo_process_.get_render_pass(device);
	vk::PipelineLayoutCreateInfo layout_create_info =
	{
		
	};
	pipeline_layout = device.createPipelineLayout(layout_create_info);
	graphicsPipeline = demo_process_.get_graphics_pipeline(device, pipeline_layout, { "D:/vulkan/du/vert.spv","D:/vulkan/du/frag.spv" }, render_pass, nullptr);
	frameBuffers = demo_process_.get_framebuffers(device, render_pass, swapchain_imageViews);
	command_pool = demo_process_.get_command_pool(device, demo_base_);
	graphics_queue = device.getQueue(demo_base_.graphicsQueueIndex, 0);
	present_queue = device.getQueue(demo_base_.presentQueueIndex, 0);
}

void Example::init_SDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << SDL_GetError();
	}
	window = SDL_CreateWindow("vulkan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}

void Example::mainLoop()
{
	drawFrames();
	SDL_Event gevent;
	SDL_PollEvent(&gevent);
	while (gevent.type != SDL_QUIT)
	{
		SDL_PollEvent(&gevent);
		if (gevent.type == SDL_KEYDOWN)
		{
			if (gevent.key.keysym.sym == SDLK_ESCAPE)
			{
				gevent.type = SDL_QUIT;
			}
		}
		drawFrames();
	}
	device.waitIdle();
}

void Example::start_command_buffers()
{
	vk::CommandBufferAllocateInfo allocate_info =
	{
		command_pool,vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(frameBuffers.size())
	};

	command_buffers = device.allocateCommandBuffers(allocate_info);

	for (int i=0;i<command_buffers.size();i++)
	{
		vk::CommandBufferBeginInfo begin_info =
		{
			
		};
		vk::ClearColorValue clear_color;
		clear_color.setFloat32({ 0.1f,0.2f,0.3f,1.0f });
		vk::ClearValue clear_value;
		clear_value.setColor(clear_color);
		vk::RenderPassBeginInfo render_pass_begin_info =
		{
			render_pass,frameBuffers[i],{{0,0},{800,600}},1,&clear_value
		};

		command_buffers[i].begin(begin_info);
		command_buffers[i].beginRenderPass(render_pass_begin_info,vk::SubpassContents::eInline);

		command_buffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
		command_buffers[i].draw(3, 1, 0, 0);

		command_buffers[i].endRenderPass();
		command_buffers[i].end();
	}
}

void Example::create_semphores()
{
	vk::SemaphoreCreateInfo semaphore_create_info = {};
	image_semaphore = device.createSemaphore(semaphore_create_info);
	render_semaphore = device.createSemaphore(semaphore_create_info);
}

void Example::drawFrames()
{
	uint32_t image_index;
	device.acquireNextImageKHR(swapchain, UINT64_MAX, image_semaphore, nullptr, &image_index);

	vk::PipelineStageFlags stage_flags[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::SubmitInfo submit_info =
	{
		1,&image_semaphore,stage_flags,1,&command_buffers[image_index],1,&render_semaphore
	};

	graphics_queue.submit(1, &submit_info, nullptr);

	vk::PresentInfoKHR present_info =
	{
		1,&render_semaphore,1,&swapchain,&image_index
	};
	present_queue.presentKHR(&present_info);

	present_queue.waitIdle();
}

void Example::destroy()
{
	device.destroySemaphore(image_semaphore);
	device.destroySemaphore(render_semaphore);
	
	device.freeCommandBuffers(command_pool, command_buffers);
	device.destroyCommandPool(command_pool);
	for (int i=0;i<frameBuffers.size();i++)
	{
		device.destroyFramebuffer(frameBuffers[i]);
	}
	device.destroyPipeline(graphicsPipeline);
	device.destroyRenderPass(render_pass);
	device.destroyPipelineLayout(pipeline_layout);

	for (int i=0;i<swapchain_imageViews.size();i++)
	{
		device.destroyImageView(swapchain_imageViews[i]);
	}
	device.destroySwapchainKHR(swapchain);
	instance.destroySurfaceKHR(surface);
	device.destroy();
	instance.destroy();

	SDL_DestroyWindow(window);
	SDL_Quit();
}
