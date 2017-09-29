// Copyright 2017 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <cstring>
#include "application_sandbox/sample_application_framework/sample_application.h"
#include "support/entry/entry.h"
#include "vulkan_helpers/helper_functions.h"
#include "vulkan_helpers/vulkan_application.h"

uint32_t compute_shader[] =

#if defined(USE_GLSL)
#include "wrap.comp.spv"
#define KERNEL_NAME "main"
#define EXTENSIONS  {}

#elif defined(USE_CALL_ASM)
#include "wrap_call.spvasm.spv"
#define KERNEL_NAME "write_call"
#define EXTENSIONS  {"VK_KHR_storage_buffer_storage_class"}

#elif defined(USE_INLINE_ASM)
#include "wrap_inline.spvasm.spv"
#define KERNEL_NAME "write_inline"
#define EXTENSIONS  {"VK_KHR_storage_buffer_storage_class"}
#else
#error "misconfigured"
#endif
    ;


// This sample will create N storage buffers, and bind them to successive
// locations in a compute shader.
int main_entry(const entry::entry_data* data) {
  data->log->LogInfo("Application Startup");

  vulkan::VulkanApplication app(data->root_allocator, data->log.get(), data,
                                EXTENSIONS);
  vulkan::VkDevice& device = app.device();

  // Use vec4 elements
  using ScalarType = float;
  const uint32_t kVecElemCount = 4;
  const ScalarType kInitValue = 7.0;
  const uint32_t kBufferElements = 4;
  const uint32_t kInvocationCount = 1;
  const uint32_t kBufferScalarCount = kBufferElements * kVecElemCount;
  const uint32_t kBufferSize = kBufferScalarCount * sizeof(ScalarType);

  VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

  //containers::unique_ptr<vulkan::VulkanApplication::Buffer> storage_buffer =
  auto storage_buffer =
      app.CreateAndBindDefaultExclusiveHostBuffer(kBufferSize, usage);

  VkDescriptorBufferInfo buffer_info{*(storage_buffer.get()), 0, VK_WHOLE_SIZE};

  VkDescriptorSetLayoutBinding binding0{
      0u,                                 // binding
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  // descriptorType
      1,                                  // descriptorCount
      VK_SHADER_STAGE_COMPUTE_BIT,        // stageFlags
      nullptr,                            // pImmutableSamplers
  };

  auto compute_descriptor_set = containers::make_unique<vulkan::DescriptorSet>(
      data->root_allocator, app.AllocateDescriptorSet({binding0}));

  const VkWriteDescriptorSet write_descriptor_set{
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,      // sType
      nullptr,                                     // pNext
      *compute_descriptor_set,                     // dstSet
      0,                                           // dstBinding
      0,                                           // dstArrayElement
      1,                                           // descriptorCount
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           // descriptorType
      nullptr,                                     // pImageInfo
      &buffer_info,                                // pBufferInfo
      nullptr,                                     // pTexelBufferView
  };
  device->vkUpdateDescriptorSets(device, 1, &write_descriptor_set, 0, nullptr);

  // Create pipeline
  auto compute_pipeline_layout =
      containers::make_unique<vulkan::PipelineLayout>(
          data->root_allocator,
          app.CreatePipelineLayout({{binding0}}));
  auto compute_pipeline =
      containers::make_unique<vulkan::VulkanComputePipeline>(
          data->root_allocator,
          app.CreateComputePipeline(
              compute_pipeline_layout.get(),
              VkShaderModuleCreateInfo{
                  VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0,
                  sizeof(compute_shader), compute_shader},
              KERNEL_NAME));
  {
    // 1. vkCmdDispatch
    auto cmd_buf = app.GetCommandBuffer();
    app.BeginCommandBuffer(&cmd_buf);

    // Set inital values for the in-buffer and clear the out-buffer
    containers::vector<ScalarType> initial_buffer_values(data->root_allocator);
    initial_buffer_values.insert(initial_buffer_values.begin(), kBufferScalarCount,
                                 kInitValue);
    app.FillHostVisibleBuffer(
        &*storage_buffer,
        reinterpret_cast<const char*>(initial_buffer_values.data()),
        initial_buffer_values.size() * sizeof(uint32_t), 0, &cmd_buf,
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);

    // Call dispatch
    cmd_buf->vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE,
                               *compute_pipeline);
    cmd_buf->vkCmdBindDescriptorSets(
        cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, *compute_pipeline_layout, 0, 1,
        &compute_descriptor_set->raw_set(), 0, nullptr);
    cmd_buf->vkCmdDispatch(cmd_buf, kInvocationCount, 1, 1);
    LOG_ASSERT(==, data->log, VK_SUCCESS,
               app.EndAndSubmitCommandBufferAndWaitForQueueIdle(
                   &cmd_buf, &app.render_queue()));

    // Check the output values
    containers::vector<uint32_t> output_raw = vulkan::GetHostVisibleBufferData(
        data->root_allocator, &*storage_buffer);
    containers::vector<ScalarType> output(output_raw.size(), data->root_allocator);
    std::memcpy(output.data(), output_raw.data(), output_raw.size() * sizeof(ScalarType));

    std::ostringstream str;
    str << "Output:";
    for (auto& v : output) {
      str << " " << v;
    }
    data->log->LogInfo(str.str());
  }

  data->log->LogInfo("Application Shutdown");
  return 0;
}
