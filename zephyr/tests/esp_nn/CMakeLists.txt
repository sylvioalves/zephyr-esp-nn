# Copyright (c) 2026 Espressif Systems (Shanghai) Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp_nn_kernels)

file(GLOB esp_nn_tests ${ZEPHYR_ESP_NN_MODULE_DIR}/tests/src/*.c)
set_source_files_properties(src/main.c ${esp_nn_tests} PROPERTIES
  COMPILE_OPTIONS "-Wno-unused-function;-Wno-pointer-arith"
)

target_sources(app PRIVATE src/main.c ${esp_nn_tests})
target_include_directories(app PRIVATE
  ${ZEPHYR_ESP_NN_MODULE_DIR}/tests/include
  ${ZEPHYR_ESP_NN_MODULE_DIR}/src/common
)
