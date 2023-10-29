#include "universal_uart_component.h"

namespace esphome {
  namespace universal_uart {
    void UniversalUartComponent::setup() {
    }

    void UniversalUartComponent::update() {

    }

    void UniversalUartComponent::loop() {
      std::vector<uint8_t> trigger_data;
      uint8_t data[data_length_] = {};
      while(available() >= data_length_) {
        for (int i = 0; i < data_length_; i++) {
          uint8_t d = read();
          data[i] = d;
          trigger_data.push_back(d);
        }

        ESP_LOGD(TAG, "uart bus receive %s", format_uart_data(data, data_length_));

        uint16_t crc = crc16(data, sizeof(data) - 2);
        uint8_t crc_h = static_cast<uint8_t>(crc & 0x00FF);
        uint8_t crc_l = static_cast<uint8_t>((crc & 0xFF00) >> 8);
        if ((data[sizeof(data) - 2] != crc_h) || (data[sizeof(data) - 1] != crc_l)) {
          ESP_LOGD(TAG, "crc check failed, ignore data");
          while(available() > 0) {
            read();
          }
          break;
        }

        for (auto *trigger : this->triggers_) {
          trigger->trigger(trigger_data);
        }
      }
    }

    uint16_t crc16(uint8_t *data, uint8_t length) {
      uint16_t crc = 0xFFFF;

      for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 8; j > 0; j--) {
          if (crc & 0x0001) {
            crc = (crc >> 1) ^ 0xA001;
          } else {
            crc >>= 1;
          }
        }
      }

      return crc;
    }

    char* format_uart_data(uint8_t *uart_data, int length) {
      static char str[256] = {0};  // Output buffer
      char tmp[10];  // Temporary buffer

      str[0] = '\0';  // Clear the buffer
      for (int i = 0; i < length; i++) {
        sprintf(tmp, "%x:", uart_data[i]);
        strcat(str, tmp);  // Append to str
      }

      str[strlen(str)-1] = '\0';  // Replace the last colon with a null terminator

      return str;
    }
  }

}
