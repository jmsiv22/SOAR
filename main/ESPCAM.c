    #include <stdio.h>
    #include "esp_log.h"
    #include "esp_camera.h"
    #include "driver/i2c.h"
    #include "ESPWEB.h"
    #include "driver/gpio.h"

    #define TAG "CameraStream"

    #define SDA_PIN 4   // I2C SDA pin
    #define SCL_PIN 5   // I2C SCL pin

    void start_camera_stream(void) {
        printf("Initializing camera...\n");
        // Camera configuration
        camera_config_t config = {      
            .sccb_i2c_port = I2C_NUM_0,
            .pin_sccb_sda = SDA_PIN,
            .pin_sccb_scl = SCL_PIN,
            .pin_xclk = 15,
            .pin_pclk = 13,
            .pin_vsync = 6,
            .pin_href = 7,
            .pin_d0 = 11,
            .pin_d1 = 9,
            .pin_d2 = 8,
            .pin_d3 = 10,
            .pin_d4 = 12,
            .pin_d5 = 18,
            .pin_d6 = 17,
            .pin_d7 = 16,
            .pin_reset = -1,
            .pin_pwdn = 48,
            .xclk_freq_hz   = 2000000,     
            .fb_count = 1,  
            .fb_location = CAMERA_FB_IN_DRAM,   
            .grab_mode = CAMERA_GRAB_WHEN_EMPTY, //CAMERA_GRAB_LATEST
            .jpeg_quality = 16,
            .frame_size = FRAMESIZE_QVGA,
            .pixel_format = PIXFORMAT_JPEG
        };
            // Configure GPIO35 as output
        gpio_config_t io_conf = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << GPIO_NUM_35
        };
        gpio_config(&io_conf);

        // Set GPIO35 LOW
        gpio_set_level(GPIO_NUM_35, 0);
        // Initialize the camera
        
        printf("Free heap before I2C init: %lu bytes\n", (unsigned long)esp_get_free_heap_size());
        vTaskDelay(pdMS_TO_TICKS(1000));
        esp_err_t err = esp_camera_init(&config);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Camera init failed with error 0x%x", err);
            return;
        }
        ESP_LOGI(TAG, "Camera initialized successfully!");
        printf("Camera ready!!!\n");


        start_webserver();
    }

    void start_stream(void) {
        start_camera_stream();
    }
