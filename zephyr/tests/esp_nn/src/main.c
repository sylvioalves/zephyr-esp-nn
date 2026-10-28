/*
 * Copyright (c) 2026 Espressif Systems (Shanghai) Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <esp_cpu.h>

#include <test_functions.h>
#include <test_utils.h>

static uint32_t start_c;
static uint32_t start_opt;

void profile_c_start(void)
{
	start_c = esp_cpu_get_cycle_count();
}

uint32_t profile_c_end(void)
{
	return esp_cpu_get_cycle_count() - start_c;
}

void profile_opt_start(void)
{
	start_opt = esp_cpu_get_cycle_count();
}

uint32_t profile_opt_end(void)
{
	return esp_cpu_get_cycle_count() - start_opt;
}

#define ESP_NN_TEST(name, fn)                                                                      \
	ZTEST(esp_nn, test_##name)                                                                 \
	{                                                                                          \
		uint32_t failed = esp_nn_test_failed;                                              \
                                                                                                   \
		fn();                                                                              \
		zassert_equal(esp_nn_test_failed, failed, "%u case(s) failed",                     \
			      esp_nn_test_failed - failed);                                        \
	}

ESP_NN_TEST(add_s8, esp_nn_add_elementwise_s8_test)
ESP_NN_TEST(mul_s8, esp_nn_mul_elementwise_s8_test)
ESP_NN_TEST(mul_broadcast_ch_s8, esp_nn_mul_broadcast_channel_s8_test)
ESP_NN_TEST(depthwise_conv_s8, esp_nn_depthwise_conv_s8_test)
ESP_NN_TEST(conv_s8, esp_nn_conv_s8_test)
ESP_NN_TEST(relu6_s8, esp_nn_relu6_s8_test)
ESP_NN_TEST(avg_pool_s8, esp_nn_avg_pool_s8_test)
ESP_NN_TEST(max_pool_s8, esp_nn_max_pool_s8_test)
ESP_NN_TEST(fc_s8, esp_nn_fully_connected_s8_test)
ESP_NN_TEST(fc_per_ch_s8, esp_nn_fully_connected_per_ch_s8_test)
ESP_NN_TEST(fc_per_ch_s8_batch, esp_nn_fully_connected_per_ch_s8_batch_test)
ESP_NN_TEST(fc_align_s8, esp_nn_fully_connected_align_s8_test)
ESP_NN_TEST(fc_perf, esp_nn_fully_connected_perf_test)
ESP_NN_TEST(softmax_s8, esp_nn_softmax_s8_test)
ESP_NN_TEST(hard_swish_s8, esp_nn_hard_swish_s8_test)
ESP_NN_TEST(mean_nhwc_s8, esp_nn_mean_nhwc_s8_test)

ZTEST_SUITE(esp_nn, NULL, NULL, NULL, NULL, NULL);
