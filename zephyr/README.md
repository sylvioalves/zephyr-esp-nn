# ESP-NN for Zephyr

This directory makes ESP-NN a Zephyr module. It builds the ESP-NN library
of neural network kernels and, when TensorFlow Lite Micro is enabled,
replaces the tflite-micro reference add, conv, depthwise conv, fully
connected, mul, pooling and softmax kernels with kernels built on ESP-NN.

## Getting the module

The module follows Zephyr `main`.

### New workspace

Create a `west.yml` in an empty manifest directory, for example
`my-workspace/manifest/west.yml`:

```yaml
manifest:
  remotes:
    - name: zephyrproject-rtos
      url-base: https://github.com/zephyrproject-rtos
    - name: sylvioalves
      url-base: https://github.com/sylvioalves
  projects:
    - name: zephyr
      remote: zephyrproject-rtos
      revision: main
      import:
        name-allowlist:
          - hal_espressif
          - tflite-micro
    - name: zephyr-esp-nn
      remote: sylvioalves
      revision: main
      path: modules/lib/esp-nn
```

Then initialize and update the workspace:

```
cd my-workspace
west init -l manifest
west config manifest.project-filter -- +tflite-micro
west update
west blobs fetch hal_espressif
```

TensorFlow Lite Micro is an optional Zephyr module, so `west update` only
fetches it with the project filter above. The library itself works
without it.

### Existing workspace

Add the module with a submanifest, for example
`zephyr/submanifests/esp-nn.yaml`:

```yaml
manifest:
  projects:
    - name: zephyr-esp-nn
      url: https://github.com/sylvioalves/zephyr-esp-nn
      revision: main
      path: modules/lib/esp-nn
```

Then run `west config manifest.project-filter -- +tflite-micro` if
tflite-micro is not in the workspace yet, and `west update`.

Use a commit hash instead of `main` as `revision` for reproducible builds.

## Configuration

| Option                               | Default | Description                                         |
|--------------------------------------|---------|-----------------------------------------------------|
| `CONFIG_ESP_NN`                      | n       | Build the ESP-NN library                            |
| `CONFIG_ESP_NN_OPTIMIZED`            | y       | Kernels optimized for the selected SoC              |
| `CONFIG_ESP_NN_ANSI_C`               | n       | Portable reference kernels, for verification        |
| `CONFIG_ESP_NN_SKIP_NUDGE`           | n       | Faster requantization, may differ by one bit        |
| `CONFIG_ESP_NN_TFLITE_MICRO_KERNELS` | y       | Replace the tflite-micro kernels with ESP-NN ones   |

`CONFIG_ESP_NN` selects `CONFIG_REQUIRES_FULL_LIBC`.

## Using it with TensorFlow Lite Micro

Enable both libraries in `prj.conf`:

```
CONFIG_TENSORFLOW_LITE_MICRO=y
CONFIG_ESP_NN=y
```

The ESP-NN kernels replace the tflite-micro reference kernels at build
time. No change to the application code is needed, and the operators
without an ESP-NN kernel keep the reference implementation.

For example, the Zephyr tflite-micro Hello World sample, whose int8 model
runs on the ESP-NN fully connected kernel:

```
west build -b esp32s3_eye/esp32s3/procpu zephyr/samples/modules/tflite-micro/hello_world \
  -- -DCONFIG_ESP_NN=y
west flash
```

The ESP-NN tflite-micro kernels follow the tflite-micro revision of Zephyr
`main`. The build fails with an `ESP-NN: replaced N of 7 tflite-micro
kernels` error if the tflite-micro kernel sources do not match the
expected ones.

## Using the library directly

The library headers are on the include path of the application. `esp_nn.h`
maps each kernel to the implementation selected in Kconfig. For example, a
fully connected int8 layer:

```c
#include <esp_nn.h>

esp_nn_fully_connected_s8(input, input_offset, row_len, filter, filter_offset,
			  bias, output, out_channels, out_offset, out_shift, out_mult,
			  activation_min, activation_max);
```

