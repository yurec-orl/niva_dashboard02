#pragma once

#include <Arduino.h>
#include <vector>

#include "DashGfxWrapper.hpp"
#include "DashSensorReader.hpp"

class DashIndicatorLayout
{
public:
    DashIndicatorLayout(int x, int y, int w, int h, const std::vector<int> &colors)
        : m_x(x), m_y(y), m_w(w), m_h(h), m_colors(colors) {}

    // Convenience constructor for backward compatibility with 2 colors
    DashIndicatorLayout(int x, int y, int w, int h, int color, int bkcolor)
        : m_x(x), m_y(y), m_w(w), m_h(h), m_colors({color, bkcolor}) {}

    int x() const { return m_x; }
    int y() const { return m_y; }
    int w() const { return m_w; }
    int h() const { return m_h; }    // Access colors by index
    int colorAt(size_t index) const
    {
        if (m_colors.empty() || index >= m_colors.size()) {
            return 0; // Return safe default for invalid indices
        }
        return m_colors[index];
    }

    // Backward compatibility methods
    int color() const { return colorAt(0); }
    int bkcolor() const { return colorAt(1); }

    void setPosition(int x, int y)
    {
        m_x = x;
        m_y = y;
    }
    void setSize(int w, int h)
    {
        m_w = w;
        m_h = h;
    }
    void setColors(const std::vector<int> &colors) { m_colors = colors; }
    void setColors(int color, int bkcolor) { m_colors = {color, bkcolor}; }

private:
    int m_x, m_y, m_w, m_h;
    std::vector<int> m_colors;
};

class IDashIndicator
{
public:
    IDashIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader)
        : m_gfx(gfx), m_reader(reader)
    {
    }

    virtual void draw(const DashIndicatorLayout &layout, bool full_draw = false) = 0;

protected:
    IDashGfxWrapper &m_gfx;
    IDashSensorReader &m_reader;

    virtual int read() = 0;
};

class DashVBarIndicator : public IDashIndicator
{
public:
    DashVBarIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader,
                      const char *label, int labelSz = 2, int borderW = 4)
        : IDashIndicator(gfx, reader),
          m_label(label), m_labelSz(labelSz), m_borderW(borderW) {}

    void draw(const DashIndicatorLayout &layout, bool full_draw = false) override
    {
        // Calculate bar area (leave space for label below)
        int label_height = m_gfx.chHeight(m_labelSz);
        int bar_x = layout.x() + m_borderW;
        int bar_y = layout.y() + m_borderW;
        int bar_w = layout.w() - 2 * m_borderW;
        int bar_h = layout.h() - label_height - 2 * m_borderW;

        if (full_draw)
        {
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h(), layout.bkcolor());
            // Draw border using two filled rectangles
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h() - label_height, layout.color());                                                            // outer border
            m_gfx.fillRect(layout.x() + m_borderW, layout.y() + m_borderW, layout.w() - 2 * m_borderW, layout.h() - label_height - 2 * m_borderW, layout.colorAt(2)); // erase inside with color index 2
        }        // Get sensor value and normalize
        int val = read();
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal <= minVal)
            maxVal = minVal + 1; // avoid div by zero and handle invalid ranges

        // Use double precision to avoid potential overflow with large sensor ranges
        double frac = double(val - minVal) / double(maxVal - minVal);
        if (frac < 0.0)
            frac = 0.0;
        if (frac > 1.0)
            frac = 1.0;

        int fill_h = int(bar_h * frac + 0.5);

        // Draw bar fill
        if (fill_h > 0)
            m_gfx.fillRect(bar_x, bar_y + bar_h - fill_h, bar_w, fill_h, layout.color());

        if (full_draw)
        {
            int label_y = layout.y() + layout.h() - label_height;
            int label_w = m_gfx.textWidth(m_label, m_labelSz);
            int label_x = layout.x() + (layout.w() - label_w) / 2;
            m_gfx.userTextWrite(label_x, label_y, m_labelSz, layout.color(), layout.bkcolor(), m_label);
        }
    }

protected:
    int read() override { return m_reader.read(); }

private:
    const char *m_label;
    const int m_labelSz;
    int m_borderW;
};

