#pragma once

#include "widget.h"


// Forward declaration (see below class)
static void updateNeedleValue_cb(void * var, int32_t v);


// TODO: get implementations out of here
class Gauge : public Widget {
  public:
    Gauge(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1) : Widget(parentGrid, xPos, yPos, xSpan, ySpan) {}

    void init(int minVal, int maxVal, int majorTickIncrements, int minorTickIncrements, int redAreaBegin, const char* unitText, const char* valueFmt = "%4.1f", int fraction = 100) {
        _minValue = minVal * fraction;
        _maxValue = maxVal * fraction;
        _majorTickIncrements = majorTickIncrements * fraction;
        _minorTickIncrements = minorTickIncrements * fraction;
        _redAreaBegin = redAreaBegin * fraction;
        _unitText = unitText;
        _valueFmt = valueFmt;
        _fraction = fraction;

        lv_anim_init(&_anim);
        lv_anim_set_exec_cb(&_anim, (lv_anim_exec_xcb_t)updateNeedleValue_cb);
        lv_anim_set_var(&_anim, this);  // pass this instance to updateNeedleValue_cb for animation
        lv_anim_set_time(&_anim, UPDATE_RATE);
        lv_anim_set_path_cb(&_anim, lv_anim_path_ease_in_out);
    }

    void updateNeedleValue(int32_t value) {
        lv_meter_set_indicator_value(_meter, _needle, value);
    }

    void setValue(float val) {
        int valInt = (val * _fraction) + 0.5; // round before converting to int
        
        lv_meter_set_indicator_value(_meter, _needle, _anim.end_value); // snap needle to previous end position in case updates come in faster than the animation duration
        lv_anim_set_values(&_anim, _needle->start_value, valInt);
        lv_anim_start(&_anim);

        if (_valueFmt && _valueLabel) {
            lv_label_set_text_fmt(_valueLabel, _valueFmt, val);
        }
    }

    void draw() {
        _meter = lv_meter_create(_parent);
        lv_obj_set_grid_cell(_meter, LV_GRID_ALIGN_STRETCH, _xPos, _xSpan, LV_GRID_ALIGN_STRETCH, _yPos, _ySpan);
        lv_obj_set_style_pad_all(_meter, 3, LV_PART_MAIN);

        // Scale (numbers and tick marks)
        _scale = lv_meter_add_scale(_meter);
        int angleRange = 270;
        lv_meter_set_scale_range(_meter, _scale, _minValue, _maxValue, angleRange, 90 + (360 - angleRange) / 2);     // set range of scale and calculate rotation so it's centered
        int range = _maxValue - _minValue;
        int majorTickCount = (range / _majorTickIncrements) + 1;
        int minorTickCount = (range / _minorTickIncrements) + 1;
        lv_meter_set_scale_ticks(_meter, _scale, minorTickCount, 2, 10, lv_palette_main(LV_PALETTE_GREY));
        lv_color_t textColor = lv_obj_get_style_text_color(_meter, LV_PART_MAIN);
        lv_meter_set_scale_major_ticks(_meter, _scale, _majorTickIncrements / _minorTickIncrements, 4, 15, textColor, 12);

        lv_meter_indicator_t *indic;
        // Add red arc to end of range
        if (_redAreaBegin) {
            indic = lv_meter_add_arc(_meter, _scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
            lv_meter_set_indicator_start_value(_meter, indic, _redAreaBegin);
            lv_meter_set_indicator_end_value(_meter, indic, _maxValue);

            // Make tick lines red
            indic = lv_meter_add_scale_lines(_meter, _scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
            lv_meter_set_indicator_start_value(_meter, indic, _redAreaBegin);
            lv_meter_set_indicator_end_value(_meter, indic, _maxValue);
        }

        // Unit label
        if (_unitText) {
            lv_obj_t * unitLabel = lv_label_create(_meter);
            lv_obj_align(unitLabel, LV_ALIGN_BOTTOM_MID, 0, LV_PCT(-30));
            lv_obj_set_style_text_font(unitLabel, lv_theme_default_get()->font_small, LV_PART_MAIN);
            lv_obj_set_style_text_align(unitLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
            lv_label_set_text(unitLabel, _unitText);
        }

        // Numerical value
        if (_valueFmt) {
            _valueLabel = lv_label_create(_meter);
            lv_obj_set_style_text_font(_valueLabel, lv_theme_default_get()->font_large, LV_PART_MAIN);
            lv_obj_align(_valueLabel, LV_ALIGN_BOTTOM_MID, 0, 0);
        }

        // Needle
        _needle = lv_meter_add_needle_line(_meter, _scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);

        // Draw callback to override "multiplied" tick numbers due to fraction handling
        lv_obj_add_event_cb(_meter, _meter_cb, LV_EVENT_DRAW_PART_BEGIN, &_fraction);
    }

    static void _meter_cb(lv_event_t * e) {
        lv_obj_draw_part_dsc_t* dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
        int fraction = *(int*)lv_event_get_user_data(e);

        if (dsc->text != NULL) { // Filter major ticks...
            snprintf(dsc->text, sizeof(dsc->text), "%d", dsc->value / fraction);
        }
    }

  protected:
    const char *_unitText, *_valueFmt;
    int32_t _minValue, _maxValue;
    // int _majorTicks, _minorTicks;
    int _majorTickIncrements, _minorTickIncrements;
    int _redAreaBegin;
    int _fraction; // multiplier for values so gauge can display more increments between integers

    lv_obj_t *_meter;
    lv_obj_t *_valueLabel;
    lv_meter_indicator_t *_needle;
    lv_meter_scale_t *_scale;
    lv_anim_t _anim;    // animation for smoother needle movement


};

// Handler for the lv_anim_cb format (can't take 3 arguments)
static void updateNeedleValue_cb(void * var, int32_t v) {
    Gauge* gauge = (Gauge*)var;
    gauge->updateNeedleValue(v);
}