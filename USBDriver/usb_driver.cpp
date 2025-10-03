#include "usb_driver.h"


namespace HALClass::usb {
    UsbDriver::UsbDriver(PCD_TypeDef *pcdInstance) {
        _hpcd.Instance = pcdInstance;
    }

    bool UsbDriver::init(UsbSpeed speed) {
        switch (speed) {
            case UsbSpeed::Full: {
                _hpcd.Init.speed = PCD_SPEED_FULL;
            }
        }
        RCC_PeriphCLKInitTypeDef PCC_PeriphClkInitStruct{};
        bool ret = true;

        PCC_PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
        PCC_PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
        ret &= HAL_RCCEx_PeriphCLKConfig(&PCC_PeriphClkInitStruct) == HAL_OK;

        HAL_NVIC_SetPriority(USB_UCPD1_2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(USB_UCPD1_2_IRQn);

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_USB_CLK_ENABLE();
        if (__HAL_RCC_PWR_IS_CLK_DISABLED()) {
            __HAL_RCC_PWR_CLK_ENABLE();
            HAL_PWREx_EnableVddUSB();
        } else {
            HAL_PWREx_EnableVddUSB();
        }

        _hpcd.Init.dev_endpoints = 8;
        _hpcd.Init.Host_channels = 8;
        _hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
        _hpcd.Init.Sof_enable = DISABLE;
        _hpcd.Init.lpm_enable = DISABLE;
        _hpcd.Init.low_power_enable = DISABLE;
        _hpcd.Init.vbus_sensing_enable = DISABLE;
        _hpcd.Init.battery_charging_enable = DISABLE;
        _hpcd.Init.iso_singlebuffer_enable = DISABLE;
        _hpcd.Init.bulk_doublebuffer_enable = DISABLE;
        ret &= HAL_PCD_Init(&_hpcd) == HAL_OK;

        (*this)[EP0_In].open(EPTranType::Control, 64);
        (*this)[EP0_Out].open(EPTranType::Control, 64);
        HAL_PCDEx_PMAConfig(&_hpcd, EP0_In, PCD_SNG_BUF, 0x18);
        HAL_PCDEx_PMAConfig(&_hpcd, EP0_Out, PCD_SNG_BUF, 0x58);
        if (ret) _initialized = true;
        return ret;
    }

    void UsbDriver::connect() {
        HAL_PCD_Start(&_hpcd);
    }

    void UsbDriver::disconnect() {
        HAL_PCD_Stop(&_hpcd);
    }

    void UsbDriver::openEP(EPAddr epAddr, EPTranType tranType, uint16_t packetSizeMax) {
        (*this)[epAddr].open(tranType, packetSizeMax);
    }

    void UsbDriver::closeEP(EPAddr epAddr) {
        (*this)[epAddr].close();
    }

    UsbDriver::~UsbDriver() {
        if (_initialized) {
            HAL_PCD_DeInit(&_hpcd);
            _initialized = false;
        }
    }

    UsbEndpoint &UsbDriver::getEndpoint(EPAddr epAddr) {
        uint8_t num_addr = static_cast<uint8_t>(epAddr)& 0x7F;
        return this->endpoints[(epAddr & 0x80) == 0 ? num_addr : 8 + num_addr];
    }


    UsbEndpoint &UsbDriver::operator[](EPAddr epAddr) {
        return getEndpoint(epAddr);
    }


    UsbEndpoint::UsbEndpoint(UsbDriver *usb, EPAddr addr): epAddr(addr), usb(usb) {
        epState = EPState::Disabled;

    }

    UsbEndpoint::UsbEndpoint(UsbDriver *usb, EPAddr addr, EPTranType tranType): epAddr(addr), usb(usb), tranType(tranType) {
        epState = EPState::Disabled;
    }

    void UsbEndpoint::open(EPTranType tranType, uint16_t packetSizeMax) {
        uint8_t type;
        this->tranType = tranType;
        switch (tranType) {
            case EPTranType::Control: {
                type = EP_TYPE_CTRL;
                break;
            }
            case EPTranType::Interrupt: {
                type = EP_TYPE_INTR;
                break;
            }
            case EPTranType::Bulk: {
                type = EP_TYPE_BULK;
                break;
            }
            case EPTranType::Isochronous: {
                type = EP_TYPE_ISOC;
                break;
            }
            default: {
                type = EP_TYPE_CTRL;
                break;
            }
        }
        HAL_PCD_EP_Open(const_cast<PCD_HandleTypeDef*>(&usb->_hpcd), this->epAddr, packetSizeMax, type);
    }

    bool UsbEndpoint::Transmit(uint8_t *dataAddr, uint16_t dataLen) {
        bool ret = false;
        HAL_PCD_EP_Transmit(&const_cast<UsbDriver*>(usb)->_hpcd, epAddr, dataAddr, dataLen);
        return ret;
    }

    bool UsbEndpoint::Receive(uint8_t *bufferAddr, uint16_t bufferLen) {
        bool ret = false;
        HAL_PCD_EP_Receive(&const_cast<UsbDriver*>(usb)->_hpcd, epAddr, bufferAddr, bufferLen);
        return ret;
    }



    void UsbEndpoint::reset() {
    }

    void UsbEndpoint::close() {
        HAL_PCD_EP_Close(const_cast<PCD_HandleTypeDef*>(&usb->_hpcd), this->epAddr);
        this->tranType = EPTranType::Control;
        reset();
    }

    UsbEndpoint::~UsbEndpoint() {
        this->reset();
    }


    inline UsbEndpoint::operator EPAddr() const {
        return this->epAddr;
    }

    inline UsbEndpoint::operator EPState() const {
        return this->epState;
    }

    inline UsbEndpoint::operator EPTranType() const {
        return this->tranType;
    }


}
