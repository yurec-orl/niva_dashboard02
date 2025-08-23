#pragma once

#include <Arduino.h>

#include "DashGfxWrapper.hpp"
#include "DashSensorReader.hpp"

class IDashIndicator
{
public:
    IDashIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader, int x, int y, int w, int h, int color, int bkcolor)
        : m_gfx(gfx)
        , m_reader(reader)
        , m_x(x)
        , m_y(y)
        , m_w(w)
        , m_h(h)
        , m_color(color)
        , m_bkcolor(bkcolor)
        {}
    
    virtual void draw(bool full_draw = false) = 0;
protected:
    int m_x;
    int m_y;
    int m_w;
    int m_h;
    int m_color;
    int m_bkcolor;

    IDashGfxWrapper &m_gfx;
    IDashSensorReader &m_reader;
};

class DashVBarIndicator: public IDashIndicator
{
public:
    DashVBarIndicator(IDashGfxWrapper &gfx, IDashSensorReader &reader, int x, int y, int w, int h,
        int color, int bkcolor, const char* label, int labelSz = 2, int borderW = 2)
        : IDashIndicator(gfx, reader, x, y, w, h, color, bkcolor),
        m_label(label), m_labelSz(labelSz), m_borderW(borderW) {}

    void draw(bool full_draw = false) override
    {
        // Calculate bar area (leave space for label below)
        int label_height = m_gfx.chHeight(m_labelSz);
        int bar_x = m_x + m_borderW;
        int bar_y = m_y + m_borderW;
        int bar_w = m_w - 2 * m_borderW;
        int bar_h = m_h - label_height - 2 * m_borderW;

        if (full_draw) {
            m_gfx.fillRect(m_x, m_y, m_w, m_h, m_bkcolor);
            // Draw border using two filled rectangles
            m_gfx.fillRect(m_x, m_y, m_w, m_h - label_height, m_color); // outer border
            m_gfx.fillRect(m_x + m_borderW, m_y + m_borderW, m_w - 2 * m_borderW, m_h - label_height - 2 * m_borderW, m_bkcolor); // erase inside
        }

        // Get sensor value and normalize
        int val = 256;//m_reader.read();
        int minVal = m_reader.minVal();
        int maxVal = m_reader.maxVal();
        if (maxVal == minVal) maxVal = minVal + 1; // avoid div by zero

        float frac = float(val - minVal) / float(maxVal - minVal);
        if (frac < 0) frac = 0;
        if (frac > 1) frac = 1;

        int fill_h = int(bar_h * frac + 0.5f);

        // Draw bar fill
        if (fill_h > 0)
            m_gfx.fillRect(bar_x, bar_y + bar_h - fill_h, bar_w, fill_h, m_color);

        if (full_draw) {
            int label_y = m_y + m_h - label_height;
            int label_w = m_gfx.textWidth((char*)m_label, m_labelSz);
            int label_x = m_x + (m_w - label_w) / 2;
            m_gfx.textWrite(label_x, label_y, m_labelSz, m_color, m_bkcolor, m_label);
        }
    }
private:
    const char* m_label;
    const int m_labelSz;
    int m_borderW;
};