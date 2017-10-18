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

add_shader_library(tiny_wrap_shaders
  SOURCES
    write_glsl.comp
    write_w.spvasm
    write_w0.spvasm
  SHADER_DEPS
    shader_library
)

add_vulkan_sample_application(tiny_wrap_glsl
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    tiny_wrap_shaders
)
set_target_properties(tiny_wrap_glsl PROPERTIES COMPILE_FLAGS "-DUSE_GLSL")

add_vulkan_sample_application(tiny_wrap_write_w
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    tiny_wrap_shaders
)
set_target_properties(tiny_wrap_write_w PROPERTIES COMPILE_FLAGS "-DUSE_ASM")

add_vulkan_sample_application(tiny_wrap_write_w0
  SOURCES main.cpp
  LIBS
    vulkan_helpers
  SHADERS
    tiny_wrap_shaders
)
set_target_properties(tiny_wrap_write_w0 PROPERTIES COMPILE_FLAGS "-DUSE_ASM0")
