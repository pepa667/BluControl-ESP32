#include "ota_updater.h"

#define LOG_TAG "BLU OTA"

int partition_id = 0;
esp_ota_handle_t update_handle;

esp_err_t respond_with_ok(httpd_req_t *req);

esp_err_t boot_to_app(bool restart)
{
    const esp_partition_t *partition = esp_partition_find_first(
        ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_APP_OTA_1,
        NULL);
    ESP_LOGI(LOG_TAG, "Switching to App 0\n");
    if (partition != NULL)
    {
        esp_err_t err = esp_ota_set_boot_partition(partition);
        if (err != ESP_OK)
        {
            ESP_LOGE(LOG_TAG, "esp_ota_set_boot_partition failed (%s)", esp_err_to_name(err));
            abort();
            return err;
        }
        if (restart)
        {
            esp_restart();
        }
    }
    else
    {
        ESP_LOGE(LOG_TAG, "No partition found for App 0.");
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t end_ota_handler(httpd_req_t *req)
{
    ESP_LOGI(LOG_TAG, "end_ota was called");

    esp_err_t res = esp_ota_end(update_handle);
    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(res));
        return res;
    }
    return respond_with_ok(req);
}

esp_err_t reset_device_handler(httpd_req_t *req)
{
    ESP_LOGI(LOG_TAG, "reset_device was called");

    esp_err_t res = boot_to_app(false);
    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(res));
        return res;
    }
    res = respond_with_ok(req);
    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(res));
        return res;
    }
    esp_restart();
    return ESP_OK;
}

esp_err_t respond_with_ok(httpd_req_t *req)
{
    esp_err_t res = httpd_resp_set_type(req, "text/plain");
    if (res != ESP_OK)
    {
        return res;
    }
    res = httpd_resp_send_chunk(req, "OK", 2);
    if (res != ESP_OK)
    {
        return res;
    }
    return httpd_resp_send_chunk(req, NULL, 0);
}

uint32_t current_pos = 0;
esp_err_t send_ota_binary_handler(httpd_req_t *req)
{
    ESP_LOGI(LOG_TAG, "send_ota_binary was called with %d bytes", req->content_len);

    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((server_context_t*)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len > SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }

    esp_err_t res = esp_ota_write(update_handle, (const void *)buf, total_len);
    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(res));
        return res;
    }
    current_pos += total_len;

    return respond_with_ok(req);
}

esp_err_t set_ota_meta_handler(httpd_req_t *req)
{
    ESP_LOGI(LOG_TAG, "set_ota_meta was called with %d bytes", req->content_len);

    int required_len = 1;
    int total_len = req->content_len;
    if (total_len < required_len)
    {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "blank content not admitted");
        return ESP_FAIL;
    }

    int cur_len = 0;
    char *buf = ((server_context_t*)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }

    //Version
    switch (buf[0])
    {
        case 1:
            required_len += 1 + 7;
            if (total_len < required_len)
            {
                httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "uncomplete package");
                return ESP_FAIL;
            }

            partition_id = buf[1];
            current_pos = 0;
            break;
        default:
            httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "update package version not allowed");
            return ESP_FAIL;
    }
    const esp_partition_t *partition;
    if (partition_id < 10)
    {
        partition = esp_partition_find_first(
                        ESP_PARTITION_TYPE_APP,
                        ESP_PARTITION_SUBTYPE_APP_OTA_1 + partition_id,
                        NULL);
    }
    else
    {
        partition = esp_ota_get_running_partition();
    }

    esp_err_t res = esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &update_handle);
    if (res != ESP_OK)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, esp_err_to_name(res));
        return res;
    }
    return respond_with_ok(req);
}