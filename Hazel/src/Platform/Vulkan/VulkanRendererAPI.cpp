#include "hzpch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiAPI.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"


namespace Hazel {

	Ref<VulkanRendererAPI::FrameInfo> VulkanRendererAPI::s_CurrentFrame = nullptr;

	void VulkanRendererAPI::Init()
	{
		VkResult result;
		VkDevice* device = VulkanContext::GetContext()->GetDevice();

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
	}

	void VulkanRendererAPI::BeginScene()
	{
		uint32_t imageIndex;
		VkResult result;
		VkClearValue clearColor = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, m_ClearColor.a };
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();
		
		vkAcquireNextImageKHR(*vulkanContext->GetDevice(), *vulkanSwapChain->GetSwapChain(), UINT64_MAX, m_ImageAvailableSemaphores[m_FrameIndex], VK_NULL_HANDLE, &imageIndex);

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
		renderPassBeginInfo.renderPass = *vulkanSwapChain->GetRenderPass();
		renderPassBeginInfo.framebuffer = vulkanSwapChain->GetFramebuffers()->at(imageIndex);
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = *vulkanSwapChain->GetExtent2D();
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers->at(imageIndex), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
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

	void VulkanRendererAPI::EndScene()
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
		submitInfo.pSignalSemaphores = signalSemiphores;

		result = vkQueueSubmit(*vulkanContext->GetGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_FrameIndex]);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit command queue!");

		m_FrameIndex = (m_FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4 & color)
	{
		m_ClearColor = color;
	}

	void VulkanRendererAPI::Clear()
	{
	}

	void VulkanRendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4 & transform, const glm::mat4 & viewProjection)
	{
		VulkanContext* vulkanContext = VulkanContext::GetContext();
		Ref<VulkanSwapChain> vulkanSwapChain = vulkanContext->GetSwapChain();
		std::vector<VkCommandBuffer>* commandBuffers = vulkanContext->GetSwapChain()->GetCommandBuffers();
		std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers = vertexArray->GetVertexBuffers();
		Ref<VulkanUniformBuffer> uniformBuffer = std::static_pointer_cast<VulkanUniformBuffer>(shader->GetUniformBuffer("Matrices"));
		struct UBO
		{
			glm::mat4 transform;
			glm::mat4 viewProjection;
		} ubo = { transform, viewProjection };
		void* data;
		vkMapMemory(*vulkanContext->GetDevice(), *uniformBuffer->GetBufferMemory(), *uniformBuffer->GetBufferSize() * s_CurrentFrame->imageIndex, *uniformBuffer->GetBufferSize(), 0, &data);
		memcpy(data, &ubo, *uniformBuffer->GetBufferSize());
		vkUnmapMemory(*vulkanContext->GetDevice(), *uniformBuffer->GetBufferMemory());
		
		vkCmdBindPipeline(commandBuffers->at(s_CurrentFrame->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, *std::static_pointer_cast<VulkanShader>(shader)->GetGraphicsPipeline());
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers->at(s_CurrentFrame->imageIndex), 0, 1, std::static_pointer_cast<VulkanVertexBuffer>(vertexArray->GetVertexBuffers().at(0))->GetBuffer(), offsets);
		vkCmdBindIndexBuffer(commandBuffers->at(s_CurrentFrame->imageIndex), *std::static_pointer_cast<VulkanIndexBuffer>(vertexArray->GetIndexBuffer())->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		const VkDescriptorSet descriptorSet = std::static_pointer_cast<VulkanUniformBuffer>(shader->GetUniformBuffer("Matrices"))->GetDescriptorSets()->at(s_CurrentFrame->imageIndex);
		vkCmdBindDescriptorSets(commandBuffers->at(s_CurrentFrame->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, *std::static_pointer_cast<VulkanShader>(shader)->GetGraphicsPipelineLayout(),
			0, 1, &descriptorSet, 0, nullptr);

		vkCmdDrawIndexed(commandBuffers->at(s_CurrentFrame->imageIndex), vertexArray->GetIndexBuffer()->GetCount(), 1, 0, 0, 0);
	}

	void VulkanRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray)
	{
	}

}