#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

#define TAG "universal_uart"

namespace esphome {
  namespace universal_uart {
    class OnDataTrigger;

    class UniversalUartComponent : public PollingComponent, public uart::UARTDevice {
    public:
      UniversalUartComponent(uart::UARTComponent *uartComponent) : PollingComponent(60000), UARTDevice(uartComponent) {
        this->uart_ = uartComponent;
      };
      void setup() override;
      void update() override;
      void loop() override;
      uart::UARTComponent* get_uart() {
        return this->uart_;
      }
      void add_trigger(OnDataTrigger *trigger) {
        this->triggers_.push_back(trigger);
      }
      void set_data_length(uint8_t data_length) {
        this->data_length_ = data_length;
      }
    protected:
      uart::UARTComponent* uart_;
      uint8_t data_length_;
      std::vector<OnDataTrigger *> triggers_{};
    };

    uint16_t crc16(uint8_t *data, uint8_t length);
    char* format_uart_data(uint8_t *uart_data, int length);

    class OnDataTrigger : public Trigger<std::vector<uint8_t>>, public Component {
      friend class UniversalUartComponent;

    public:
      explicit OnDataTrigger(UniversalUartComponent *parent)
        : parent_(parent){};

      void setup() override { this->parent_->add_trigger(this); }

    protected:
      UniversalUartComponent *parent_;
    };
  }
}
