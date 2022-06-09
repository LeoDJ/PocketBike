#pragma once

#include "widget.h"
#include "stdio.h"

// TODO: get implementations out of here
class Graph : public Widget {
  public:
    typedef struct {
        lv_coord_t minVal;
        lv_coord_t maxVal;
        lv_coord_t majorIncrementValue; // step size of major ticks
        lv_coord_t minorIncrementNumber; // number minor ticks between major ticks
        const char *axisUnitText = "";
    } graphAxisConfig_t;

    Graph(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1) : Widget(parentGrid, xPos, yPos, xSpan, ySpan) {}

    void init(uint16_t pointCount, graphAxisConfig_t xAxisCfg, graphAxisConfig_t y1AxisCfg) {
        _pointCount = pointCount;
        _xAxis = xAxisCfg;
        _y1Axis = y1AxisCfg;
        
        _y1ZeroTickNumber = _y1Axis.maxVal / _y1Axis.majorIncrementValue;
    }

    void init(uint16_t pointCount, graphAxisConfig_t xAxisCfg, graphAxisConfig_t y1AxisCfg, graphAxisConfig_t y2AxisCfg) {
        _y2Axis = y2AxisCfg;
        _useSecondaryAxis = true;
        init(pointCount, xAxisCfg, y1AxisCfg);
    }

    void setValue(float val1, float val2 = 0) {
        lv_chart_set_next_value(_chart, _y1Series, val1);
        if (_useSecondaryAxis) {
            lv_chart_set_next_value(_chart, _y2Series, val2);
        }
    }

