idf_component_register(SRCS "main.c" "ESPCAM.c" "ESPWEB.c"
                       INCLUDE_DIRS "." 
		       REQUIRES nvs_flash esp32-camera platform esp_wifi espressif__esp-now esp_http_server esp_netif
                       LDFRAGMENTS linker_fragment.lf)
target_compile_options(${COMPONENT_LIB} PRIVATE -mfix-esp32-psram-cache-issue)