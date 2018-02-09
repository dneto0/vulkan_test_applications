# Copyright 2017 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

add_shader_library(stripe_shaders
  SOURCES
    stripe.comp
    stripe.cl
    stripe.spvasm
    stripe_opt0.spvasm
    stripe_mem2reg.spvasm
    stripe_reg.spvasm
  SHADER_DEPS
    shader_library
)

add_vulkan_sample_application(stripe_glsl
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    stripe_shaders
)
set_target_properties(stripe_glsl PROPERTIES COMPILE_FLAGS "-DUSE_GLSL")

add_vulkan_sample_application(stripe_opt0
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    stripe_shaders
)
set_target_properties(stripe_opt0 PROPERTIES COMPILE_FLAGS "-DUSE_ASM -Dsrc_opt0")

add_vulkan_sample_application(stripe_mem2reg
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    stripe_shaders
)
set_target_properties(stripe_mem2reg PROPERTIES COMPILE_FLAGS "-DUSE_ASM -Dsrc_mem2reg")

add_vulkan_sample_application(stripe_asm
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    stripe_shaders
)
set_target_properties(stripe_asm PROPERTIES COMPILE_FLAGS "-DUSE_ASM -Dsrc_asm")

add_vulkan_sample_application(stripe_reg
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    stripe_shaders
)
set_target_properties(stripe_reg PROPERTIES COMPILE_FLAGS "-DUSE_ASM -Dsrc_reg")
