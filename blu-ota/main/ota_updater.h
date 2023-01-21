#ifndef OTA_UPD_H
#define OTA_UPD_H

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_vfs.h"

#define SCRATCH_BUFSIZE 10240

esp_err_t boot_to_app(bool restart);
esp_err_t end_ota_handler(httpd_req_t *req);
esp_err_t reset_device_handler(httpd_req_t *req);
esp_err_t send_ota_binary_handler(httpd_req_t *req);
esp_err_t set_ota_meta_handler(httpd_req_t *req);

typedef struct server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} server_context_t;

#endif