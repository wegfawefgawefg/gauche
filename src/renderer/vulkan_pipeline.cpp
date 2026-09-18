#include "shaders_spirv.hpp"
#include "vulkan_internal.hpp"
#include <cstddef>
namespace tr::vk {
bool init_pipeline(Context &g) {
    VkAttachmentDescription attachment{};
    attachment.format = VK_FORMAT_R8G8B8A8_UNORM;
    attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    VkAttachmentReference reference{0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL};
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &reference;
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    VkRenderPassCreateInfo pass{VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    pass.attachmentCount = 1;
    pass.pAttachments = &attachment;
    pass.subpassCount = 1;
    pass.pSubpasses = &subpass;
    pass.dependencyCount = 1;
    pass.pDependencies = &dependency;
    if (!check(vkCreateRenderPass(g.device, &pass, nullptr, &g.pass), "render pass"))
        return false;
    VkDescriptorSetLayoutBinding bindings[2]{};
    for (int i = 0; i < 2; ++i) {
        bindings[i].binding = static_cast<std::uint32_t>(i);
        bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[i].descriptorCount = 1;
        bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    VkDescriptorSetLayoutCreateInfo set{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    set.bindingCount = 2;
    set.pBindings = bindings;
    if (!check(vkCreateDescriptorSetLayout(g.device, &set, nullptr, &g.set_layout), "descriptor layout"))
        return false;
    VkPushConstantRange range{VK_SHADER_STAGE_VERTEX_BIT, 0, 16};
    VkPipelineLayoutCreateInfo layout{VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
    layout.setLayoutCount = 1;
    layout.pSetLayouts = &g.set_layout;
    layout.pushConstantRangeCount = 1;
    layout.pPushConstantRanges = &range;
    if (!check(vkCreatePipelineLayout(g.device, &layout, nullptr, &g.pipeline_layout),
               "pipeline layout"))
        return false;
    VkDescriptorPoolSize size{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 65536};
    VkDescriptorPoolCreateInfo pool{VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
    pool.maxSets = 32768;
    pool.poolSizeCount = 1;
    pool.pPoolSizes = &size;
    if (!check(vkCreateDescriptorPool(g.device, &pool, nullptr, &g.descriptors), "descriptor pool"))
        return false;
    VkSamplerCreateInfo sampler{VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};
    sampler.magFilter = sampler.minFilter = VK_FILTER_NEAREST;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler.addressModeU = sampler.addressModeV = sampler.addressModeW =
        VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.maxLod = 0;
    if (!check(vkCreateSampler(g.device, &sampler, nullptr, &g.nearest), "nearest sampler"))
        return false;
    sampler.magFilter = sampler.minFilter = VK_FILTER_LINEAR;
    return check(vkCreateSampler(g.device, &sampler, nullptr, &g.linear), "linear sampler");
}
VkPipeline pipeline(Context &g, SDL_BlendMode blend) {
    if (auto it = g.pipelines.find(blend); it != g.pipelines.end())
        return it->second;
    VkShaderModule vert = VK_NULL_HANDLE, frag = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo shader{VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};
    shader.codeSize = sizeof(sprite_vert);
    shader.pCode = sprite_vert;
    if (!check(vkCreateShaderModule(g.device, &shader, nullptr, &vert), "vertex shader"))
        return VK_NULL_HANDLE;
    shader.codeSize = sizeof(sprite_frag);
    shader.pCode = sprite_frag;
    if (!check(vkCreateShaderModule(g.device, &shader, nullptr, &frag), "fragment shader")) {
        vkDestroyShaderModule(g.device, vert, nullptr);
        return VK_NULL_HANDLE;
    }
    VkPipelineShaderStageCreateInfo stages[2]{};
    for (auto &stage : stages) {
        stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stage.pName = "main";
    }
    stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    stages[0].module = vert;
    stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    stages[1].module = frag;
    VkVertexInputBindingDescription binding{0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX};
    VkVertexInputAttributeDescription attributes[]{
        {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, x)},
        {1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, r)},
        {2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, u)},
        {3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, lu)}};
    VkPipelineVertexInputStateCreateInfo input{
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    input.vertexBindingDescriptionCount = 1;
    input.pVertexBindingDescriptions = &binding;
    input.vertexAttributeDescriptionCount = 4;
    input.pVertexAttributeDescriptions = attributes;
    VkPipelineInputAssemblyStateCreateInfo assembly{
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPipelineViewportStateCreateInfo viewport{VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewport.viewportCount = viewport.scissorCount = 1;
    VkPipelineRasterizationStateCreateInfo raster{
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    raster.polygonMode = VK_POLYGON_MODE_FILL;
    raster.cullMode = VK_CULL_MODE_NONE;
    raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
    raster.lineWidth = 1;
    VkPipelineMultisampleStateCreateInfo multisample{
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    VkPipelineColorBlendAttachmentState color{};
    color.colorWriteMask = 15;
    color.blendEnable = blend != SDL_BLENDMODE_NONE;
    color.colorBlendOp = color.alphaBlendOp = VK_BLEND_OP_ADD;
    color.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    color.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    color.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    color.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    if (blend == SDL_BLENDMODE_ADD || blend == SDL_BLENDMODE_ADD_PREMULTIPLIED) {
        color.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
        color.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    }
    if (blend == SDL_BLENDMODE_BLEND_PREMULTIPLIED || blend == SDL_BLENDMODE_ADD_PREMULTIPLIED)
        color.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    if (blend == SDL_BLENDMODE_MOD) {
        color.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        color.dstColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR;
        color.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    }
    if (blend == SDL_BLENDMODE_MUL) {
        color.srcColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR;
        color.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        color.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    }
    VkPipelineColorBlendStateCreateInfo blending{
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    blending.attachmentCount = 1;
    blending.pAttachments = &color;
    VkDynamicState dynamic_states[]{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
    VkPipelineDynamicStateCreateInfo dynamic{VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamic.dynamicStateCount = 2;
    dynamic.pDynamicStates = dynamic_states;
    VkGraphicsPipelineCreateInfo info{VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    info.stageCount = 2;
    info.pStages = stages;
    info.pVertexInputState = &input;
    info.pInputAssemblyState = &assembly;
    info.pViewportState = &viewport;
    info.pRasterizationState = &raster;
    info.pMultisampleState = &multisample;
    info.pColorBlendState = &blending;
    info.pDynamicState = &dynamic;
    info.layout = g.pipeline_layout;
    info.renderPass = g.pass;
    VkPipeline result = VK_NULL_HANDLE;
    const bool ok =
        check(vkCreateGraphicsPipelines(g.device, VK_NULL_HANDLE, 1, &info, nullptr, &result),
              "graphics pipeline");
    vkDestroyShaderModule(g.device, vert, nullptr);
    vkDestroyShaderModule(g.device, frag, nullptr);
    if (!ok)
        return VK_NULL_HANDLE;
    g.pipelines[blend] = result;
    return result;
}
VkDescriptorSet descriptors(Context &g, Texture *texture, Texture *light, SDL_ScaleMode filter) {
    const auto key = std::make_tuple(&img(texture), &img(light), static_cast<int>(filter));
    if (auto it = g.sets.find(key); it != g.sets.end())
        return it->second;
    VkDescriptorSetAllocateInfo alloc{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
    alloc.descriptorPool = g.descriptors;
    alloc.descriptorSetCount = 1;
    alloc.pSetLayouts = &g.set_layout;
    VkDescriptorSet set;
    if (!check(vkAllocateDescriptorSets(g.device, &alloc, &set), "allocate descriptors"))
        return VK_NULL_HANDLE;
    VkDescriptorImageInfo images[]{
        {filter == SDL_SCALEMODE_LINEAR ? g.linear : g.nearest, img(texture).view,
         VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {g.linear, img(light).view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}};
    VkWriteDescriptorSet writes[2]{};
    for (int i = 0; i < 2; ++i) {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = set;
        writes[i].dstBinding = static_cast<std::uint32_t>(i);
        writes[i].descriptorCount = 1;
        writes[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writes[i].pImageInfo = &images[i];
    }
    vkUpdateDescriptorSets(g.device, 2, writes, 0, nullptr);
    g.sets[key] = set;
    return set;
}
} // namespace tr::vk
