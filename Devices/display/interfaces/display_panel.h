#pragma once
#include <stdint.h>
#include <stddef.h>

class Panel {
    public:
        const uint32_t _width;
        const uint32_t _height;

        Panel(uint32_t width, uint32_t height): _width(width), _height(height) {};

        void virtual init() = 0;
        void virtual draw(uint8_t *data) = 0;

        virtual ~Panel() = default;
};


