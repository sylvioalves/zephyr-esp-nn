/*
 * Copyright (c) 2026 Espressif Systems (Shanghai) Co., Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "tensorflow/lite/micro/testing/micro_test_v2.h"

int main(void)
{
	tflite::InitializeTest();

	return RUN_ALL_TESTS();
}
