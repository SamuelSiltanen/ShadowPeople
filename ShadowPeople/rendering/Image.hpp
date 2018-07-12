/*
    Copyright 2018 Samuel Siltanen
    Image.hpp
*/

#pragma once

#include <stdint.h>
#include <memory>
#include <vector>
#include <functional>

#include "../Types.hpp"

namespace rendering
{
    class Image
    {
    public:
        Image() = default;
        Image(uint16_t width, uint16_t height, uint8_t bpp);

        void setDimensions(uint16_t width, uint16_t height, uint8_t bpp);
        void fillData(Range<const uint8_t> data);

        void copy(const Image& srcImage, Rect<int, 2> dstRect,
                  std::function<void(void* dst, const void* src)> convertAndAssign);

        const uint8_t* data() const;
        uint32_t dataSize() const;

        uint16_t width() const  { return m_width; }
        uint16_t height() const { return m_height; }
        uint8_t  bpp() const    { return m_bpp; }
    private:
        std::shared_ptr<std::vector<uint8_t>> m_data;

        uint16_t    m_width;
        uint16_t    m_height;
        uint8_t     m_bpp;
    };
}
