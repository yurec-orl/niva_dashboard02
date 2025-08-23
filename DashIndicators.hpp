#pragma once

#include <Arduino.h>

#include "DashGfxWrapper.hpp"
#include "DashSensorReader.hpp"

class DashIndicatorLayout
{
public:
    DashIndicatorLayout(int x, int y, int w, int h, int color, int bkcolor)
        : m_x(x), m_y(y), m_w(w), m_h(h), m_color(color), m_bkcolor(bkcolor) {}

    int x() const { return m_x; }
    int y() const { return m_y; }
    int w() const { return m_w; }
    int h() const { return m_h; }
    int color() const { return m_color; }
    int bkcolor() const { return m_bkcolor; }

    void setPosition(int x, int y) { m_x = x; m_y = y; }
    void setSize(int w, int h) { m_w = w; m_h = h; }
    void setColors(int color, int bkcolor) { m_color = color; m_bkcolor = bkcolor; }

private:
    int m_x, m_y, m_w, m_h;
    int m_color, m_bkcolor;
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
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h() - label_height, layout.color());                                                           // outer border
            m_gfx.fillRect(layout.x() + m_borderW, layout.y() + m_borderW, layout.w() - 2 * m_borderW, layout.h() - label_height - 2 * m_borderW, layout.bkcolor()); // erase inside
        }

        // Get sensor value and normalize
        int val = m_reader.read();
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal == minVal)
            maxVal = minVal + 1; // avoid div by zero

        float frac = float(val - minVal) / float(maxVal - minVal);
        if (frac < 0)
            frac = 0;
        if (frac > 1)
            frac = 1;

        int fill_h = int(bar_h * frac + 0.5f);

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
          m_labelSz(labelSz), m_lastFilledSegments(-1) {}    void draw(const DashIndicatorLayout &layout, bool full_draw = false) override
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
        }

        // Get sensor value and normalize to number of segments
        int val = m_reader.read();
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal == minVal)
            maxVal = minVal + 1;

        float frac = float(val - minVal) / float(maxVal - minVal);
        if (frac < 0)
            frac = 0;
        if (frac > 1)
            frac = 1;

        int currentFilledSegments = int(m_numSegments * frac + 0.5f);

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
    int m_lastFilledSegments;    void draw_border_label(const DashIndicatorLayout &layout, int label_height)
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
        int segment_w = (bar_w - (m_numSegments - 1) * m_gapWidth) / m_numSegments;
        int start_segment = (full_draw || m_lastFilledSegments < 0) ? 0 : m_lastFilledSegments;

        // Add new segments
        if (currentFilledSegments > start_segment)
        {
            for (int i = start_segment; i < currentFilledSegments; ++i)
            {
                int segment_x = bar_x + i * (segment_w + m_gapWidth);
                m_gfx.fillRect(segment_x, bar_y, segment_w, bar_h, layout.color());
            }
        }

        // Remove old segments
        int end_segment = (full_draw || m_lastFilledSegments < 0) ? m_numSegments : m_lastFilledSegments;
        if (currentFilledSegments < end_segment)
        {
            for (int i = currentFilledSegments; i < end_segment; ++i)
            {
                int segment_x = bar_x + i * (segment_w + m_gapWidth);
                m_gfx.fillRect(segment_x, bar_y, segment_w, bar_h, layout.bkcolor());
            }
        }
    }
};

class DashSegmentIndicator : public IDashIndicator
{
public:
    DashSegmentIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader)
        : IDashIndicator(gfx, reader),
          m_lastValue(-1) {}    
          
    void draw(const DashIndicatorLayout &layout, bool full_draw = false) override
    {
        int value = m_reader.read();
        if (value < 0) value = 0;
        if (value > 999) value = 999;

        if (value == m_lastValue && !full_draw)
        {
            return;
        }

        if (full_draw)
        {
            m_gfx.fillRect(layout.x(), layout.y(), layout.w(), layout.h(), layout.bkcolor());
        }

        std::array<int, 3> digits;
        digits[0] = value / 100;
        digits[1] = (value / 10) % 10;
        digits[2] = value % 10;

        int digits_count = 3;
        for (auto &d : digits)
        {
            if (d == 0)
            {
                d = -1; // blank leading zeros
                --digits_count;
            } else
                break;
        }

        std::array<int, 3> last_digits = {-1, -1, -1};
        if (m_lastValue != -1)
        {
            last_digits[0] = m_lastValue / 100;
            last_digits[1] = (m_lastValue / 10) % 10;
            last_digits[2] = m_lastValue % 10;
        }

        int seg_thickness = layout.w() / 17; // (3 * 5) for digits + 2 for gaps
        if (seg_thickness < 1) seg_thickness = 1;
        int digit_w = 5 * seg_thickness;
        int gap_w = seg_thickness;

        // Calculate the total width of visible digits and center them horizontally
        int total_digits_width = digits_count * digit_w + (digits_count - 1) * gap_w;
        int start_x_offset = (layout.w() - total_digits_width) / 2;

        // Find the starting index for visible digits (skip leading zeros marked as -1)
        int visible_digit_index = 0;
        for (int i = 0; i < 3; ++i)
        {
            if (digits[i] != -1)
            {
                if ((full_draw || digits[i] != last_digits[i]))
                {
                    draw_digit(layout, digits[i],
                        layout.x() + start_x_offset + visible_digit_index * (digit_w + gap_w),
                        layout.y(), digit_w, layout.h(), seg_thickness);
                }
                visible_digit_index++;
            }
        }

        m_lastValue = value;
    }

private:
    int m_lastValue;
    static const uint8_t s_segmentMap[10];    void draw_digit(const DashIndicatorLayout &layout, int digit, int x, int y, int w, int h, int seg_thickness)
    {
        uint8_t segments = s_segmentMap[digit];
        // Erase the digit area first
        m_gfx.fillRect(x, y, w, h, layout.bkcolor());

        int seg_len_h = w - 2 * seg_thickness;
        int seg_len_v = (h - 3 * seg_thickness) / 2;

        // A (top horizontal)
        if (segments & 0b0000001) draw_h_segment(layout, x + seg_thickness, y, seg_len_h, seg_thickness);
        // B (top-right vertical)
        if (segments & 0b0000010) draw_v_segment(layout, x + w - seg_thickness, y + seg_thickness, seg_thickness, seg_len_v);
        // C (bottom-right vertical)
        if (segments & 0b0000100) draw_v_segment(layout, x + w - seg_thickness, y + 2 * seg_thickness + seg_len_v, seg_thickness, seg_len_v);
        // D (bottom horizontal)
        if (segments & 0b0001000) draw_h_segment(layout, x + seg_thickness, y + h - seg_thickness, seg_len_h, seg_thickness);
        // E (bottom-left vertical)
        if (segments & 0b0010000) draw_v_segment(layout, x, y + 2 * seg_thickness + seg_len_v, seg_thickness, seg_len_v);
        // F (top-left vertical)
        if (segments & 0b0100000) draw_v_segment(layout, x, y + seg_thickness, seg_thickness, seg_len_v);
        // G (middle horizontal)
        if (segments & 0b1000000) draw_h_segment(layout, x + seg_thickness, y + seg_thickness + seg_len_v, seg_len_h, seg_thickness);
    }    void draw_h_segment(const DashIndicatorLayout &layout, int x, int y, int w, int h)
    {
        m_gfx.fillRect(x, y, w, h, layout.color());
    }

    void draw_v_segment(const DashIndicatorLayout &layout, int x, int y, int w, int h)
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