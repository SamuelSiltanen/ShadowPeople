/*
    Copyright 2018 Samuel Siltanen
    Image.cpp
*/

#include "Image.hpp"
#include "../Math.hpp"

namespace graphics
{
    Image::Image(uint8_t bpp, uint16_t width, uint16_t height, uint16_t depth) :
        m_width(0), // Note: Set everything to zero so that setDimensions() detects a change
        m_height(0),
        m_depth(0),
        m_bpp(0)
    {
        setDimensions(bpp, width, height, depth);
    }

    void Image::setDimensions(uint8_t bpp, uint16_t width, uint16_t height, uint16_t depth)
    {
        if ((m_bpp != bpp) || (m_width != width) || (m_height != height) || (m_depth != depth))
        {
            m_width     = width;
            m_height    = height;
            m_depth     = depth;
            m_bpp       = bpp;

            m_data = std::make_shared<std::vector<uint8_t>>(dataSize());
        }
    }

    void Image::fillData(Range<const uint8_t> data)
    {
        memcpy(m_data->data(), data.begin(), dataSize());
    }

    void Image::copy(const Image& srcImage, Rect<int, 2> dstRect,
                     std::function<void(void* dst, const void* src)> convertAndAssign)
    {
        uint8_t srcElemSize = math::divRoundUp<uint8_t>(srcImage.bpp(), 8);
        uint8_t dstElemSize = math::divRoundUp<uint8_t>(m_bpp, 8);
        
        for (uint32_t y = 0; y < srcImage.height(); y++)
        {
            uint32_t dstY = y + dstRect.minCorner()[1];
            uint32_t srcY = y;
            for (uint32_t x = 0; x < srcImage.width(); x++)
            {
                uint32_t dstX = x + dstRect.minCorner()[0];
                uint32_t srcX = x;
                
                void *dstPtr        = m_data->data() + (dstY * m_width + dstX) * dstElemSize;
                const void *srcPtr  = srcImage.data() + (srcY * srcImage.width() + srcX) * srcElemSize;

                convertAndAssign(dstPtr, srcPtr);
            }
        }
    }

    const uint8_t* Image::data() const
    {
        return m_data->data();
    }

    uint32_t Image::dataSize() const
    {
        uint32_t bytesPerPixel  = math::divRoundUp<uint8_t>(m_bpp, 8);
        uint32_t dataSize       = m_width * m_height * m_depth * bytesPerPixel;
        return dataSize;
    }

    uint32_t Image::stride() const
    {
        uint32_t bytesPerPixel  = math::divRoundUp<uint8_t>(m_bpp, 8);
        return m_width * bytesPerPixel;
    }

    uint32_t Image::depthStride() const
    {
        uint32_t bytesPerPixel  = math::divRoundUp<uint8_t>(m_bpp, 8);
        return m_width * m_height * bytesPerPixel;
    }
}