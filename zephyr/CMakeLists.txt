# Copyright (c) 2026 Espressif Systems (Shanghai) Co., Ltd.
# SPDX-License-Identifier: Apache-2.0

if(CONFIG_ESP_NN)

  set(ESP_NN_DIR ${ZEPHYR_CURRENT_MODULE_DIR})

  zephyr_library()

  zephyr_include_directories(${ESP_NN_DIR}/include)
  zephyr_library_include_directories(
    ${ESP_NN_DIR}/src/common
    ${ZEPHYR_HAL_ESPRESSIF_MODULE_DIR}/zephyr/common/include
  )

  zephyr_library_compile_options($<TARGET_PROPERTY:compiler,optimization_speed>)
  zephyr_library_compile_options(-Wno-unused-function)
  zephyr_library_compile_definitions(ESP_NN_SINGLE_CORE)
  zephyr_library_compile_definitions_ifdef(CONFIG_ESP_NN_SKIP_NUDGE SKIP_NUDGE)
  zephyr_compile_definitions_ifdef(CONFIG_ESP_NN_OPTIMIZED ESP_NN_OPTIMIZED)

  zephyr_library_sources(
    ${ESP_NN_DIR}/src/activation_functions/esp_nn_relu_ansi.c
    ${ESP_NN_DIR}/src/activation_functions/esp_nn_hard_swish_ansi.c
    ${ESP_NN_DIR}/src/common/esp_nn_mean_ansi.c
    ${ESP_NN_DIR}/src/common/esp_nn_multicore.c
    ${ESP_NN_DIR}/src/basic_math/esp_nn_add_ansi.c
    ${ESP_NN_DIR}/src/basic_math/esp_nn_mul_ansi.c
    ${ESP_NN_DIR}/src/basic_math/esp_nn_mul_mt.c
    ${ESP_NN_DIR}/src/convolution/esp_nn_conv_ansi.c
    ${ESP_NN_DIR}/src/convolution/esp_nn_conv_opt.c
    ${ESP_NN_DIR}/src/convolution/esp_nn_conv_mt_split.c
    ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_ansi.c
    ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_opt.c
    ${ESP_NN_DIR}/src/fully_connected/esp_nn_fully_connected_ansi.c
    ${ESP_NN_DIR}/src/softmax/esp_nn_softmax_ansi.c
    ${ESP_NN_DIR}/src/softmax/esp_nn_softmax_opt.c
    ${ESP_NN_DIR}/src/logistic/esp_nn_logistic_ansi.c
    ${ESP_NN_DIR}/src/pooling/esp_nn_avg_pool_ansi.c
    ${ESP_NN_DIR}/src/pooling/esp_nn_max_pool_ansi.c
  )

  if(CONFIG_SOC_SERIES_ESP32S3)
    zephyr_library_compile_options(-mlongcalls -fno-unroll-loops)
    zephyr_library_sources(
      ${ESP_NN_DIR}/src/common/esp_nn_common_functions_esp32s3.S
      ${ESP_NN_DIR}/src/common/esp_nn_dot_s8_esp32s3.S
      ${ESP_NN_DIR}/src/common/esp_nn_multiply_by_quantized_mult_esp32s3.S
      ${ESP_NN_DIR}/src/common/esp_nn_multiply_by_quantized_mult_ver1_esp32s3.S
      ${ESP_NN_DIR}/src/common/esp_nn_mean_s8_esp32s3.c
      ${ESP_NN_DIR}/src/activation_functions/esp_nn_relu_s8_esp32s3.S
      ${ESP_NN_DIR}/src/activation_functions/esp_nn_hard_swish_s8_esp32s3.c
      ${ESP_NN_DIR}/src/basic_math/esp_nn_add_s8_esp32s3.S
      ${ESP_NN_DIR}/src/basic_math/esp_nn_add_mt_esp32s3.c
      ${ESP_NN_DIR}/src/basic_math/esp_nn_mul_s8_esp32s3.S
      ${ESP_NN_DIR}/src/basic_math/esp_nn_mul_broadcast_s8_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_esp32s3.c
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s8_1x1_esp32s3.c
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s8_3x3_opt_esp32s3.c
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s16_mult8_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s8_mult8_1x1_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s16_mult4_1x1_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_conv_s8_filter_aligned_input_padded_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s8_esp32s3.c
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s8_kx1_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s8_mult1_3x3_padded_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult1_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult1_3x3_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult1_3x3_no_pad_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult8_3x3_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult4_esp32s3.S
      ${ESP_NN_DIR}/src/convolution/esp_nn_depthwise_conv_s16_mult8_esp32s3.S
      ${ESP_NN_DIR}/src/fully_connected/esp_nn_fully_connected_esp32s3.c
      ${ESP_NN_DIR}/src/fully_connected/esp_nn_fc_s8_mac16_esp32s3.S
      ${ESP_NN_DIR}/src/fully_connected/esp_nn_fully_connected_s8_esp32s3.S
      ${ESP_NN_DIR}/src/fully_connected/esp_nn_fully_connected_per_ch_s8_esp32s3.S
      ${ESP_NN_DIR}/src/pooling/esp_nn_max_pool_s8_esp32s3.S
      ${ESP_NN_DIR}/src/pooling/esp_nn_avg_pool_s8_esp32s3.c
      ${ESP_NN_DIR}/src/pooling/esp_nn_avg_pool_s8_esp32s3.S
      ${ESP_NN_DIR}/src/softmax/esp_nn_softmax_s8_esp32s3.c
    )
  endif()

  if(CONFIG_ESP_NN_OPTIMIZED AND NOT CONFIG_SOC_SERIES_ESP32S3)
    zephyr_compile_definitions(ESP_NN_GENERIC_OPT)
  endif()

endif()