    void draw() {
        lv_obj_t *container = lv_obj_create(_parent);
        lv_obj_set_grid_cell(container, LV_GRID_ALIGN_STRETCH, _xPos, _xSpan, LV_GRID_ALIGN_STRETCH, _yPos, _ySpan);
        lv_obj_set_style_pad_all(container, 3, LV_PART_MAIN);

        // Create chart
        _chart = lv_chart_create(container);
        lv_obj_align(_chart, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_size(_chart, lv_pct(100), lv_pct(100));          // Expand chart up until the padding (defined below)
        lv_obj_set_style_pad_all(_chart, 0, LV_PART_MAIN);          // Remove padding around actual data area
        lv_chart_set_type(_chart, LV_CHART_TYPE_LINE);
        lv_obj_set_style_size(_chart, 0, LV_PART_INDICATOR);        // No dots
        lv_obj_set_style_text_font(_chart, _legendFont, LV_PART_TICKS);
        lv_chart_set_point_count(_chart, _pointCount);
        lv_chart_set_range(_chart, LV_CHART_AXIS_PRIMARY_Y, _y1Axis.minVal, _y1Axis.maxVal);
        if (_useSecondaryAxis) {
            lv_chart_set_range(_chart, LV_CHART_AXIS_SECONDARY_Y, _y2Axis.minVal, _y2Axis.maxVal);
        }
        lv_chart_set_div_line_count(_chart, _calcMajorTickNumber(_y1Axis), _calcMajorTickNumber(_xAxis));
        lv_obj_add_event_cb(_chart, _overridePartDrawing, LV_EVENT_DRAW_PART_BEGIN, this);  // Register callback for graph customizations (units and thicker zero line)

        // Add series
        _y1Series = lv_chart_add_series(_chart, lv_theme_default_get()->color_primary, LV_CHART_AXIS_PRIMARY_Y);
        if (_useSecondaryAxis) {
            _y2Series = lv_chart_add_series(_chart, lv_theme_default_get()->color_secondary, LV_CHART_AXIS_SECONDARY_Y);
        }

        // Add ticks and labels
        lv_chart_set_axis_tick(_chart, LV_CHART_AXIS_PRIMARY_X, _majorTickLen, _minorTickLen, _calcMajorTickNumber(_xAxis), _xAxis.minorIncrementNumber + 1, true, 100);
        lv_chart_set_axis_tick(_chart, LV_CHART_AXIS_PRIMARY_Y, _majorTickLen, _minorTickLen, _calcMajorTickNumber(_y1Axis), _y1Axis.minorIncrementNumber + 1, true, 100);
        if (_useSecondaryAxis) {
            lv_chart_set_axis_tick(_chart, LV_CHART_AXIS_SECONDARY_Y, _majorTickLen, _minorTickLen, _calcMajorTickNumber(_y2Axis), _y2Axis.minorIncrementNumber + 1, true, 100);
        }

        // Calculate size of legend to position the graph correctly, otherwise the ticks and labels would be drawn outside of the container
        lv_point_t xLabelSize = _getSizeOfAxisLabel(_xAxis, _legendFont);
        lv_obj_set_style_pad_bottom(container, xLabelSize.y + _majorTickLen, LV_PART_MAIN);
        lv_obj_set_style_pad_right(container, xLabelSize.x / 2, LV_PART_MAIN); 

        lv_coord_t additionalPadding = lv_obj_get_style_radius(container, LV_PART_MAIN) / 2 + _majorTickLen;
        lv_coord_t y1LegendWidth = _getMaxWidthOfAxisLabel(_y1Axis, _legendFont) + additionalPadding;
        lv_obj_set_style_pad_left(container, y1LegendWidth, LV_PART_MAIN);
        if (_useSecondaryAxis) {
            lv_coord_t y2LegendWidth = _getMaxWidthOfAxisLabel(_y2Axis, _legendFont) + additionalPadding;
            lv_obj_set_style_pad_right(container, y2LegendWidth, LV_PART_MAIN);
        }
    }


  protected:
    uint16_t _pointCount;
    graphAxisConfig_t _xAxis, _y1Axis, _y2Axis;
    bool _useSecondaryAxis = false;

    lv_obj_t *_chart;
    lv_chart_series_t *_y1Series, *_y2Series;

    int32_t _y1ZeroTickNumber;  // the n-th tick (counted from the top) that is the zero line

    const lv_coord_t _majorTickLen = 4;
    const lv_coord_t _minorTickLen = 2;
    const lv_font_t *_legendFont = lv_theme_default_get()->font_small;



    static inline lv_coord_t _calcMajorTickNumber(graphAxisConfig_t axis) { return (axis.maxVal - axis.minVal) / axis.majorIncrementValue + 1; }

    static void _overridePartDrawing(lv_event_t *e) {
        lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
        Graph *_graph = (Graph *)e->user_data;  // need to pass the instance as a parameter, because callbacks need to be static

        if (lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) {                 // tick label drawing override
            if (dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
                int32_t newVal = _graph->_xAxis.minVal + dsc->value * _graph->_xAxis.majorIncrementValue;
                int offset = lv_snprintf(dsc->text, dsc->text_length, "%d", newVal);
                lv_snprintf(dsc->text + offset, dsc->text_length - offset, _graph->_xAxis.axisUnitText);        // Print X axis unit
            }

            if (dsc->id == LV_CHART_AXIS_PRIMARY_Y) {
                if (dsc->text) {
                    int offset = lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value);  
                    lv_snprintf(dsc->text + offset, dsc->text_length - offset, _graph->_y1Axis.axisUnitText);   // Print Y axis unit
                }
                if (dsc->label_dsc) {
                    dsc->label_dsc->color = lv_color_darken(lv_theme_default_get()->color_primary, 4);
                }
            }

            if (dsc->id == LV_CHART_AXIS_SECONDARY_Y && _graph->_useSecondaryAxis) {
                if (dsc->text) {
                    int offset = lv_snprintf(dsc->text, dsc->text_length, "%d", dsc->value);  
                    lv_snprintf(dsc->text + offset, dsc->text_length - offset, _graph->_y2Axis.axisUnitText);  // Print secondary Y axis unit
                }
                if (dsc->label_dsc) {
                    dsc->label_dsc->color = lv_color_darken(lv_theme_default_get()->color_secondary, 4);
                }
            }
        }
        else if (lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_DIV_LINE_HOR)) {          // horizontal line drawing override
            // Make horizontal zero line thicker
            if(dsc->type == LV_CHART_DRAW_PART_DIV_LINE_HOR) {
                if (dsc->id == _graph->_y1ZeroTickNumber)
                    dsc->line_dsc->width = 3;   // TODO: custom line thickness?
                else 
                    dsc->line_dsc->width = 1;
            }
        }
    }

    static lv_coord_t _getMaxWidthOfAxisLabel(graphAxisConfig_t axis, const lv_font_t *font) {
        lv_coord_t maxWidth = 0;
        char buf[64];
        for (int i = 0; i < _calcMajorTickNumber(axis); i++) {
            int offset = snprintf(buf, sizeof(buf), "%d", axis.minVal + i * axis.majorIncrementValue);
            snprintf(buf + offset, sizeof(buf) - offset, axis.axisUnitText);
            lv_coord_t width = lv_txt_get_width(buf, strlen(buf), font, 0, LV_TEXT_FLAG_NONE);
            maxWidth = LV_MAX(maxWidth, width);
        }
        return maxWidth;
    }

    // used for X axis (shouldn't need a loop)
    static lv_point_t _getSizeOfAxisLabel(graphAxisConfig_t axis, const lv_font_t *font) {
        lv_point_t p;
        char buf[64];
        int offset = snprintf(buf, sizeof(buf), "%d", axis.maxVal);
        snprintf(buf + offset, sizeof(buf) - offset, axis.axisUnitText);
        lv_txt_get_size(&p, buf, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        return p;
    }
};