  #include "program.h"
#include "gpio.h"
#include "usb_driver.h"

using namespace HALClass::usb;

#define LED(level) HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (GPIO_PinState)level)

Descriptor_Device descriptor_device = {
    .bcdUSB = 0x0200,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = 0x40,
    .idVendor = 0x89AB,
    .idProduct = 0xCDEF,
    .bcdDevice = 0x0100,
    .iManufacturer = 0,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1
};

UsbDriver usb;

void user_main() {
    LED(0);
    usb.init();
    usb.connect();

    while (true) {
        LED(!HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin));
        HAL_Delay(50);
    }
}

void handleGetDescriptor(PCD_HandleTypeDef *hpcd, Packet_Setup *setup) {
    uint8_t descType = (setup->wValue >> 8)& 0xFF;
    uint8_t descIndex = setup->wValue & 0xFF;
    uint16_t reqLength = setup->wLength;

    if (hpcd->State != HAL_PCD_STATE_READY) {
        HAL_PCD_EP_SetStall(hpcd, EP0_In);
        return;
    }

    const uint8_t* data = nullptr;
    uint16_t dataLen = 0;

    switch (descType) {
        case 1: {
            data = reinterpret_cast<const uint8_t*>(&descriptor_device);
            dataLen = descriptor_device.bLength;
            break;
        }
        default: {
            HAL_PCD_EP_SetStall(hpcd, EP0_In);
            return;
        }
    }
    uint16_t sendLen = (reqLength < dataLen)? reqLength : dataLen;
    HAL_PCD_EP_Transmit(hpcd, EP0_In, const_cast<uint8_t*>(data), sendLen);
    HAL_PCD_EP_Receive(hpcd, EP0_Out, nullptr, 0);
}

void handleStandardRequest(PCD_HandleTypeDef *hpcd, Packet_Setup *setup) {
    switch (setup->bRequestID) {
        case 0x00: {
            uint16_t status = 0x0001;
            HAL_PCD_EP_Transmit(hpcd, EP0_In, reinterpret_cast<uint8_t*>(&status), 2);
            break;
        }
        case 0x01:
        case 0x03: {
            HAL_PCD_EP_Transmit(hpcd, EP0_In, nullptr, 0);
            break;
        }
        case 0x05: {
            HAL_PCD_EP_Transmit(hpcd, EP0_In, nullptr, 0);
            break;
        }
        case 0x06: {
            handleGetDescriptor(hpcd, setup);
            break;
        }
        case 0x08: {
            break;
        }
        case 0x09: {
            HAL_PCD_EP_Transmit(hpcd, EP0_In, nullptr, 0);
            break;
        }
        default: {
            HAL_PCD_EP_SetStall(hpcd, EP0_In);
            break;
        }
    }

}


void USB_UCPD1_2_IRQHandler() {
    HAL_PCD_IRQHandler(&usb._hpcd);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
    Packet_Setup *setup = reinterpret_cast<Packet_Setup*>(hpcd->Setup);
    uint8_t recipient = setup->bmRequest & 0x1F;
    uint8_t type = setup->bmRequest &0x60;
    uint8_t direction = setup->bmRequest & 0x80;
    switch (type) {
        case 0: {
            handleStandardRequest(hpcd, setup);
            break;
        }
        case 1:
        case 2:
        default: {
            HAL_PCD_EP_SetStall(hpcd, EP0_In);
            break;
        }
    }
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
    for (uint8_t i = 1; i < 8; i++) {
        usb[static_cast<EPAddr>(i)].close();
        usb[static_cast<EPAddr>(0x80 | i)].close();
    }
    usb[EP0_In].open(EPTranType::Control, 64);
    usb[EP0_Out].open(EPTranType::Control, 64);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {

}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {

}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {

}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {

}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {

}

