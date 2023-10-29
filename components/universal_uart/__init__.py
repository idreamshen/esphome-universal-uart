import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)

DEPENDENCIES = ['uart']

universal_uart_ns = cg.esphome_ns.namespace('universal_uart')
UniversalUartComponent = universal_uart_ns.class_('UniversalUartComponent', cg.PollingComponent, uart.UARTDevice)
OnDataTrigger = universal_uart_ns.class_(
    "OnDataTrigger", automation.Trigger.template(cg.std_vector.template(cg.uint8)), cg.Component
)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UniversalUartComponent),
    cv.Required("data_length"): cv.int_range(min=1, max=32),
    cv.Optional("on_data"): automation.validate_automation(
        {
            cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(OnDataTrigger),
        }
    ),
}).extend(cv.COMPONENT_SCHEMA).extend(uart.UART_DEVICE_SCHEMA)

def to_code(config):
    u = yield cg.get_variable(config["uart_id"])
    var = cg.new_Pvariable(config[CONF_ID], u)
    yield cg.register_component(var, config)
    cg.add(var.set_data_length(config["data_length"]))
    yield uart.register_uart_device(var, config)

    for conf in config.get("on_data", []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        yield cg.register_component(trigger, conf)
        yield automation.build_automation(trigger, [(cg.std_vector.template(cg.uint8), "data")], conf)
