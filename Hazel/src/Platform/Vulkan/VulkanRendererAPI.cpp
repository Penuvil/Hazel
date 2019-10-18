#include "hzpch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiAPI.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanVertexArray.h"
#include "Platform/Vulkan/VulkanTexture.h"
#include "Hazel/Renderer/OrthographicCamera.h"


namespace Hazel {

	Ref<VulkanRendererAPI::FrameInfo> VulkanRendererAPI::s_CurrentFrame = nullptr;

	void VulkanRendererAPI::Init()
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		const VkExtent2D* swapChainExtent = VulkanContext::GetContext()->GetSwapChain()->GetExtent2D();

		s_CurrentFrame.reset(new FrameInfo);

		m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_LayerCompleteSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = NULL;
		semaphoreCreateInfo.flags = 0;

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = NULL;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			result = vkCreateSemaphore(*device, &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create semaphore!");
			result = vkCreateSemaphore(*device, &semaphoreCreateInfo, nullptr, &m_LayerCompleteSemaphores[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create semaphore!");
			result = vkCreateSemaphore(*device, &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create semaphore!");
			result = vkCreateFence(*device, &fenceCreateInfo, nullptr, &m_InFlightFences[i]);
			HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Frnce!");
		}

		SetViewport(0, swapChainExtent->height, swapChainExtent->width,  swapChainExtent->height);
		m_ResizeEvent = false;
	}

	void VulkanRendererAPI::BeginScene()
	{
		uint32_t imageIndex;
		VkResult result;
		VkClearValue clearColor = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();
		
		result = vkAcquireNextImageKHR(*vulkanContext->GetDevice(), *vulkanSwapChain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_FrameIndex], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_ResizeEvent == true)
		{
			vulkanSwapChain->RecreateSwapChain();
			m_ResizeEvent = false;
			result = vkAcquireNextImageKHR(*vulkanContext->GetDevice(), *vulkanSwapChain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_FrameIndex], VK_NULL_HANDLE, &imageIndex);
		}


		s_CurrentFrame->imageIndex = imageIndex;
		s_CurrentFrame->frameIndex = m_FrameIndex;
		s_CurrentFrame->clearColor = &clearColor;
		s_CurrentFrame->imageAvailableSemaphore = &m_ImageAvailableSemaphores[m_FrameIndex];
		s_CurrentFrame->layerCompleteSemaphore = &m_LayerCompleteSemaphores[m_FrameIndex];
		s_CurrentFrame->renderFinishedSemaphore = &m_RenderFinishedSemaphores[m_FrameIndex];
		s_CurrentFrame->inFlightFence = &m_InFlightFences[m_FrameIndex];

		vkWaitForFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex], VK_TRUE, UINT64_MAX);
		vkResetFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex]);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = NULL;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(commandBuffers->at(imageIndex), &commandBufferBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! " + result);

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = vulkanSwapChain->GetRenderPass("Clear");
		renderPassBeginInfo.framebuffer = vulkanSwapChain->GetFramebuffers()->at(imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = *vulkanSwapChain->GetExtent2D();
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers->at(imageIndex), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		const VkExtent2D* swapChainExtent = vulkanSwapChain->GetExtent2D();

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = *swapChainExtent;

		vkCmdSetViewport(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, &m_Viewport);
		vkCmdSetScissor(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, &scissor);
		
		vkCmdEndRenderPass(commandBuffers->at(s_CurrentFrame->imageIndex));

		result = vkEndCommandBuffer(commandBuffers->at(s_CurrentFrame->imageIndex));
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to en command buffer! " + result);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_FrameIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemiphores[] = { m_LayerCompleteSemaphores[m_FrameIndex] };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers->at(s_CurrentFrame->imageIndex);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = waitSemaphores;

		result = vkQueueSubmit(*vulkanContext->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command queue!");
	}

	void VulkanRendererAPI::EndScene()
	{
		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();

		vkWaitForFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex], VK_TRUE, UINT64_MAX);
		vkResetFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex]);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = NULL;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(commandBuffers->at(s_CurrentFrame->imageIndex), &commandBufferBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! " + result);

//		vkCmdEndRenderPass(commandBuffers->at(s_CurrentFrame->imageIndex));

		result = vkEndCommandBuffer(commandBuffers->at(s_CurrentFrame->imageIndex));
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to en command buffer! " + result);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_FrameIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemiphores[] = { m_LayerCompleteSemaphores[m_FrameIndex] };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers->at(s_CurrentFrame->imageIndex);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemiphores;

		result = vkQueueSubmit(*vulkanContext->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command queue!");

		m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		m_Viewport.x = (float)x;
		m_Viewport.y = (float)height;
		m_Viewport.width = (float)width;
		m_Viewport.height = -((float)height);
		m_Viewport.minDepth = 0.0f;
		m_Viewport.maxDepth = 1.0f;

		m_ResizeEvent = true;
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4 & color)
	{
		m_ClearColor = color;
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, uint32_t instanceId, const glm::vec4& fragColor, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers = vertexArray->GetVertexBuffers();
		Ref<VulkanUniformBuffer> matricesBuffer = std::static_pointer_cast<VulkanUniformBuffer>(vertexArray->GetUniformBuffer(instanceId, "Matrices"));
		Ref<VulkanUniformBuffer> colorBuffer = std::static_pointer_cast<VulkanUniformBuffer>(vertexArray->GetUniformBuffer(instanceId, "Color"));
		struct Matrices
		{
			glm::mat4 viewProjection;
			glm::mat4 transform;
		} matrices = { viewProjection, transform };
		void* data;
		vkMapMemory(*vulkanContext->GetDevice(), *matricesBuffer->GetBufferMemory(), *matricesBuffer->GetBufferSize() * s_CurrentFrame->imageIndex, *matricesBuffer->GetBufferSize(), 0, &data);
		memcpy(data, &matrices, *matricesBuffer->GetBufferSize());
		vkUnmapMemory(*vulkanContext->GetDevice(), *matricesBuffer->GetBufferMemory());

		struct Color
		{
			glm::vec4 color;
		} color = { fragColor };
		vkMapMemory(*vulkanContext->GetDevice(), *colorBuffer->GetBufferMemory(), *colorBuffer->GetBufferSize() * s_CurrentFrame->imageIndex, *colorBuffer->GetBufferSize(), 0, &data);
		memcpy(data, &color, *colorBuffer->GetBufferSize());
		vkUnmapMemory(*vulkanContext->GetDevice(), *colorBuffer->GetBufferMemory());


		
		vkCmdBindPipeline(commandBuffers->at(s_CurrentFrame->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, *std::static_pointer_cast<VulkanShader>(shader)->GetGraphicsPipeline());
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, std::static_pointer_cast<VulkanVertexBuffer>(vertexArray->GetVertexBuffers().at(0))->GetBuffer(), offsets);
		vkCmdBindIndexBuffer(commandBuffers->at(s_CurrentFrame->imageIndex), *std::static_pointer_cast<VulkanIndexBuffer>(vertexArray->GetIndexBuffer())->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		const std::vector<VkDescriptorSet>* descriptorSets = std::static_pointer_cast<VulkanVertexArray>(vertexArray)->GetDescriptorSets(instanceId);
		vkCmdBindDescriptorSets(commandBuffers->at(s_CurrentFrame->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, *std::static_pointer_cast<VulkanShader>(shader)->GetGraphicsPipelineLayout(),
			0, 1, &descriptorSets->at(s_CurrentFrame->imageIndex), 0, nullptr);

		vkCmdDrawIndexed(commandBuffers->at(s_CurrentFrame->imageIndex), vertexArray->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, Ref<Texture2D> texture, uint32_t instanceId, const glm::vec4 & fragColor, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		std::static_pointer_cast<VulkanTexture2D>(texture)->Bind(std::static_pointer_cast<VulkanVertexArray>(vertexArray)->GetDescriptorSet(instanceId, s_CurrentFrame->imageIndex));
		Submit(shader, vertexArray, instanceId, fragColor, transform, viewProjection);
	}

	void VulkanRendererAPI::BeginRender()
	{
		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();

		vkWaitForFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex], VK_TRUE, UINT64_MAX);
		vkResetFences(*vulkanContext->GetDevice(), 1, &m_InFlightFences[m_FrameIndex]);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.pNext = NULL;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		result = vkBeginCommandBuffer(commandBuffers->at(s_CurrentFrame->imageIndex), &commandBufferBeginInfo);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to begin command buffer! " + result);

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = NULL;
		renderPassBeginInfo.renderPass = vulkanSwapChain->GetRenderPass("NoClear");
		renderPassBeginInfo.framebuffer = vulkanSwapChain->GetFramebuffers()->at(s_CurrentFrame->imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = *vulkanSwapChain->GetExtent2D();
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = s_CurrentFrame->clearColor;

		vkCmdBeginRenderPass(commandBuffers->at(s_CurrentFrame->imageIndex), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		const VkExtent2D* swapChainExtent = vulkanSwapChain->GetExtent2D();

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)swapChainExtent->height;
		viewport.width = (float)swapChainExtent->width;
		viewport.height = -((float)swapChainExtent->height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = *swapChainExtent;

		vkCmdSetViewport(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, &scissor);
	}

	void VulkanRendererAPI::EndRender()
	{
		VkResult result;
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();

		vkCmdEndRenderPass(commandBuffers->at(s_CurrentFrame->imageIndex));

		result = vkEndCommandBuffer(commandBuffers->at(s_CurrentFrame->imageIndex));
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to en command buffer! " + result);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_FrameIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemiphores[] = { m_LayerCompleteSemaphores[m_FrameIndex] };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = NULL;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers->at(s_CurrentFrame->imageIndex);
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = waitSemaphores;

		result = vkQueueSubmit(*vulkanContext->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command queue!");
	}

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
	}

}