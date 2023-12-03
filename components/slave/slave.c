#include "slave.h"

#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Please consult the datasheet of your servo before changing the following parameters
#define SERVO_MIN_PULSEWIDTH_US 500   // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US 2500  // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE -90          // Minimum angle
#define SERVO_MAX_DEGREE 90           // Maximum angle

#define SERVO_PULSE_GPIO 0                    // GPIO connects to the PWM signal line
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000  // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD 20000           // 20000 ticks, 20ms
#define TAG "SLAVE"

static inline uint32_t example_angle_to_compare(int angle) {
  return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

static uint8_t i2c_buffer[5];
mcpwm_timer_handle_t timer = NULL;
mcpwm_timer_config_t timer_config = {
    .group_id = 0,
    .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
    .resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
    .period_ticks = SERVO_TIMEBASE_PERIOD,
    .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
};

mcpwm_oper_handle_t oper = NULL;
mcpwm_operator_config_t operator_config = {
    .group_id = 0,  // operator must be in the same group to the timer
};
mcpwm_cmpr_handle_t comparator = NULL;
mcpwm_comparator_config_t comparator_config = {
    .flags.update_cmp_on_tez = true,
};

mcpwm_gen_handle_t generator = NULL;
mcpwm_generator_config_t generator_config = {
    .gen_gpio_num = 11,
};
void execute() {
  // i2c_master_write_to_device(I2C_MASTER_NUM, SLAVE_ADDR, i2c_buffer, sizeof(i2c_buffer), pdMS_TO_TICKS(100));
  // vTaskDelay(pdMS_TO_TICKS(10));
}

void slave_reset() {
  i2c_buffer[0] = 0;
  execute();
  ESP_LOGI(TAG, "Create timer and operator");
  ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));
  ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &oper));

  ESP_LOGI(TAG, "Connect timer and operator");
  ESP_ERROR_CHECK(mcpwm_operator_connect_timer(oper, timer));

  ESP_LOGI(TAG, "Create comparator and generator from the operator");
  ESP_ERROR_CHECK(mcpwm_new_comparator(oper, &comparator_config, &comparator));

  ESP_ERROR_CHECK(mcpwm_new_generator(oper, &generator_config, &generator));

  // set the initial compare value, so that the servo will spin to the center position
  ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(0)));

  ESP_LOGI(TAG, "Set generator action on timer and compare event");
  // go high on counter empty
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_timer_event(generator,
                                                            MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)));
  // go low on compare threshold
  ESP_ERROR_CHECK(mcpwm_generator_set_action_on_compare_event(generator,
                                                              MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW)));

  ESP_LOGI(TAG, "Enable and start timer");
  ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
  ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

void rgb(uint8_t red, uint8_t green, uint8_t blue) {
  i2c_buffer[0] = 0x1;
  i2c_buffer[1] = red;
  i2c_buffer[2] = green;
  i2c_buffer[3] = blue;
  execute();
}

void buzzer(uint32_t duration) {
  i2c_buffer[0] = 0x2;
  i2c_buffer[1] = duration >> 24;
  i2c_buffer[2] = duration >> 16;
  i2c_buffer[3] = duration >> 8;
  i2c_buffer[4] = duration;
  execute();
}

void servo(uint8_t id, int16_t ang) {
  i2c_buffer[0] = 0x3;
  i2c_buffer[1] = id;
  i2c_buffer[2] = ang < 0 ? 1 : 0;
  i2c_buffer[3] = ang < 0 ? -ang : ang;
  execute();
  mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(ang));
}
