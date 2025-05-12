#include "esp_http_server.h"
#include "esp_camera.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/ip4_addr.h"
#include "esp_task_wdt.h"
#define TAG "ESPWeb"
#include "esp_system.h"

// Global variables for frame buffer
static camera_fb_t *latest_frame = NULL;

// Capture frame task - this captures frames at regular intervals
static void capture_frame_task(void *pvParameters) {
    
    while (true) {
        size_t free_heap_size = esp_get_free_heap_size();
        //printf("Free heap size: %d bytes\n", free_heap_size);
       // printf("Stack size before frame: %d\n", uxTaskGetStackHighWaterMark(NULL));
        // Wait for a short delay before capturing the next frame (adjust as needed)
        //vTaskDelay(pdMS_TO_TICKS(1000));  // Capture every 1 second to give the camera enough time
        // Capture a frame
        camera_fb_t *fb = esp_camera_fb_get();
        vTaskDelay(pdMS_TO_TICKS(100));
        //printf(" After: %d\n", uxTaskGetStackHighWaterMark(NULL));

        if (fb) {   
            // If a previous frame exists, release it
            if (latest_frame) {
                // Frame buffer is not freed immediately here; it's freed after being sent.
                esp_camera_fb_return(latest_frame);
                latest_frame = NULL;
                
            }

            // Store the latest frame in the global variable
            latest_frame = fb;
        } else {
            ESP_LOGE(TAG, "Camera capture failed");
            esp_camera_fb_return(latest_frame);
        }
    }
}

// Handler to send the image when requested
static esp_err_t image_handler(httpd_req_t *req) {
    if (latest_frame) {
        vTaskDelay(pdMS_TO_TICKS(50));  // Small delay for synchronization
        // Set HTTP headers for image/jpeg content
        httpd_resp_set_type(req, "image/jpeg");

        // Convert frame length to string and set Content-Length header
        char content_length[16];
        snprintf(content_length, sizeof(content_length), "%d", latest_frame->len);
        httpd_resp_set_hdr(req, "Content-Length", content_length);

        // Send the frame content (JPEG image)
        esp_err_t res = httpd_resp_send(req, (const char *)latest_frame->buf, latest_frame->len);
        if (res != ESP_OK) {
            ESP_LOGE(TAG, "Failed to send image");
        }

        // After sending, return the frame to free the buffer
        esp_camera_fb_return(latest_frame);
        latest_frame = NULL;  // Ensure frame is reset
    } else {
        ESP_LOGW(TAG, "No frame available");
        httpd_resp_send_404(req);  // Return a 404 if no frame is available
    }

    return ESP_OK;
}

// Handler to serve the HTML page
static esp_err_t page_handler(httpd_req_t *req) {
    const char* response = "<html><body>"
        "<h1>ESP32 Camera Snapshot</h1>"
        "<img src='/image' style='width:100%;' />"
        "</body></html>";

    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, response, strlen(response));
}

// Function to start the web server
void start_webserver(void) {
    vTaskDelay(pdMS_TO_TICKS(8000));  // Delay to ensure proper initialization

    // Configure the HTTP server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    static httpd_handle_t server = NULL;

    if (server == NULL) {
        if (httpd_start(&server, &config) == ESP_OK) {
            // Register URI handlers for image and main page
            httpd_uri_t image_uri = {
                .uri = "/image",
                .method = HTTP_GET,
                .handler = image_handler,
                .user_ctx = NULL
            };
            httpd_register_uri_handler(server, &image_uri);

            httpd_uri_t page_uri = {
                .uri = "/",
                .method = HTTP_GET,
                .handler = page_handler,
                .user_ctx = NULL
            };
            httpd_register_uri_handler(server, &page_uri);

            ESP_LOGI(TAG, "Web server started, snapshot available at /image");
        } else {
            ESP_LOGE(TAG, "Failed to start web server");
        }
    } else {
        ESP_LOGW(TAG, "Server already running");
    }

    // Create the capture task that will run in the background
    xTaskCreatePinnedToCore(capture_frame_task, "capture_frame_task", 16384, NULL, 5, NULL, 1); // Pinning to core 1

    // Get AP Mode IP Address
    esp_netif_ip_info_t ip_info;
    esp_netif_t *ap_netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");

    if (esp_netif_get_ip_info(ap_netif, &ip_info) == ESP_OK) {
        ESP_LOGI(TAG, "ESP32 AP IP: %s", ip4addr_ntoa(&ip_info.ip));
    } else {
        ESP_LOGE(TAG, "Failed to get AP IP");
    }
}
