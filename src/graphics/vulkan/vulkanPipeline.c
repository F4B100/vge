//
// Created by fabio on 6/16/2025.
//

#include "vulkanPipeline.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

spirVCode *readSPIRVFile(char *filename) {
    FILE * file = fopen(filename, "rb");

    if (!file) {
        printf("File opening failed errno: %d | %s\n", errno, filename);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    uint64_t size = ftell(file);
    rewind(file);

    if (size % sizeof(uint32_t) != 0) {
        printf("Error: SPIR-V file size is not a multiple of 4 bytes!\n");
        fclose(file);
        return nullptr;
    }

    uint32_t *contents = malloc(size);
    if (!contents) {
        printf("Memory allocation failed");
        fclose(file);
        return nullptr;
    }

    if (fread(contents, size, 1, file) != 1) {
        printf("Error with fread on SpirvFile: %s\n", filename);
        fclose(file);
        free(contents);
        return nullptr;
    }

    fclose(file);

    if (contents[0] != SPIRV_MAGIC_CONSTANT) {
        printf("Error: File is not a valid SPIR-V module in %s\n", filename);
        free(contents);
        return nullptr;
    }

    spirVCode *code = malloc(sizeof(spirVCode));
    if (!code) {
        printf("Memory allocation failed");
        fclose(file);
        return nullptr;
    }
    code->code = contents;
    code->size = size;
    return code;
}

VkShaderModule createShaderModule(VkDevice device, spirVCode *code) {
    VkShaderModuleCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code->size,
        .pCode = code->code
    };
    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule)) {
        printf("failed to create shader module!\n");
    }
    return shaderModule;
}

void createRenderPass(VkDevice device, VkFormat swapChainImageFormat, VkRenderPass *toCreate) {
    VkAttachmentDescription colorAttachment = {
        .format = swapChainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef = {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass = {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef

    };

    VkSubpassDependency dependency = {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = 0,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    if (vkCreateRenderPass(device, &renderPassInfo, nullptr, toCreate) != VK_SUCCESS) {
        printf("failed to create render pass!");
    }
}

void createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout *toCreate) {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
    .binding = 0,
    .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    .descriptorCount = 1,
    .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    .pImmutableSamplers = nullptr
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
    .bindingCount = 1,
    .pBindings = &uboLayoutBinding,
    };

    if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutInfo, nullptr, toCreate) != VK_SUCCESS) {
        printf("failed to create descriptor set layout!");
    }
}

void createDescriptorPool(VkDevice device, VkDescriptorPool *toCreate) {
    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1
    };

    VkDescriptorPoolCreateInfo poolInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 1,
        .maxSets = 1,
        .pPoolSizes = &poolSize,
    };

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, toCreate) != VK_SUCCESS) {
        printf("failed to create descriptor pool!");
    }
}

VkVertexInputBindingDescription *getBindingDescription2D(uint32_t stride) {
    VkVertexInputBindingDescription *bindingDescription = malloc(sizeof(VkVertexInputBindingDescription));
    bindingDescription->binding = 0;
    bindingDescription->stride = stride;
    bindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}
VkVertexInputBindingDescription *getBindingDescription3D(uint32_t stride) {
    VkVertexInputBindingDescription *bindingDescription = malloc(sizeof(VkVertexInputBindingDescription));
    bindingDescription->binding = 0;
    bindingDescription->stride = stride;
    bindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

VkVertexInputAttributeDescription *getAttributeDescription2D() {

    VkVertexInputAttributeDescription *attributeDescription = malloc(sizeof(VkVertexInputAttributeDescription) * 3);

    attributeDescription[0].location = 0;
    attributeDescription[0].binding = 0;
    attributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[0].offset = 0;

    attributeDescription[1].location = 1;
    attributeDescription[1].binding = 0;
    attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[1].offset = sizeof(float) * 2;

    attributeDescription[2].location = 2;
    attributeDescription[2].binding = 0;
    attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[2].offset = sizeof(float) * 4;
    return attributeDescription;
}
VkVertexInputAttributeDescription *getAttributeDescription3D() {
    VkVertexInputAttributeDescription *attributeDescription = malloc(sizeof(VkVertexInputAttributeDescription) * 3);

    attributeDescription[0].location = 0;
    attributeDescription[0].binding = 0;
    attributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[0].offset = 0;

    attributeDescription[1].location = 1;
    attributeDescription[1].binding = 0;
    attributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription[1].offset = sizeof(float) * 3;

    attributeDescription[2].location = 2;
    attributeDescription[2].binding = 0;
    attributeDescription[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescription[2].offset = sizeof(float) * 5;
    return attributeDescription;
}

vgePipelineGraphics *createGraphicsPipeline(VgePipelineGraphicsCreateInfo *info) {
    vgePipelineGraphics * newPipeline= malloc(sizeof(vgePipelineGraphics));
    memset(newPipeline, 0, sizeof(vgePipelineGraphics));

    spirVCode *codeVert = readSPIRVFile(info->vertShaderPath);
    spirVCode *codeFrag = readSPIRVFile(info->fragShaderPath);

    VkShaderModule vertShaderModule = createShaderModule(
        info->device,
        codeVert
    );
    VkShaderModule fragShaderModule = createShaderModule(
        info->device,
        codeFrag
    );

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = {
        vertShaderStageInfo,
        fragShaderStageInfo
    };

    VkVertexInputBindingDescription *bindingDescriptor = getBindingDescription2D(28);

    VkVertexInputAttributeDescription *attributeDescriptor = getAttributeDescription2D();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = bindingDescriptor,
        .vertexAttributeDescriptionCount = 3,
        .pVertexAttributeDescriptions = attributeDescriptor
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float) info->viewportExtent.width,
        .height = (float) info->viewportExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = info->viewportExtent
    };

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamicStates
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterizer = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f
    };

    VkPipelineMultisampleStateCreateInfo multisampling = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE
    };

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    VkPipelineColorBlendStateCreateInfo colorBlending = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants[0] = 0.0f,
        .blendConstants[1] = 0.0f,
        .blendConstants[2] = 0.0f,
        .blendConstants[3] = 0.0f
    };

    VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;

	createDescriptorSetLayout(info->device, &descriptorSetLayout);
	createDescriptorPool(info->device, &descriptorPool);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &descriptorSetLayout,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr
    };

    if (vkCreatePipelineLayout(info->device, &pipelineLayoutInfo, nullptr, &newPipeline->pipelineLayout)) {
        printf("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = newPipeline->pipelineLayout,
        .renderPass = info->renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    if (vkCreateGraphicsPipelines(info->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline->pipeline) != VK_SUCCESS) {
        printf("failed to create graphics pipeline!\n");
    }

    newPipeline->fragShaderModule = vertShaderModule;
    newPipeline->vertShaderModule = fragShaderModule;
	newPipeline->descriptorSetLayout = descriptorSetLayout;
	newPipeline->descriptorPool = descriptorPool;


    free(codeVert->code);
    free(codeVert);
    free(codeFrag->code);
    free(codeFrag);
    free(bindingDescriptor);
    free(attributeDescriptor);
    return newPipeline;
}

void destroyGraphicsPipeline(VkDevice device, vgePipelineGraphics *pipeline) {
    vkDestroyShaderModule(device, pipeline->fragShaderModule, nullptr);
    vkDestroyShaderModule(device, pipeline->vertShaderModule, nullptr);

    vkDestroyPipelineLayout(device, pipeline->pipelineLayout, nullptr);
    vkDestroyPipeline(device, pipeline->pipeline, nullptr);
}