idf_component_register(SRCS "wifi_esp32.c"
                      INCLUDE_DIRS "." "include"
		      INCLUDE_DIRS "../../../../managed_components/espressif__esp-now/src/espnow/include"
                      REQUIRES crazyflie platform config esp_wifi)