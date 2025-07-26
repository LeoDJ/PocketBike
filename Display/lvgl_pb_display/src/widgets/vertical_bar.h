#pragma once

#include "widget.h"

// TODO: get implementations out of here
class VerticalBar : public Widget {
  public:
    VerticalBar(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1) : Widget(parentGrid, xPos, yPos, xSpan, ySpan) {}

    // pass NULL for valueFmt to disable the numerical value on the bottom completely
    void init(int minVal, int maxVal, const char *title, const char *valueFmt = "%2.0f%%") {
        _minVal = minVal;
        _maxVal = maxVal;
        _title = title;
        _valueFmt = valueFmt;
    }

    void setValue(float val) {
        int valInt = val + 0.5; // round before converting to int
        lv_bar_set_value(_bar, valInt, LV_ANIM_ON);
        lv_label_set_text_fmt(_valueLabel, _valueFmt, val);
    }

    void draw() {
        // Create container
        lv_obj_t *container = lv_obj_create(_parent);
        lv_obj_set_grid_cell(container, LV_GRID_ALIGN_STRETCH, _xPos, _xSpan, LV_GRID_ALIGN_STRETCH, _yPos, _ySpan);
        lv_obj_set_style_pad_all(container, 3, LV_PART_MAIN);               // reduce padding
        lv_obj_set_style_opa(container, LV_OPA_TRANSP, LV_PART_SCROLLBAR);  // hide scrollbars
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);               // enable flex layout
        lv_obj_set_style_pad_row(container, 3, LV_PART_MAIN);               // reduce padding between objects in flex layout
        lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Title
        if (_title) {
            lv_obj_t * titleLabel = lv_label_create(container);
            lv_obj_set_style_text_font(titleLabel, lv_theme_default_get()->font_small, LV_PART_MAIN);
            lv_label_set_text(titleLabel, _title);
        }

        // Indicator Bar
        _bar = lv_bar_create(container);
        lv_obj_set_width(_bar, LV_PCT(95));
        lv_obj_set_flex_grow(_bar, 1);                              // enable flex grow so it fills the remaining space
        lv_obj_set_style_radius(_bar, 3, LV_PART_MAIN);             // make it a bit more blocky
        lv_obj_set_style_radius(_bar, 3, LV_PART_INDICATOR);
        lv_bar_set_range(_bar, _minVal, _maxVal);

        int span = _maxVal - _minVal;
        int color1_pct = (_maxVal - std::get<int>(_colors[1])) * 255 / span;
        int color2_pct = (_maxVal - std::get<int>(_colors[2])) * 255 / span;

        _gradient.dir = LV_GRAD_DIR_VER;
        _gradient.stops_count = 4;
        _gradient.stops[0].color = lv_palette_main(std::get<lv_palette_t>(_colors[0]));
        _gradient.stops[1].color = lv_palette_main(std::get<lv_palette_t>(_colors[1]));
        _gradient.stops[2].color = lv_palette_main(std::get<lv_palette_t>(_colors[1]));
        _gradient.stops[3].color = lv_palette_main(std::get<lv_palette_t>(_colors[2]));
        _gradient.stops[0].frac = color1_pct;
        _gradient.stops[1].frac = color1_pct + _gradientSize;
        _gradient.stops[2].frac = color2_pct - _gradientSize;
        _gradient.stops[3].frac = color2_pct;
        lv_obj_set_style_bg_grad(_bar, &_gradient, LV_PART_INDICATOR);

        // Numerical value
        if (_valueFmt) {
            _valueLabel = lv_label_create(container);
            lv_label_set_text_fmt(_valueLabel, _valueFmt, 0);
            lv_label_set_text(_valueLabel, "");
        }
    }

    // set colors of the bar, starting from the top
    // (first color stop is always 100%)
    void setColors(std::array<std::tuple<lv_palette_t, int>, 3> colors) {
        _colors = colors;
    }

  protected:
    const char *_title;
    const char *_valueFmt;

    lv_obj_t *_bar;
    lv_obj_t *_valueLabel;
    lv_grad_dsc_t _gradient;

    // int _color2BeginVal = 30;
    // int _color3BeginVal = 10;
    int _gradientSize = 15;  // size of the actual gradient band between colors in 1/255
    std::array<std::tuple<lv_palette_t, int>, 3> _colors = {{
        { LV_PALETTE_GREEN,     100 },
        { LV_PALETTE_YELLOW,    30  },
        { LV_PALETTE_RED,       10  }
    }};
    int _minVal = 0;
    int _maxVal = 100;
};