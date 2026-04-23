// SPDX-License-Identifier: MIT

#define DT_DRV_COMPAT zmk_input_processor_dominant_axis

#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <drivers/input_processor.h>

struct dominant_axis_config {
    uint8_t type;
    uint16_t x_code;
    uint16_t y_code;
    int16_t deadzone;
    uint8_t ratio_num;
    uint8_t ratio_den;
};

struct dominant_axis_data {
    int16_t last_x_abs;
    int16_t last_y_abs;
};

static int dominant_axis_handle_event(const struct device *dev, struct input_event *event,
                                      uint32_t param1, uint32_t param2,
                                      struct zmk_input_processor_state *state) {
    ARG_UNUSED(param1);
    ARG_UNUSED(param2);
    ARG_UNUSED(state);

    const struct dominant_axis_config *cfg = dev->config;
    struct dominant_axis_data *data = dev->data;
    const int32_t ratio_num = (cfg->ratio_num > 0) ? cfg->ratio_num : 1;
    const int32_t ratio_den = (cfg->ratio_den > 0) ? cfg->ratio_den : 1;

    if (event->type != cfg->type) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    if (event->code != cfg->x_code && event->code != cfg->y_code) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    int16_t value_abs = (int16_t)abs(event->value);
    if (value_abs <= cfg->deadzone) {
        event->value = 0;
        return ZMK_INPUT_PROC_CONTINUE;
    }

    if (event->code == cfg->x_code) {
        data->last_x_abs = value_abs;

        if ((int32_t)data->last_x_abs * ratio_den < (int32_t)data->last_y_abs * ratio_num) {
            event->value = 0;
        }
    } else {
        data->last_y_abs = value_abs;

        if ((int32_t)data->last_y_abs * ratio_den < (int32_t)data->last_x_abs * ratio_num) {
            event->value = 0;
        }
    }

    return ZMK_INPUT_PROC_CONTINUE;
}

static const struct zmk_input_processor_driver_api dominant_axis_driver_api = {
    .handle_event = dominant_axis_handle_event,
};

#define DOMINANT_AXIS_INST(n)                                                                      \
    static struct dominant_axis_data dominant_axis_data_##n = {0};                                \
    static const struct dominant_axis_config dominant_axis_config_##n = {                          \
        .type = DT_INST_PROP_OR(n, type, INPUT_EV_REL),                                            \
        .x_code = DT_INST_PROP_OR(n, x_code, INPUT_REL_HWHEEL),                                    \
        .y_code = DT_INST_PROP_OR(n, y_code, INPUT_REL_WHEEL),                                     \
        .deadzone = DT_INST_PROP_OR(n, deadzone, 1),                                               \
        .ratio_num = DT_INST_PROP_OR(n, dominance_ratio_num, 1),                                   \
        .ratio_den = DT_INST_PROP_OR(n, dominance_ratio_den, 1),                                   \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, &dominant_axis_data_##n, &dominant_axis_config_##n,     \
                          POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                        \
                          &dominant_axis_driver_api);

DT_INST_FOREACH_STATUS_OKAY(DOMINANT_AXIS_INST)
