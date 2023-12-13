#pragma once

#include <map>
#include "driver/i2c.h"

#define ESP_PANEL_HOST_I2C      (0)
#define ESP_PANEL_HOST_SPI      (1)
#define ESP_PANEL_HOST_I80      (2)
#define ESP_PANEL_HOST_RGB      (3)

class ESP_PanelHost {
public:
    ESP_PanelHost();
    ~ESP_PanelHost();

    bool addHostI2C(int host_id, int scl_io, int sda_io);

    void begin(void);

private:
    std::map<int, i2c_config_t> _i2c_host_config_map;
};