class DashHBarIndicator : public IDashIndicator
{
public:
    DashHBarIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader,
                      const char *label = nullptr, int labelSz = 2,
                      int borderW = 4, int numSegments = 20, int gapWidth = 2)
        : IDashIndicator(gfx, reader),
          m_label(label), m_numSegments(numSegments), m_gapWidth(gapWidth), m_borderW(borderW),
          m_labelSz(labelSz), m_lastFilledSegments(-1) {}

    void draw(const DashIndicatorLayout &layout, bool full_draw = false) override
    {
        int label_height = (m_label == nullptr) ? 0 : m_gfx.chHeight(m_labelSz);
        int bar_x = layout.x() + m_borderW + m_gapWidth;
        int bar_y = layout.y() + m_borderW + m_gapWidth;
        int bar_w = layout.w() - 2 * (m_borderW + m_gapWidth);
        int bar_h = layout.h() - label_height - 2 * (m_borderW + m_gapWidth);

        if (full_draw)
        {
            draw_border_label(layout, label_height);
            m_lastFilledSegments = -1; // Force redraw of all segments
        } // Get sensor value and normalize to number of segments

        int val = read();
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal <= minVal)
            maxVal = minVal + 1; // avoid div by zero and handle invalid ranges

        // Use double precision to avoid potential overflow with large sensor ranges
        double frac = double(val - minVal) / double(maxVal - minVal);
        if (frac < 0.0)
            frac = 0.0;
        if (frac > 1.0)
            frac = 1.0;

        int currentFilledSegments = int(m_numSegments * frac + 0.5);

        if (m_lastFilledSegments == currentFilledSegments && !full_draw)
        {
            return; // Nothing to update
        }

        draw_segments(layout, full_draw, currentFilledSegments, bar_x, bar_y, bar_w, bar_h);

        m_lastFilledSegments = currentFilledSegments;
    }

private:
    const char *m_label;
    const int m_numSegments;
    const int m_gapWidth;
    const int m_borderW;
    const int m_labelSz;
    int m_lastFilledSegments;

protected:
    int read() override { return m_reader.read(); }

private:
    void draw_border_label(const DashIndicatorLayout &layout, int label_height)
    {
        m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h(), layout.bkcolor());

        if (m_borderW > 0)
        {
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h() - label_height, layout.color());                                                           // outer border
            m_gfx.fillRect(layout.x() + m_borderW, layout.y() + m_borderW, layout.w() - 2 * m_borderW, layout.h() - label_height - 2 * m_borderW, layout.bkcolor()); // erase inside
        }

        if (m_label)
        {
            int label_y = layout.y() + layout.h() - label_height;
            int label_w = m_gfx.textWidth(m_label, m_labelSz);
            int label_x = layout.x() + (layout.w() - label_w) / 2;
            m_gfx.textWrite(label_x, label_y, m_labelSz, layout.color(), layout.bkcolor(), m_label);
        }
    }    void draw_segments(const DashIndicatorLayout &layout, bool full_draw, int currentFilledSegments, int bar_x, int bar_y, int bar_w, int bar_h)
    {
        // Safety check: avoid division by zero and negative segments
        if (m_numSegments <= 0) {
            return;
        }
        
        int segment_w = (bar_w - (m_numSegments - 1) * m_gapWidth) / m_numSegments;
        
        // Ensure segment width is positive
        if (segment_w <= 0) {
            return;
        }

        if (full_draw)
        {
            // Draw all segments when doing a full redraw
            for (int i = 0; i < m_numSegments; ++i)
            {
                int segment_x = bar_x + i * (segment_w + m_gapWidth);
                int segment_color = (i < currentFilledSegments) ? layout.colorAt(0) : layout.colorAt(2);
                m_gfx.fillRect(segment_x, bar_y, segment_w, bar_h, segment_color);
            }
        }
        else
        {
            int start_segment = (m_lastFilledSegments < 0) ? 0 : m_lastFilledSegments;

            // Add new active segments
            if (currentFilledSegments > start_segment)
            {
                for (int i = start_segment; i < currentFilledSegments; ++i)
                {
                    int segment_x = bar_x + i * (segment_w + m_gapWidth);
                    m_gfx.fillRect(segment_x, bar_y, segment_w, bar_h, layout.colorAt(0));
                }
            }

            // Convert active segments to inactive
            int end_segment = (m_lastFilledSegments < 0) ? m_numSegments : m_lastFilledSegments;
            if (currentFilledSegments < end_segment)
            {
                for (int i = currentFilledSegments; i < end_segment; ++i)
                {
                    int segment_x = bar_x + i * (segment_w + m_gapWidth);
                    m_gfx.fillRect(segment_x, bar_y, segment_w, bar_h, layout.colorAt(2));
                }
            }
        }
    }
};

