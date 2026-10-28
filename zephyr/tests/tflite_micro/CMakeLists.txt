# Copyright (c) 2026 Espressif Systems (Shanghai) Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.28.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(esp_nn_tflite_micro)

set(tflm_kernels_dir ${ZEPHYR_TFLITE_MICRO_MODULE_DIR}/tensorflow/lite/micro/kernels)

target_sources(app PRIVATE
  src/main.cpp
  ${tflm_kernels_dir}/conv_test_common.cc
  ${tflm_kernels_dir}/testdata/conv_test_data.cc
)

foreach(test add conv depthwise_conv fully_connected mul pooling softmax)
  target_sources(app PRIVATE ${tflm_kernels_dir}/${test}_test.cc)
  set_source_files_properties(${tflm_kernels_dir}/${test}_test.cc PROPERTIES
    COMPILE_DEFINITIONS main=tflm_${test}_test_main
  )
endforeach()
