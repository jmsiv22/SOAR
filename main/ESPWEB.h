#ifndef ESP_WEB_H
#define ESP_WEB_H

#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_camera.h"

// Define the TAG for logging
#define TAG "ESPWeb"

// Function prototypes
esp_err_t stream_handler(httpd_req_t *req);
void start_webserver(void);

#endif // ESP_WEB_H
