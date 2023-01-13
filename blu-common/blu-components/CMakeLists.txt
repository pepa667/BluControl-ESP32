idf_component_register(SRCS 
                    "blu-hardware.c"
                    "sticks/blu-analog-pad.c"
                    "sticks/blu-buttons-pad.c"
                    "sticks/blun64-pad.c"
                    INCLUDE_DIRS
                    "include"
                    "include/sticks"
                    REQUIRES app_update driver
                    esp_adc esp_partition esp_system)
