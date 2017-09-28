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


#include "application_sandbox/sample_application_framework/sample_application.h"
#include "inputs.h"
#include "support/entry/entry.h"
#include "vulkan_helpers/helper_functions.h"
#include "vulkan_helpers/vulkan_application.h"

uint32_t compute_shader[] =
#if USE_CL
#include "add_numbers.cl.spv"
#elif USE_SPVASM
#include "add_numbers.spvasm.spv"
#else
#include "add_numbers.comp.spv"
#endif
    ;

#if USE_CL
// clspv does not allow a kernel function to be named "main".
#define KERNEL_NAME "adder"
#define EXTENSIONS  {"VK_KHR_storage_buffer_storage_class","VK_KHR_variable_pointers"}
#elif USE_SPVASM
// Use a different entry point name to be sure I'm running
// the right example.
#define KERNEL_NAME "asmadder"
#define EXTENSIONS  {"VK_KHR_storage_buffer_storage_class", "VK_KHR_variable_pointers"}
#else
// GLSL requires the shader entry point to be named "main".
#define KERNEL_NAME "main"
#define EXTENSIONS  {}
#endif


enum {
  // We have use input buffers, and one output buffer.
  kNumStorageBuffers = 3
};

// This sample will create N storage buffers, and bind them to successive
// locations in a compute shader.
int main_entry(const entry::entry_data* data) {
  data->log->LogInfo("Application Startup");

  vulkan::VulkanApplication app(data->root_allocator, data->log.get(), data,
                                EXTENSIONS);
  vulkan::VkDevice& device = app.device();

  const uint32_t kOutputBuffer =
      kNumStorageBuffers - 1;  // Which buffer contains the "output" data
  containers::unique_ptr<vulkan::VulkanApplication::Buffer>
      storage_buffers[kNumStorageBuffers + 1]; // +1 for pod

  VkDescriptorSetLayoutBinding binding0{
      static_cast<uint32_t>(0),           // binding
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,  // descriptorType
      1,                                  // descriptorCount
      VK_SHADER_STAGE_COMPUTE_BIT,        // stageFlags
      nullptr,                            // pImmutableSamplers
  };
  VkDescriptorSetLayoutBinding binding1 = binding0;
  binding1.binding = 1;
  VkDescriptorSetLayoutBinding binding2 = binding0;
  binding2.binding = 2;
  VkDescriptorSetLayoutBinding binding3 = binding0;
  binding3.binding = 3;


  struct Pod { uint32_t m, n, o; };

  Pod pod_data{10,20,30};

  containers::vector<VkDescriptorBufferInfo> buffer_infos(data->root_allocator);
  buffer_infos.resize(kNumStorageBuffers + 1);
  // Both input and output buffers have 512 32-bit integers.
  const uint32_t kBufferElements = 512;
  const uint32_t kBufferSize = kBufferElements * sizeof(uint32_t);

  VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                             VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                             VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
  for (size_t i = 0; i < kNumStorageBuffers; ++i) {
    storage_buffers[i] =
        app.CreateAndBindDefaultExclusiveHostBuffer(kBufferSize, usage);
    buffer_infos[i] =
        VkDescriptorBufferInfo{*storage_buffers[i], 0, VK_WHOLE_SIZE};
  }
  // For Pod
  storage_buffers[3] =
      app.CreateAndBindDefaultExclusiveHostBuffer(sizeof(pod_data), usage);
  buffer_infos[3] =
      VkDescriptorBufferInfo{*storage_buffers[3], 0, sizeof(pod_data)};

  auto compute_descriptor_set = containers::make_unique<vulkan::DescriptorSet>(
      data->root_allocator,
      app.AllocateDescriptorSet({binding0, binding1, binding2, binding3}));

  const VkWriteDescriptorSet write_descriptor_set{
      VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,      // sType
      nullptr,                                     // pNext
      *compute_descriptor_set,                     // dstSet
      0,                                           // dstBinding
      0,                                           // dstArrayElement
      static_cast<uint32_t>(buffer_infos.size()),  // descriptorCount
      VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,           // descriptorType
      nullptr,                                     // pImageInfo
      buffer_infos.data(),                         // pBufferInfo
      nullptr,                                     // pTexelBufferView
  };
  device->vkUpdateDescriptorSets(device, 1, &write_descriptor_set, 0, nullptr);

  // Create pipeline
  auto compute_pipeline_layout =
      containers::make_unique<vulkan::PipelineLayout>(
          data->root_allocator,
          app.CreatePipelineLayout({{binding0, binding1, binding2, binding3}}));
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
    containers::vector<uint32_t> initial_buffer_values(data->root_allocator);
    initial_buffer_values.insert(initial_buffer_values.begin(), kBufferElements,
                                 1);
    for (size_t i = 0; i < kNumStorageBuffers; ++i) {
      app.FillHostVisibleBuffer(
          &*storage_buffers[i],
          reinterpret_cast<const char*>(initial_buffer_values.data()),
          initial_buffer_values.size() * sizeof(uint32_t), 
          0, &cmd_buf,
          VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
          VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
    }
#if 1
    // Fill Pod
    app.FillHostVisibleBuffer(
        &*storage_buffers[3], 

        reinterpret_cast<const char*>(&pod_data),
        sizeof(pod_data), 
        
        0, &cmd_buf,
        VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT);
#endif

    // Call dispatch
    cmd_buf->vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE,
                               *compute_pipeline);
    cmd_buf->vkCmdBindDescriptorSets(
        cmd_buf, VK_PIPELINE_BIND_POINT_COMPUTE, *compute_pipeline_layout, 0, 1,
        &compute_descriptor_set->raw_set(), 0, nullptr);
    cmd_buf->vkCmdDispatch(cmd_buf, kBufferElements / LOCAL_X_SIZE, 1, 1);
    LOG_ASSERT(==, data->log, VK_SUCCESS,
               app.EndAndSubmitCommandBufferAndWaitForQueueIdle(
                   &cmd_buf, &app.render_queue()));

    // Check the output values
    containers::vector<uint32_t> output = vulkan::GetHostVisibleBufferData(
        data->root_allocator, &*storage_buffers[kOutputBuffer]);
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
