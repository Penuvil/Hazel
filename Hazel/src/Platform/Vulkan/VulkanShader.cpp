#include "hzpch.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanBuffer.h"



namespace Hazel {

	VulkanShader::VulkanShader(const std::string & filepath, const BufferLayout& vertexBufferLayout)
	{
		std::string source = ReadFile(filepath);
		std::unordered_map<shaderc_shader_kind, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			HZ_CORE_ASSERT(type == "vertex" || type == "fragment", "Invalid shader type specified");

			size_t nextLinePos = source.find_first_not_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			if (type == "vertex") {
				shaderSources[shaderc_vertex_shader] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
			}
			else
			{
				shaderSources[shaderc_fragment_shader] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));

			}
		}
		CreateGraphicsPipeline(shaderSources[shaderc_vertex_shader], shaderSources[shaderc_fragment_shader], vertexBufferLayout);

		auto lastSlash = filepath.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	VulkanShader::VulkanShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSource, const BufferLayout& vertexBufferLayout)
		:m_Name(name)
	{
		CreateGraphicsPipeline(vertexSrc, fragmentSource, vertexBufferLayout);
	}

	VulkanShader::~VulkanShader()
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		vkDestroyPipeline(*device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(*device, m_PipelineLayout, nullptr);
//		vkDestroyDescriptorSetLayout(*device, m_DescriptorSetLayout, nullptr);
	}

	VkShaderModule VulkanShader::CreateShaderModule(std::vector<uint32_t>& code)
	{
		VkResult result;
		VkShaderModule shaderModule;

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = NULL;
		shaderModuleCreateInfo.flags = 0;
		shaderModuleCreateInfo.codeSize = sizeof(code[0]) * code.size() ;
		shaderModuleCreateInfo.pCode = code.data();

		result = vkCreateShaderModule(*VulkanContext::GetContext()->GetDevice(), &shaderModuleCreateInfo, nullptr, &shaderModule);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create shader module! " + result);
		
		return shaderModule;
	}

	std::vector<uint32_t> VulkanShader::Compile(const std::string& shaderSource, shaderc_shader_kind shaderType)
	{
		shaderc::Compiler shaderCompiler;
		shaderc::CompileOptions compileOptions;
		shaderc::SpvCompilationResult spvCompilationResult;
		
		spvCompilationResult = shaderCompiler.CompileGlslToSpv(shaderSource, shaderType, "Shader", "main", compileOptions);
		HZ_CORE_ASSERT(spvCompilationResult.GetCompilationStatus() == shaderc_compilation_status_success, "Shader compilaton failed! " + spvCompilationResult.GetErrorMessage());
		std::vector<uint32_t> buffer;		
		buffer.reserve(std::distance(spvCompilationResult.cbegin(), spvCompilationResult.cend()) + 1);
		for (auto iter = spvCompilationResult.cbegin(); iter < spvCompilationResult.cend(); iter++)
		{
			buffer.push_back(*iter);
		}
		return buffer;
	}

	void VulkanShader::CreateGraphicsPipeline(const std::string & vertexSrc, const std::string & fragmentSrc, const BufferLayout& vertexBufferLayout)
	{
		VkDevice* device = VulkanContext::GetContext()->GetDevice();
		std::vector<uint32_t> vertexShaderCode = Compile(vertexSrc, shaderc_vertex_shader);
		std::vector<uint32_t> fragmentShaderCode = Compile(fragmentSrc, shaderc_fragment_shader);

		VkShaderModule vertexShaderModule = CreateShaderModule(vertexShaderCode);
		VkShaderModule fragmentShaderModule = CreateShaderModule(fragmentShaderCode);

		VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {};
		vertexShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderStageCreateInfo.pNext = NULL;
		vertexShaderStageCreateInfo.flags = 0;
		vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderStageCreateInfo.module = vertexShaderModule;
		vertexShaderStageCreateInfo.pName = "main";
		vertexShaderStageCreateInfo.pSpecializationInfo = NULL;

		VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {};
		fragmentShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragmentShaderStageCreateInfo.pNext = NULL;
		fragmentShaderStageCreateInfo.flags = 0;
		fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragmentShaderStageCreateInfo.module = fragmentShaderModule;
		fragmentShaderStageCreateInfo.pName = "main";
		fragmentShaderStageCreateInfo.pSpecializationInfo = NULL;

		VkPipelineShaderStageCreateInfo shaderStageCreateInfos[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

		VkVertexInputBindingDescription vertexInputBindingDescription = {};
		vertexInputBindingDescription.binding = 0;
		vertexInputBindingDescription.stride = vertexBufferLayout.GetStride();
		vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		uint32_t attributeCount = vertexBufferLayout.GetElements().size();
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		vertexInputAttributeDescriptions.resize(attributeCount);
		
		for (uint32_t i = 0; i < attributeCount; i++)
		{
			vertexInputAttributeDescriptions[i].location = i;
			vertexInputAttributeDescriptions[i].binding = 0;
			switch (vertexBufferLayout.GetElements()[i].GetComponentCount())
			{
			case 1:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32_SFLOAT;
				break;
			case 2:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32G32_SFLOAT;
				break;
			case 3:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case 4:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			case 9:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32G32B32_SFLOAT;
				break;
			case 16:
				vertexInputAttributeDescriptions[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
				break;
			}
			vertexInputAttributeDescriptions[i].offset = vertexBufferLayout.GetElements()[i].Offset;
		}
/*		vertexInputAttributeDescriptions[1].location = 1;
		vertexInputAttributeDescriptions[1].binding = 0;
		switch (vertexBufferLayout.GetElements()[1].GetComponentCount())
		{
		case 1:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32_SFLOAT;
			break;
		case 2:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case 3:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case 4:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		case 9:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case 16:
			vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		}
		vertexInputAttributeDescriptions[1].offset = vertexBufferLayout.GetElements()[1].Offset;
*/
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pNext = NULL;
		vertexInputStateCreateInfo.flags = 0;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.pNext = NULL;
		inputAssemblyStateCreateInfo.flags = 0;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

		const VkExtent2D* swapChainExtent = VulkanContext::GetContext()->GetSwapChain()->GetExtent2D();
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<uint32_t>(swapChainExtent->width);
		viewport.height = static_cast<uint32_t>(swapChainExtent->height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = *swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pNext = NULL;
		viewportStateCreateInfo.flags = 0;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.pNext = NULL;
		rasterizationStateCreateInfo.flags = 0;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.0f;
		rasterizationStateCreateInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.pNext = NULL;
		multisampleStateCreateInfo.flags = 0;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		multisampleStateCreateInfo.minSampleShading = 1.0f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
		colorBlendAttachmentState.blendEnable = VK_TRUE;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.pNext = NULL;
		colorBlendStateCreateInfo.flags = 0;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[1];
		colorBlendStateCreateInfo.blendConstants[2];
		colorBlendStateCreateInfo.blendConstants[3];
		colorBlendStateCreateInfo.blendConstants[4];

		VkDynamicState dynamicStates[] = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.pNext = NULL;
		dynamicStateCreateInfo.flags = 0;
		dynamicStateCreateInfo.dynamicStateCount = 2;
		dynamicStateCreateInfo.pDynamicStates = dynamicStates;

		VkResult result;

		std::vector<VkDescriptorSetLayout>* descriptorSetLayouts = VulkanContext::GetContext()->GetSwapChain()->GetDescriptorSetLayouts();

		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutCreateInfo.pNext = NULL;
		layoutCreateInfo.flags = 0;
		layoutCreateInfo.setLayoutCount = (uint32_t) descriptorSetLayouts->size();
		layoutCreateInfo.pSetLayouts = descriptorSetLayouts->data();
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.pPushConstantRanges = nullptr;

		result = vkCreatePipelineLayout(*device, &layoutCreateInfo, nullptr, &m_PipelineLayout);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed tp create pipeline layout! " + result);

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.pNext = NULL;
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStageCreateInfos;
		pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		pipelineCreateInfo.pTessellationState = nullptr;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr;
		pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = m_PipelineLayout;
		pipelineCreateInfo.renderPass = VulkanContext::GetContext()->GetSwapChain()->GetRenderPass("NoClear");
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		result = vkCreateGraphicsPipelines(*device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_GraphicsPipeline);
		HZ_CORE_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline! " + result);

		vkDestroyShaderModule(*device, vertexShaderModule, nullptr);
		vkDestroyShaderModule(*device, fragmentShaderModule, nullptr);
	}

	void VulkanShader::Bind()
	{
		std::vector<VkCommandBuffer>* commandBuffers = VulkanContext::GetContext()->GetSwapChain()->GetCommandBuffers();
		vkCmdBindPipeline(commandBuffers->at(VulkanRendererAPI::GetFrame()->imageIndex), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		VulkanRendererAPI::SetBatchShader(shared_from_this());
	}

	void VulkanShader::Unbind() const
	{
	}

/*	void VulkanShader::SetUniformBuffers(const Ref<std::unordered_map<std::string, Ref<UniformBuffer>>> buffers)
	{
		m_UniformBuffers = buffers;
	}

	Ref<UniformBuffer> VulkanShader::GetUniformBuffer(const std::string & name) const
	{
		HZ_CORE_ASSERT(m_UniformBuffers->find(name) != m_UniformBuffers->end(), "Uniform buffer not found! {0}", name);
		return m_UniformBuffers->find(name)->second;
	}
*/
}