The convolution, depthwise convolution and softmax kernels need a scratch
buffer, sized with the matching `esp_nn_get_*_scratch_size()` function and
set with `esp_nn_set_*_scratch_buf()` before the first call:

```c
static uint8_t scratch[SCRATCH_SIZE] __aligned(16);

if (esp_nn_get_conv_scratch_size(&input_dims, &filter_dims, &output_dims,
				 &conv_params) > sizeof(scratch)) {
	return -ENOMEM;
}

esp_nn_set_conv_scratch_buf(scratch);
esp_nn_conv_s8(&input_dims, input, &filter_dims, filter, bias, &output_dims,
	       output, &conv_params, &quant_data);
```

The kernel declarations are in `include/esp_nn_ansi_headers.h`.

## SoC support

| SoC                  | Kernels               | Tested on hardware |
|----------------------|-----------------------|--------------------|
| ESP32-S3             | 128-bit SIMD assembly | yes                |
| other Espressif SoCs | portable C            | build only         |

The ESP32-S3 kernels use SIMD registers that Zephyr does not save on a
context switch, so only one thread may run them.

## Tests

| Test                                       | What it checks                                         |
|--------------------------------------------|--------------------------------------------------------|
| `libraries.esp_nn`                         | Optimized kernels against their reference              |
| `libraries.esp_nn.ansi`                    | Portable kernels against their reference               |
| `libraries.esp_nn.tflite_micro`            | tflite-micro kernel unit tests with the ESP-NN kernels |
| `libraries.esp_nn.tflite_micro.reference`  | The same unit tests with the reference kernels         |

Build the tests without hardware:

```
west twister --build-only -T modules/lib/esp-nn/zephyr/tests
```

Run them on a board, here an ESP32-S3-EYE, with a hardware map:

```yaml
- connected: true
  id: /dev/ttyACM0
  platform: esp32s3_eye/esp32s3/procpu
  product: USB JTAG/serial debug unit
  runner: esp32
  serial: /dev/ttyACM0
  baud: 115200
```

```
west twister -p esp32s3_eye/esp32s3/procpu --device-testing \
  --hardware-map map.yaml --flash-before -T modules/lib/esp-nn/zephyr/tests
```

A board whose console is on a UART, such as the ESP32-S3-DevKitC, uses
the UART port in `id` and `serial`. To use its USB-Serial/JTAG port
instead, add `-x EXTRA_DTC_OVERLAY_FILE=<overlay>` with an overlay that
sets `zephyr,console` and `zephyr,shell-uart` to `&usb_serial`.

## Performance on the ESP32-S3

Person detection model (int8, 96x96 grayscale input), one inference:

|                       | Reference kernels | ESP-NN        | Gain   |
|-----------------------|------------------:|--------------:|-------:|
| 10 stored images      | 4103 ms           | 49.7 ms       | 82x    |
| Live ESP32-S3-EYE camera | 4106.2 ms      | 58.0 ms       | 70.8x  |
| Camera to display     | 0.23 fps          | 3.90 fps      | 17x    |
| Scores                |                   | bit-identical |        |
| Tensor arena          | 82.7 KB           | 121.4 KB      | +38.7 KB |

Time per operator on the live camera:

| Operator          | Reference | ESP-NN   | Gain  |
|-------------------|----------:|---------:|------:|
| CONV_2D           | 3476.9 ms | 35.5 ms  | 98x   |
| DEPTHWISE_CONV_2D | 628.2 ms  | 21.7 ms  | 29x   |
| AVERAGE_POOL_2D   | 0.92 ms   | 0.13 ms  | 7x    |
| SOFTMAX           | 0.13 ms   | 0.50 ms  | 0.26x |

`CONFIG_ESP_NN_SKIP_NUDGE` brings the inference down to 48.0 ms, with
scores that may differ from the reference by one least significant bit.

Image size of the tflite-micro kernel tests:

|                   | Reference kernels | ESP-NN    | Difference |
|-------------------|------------------:|----------:|-----------:|
| Flash image       | 302,996 B         | 369,140 B | +66,144 B  |
| DRAM              | 68,264 B          | 68,296 B  | +32 B      |
| Unit tests passed | 110/110           | 110/110   |            |
