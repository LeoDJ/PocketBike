#include "widget.h"
#include <stdio.h>

// TODO: get implementations out of here
class VerticalBar : public Widget {
  public:
    VerticalBar(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1) : Widget(parentGrid, xPos, yPos, xSpan, ySpan) {}

    void init(const char *title, const char *valueFmt = "%2.0f%%") {
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
        lv_obj_set_style_pad_all(container, 3, LV_PART_MAIN);               // decrease padding
        lv_obj_set_style_opa(container, LV_OPA_TRANSP, LV_PART_SCROLLBAR);  // hide scrollbars
        lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);               // enable flex layout
        lv_obj_set_style_pad_row(container, 3, LV_PART_MAIN);               // decrease padding between objects in flex layout
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

        _gradient.dir = LV_GRAD_DIR_VER;
        _gradient.stops_count = 4;
        _gradient.stops[0].color = _colors[0];
        _gradient.stops[1].color = _colors[1];
        _gradient.stops[2].color = _colors[1];
        _gradient.stops[3].color = _colors[2];
        _gradient.stops[0].frac = ((100 - _color2BeginVal) * 255 / 100);
        _gradient.stops[1].frac = ((100 - _color2BeginVal + _gradientSize) * 255 / 100);
        _gradient.stops[2].frac = ((100 - _color3BeginVal - _gradientSize) * 255 / 100);
        _gradient.stops[3].frac = ((100 - _color3BeginVal) * 255 / 100);
        lv_obj_set_style_bg_grad(_bar, &_gradient, LV_PART_INDICATOR);

        // Numerical value
        if (_valueFmt) {
            _valueLabel = lv_label_create(container);
            lv_label_set_text_fmt(_valueLabel, _valueFmt, 0);
        }
    }

  protected:
    const char *_title;
    const char *_valueFmt;

    lv_obj_t *_bar;
    lv_obj_t *_valueLabel;
    lv_grad_dsc_t _gradient;

    int _color2BeginVal = 30;
    int _color3BeginVal = 10;
    int _gradientSize = 5;
    lv_color_t _colors[3] = { lv_palette_main(LV_PALETTE_GREEN),
                              lv_palette_main(LV_PALETTE_YELLOW),
                              lv_palette_main(LV_PALETTE_RED) };
    int _minVal = 0;
    int _maxVal = 100;
};