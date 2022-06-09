#pragma once

#include "widget.h"

// TODO: get implementations out of here
class NumericalValue : public Widget {
  public:
    NumericalValue(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1) : Widget(parentGrid, xPos, yPos, xSpan, ySpan) {}

    void init(const char* unitText, const char* valueFmt = "%5.1f") {
        _unitText = unitText;
        _valueFmt = valueFmt;
    }

    void setValue(float val) {
        lv_label_set_text_fmt(_valueLabel, _valueFmt, val);
    }

    void draw() {
        lv_obj_t * container = lv_obj_create(_parent);
        lv_obj_set_grid_cell(container, LV_GRID_ALIGN_STRETCH, _xPos, _xSpan, LV_GRID_ALIGN_STRETCH, _yPos, _ySpan);
        lv_obj_set_style_pad_all(container, 3, LV_PART_MAIN);                   // reduce padding
        lv_obj_set_style_opa(container, LV_OPA_TRANSP, LV_PART_SCROLLBAR);      // hide scrollbars

        _valueLabel = lv_label_create(container);
        lv_obj_set_style_text_font(_valueLabel, lv_theme_default_get()->font_large, LV_PART_MAIN);
        lv_obj_align(_valueLabel, LV_ALIGN_LEFT_MID, 0, 0);
        lv_label_set_text(_valueLabel, "");

        if (_unitText) {
            lv_obj_t * unitLabel = lv_label_create(container);
            lv_obj_set_style_text_font(unitLabel, lv_theme_default_get()->font_small, LV_PART_MAIN);
            lv_obj_set_style_text_align(unitLabel, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
            lv_obj_align(unitLabel, LV_ALIGN_BOTTOM_RIGHT, 0, -1);
            lv_label_set_text(unitLabel, _unitText);
        }
    }

  protected:
    const char* _unitText;
    const char* _valueFmt;

    lv_obj_t * _valueLabel;
};
