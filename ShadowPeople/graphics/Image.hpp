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

namespace graphics
{
    class Image
    {
    public:
        Image() = default;
        Image(uint8_t bpp, uint16_t width, uint16_t height = 1, uint16_t depth = 1);

        void setDimensions(uint8_t bpp, uint16_t width, uint16_t height = 1, uint16_t depth = 1);
        void fillData(Range<const uint8_t> data);

        void copy(const Image& srcImage, Rect<int, 2> dstRect,
                  std::function<void(void* dst, const void* src)> convertAndAssign);

        const uint8_t* data() const;
        uint32_t dataSize() const;

        template<typename T>
        Range<T> asRange() { return Range<T>(reinterpret_cast<T*>(m_data->data()), dataSize()); }

        uint16_t width() const  { return m_width; }
        uint16_t height() const { return m_height; }
        uint8_t  bpp() const    { return m_bpp; }

        uint32_t stride() const;
        uint32_t depthStride() const;
    private:
        std::shared_ptr<std::vector<uint8_t>> m_data;

        uint16_t    m_width;
        uint16_t    m_height;
        uint16_t    m_depth;
        uint8_t     m_bpp;
    };
}
