// SPDX-License-Identifier: MIT

#define DT_DRV_COMPAT zmk_input_processor_deadzone

#include <stdlib.h>
#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/sys/util.h>
#include <drivers/input_processor.h>

struct deadzone_config {
    uint8_t type;
    uint16_t x_code;
    uint16_t y_code;
    int16_t deadzone;
};

static int deadzone_handle_event(const struct device *dev, struct input_event *event,
                                 uint32_t param1, uint32_t param2,
                                 struct zmk_input_processor_state *state) {
    ARG_UNUSED(param1);
    ARG_UNUSED(param2);
    ARG_UNUSED(state);

    const struct deadzone_config *cfg = dev->config;

    if (event->type != cfg->type) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    if (event->code != cfg->x_code && event->code != cfg->y_code) {
        return ZMK_INPUT_PROC_CONTINUE;
    }

    if (abs(event->value) <= cfg->deadzone) {
        event->value = 0;
    }

    return ZMK_INPUT_PROC_CONTINUE;
}

static const struct zmk_input_processor_driver_api deadzone_driver_api = {
    .handle_event = deadzone_handle_event,
};

#define DEADZONE_INST(n)                                                                           \
    static const struct deadzone_config deadzone_config_##n = {                                   \
        .type = DT_INST_PROP_OR(n, type, INPUT_EV_REL),                                           \
        .x_code = DT_INST_PROP_OR(n, x_code, INPUT_REL_HWHEEL),                                   \
        .y_code = DT_INST_PROP_OR(n, y_code, INPUT_REL_WHEEL),                                    \
        .deadzone = DT_INST_PROP_OR(n, deadzone, 1),                                              \
    };                                                                                             \
    DEVICE_DT_INST_DEFINE(n, NULL, NULL, NULL, &deadzone_config_##n, POST_KERNEL,                \
                          CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &deadzone_driver_api);

DT_INST_FOREACH_STATUS_OKAY(DEADZONE_INST)
