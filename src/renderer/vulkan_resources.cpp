#include "vulkan_internal.hpp"
#include <algorithm>
#include <cstring>
namespace tr::vk {
bool check(VkResult result,const char* operation){return result==VK_SUCCESS || SDL_SetError("Vulkan %s: %d",operation,static_cast<int>(result));}
std::uint32_t memory_type(Context& g,std::uint32_t mask,VkMemoryPropertyFlags flags){VkPhysicalDeviceMemoryProperties p;vkGetPhysicalDeviceMemoryProperties(g.physical,&p);for(std::uint32_t i=0;i<p.memoryTypeCount;++i)if((mask&(1U<<i))&&(p.memoryTypes[i].propertyFlags&flags)==flags)return i;return UINT32_MAX;}
bool wait(Context& g){if(!g.pending)return true;if(!check(vkWaitForFences(g.device,1,&g.fence,VK_TRUE,UINT64_MAX),"wait fence"))return false;g.pending=false;return true;}
bool begin(Context& g){
 if(!wait(g))return false;
 if(!check(vkResetCommandPool(g.device,g.pool,0),"reset commands"))return false;
 if(g.descriptors){vkResetDescriptorPool(g.device,g.descriptors,0);g.sets.clear();}
 VkCommandBufferBeginInfo info{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};info.flags=VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;return check(vkBeginCommandBuffer(g.command,&info),"begin commands");
}
bool finish(Context& g,VkSemaphore signal,VkSemaphore waits){
 if(!check(vkEndCommandBuffer(g.command),"end commands"))return false;
 VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};submit.commandBufferCount=1;submit.pCommandBuffers=&g.command;
 const VkPipelineStageFlags stage=VK_PIPELINE_STAGE_TRANSFER_BIT;
 if(waits){submit.waitSemaphoreCount=1;submit.pWaitSemaphores=&waits;submit.pWaitDstStageMask=&stage;}
 if(signal){submit.signalSemaphoreCount=1;submit.pSignalSemaphores=&signal;}
 vkResetFences(g.device,1,&g.fence);
 if(!check(vkQueueSubmit(g.queue,1,&submit,g.fence),"submit"))return false;
 g.pending=true;return true;
}
void destroy_buffer(Context& g,Buffer& b){if(b.mapped)vkUnmapMemory(g.device,b.memory);if(b.buffer)vkDestroyBuffer(g.device,b.buffer,nullptr);if(b.memory)vkFreeMemory(g.device,b.memory,nullptr);b={};}
bool buffer(Context& g,Buffer& b,VkDeviceSize size,VkBufferUsageFlags usage){
 if(b.capacity>=size)return true;destroy_buffer(g,b);b.capacity=std::max<VkDeviceSize>(size,65536);
 VkBufferCreateInfo info{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};info.size=b.capacity;info.usage=usage;info.sharingMode=VK_SHARING_MODE_EXCLUSIVE;if(!check(vkCreateBuffer(g.device,&info,nullptr,&b.buffer),"create buffer"))return false;
 VkMemoryRequirements req;vkGetBufferMemoryRequirements(g.device,b.buffer,&req);VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};alloc.allocationSize=req.size;alloc.memoryTypeIndex=memory_type(g,req.memoryTypeBits,VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);if(alloc.memoryTypeIndex==UINT32_MAX)return SDL_SetError("No host-visible coherent Vulkan memory");
 return check(vkAllocateMemory(g.device,&alloc,nullptr,&b.memory),"buffer memory")&&check(vkBindBufferMemory(g.device,b.buffer,b.memory,0),"bind buffer")&&check(vkMapMemory(g.device,b.memory,0,VK_WHOLE_SIZE,0,&b.mapped),"map buffer");
}
void transition(Context& g,Image& image,VkImageLayout layout){
 if(image.layout==layout)return;
 VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};barrier.oldLayout=image.layout;barrier.newLayout=layout;barrier.srcQueueFamilyIndex=barrier.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED;barrier.image=image.image;barrier.subresourceRange={VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};
 auto access=[](VkImageLayout l)->VkAccessFlags{switch(l){case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT|VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:return VK_ACCESS_SHADER_READ_BIT;case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:return VK_ACCESS_TRANSFER_READ_BIT;case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:return VK_ACCESS_TRANSFER_WRITE_BIT;default:return 0;}};
 barrier.srcAccessMask=access(image.layout);barrier.dstAccessMask=access(layout);
 vkCmdPipelineBarrier(g.command,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,0,0,nullptr,0,nullptr,1,&barrier);image.layout=layout;
}
bool create(Texture* t){
 auto& g=ctx(t->owner);auto* image=new Image;t->gpu=image;
 VkImageCreateInfo info{VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};info.imageType=VK_IMAGE_TYPE_2D;info.format=VK_FORMAT_R8G8B8A8_UNORM;info.extent={static_cast<std::uint32_t>(t->w),static_cast<std::uint32_t>(t->h),1};info.mipLevels=info.arrayLayers=1;info.samples=VK_SAMPLE_COUNT_1_BIT;info.tiling=VK_IMAGE_TILING_OPTIMAL;info.usage=VK_IMAGE_USAGE_SAMPLED_BIT|VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT|VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
 if(!check(vkCreateImage(g.device,&info,nullptr,&image->image),"create image"))return false;
 VkMemoryRequirements req;vkGetImageMemoryRequirements(g.device,image->image,&req);VkMemoryAllocateInfo alloc{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};alloc.allocationSize=req.size;alloc.memoryTypeIndex=memory_type(g,req.memoryTypeBits,VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
 if(!check(vkAllocateMemory(g.device,&alloc,nullptr,&image->memory),"image memory")||!check(vkBindImageMemory(g.device,image->image,image->memory,0),"bind image"))return false;
 VkImageViewCreateInfo view{VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};view.image=image->image;view.viewType=VK_IMAGE_VIEW_TYPE_2D;view.format=info.format;view.subresourceRange={VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};if(!check(vkCreateImageView(g.device,&view,nullptr,&image->view),"image view"))return false;
 if(t->target){VkFramebufferCreateInfo frame{VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO};frame.renderPass=g.pass;frame.attachmentCount=1;frame.pAttachments=&image->view;frame.width=info.extent.width;frame.height=info.extent.height;frame.layers=1;if(!check(vkCreateFramebuffer(g.device,&frame,nullptr,&image->framebuffer),"framebuffer"))return false;}
 return true;
}
void destroy(Texture* t){auto& g=ctx(t->owner);auto* image=static_cast<Image*>(t->gpu);if(!image)return;wait(g);if(image->framebuffer)vkDestroyFramebuffer(g.device,image->framebuffer,nullptr);if(image->view)vkDestroyImageView(g.device,image->view,nullptr);if(image->image)vkDestroyImage(g.device,image->image,nullptr);if(image->memory)vkFreeMemory(g.device,image->memory,nullptr);delete image;t->gpu=nullptr;}
bool upload(Texture* t,const SDL_Rect& area,const void* pixels,int pitch){
 auto& g=ctx(t->owner);if(!begin(g))return false;Buffer staging;
 if(!buffer(g,staging,static_cast<VkDeviceSize>(area.w)*area.h*4,VK_BUFFER_USAGE_TRANSFER_SRC_BIT))return false;
 for(int y=0;y<area.h;++y)std::memcpy(static_cast<char*>(staging.mapped)+y*area.w*4,static_cast<const char*>(pixels)+y*pitch,static_cast<std::size_t>(area.w)*4);
 transition(g,img(t),VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);VkBufferImageCopy copy{};copy.imageSubresource={VK_IMAGE_ASPECT_COLOR_BIT,0,0,1};copy.imageOffset={area.x,area.y,0};copy.imageExtent={static_cast<std::uint32_t>(area.w),static_cast<std::uint32_t>(area.h),1};vkCmdCopyBufferToImage(g.command,staging.buffer,img(t).image,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1,&copy);transition(g,img(t),VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
 const bool ok=finish(g)&&wait(g);destroy_buffer(g,staging);return ok;
}
bool resize_screen(Renderer* r){auto& g=ctx(r);int w=0,h=0;SDL_GetWindowSizeInPixels(r->window,&w,&h);w=std::max(w,1);h=std::max(h,1);if(g.screen.gpu&&g.screen.w==w&&g.screen.h==h)return true;destroy(&g.screen);g.screen.owner=r;g.screen.w=w;g.screen.h=h;g.screen.target=true;return create(&g.screen);}
}
