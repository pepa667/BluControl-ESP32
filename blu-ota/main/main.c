#include "main.h"

#define LOG_TAG "BLU OTA"

#define BASE_PATH "/www"

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + CONFIG_SPIFFS_OBJ_NAME_LEN)
#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath);

esp_err_t get_req_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];
    server_context_t *server_context = (server_context_t *)req->user_ctx;
    strlcpy(filepath, server_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }
    int fd = open(filepath, O_RDONLY, 0);
    if (fd == -1) {
        ESP_LOGE(LOG_TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read existing file");
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = server_context->scratch;
    ssize_t read_bytes;
    do {
        /* Read file in chunks into the scratch buffer */
        read_bytes = read(fd, chunk, SCRATCH_BUFSIZE);
        if (read_bytes == -1) {
            ESP_LOGE(LOG_TAG, "Failed to read file : %s", filepath);
        } else if (read_bytes > 0) {
            /* Send the buffer contents as HTTP response chunk */
            if (httpd_resp_send_chunk(req, chunk, read_bytes) != ESP_OK) {
                close(fd);
                ESP_LOGE(LOG_TAG, "File sending failed!");
                /* Abort sending file */
                httpd_resp_sendstr_chunk(req, NULL);
                /* Respond with 500 Internal Server Error */
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to send file");
                return ESP_FAIL;
            }
        }
    } while (read_bytes > 0);
    /* Close file after sending complete */
    close(fd);
    ESP_LOGI(LOG_TAG, "File sending complete");
    /* Respond with an empty chunk to signal HTTP response completion */
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = BASE_PATH,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(LOG_TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(LOG_TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(LOG_TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(LOG_TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(LOG_TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}

esp_err_t init_nvs(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    return ret;
}

TaskHandle_t blinkTaskHandle = NULL;
void ota_blink(void *obj)
{
    if (OTA_LED_GPIO < 0)
    {
        return;
    }
    while (true)
    {
        gpio_set_level(OTA_LED_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(OTA_LED_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

esp_err_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    httpd_handle_t server = NULL;

    esp_err_t res = httpd_start(&server, &config);
    if (res != ESP_OK)
    {
        return res;
    }

    server_context_t *server_context = calloc(1, sizeof(server_context_t));
    strlcpy(server_context->base_path, BASE_PATH, sizeof(server_context->base_path));
    
    httpd_uri_t uri_get =
    {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = server_context
    };
    httpd_register_uri_handler(server, &uri_get);

    httpd_uri_t end_ota_uri =
    {
        .uri = "/end_ota",
        .method = HTTP_POST,
        .handler = end_ota_handler,
        .user_ctx = server_context
    };
    httpd_register_uri_handler(server, &end_ota_uri);
    httpd_uri_t reset_device_uri =
    {
        .uri = "/reset_device",
        .method = HTTP_POST,
        .handler = reset_device_handler,
        .user_ctx = server_context
    };
    httpd_register_uri_handler(server, &reset_device_uri);
    httpd_uri_t set_ota_meta_uri =
    {
        .uri = "/set_ota_metadata",
        .method = HTTP_POST,
        .handler = set_ota_meta_handler,
        .user_ctx = server_context
    };
    httpd_register_uri_handler(server, &set_ota_meta_uri);
    httpd_uri_t send_binary_uri =
    {
        .uri = "/send_ota_binary",
        .method = HTTP_POST,
        .handler = send_ota_binary_handler,
        .user_ctx = server_context
    };
    httpd_register_uri_handler(server, &send_binary_uri);

    return ESP_OK;
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED)
    {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(LOG_TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(LOG_TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

esp_err_t wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = OTA_WIFI_SSID,
            .ssid_len = strlen(OTA_WIFI_SSID),
            .channel = OTA_WIFI_CHANNEL,
            .password = OTA_WIFI_PASS,
            .max_connection = OTA_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    if (strlen(OTA_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(LOG_TAG, "wifi_init_softap finished. SSID: '%s' password: '%s'", OTA_WIFI_SSID, OTA_WIFI_PASS);
    return ESP_OK;
}

void app_main(void)
{
    ESP_LOGD(LOG_TAG, "BluControl OTA Mode. Reset by=%d. HEAP=%#010lx", esp_reset_reason(), esp_get_free_heap_size());

    if (esp_reset_reason() == ESP_RST_POWERON)
    {
        boot_to_app(true);
        return;
    }

    ESP_ERROR_CHECK(init_nvs());
    ESP_ERROR_CHECK(init_fs());
    ESP_ERROR_CHECK(wifi_init_softap());
    ESP_ERROR_CHECK(setup_server());

    if (OTA_LED_GPIO >= 0)
    {
        gpio_reset_pin(OTA_LED_GPIO);
        gpio_set_direction(OTA_LED_GPIO, GPIO_MODE_OUTPUT);

        xTaskCreatePinnedToCore(ota_blink, "OTA_BLINK", 1024, NULL, tskIDLE_PRIORITY, &blinkTaskHandle, 1);
        configASSERT(blinkTaskHandle);
    }

    ESP_LOGI(LOG_TAG, "BluControl. Started!\n");
}