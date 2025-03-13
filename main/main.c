#include <stdio.h>
#include "pms7003.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define PMS_UART_PORT UART_NUM_0  // Cổng UART sử dụng
#define PMS_PIN_TX (GPIO_NUM_1)  // Chân TX của ESP32
#define PMS_PIN_RX (GPIO_NUM_3)  // Chân RX của ESP32

void pms7003_task(void *pvParameters)
{
    uint32_t pm1_0, pm2_5, pm10;
    uart_config_t uart_config = UART_CONFIG_DEFAULT();

    // Cấu hình UART
    if (pms7003_initUart(&uart_config) != ESP_OK)
    {
        ESP_LOGE("PMS7003", "Failed to initialize UART");
        vTaskDelete(NULL);  // Dừng Task nếu có lỗi
    }

    // Đặt chân TX, RX cho UART
    uart_set_pin(PMS_UART_PORT, PMS_PIN_TX, PMS_PIN_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Kích hoạt Active Mode
    if (pms7003_activeMode() != ESP_OK)
    {
        ESP_LOGE("PMS7003", "Failed to set Active Mode");
        vTaskDelete(NULL);
    }

    while (1)
    {
        if (pms7003_readData(indoor, &pm1_0, &pm2_5, &pm10) == ESP_OK)
        {
            ESP_LOGI("PMS7003", "PM1.0: %lu µg/m³, PM2.5: %lu µg/m³, PM10: %lu µg/m³", pm1_0, pm2_5, pm10);
        }
        else
        {
            ESP_LOGW("PMS7003", "Failed to read data");
        }

        vTaskDelay(pdMS_TO_TICKS(5000)); // Đọc mỗi 5 giây ...
    }
}

void app_main(void)
{
    xTaskCreate(pms7003_task, "pms7003_task", 4096, NULL, 5, NULL);
}
