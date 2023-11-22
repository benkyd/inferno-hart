#include "pipeline.hpp"

#include "device.hpp"
#include "preview_renderer/shader.hpp"
#include "renderpass.hpp"
#include "swapchain.hpp"

#include <cstdint>

#include "yolo/yolo.hpp"

namespace inferno::graphics {

Pipeline* pipeline_create(GraphicsDevice* device)
{
    Pipeline* pipeline = new Pipeline();

    pipeline->Device = device;
    pipeline->Swap = swapchain_create(device, device->SurfaceSize);

    // std::vector<VkDynamicState> dynamicStates = {
    //     VK_DYNAMIC_STATE_VIEWPORT,
    //     VK_DYNAMIC_STATE_SCISSOR,
    // };
    //
    // pipeline->DynamicStateCreateInfo.sType
    //     = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    // pipeline->DynamicStateCreateInfo.dynamicStateCount = (uint32_t)dynamicStates.size();
    // pipeline->DynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    pipeline->VertexInputInfo.sType
        = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    pipeline->VertexInputInfo.vertexBindingDescriptionCount = 0;
    pipeline->VertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
    pipeline->VertexInputInfo.vertexAttributeDescriptionCount = 0;
    pipeline->VertexInputInfo.pVertexAttributeDescriptions
        = nullptr; // pipeline->Vptional

    pipeline->InputAssembly.sType
        = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipeline->InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipeline->InputAssembly.primitiveRestartEnable = VK_FALSE;

    // NOTE: This is where the viewport and scissor are set
    // In reality, these should be dynamic, so they can be changed

    VkViewport viewport = {};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)pipeline->Swap->Extent.width;
    viewport.height = (float)pipeline->Swap->Extent.height;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;
    yolo::debug("Viewport size: {}x{}", viewport.width, viewport.height);

    VkRect2D scissor = {};
    scissor.offset = { 0, 0 };
    scissor.extent = pipeline->Swap->Extent;

    pipeline->ViewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipeline->ViewportState.viewportCount = 1;
    pipeline->ViewportState.pViewports = &viewport;
    pipeline->ViewportState.scissorCount = 1;
    pipeline->ViewportState.pScissors = &scissor;

    pipeline->Rasterizer.sType
        = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipeline->Rasterizer.depthClampEnable = VK_FALSE; // NOTE: This is for shadow mapping
    pipeline->Rasterizer.rasterizerDiscardEnable = VK_FALSE;
    pipeline->Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    pipeline->Rasterizer.lineWidth = 1.f;
    pipeline->Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    pipeline->Rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    pipeline->Rasterizer.depthBiasEnable = VK_FALSE;
    pipeline->Rasterizer.depthBiasConstantFactor = 0.f; // Optional
    pipeline->Rasterizer.depthBiasClamp = 0.f; // Optional
    pipeline->Rasterizer.depthBiasSlopeFactor = 0.f; // Optional

    pipeline->Multisampling.sType
        = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipeline->Multisampling.sampleShadingEnable = VK_FALSE;
    pipeline->Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    pipeline->Multisampling.minSampleShading = 1.0f; // Optional
    pipeline->Multisampling.pSampleMask = nullptr; // Optional
    pipeline->Multisampling.alphaToCoverageEnable = VK_FALSE; // Optional

    pipeline->ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    // Alpha pipeline->Blending basically
    pipeline->ColorBlendAttachment.blendEnable = VK_TRUE;
    pipeline->ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    pipeline->ColorBlendAttachment.dstColorBlendFactor
        = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    pipeline->ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    pipeline->ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    pipeline->ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    pipeline->ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    pipeline->ColorBlending.sType
        = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipeline->ColorBlending.logicOpEnable = VK_FALSE;
    pipeline->ColorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    pipeline->ColorBlending.attachmentCount = 1;
    pipeline->ColorBlending.pAttachments = &pipeline->ColorBlendAttachment;
    pipeline->ColorBlending.blendConstants[0] = 0.0f; // Optional
    pipeline->ColorBlending.blendConstants[1] = 0.0f; // Optional
    pipeline->ColorBlending.blendConstants[2] = 0.0f; // Optional
    pipeline->ColorBlending.blendConstants[3] = 0.0f; // pipeline->Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(
            device->VulkanDevice, &pipelineLayoutInfo, nullptr, &pipeline->Layout)
        != VK_SUCCESS) {
        yolo::error("failed to create pipeline layout!");
        return nullptr;
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

void pipeline_configure_to_renderpass(
    Pipeline* pipeline, Shader* shader, RenderPass* renderpass)
{
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shader->ShaderStages;

    pipelineInfo.pVertexInputState = &pipeline->VertexInputInfo;
    pipelineInfo.pInputAssemblyState = &pipeline->InputAssembly;
    pipelineInfo.pViewportState = &pipeline->ViewportState;
    pipelineInfo.pRasterizationState = &pipeline->Rasterizer;
    pipelineInfo.pMultisampleState = &pipeline->Multisampling;
    pipelineInfo.pDepthStencilState = nullptr; // Optional
    pipelineInfo.pColorBlendState = &pipeline->ColorBlending;
    // pipelineInfo.pDynamicState = &pipeline->DynamicStateCreateInfo;

    pipelineInfo.layout = pipeline->Layout;
    pipelineInfo.renderPass = renderpass->RenderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(pipeline->Device->VulkanDevice, VK_NULL_HANDLE, 1,
            &pipelineInfo, nullptr, &pipeline->GraphicsPipeline)
        != VK_SUCCESS) {
        yolo::error("failed to create graphics pipeline!");
    }

    yolo::info("Created graphics pipeline");
}

} // namespace inferno::graphics