class DashSegmentIndicator : public IDashIndicator
{
public:
    DashSegmentIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader, bool skip_leading_zeros = true)
        : IDashIndicator(gfx, reader),
          m_lastValue(-1), m_skip_leading_zeros(skip_leading_zeros) {}
          
    void draw(const DashIndicatorLayout &layout, bool full_draw = false) override
    {
        constexpr int digits_count = 3;

        int value = read();
        
        if (value == m_lastValue && !full_draw)
        {
            return;
        }

        if (full_draw)
        {
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h(), layout.bkcolor());
        }

        std::array<int, digits_count> digits;
        digits[0] = value / 100;
        digits[1] = (value / 10) % 10;
        digits[2] = value % 10;

        std::array<int, digits_count> last_digits = {-1};
        if (m_lastValue != -1)
        {
            last_digits[0] = m_lastValue / 100;
            last_digits[1] = (m_lastValue / 10) % 10;
            last_digits[2] = m_lastValue % 10;
        }

        // Apply leading zero logic to current digits
        if (m_skip_leading_zeros)
        {
            for (int i = 0; (i < digits_count - 1) && !digits[i]; ++i)
            {
                digits[i] = -1; // blank leading zeros
            }
        }

        int seg_thickness = layout.w() / 19; // (3 * 5) for digits + (2 * 2) for doubled gaps = 19 units total
        if (seg_thickness < 1)
            seg_thickness = 1;
        int digit_w = 5 * seg_thickness;
        int gap_w = 2 * seg_thickness; // Double the gap size        // Calculate fixed positions for all 3 digits (always assume 3 digits for positioning)
        int total_width = 3 * digit_w + 2 * gap_w; // Total width for 3 digits + 2 gaps
        int start_x_offset = (layout.w() - total_width) / 2;

        // Draw each digit at its fixed position
        for (int i = 0; i < digits.size(); ++i)
        {
            int digit_x = layout.x() + start_x_offset + i * (digit_w + gap_w);
            
            if (digits[i] != -1)
            {
                // Draw visible digit background and segments
                if (full_draw || digits[i] != last_digits[i])
                {
                    m_gfx.fillRect(digit_x, layout.y(), digit_w, layout.h(), layout.colorAt(2));
                    draw_digit(layout, digits[i], digit_x, layout.y(), digit_w, layout.h(), seg_thickness);
                }
            }
            else if (full_draw || last_digits[i] != -1)
            {
                // Clear digit that was previously visible but now should be hidden
                m_gfx.fillRect(digit_x, layout.y(), digit_w, layout.h(), layout.colorAt(2));
            }
        }
        m_lastValue = value;
    }

protected:
    int read() override 
    { 
        int rawValue = m_reader.read();
        
        // Normalize value: when read() returns maxVal(), value should equal 180
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal <= minVal)
            maxVal = minVal + 1; // avoid div by zero and handle invalid ranges
        
        // Use long long to prevent overflow during calculation
        long long range = (long long)maxVal - (long long)minVal;
        long long offset = (long long)rawValue - (long long)minVal;
        
        float frac = (float)offset / (float)range;
        if (frac < 0.0f)
            frac = 0.0f;
        if (frac > 1.0f)
            frac = 1.0f;
        
        int result = (int)(180.0f * frac + 0.5f); // Scale to 0-180 range
        
        // Additional bounds check for the result
        if (result < 0) result = 0;
        if (result > 999) result = 999; // Maximum 3-digit display
        
        return result;
    }

private:
    int m_lastValue;
    bool m_skip_leading_zeros;
    static const uint8_t s_segmentMap[10];    void draw_digit(const DashIndicatorLayout &layout, int digit, int x, int y, int w, int h, int seg_thickness)
    {
        // Bounds check for digit parameter
        if (digit < 0 || digit > 9) {
            return; // Invalid digit, don't draw anything
        }
        
        uint8_t segments = s_segmentMap[digit];

        int seg_len_h = w - 2 * seg_thickness;
        int seg_len_v = (h - 3 * seg_thickness) / 2;

        // A (top horizontal)
        if (segments & 0b0000001)
            draw_segment(layout, x + seg_thickness, y, seg_len_h, seg_thickness);
        // B (top-right vertical)
        if (segments & 0b0000010)
            draw_segment(layout, x + w - seg_thickness, y + seg_thickness, seg_thickness, seg_len_v);
        // C (bottom-right vertical)
        if (segments & 0b0000100)
            draw_segment(layout, x + w - seg_thickness, y + 2 * seg_thickness + seg_len_v, seg_thickness, seg_len_v);
        // D (bottom horizontal)
        if (segments & 0b0001000)
            draw_segment(layout, x + seg_thickness, y + h - seg_thickness, seg_len_h, seg_thickness);
        // E (bottom-left vertical)
        if (segments & 0b0010000)
            draw_segment(layout, x, y + 2 * seg_thickness + seg_len_v, seg_thickness, seg_len_v);
        // F (top-left vertical)
        if (segments & 0b0100000)
            draw_segment(layout, x, y + seg_thickness, seg_thickness, seg_len_v);
        // G (middle horizontal)
        if (segments & 0b1000000)
            draw_segment(layout, x + seg_thickness, y + seg_thickness + seg_len_v, seg_len_h, seg_thickness);
    }

    void draw_segment(const DashIndicatorLayout &layout, int x, int y, int w, int h)
    {
        m_gfx.fillRect(x, y, w, h, layout.color());
    }
};

const uint8_t DashSegmentIndicator::s_segmentMap[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};