#pragma once
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_pcd.h"
#include "usb_driver.h"


namespace HALClass::usb {
    enum EPAddr: uint8_t {
        EP0_Out = 0x00,
        EP1_Out = 0x01,
        EP2_Out = 0x02,
        EP3_Out = 0x03,
        EP4_Out = 0x04,
        EP5_Out = 0x05,
        EP6_Out = 0x06,
        EP7_Out = 0x07,
        EP0_In  = 0x80,
        EP1_In  = 0x81,
        EP2_In  = 0x82,
        EP3_In  = 0x83,
        EP4_In  = 0x84,
        EP5_In  = 0x85,
        EP6_In  = 0x86,
        EP7_In  = 0x87,
    };


    enum class UsbSpeed: uint8_t {
        Full = 0
    };

    enum class DeviceState: uint8_t {
        Detached = 0,
        Powered,
        Default,
        Addressed,
        Configured,
        Suspended,
        Error
    };

    enum class EPTranType: uint8_t {
        Control = 0,
        Interrupt,
        Bulk,
        Isochronous
    };

    enum class EPState: uint8_t {
        Disabled = 0,
        Enabled,
        Halted,
        Error
    };

    struct __attribute__((packed, aligned(1))) Descriptor_Device {
        uint8_t bLength{0x12};
        uint8_t bDescriptorType{0x01};
        uint16_t bcdUSB{};
        uint8_t bDeviceClass{};
        uint8_t bDeviceSubClass{};
        uint8_t bDeviceProtocol{};
        uint8_t bMaxPacketSize0{};
        uint16_t idVendor{};
        uint16_t idProduct{};
        uint16_t bcdDevice{};
        uint8_t iManufacturer{};
        uint8_t iProduct{};
        uint8_t iSerialNumber{};
        uint8_t bNumConfigurations{};
    };

    struct __attribute__((packed, aligned(1))) Descriptor_Config {
        uint8_t bLength{0x09};
        uint8_t bDescriptorType{0x02};
        uint16_t wTotalLength{};
        uint8_t bNumInterfaces{};
        uint8_t bConfigurationValue{};
        uint8_t iConfiguration{};
        uint8_t bmAttributes{};
        uint8_t bMaxPower{};
    };

    struct __attribute__((packed, aligned(1))) Descriptor_String {
        uint8_t bLength{2};
        uint8_t bDescriptorType{0x03};
        uint16_t *wString;
    };

    struct __attribute__((packed, aligned(1))) Descriptor_Interface {
        uint8_t bLength{0x09};
        uint8_t bDescriptorType{0x04};
        uint8_t bInterfaceNumber{};
        uint8_t bAlternateSetting{};
        uint8_t bNumEndpoints{};
        uint8_t bInterfaceClass{};
        uint8_t bInterfaceSubClass{};
        uint8_t bInterfaceProtocol{};
        uint8_t iInterface{};
    };

    struct __attribute__((packed, aligned(1))) Descriptor_Endpoint {
        uint8_t bLength{0x07};
        uint8_t bDescriptorType{0x05};
        uint8_t bEndpointAddress{};
        uint8_t bmAttributes{};
        uint16_t wMaxPacketSize{};
        uint8_t bInterval{};
    };

    struct Packet_Setup {
        uint8_t   bmRequest;
        uint8_t   bRequestID;
        uint16_t  wValue;
        uint16_t  wIndex;
        uint16_t  wLength;
    };

    class UsbDriver;

    class UsbEndpoint {
    public:
        const EPAddr epAddr;
        uint8_t *dataAddr{nullptr};
        uint16_t packetSizeMax{64};
        uint16_t data_len{0};

        UsbEndpoint(UsbDriver *usb, EPAddr addr);
        UsbEndpoint(UsbDriver *usb, EPAddr addr, EPTranType tranType);

        void open(EPTranType tranType, uint16_t packetSizeMax);
        bool Transmit(uint8_t *dataAddr, uint16_t dataLen);
        bool Receive(uint8_t *bufferAddr, uint16_t bufferLen);
        void reset();
        void close();

        UsbDriver const *getUsb() const {return this->usb;}
        EPAddr getEPAddr() const {return this->epAddr;};
        EPTranType getEPTranType() const {return this->tranType;}
        bool get_xxx() const {return this->_xxx;}

        ~UsbEndpoint();

        inline explicit operator EPAddr() const;
        inline explicit operator EPState() const;
        inline explicit operator EPTranType() const;

    private:
        UsbDriver const *usb;
        EPTranType tranType{};
        bool _xxx{false};
        EPState epState;
    };

    class UsbDriver {
        public:
            friend UsbDriver;
            PCD_HandleTypeDef _hpcd{};

            explicit UsbDriver(PCD_TypeDef *pcdInstance = USB_DRD_FS);

            bool init(UsbSpeed speed = UsbSpeed::Full);
            void connect();
            void disconnect();

            void openEP(EPAddr epAddr, EPTranType tranType, uint16_t packetSizeMax);
            void closeEP(EPAddr epAddr);

            UsbEndpoint &getEndpoint(EPAddr epAddr);

            ~UsbDriver();

            UsbEndpoint &operator[](EPAddr epAddr);

        private:
            DeviceState deviceState{};
            uint8_t _address{0};
            bool _initialized{false};

            UsbEndpoint endpoints[16] {
                UsbEndpoint(this, EP0_Out),
                UsbEndpoint(this, EP1_Out),
                UsbEndpoint(this, EP2_Out),
                UsbEndpoint(this, EP3_Out),
                UsbEndpoint(this, EP4_Out),
                UsbEndpoint(this, EP5_Out),
                UsbEndpoint(this, EP6_Out),
                UsbEndpoint(this, EP7_Out),
                UsbEndpoint(this, EP0_In),
                UsbEndpoint(this, EP1_In),
                UsbEndpoint(this, EP2_In),
                UsbEndpoint(this, EP3_In),
                UsbEndpoint(this, EP4_In),
                UsbEndpoint(this, EP5_In),
                UsbEndpoint(this, EP6_In),
                UsbEndpoint(this, EP7_In),

            };
    };
}

