#include "pipeline.hpp"

#include "device.hpp"
#include "shader.hpp"
#include "swapchain.hpp"

#include "scene/mesh.hpp"

#include <array>
#include <cstdint>

#include "yolo/yolo.hpp"

namespace inferno::graphics {

Pipeline* pipeline_create(GraphicsDevice* device, SwapChain* swap, Shader* shader,
    uint32_t descriptorSetLayoutCount, VkDescriptorSetLayout* layouts)
{
    Pipeline* pipeline = new Pipeline();

    // memset(descriptorSetLayouts, 0, sizeof(VkDescriptorSetLayout));
    // memset(&layout, 0, sizeof(VkPipelineLayout));
    // memset(&graphicsPipeline, 0, sizeof(VkPipeline));
    // memset(&dynamicStates, 0, sizeof(VkPipelineDynamicStateCreateInfo));
    // memset(&vertexInputInfo, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
    // memset(&inputAssembly, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
    // memset(&viewportState, 0, sizeof(VkPipelineViewportStateCreateInfo));
    // memset(&rasterizer, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
    // memset(&multisampling, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
    // memset(&colorBlendAttachment, 0, sizeof(VkPipelineColorBlendAttachmentState));
    // memset(&colorBlending, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
    // memset(&depthState, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));

    pipeline->Device = device;
    pipeline->Swap = swap;
    pipeline->RelaventShader = shader;

    pipeline->DescriptorSetLayoutCount = descriptorSetLayoutCount;
    pipeline->DescriptorSetLayouts = layouts;

    auto bindingDescription
        = new VkVertexInputBindingDescription(scene::get_vert_binding_description());
    auto attributeDescriptions = new std::array<VkVertexInputAttributeDescription, 2>(
        scene::get_vert_attribute_descriptions());

    VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
    vertexInputInfo.pNext = nullptr;
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount
        = static_cast<uint32_t>(attributeDescriptions->size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions->data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE; // NOTE: This is for shadow mapping
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // Alpha blending basically
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor
        = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // pipeline->Optional

    VkPipelineDepthStencilStateCreateInfo depthState {};
    depthState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthState.depthTestEnable = VK_TRUE;
    depthState.depthWriteEnable = VK_TRUE;
    depthState.depthCompareOp = VK_COMPARE_OP_LESS;
    depthState.depthBoundsTestEnable = VK_FALSE;
    depthState.minDepthBounds = 0.0f;
    depthState.maxDepthBounds = 1.0f;
    depthState.stencilTestEnable = VK_FALSE;
    depthState.front = {};
    depthState.back = {};

    VkPipelineDynamicStateCreateInfo dynamicStatesCreateInfo {};
    std::vector<VkDynamicState> dynamicStates
        = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    dynamicStatesCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStatesCreateInfo.dynamicStateCount
        = static_cast<uint32_t>(dynamicStates.size());
    dynamicStatesCreateInfo.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = descriptorSetLayoutCount;
    pipelineLayoutInfo.pSetLayouts = layouts;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional

    if (vkCreatePipelineLayout(
            device->VulkanDevice, &pipelineLayoutInfo, nullptr, &pipeline->Layout)
        != VK_SUCCESS) {
        yolo::error("failed to create pipeline layout!");
        return nullptr;
    }

    VkPipelineRenderingCreateInfo renderingPipelineInfo = {};
    renderingPipelineInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    renderingPipelineInfo.colorAttachmentCount = 1;
    renderingPipelineInfo.pColorAttachmentFormats = &swap->ImageFormat;
    renderingPipelineInfo.depthAttachmentFormat = find_depth_format(device);
    renderingPipelineInfo.pNext = nullptr;

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = SHADER_STAGES;
    pipelineInfo.pStages = shader->ShaderStages;
    pipelineInfo.pNext = &renderingPipelineInfo;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicStatesCreateInfo;
    pipelineInfo.pDepthStencilState = &depthState;
    pipelineInfo.pTessellationState = VK_NULL_HANDLE; // Optional
    pipelineInfo.layout = pipeline->Layout;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional

    // yolo::debug("All Binding Description: {} stride: {}",
    //     vertexInputInfo.pVertexBindingDescriptions->binding,
    //     vertexInputInfo.pVertexBindingDescriptions->stride);
    // yolo::debug("All Attribute Description0: {} location: {} format: {} offset: {}",
    //     vertexInputInfo.pVertexAttributeDescriptions[0].binding,
    //     vertexInputInfo.pVertexAttributeDescriptions[0].location,
    //     vertexInputInfo.pVertexAttributeDescriptions[0].format,
    //     vertexInputInfo.pVertexAttributeDescriptions[0].offset);
    // yolo::debug("All Attribute Description1: {} location: {} format: {} offset: {}",
    //     vertexInputInfo.pVertexAttributeDescriptions[1].binding,
    //     vertexInputInfo.pVertexAttributeDescriptions[1].location,
    //     vertexInputInfo.pVertexAttributeDescriptions[1].format,
    //     vertexInputInfo.pVertexAttributeDescriptions[1].offset);

    if (vkCreateGraphicsPipelines(device->VulkanDevice, VK_NULL_HANDLE, 1, &pipelineInfo,
            nullptr, &pipeline->GraphicsPipeline)
        != VK_SUCCESS) {
        yolo::error("failed to create graphics pipeline!");
    }

    return pipeline;
}

void pipeline_cleanup(Pipeline* pipeline)
{
    vkDestroyPipeline(
        pipeline->Device->VulkanDevice, pipeline->GraphicsPipeline, nullptr);
    vkDestroyPipelineLayout(pipeline->Device->VulkanDevice, pipeline->Layout, nullptr);
    delete pipeline;
}

void pipeline_recreate(Pipeline* pipeline)
{
    vkDeviceWaitIdle(pipeline->Device->VulkanDevice);
    GraphicsDevice* device = pipeline->Device;
    SwapChain* swap = pipeline->Swap;

    swapchain_recreate(swap);

    Shader* shader = pipeline->RelaventShader;
    uint32_t descriptorSetLayoutCount = descriptorSetLayoutCount;
    VkDescriptorSetLayout* layouts = pipeline->DescriptorSetLayouts;

    pipeline_cleanup(pipeline);
    pipeline = pipeline_create(device, swap, shader, descriptorSetLayoutCount, layouts);
}

} // namespace inferno::graphics
