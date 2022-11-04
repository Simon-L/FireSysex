# DISTRHO Plugin Framework (DPF)
# Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
# Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
# SPDX-License-Identifier: ISC

cmake_minimum_required(VERSION 3.7)

# base name
set(NAME imgui-demo)

# names for native and plugin
project(${NAME}-plugin)
project(${NAME}-native)

# plugin
add_subdirectory(dpf)

dpf_add_plugin(${NAME}-plugin
  TARGETS clap lv2 vst2 vst3 jack
  FILES_DSP
      src/PluginDSP.cpp
  FILES_UI
      src/CommonUI.cpp
      dpf-widgets/opengl/DearImGui.cpp)

target_compile_definitions("${NAME}-plugin" PUBLIC ISPLUGIN)
target_include_directories(${NAME}-plugin PUBLIC src)
target_include_directories(${NAME}-plugin PUBLIC dpf-widgets/generic)
target_include_directories(${NAME}-plugin PUBLIC dpf-widgets/opengl)

# native
add_executable("${NAME}-native")
target_compile_definitions("${NAME}-native" PUBLIC ISNATIVE)
target_sources("${NAME}-native" PRIVATE "src/CommonUI.cpp")
target_sources("${NAME}-native" PRIVATE "${PROJECT_SOURCE_DIR}/dpf-widgets/opengl/DearImGui.cpp")

target_include_directories(${NAME}-native PUBLIC src)
target_include_directories("${NAME}-native" PUBLIC "${DPF_ROOT_DIR}/distrho")
target_include_directories("${NAME}-native" PUBLIC "${DPF_ROOT_DIR}/dgl")
target_include_directories("${NAME}-native" PUBLIC "${PROJECT_SOURCE_DIR}/dpf-widgets/opengl")

target_link_libraries("${NAME}-native" PRIVATE dgl-opengl)
target_link_libraries("${NAME}-native" PUBLIC dgl-opengl-definitions dgl-system-libs-definitions)
