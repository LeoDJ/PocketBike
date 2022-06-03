#include "mock_pb_ui.h"

static lv_obj_t * meter_speed;

static void chart_draw_labels_event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%ds", -60 + dsc->value * 10);  // Print X axis labels. TODO: make dynamic
    }

    if(dsc->id == LV_CHART_AXIS_PRIMARY_Y && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%dA", dsc->value);  // Print Y axis unit
    }

    if(dsc->id == LV_CHART_AXIS_SECONDARY_Y && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%dkm/h", dsc->value);  // Print Y axis unit
    }
}

void mock_pb_ui() {
    // lv_obj_t * cont = lv_obj_create(lv_scr_act());
    // lv_obj_set_size(cont, 300, 220);
    // lv_obj_center(cont);
    // lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    static lv_style_t style_less_padding;
    lv_style_init(&style_less_padding);
    lv_style_set_pad_all(&style_less_padding, 3);
    
    static lv_style_t style_invisible;
    lv_style_init(&style_invisible);
    lv_color_t bg_color = lv_obj_get_style_bg_color(lv_scr_act(), LV_PART_MAIN);
    lv_style_set_border_color(&style_invisible, bg_color);
    lv_style_set_bg_color(&style_invisible, bg_color);
    

    static lv_coord_t col_dsc[] = {LV_GRID_FR(4), LV_GRID_FR(8), LV_GRID_FR(6), LV_GRID_FR(6), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_FR(12), LV_GRID_FR(12), LV_GRID_TEMPLATE_LAST};
    
    lv_obj_t * cont = lv_scr_act();
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(cont, 2, 0);
    lv_obj_set_style_pad_row(cont, 2, 0);
    lv_obj_set_style_pad_all(cont, 2, 0);




    // Speed meter
    meter_speed = lv_meter_create(cont);
    lv_obj_set_grid_cell(meter_speed, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 2);
    lv_obj_set_size(meter_speed, 150, 150);
    lv_obj_add_style(meter_speed, &style_less_padding, 0);

    lv_meter_scale_t * scale = lv_meter_add_scale(meter_speed);
    lv_meter_set_scale_range(meter_speed, scale, 0, 50, 270, 90 + (360 - 270) / 2);
    lv_meter_set_scale_ticks(meter_speed, scale, 50 / 2 + 1, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(meter_speed, scale, 5, 4, 15, lv_color_black(), 12);

    lv_meter_indicator_t * indic;
    /*Add a red arc to the end*/
    indic = lv_meter_add_arc(meter_speed, scale, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter_speed, indic, 30);
    lv_meter_set_indicator_end_value(meter_speed, indic, 50);

    /*Make the tick lines red at the end of the scale*/
    indic = lv_meter_add_scale_lines(meter_speed, scale, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter_speed, indic, 30);
    lv_meter_set_indicator_end_value(meter_speed, indic, 50);

    // Add actual needle indicator
    indic = lv_meter_add_needle_line(meter_speed, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    // lv_meter_set_indicator_value(meter_speed, indic, 100);




    // Chart
    lv_obj_t * cont_chart = lv_obj_create(cont);
    lv_obj_set_style_pad_all(cont_chart, 0, 0);
    // lv_obj_set_style_opa(cont_chart, 0, 0);
    lv_obj_add_style(cont_chart, &style_invisible, LV_PART_MAIN);
    

    lv_obj_t * chart = lv_chart_create(cont_chart);
    lv_obj_set_grid_cell(cont_chart, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_center(chart);
    lv_obj_align(chart, LV_ALIGN_TOP_MID, -5, 5);
    lv_obj_set_size(chart, lv_pct(65), lv_pct(75));
    
    lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);   // removes padding around actual data
    // lv_obj_set_style_pad_row(chart, 20, 0);
    // lv_obj_set_style_pad_column(chart, 20, 0);
    // lv_obj_set_size(chart, chart->coords.x2 - chart->coords.x1 - 50, chart->coords.y2 - chart->coords.y1 - 20);
    // lv_obj_set_pos(chart, -25, 0);
    // lv_obj_refr_pos(chart);
    // lv_obj_set_style_pad_hor(chart, 10, LV_PART_MAIN);
    // lv_obj_align(chart, LV_ALIGN_CENTER, -5, -5);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR); // no dots
    lv_obj_set_style_text_font(chart, &lv_font_montserrat_10, LV_PART_TICKS);
    lv_chart_set_point_count(chart, 60);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -100, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 50);
    lv_chart_set_div_line_count(chart, 5, 7);
    lv_obj_add_event_cb(chart, chart_draw_labels_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    lv_color_t color_current = lv_palette_main(LV_PALETTE_RED);
    lv_color_t color_speed = lv_palette_main(LV_PALETTE_BLUE);
    lv_chart_series_t * ser_current = lv_chart_add_series(chart, color_current, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser_speed = lv_chart_add_series(chart, color_speed, LV_CHART_AXIS_SECONDARY_Y);

    // add ticks and labels. TODO: place inside grid cell
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 4, 2, 7, 4, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 4, 2, 5, 2, true, 20);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 4, 2, 3, 5, true, 50);

    // lv_obj_refresh_ext_draw_size(chart);
    

    /*Set the next points on 'ser1'*/
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 10);
    lv_chart_set_next_value(chart, ser_current, 30);
    lv_chart_set_next_value(chart, ser_current, 70);
    lv_chart_set_next_value(chart, ser_current, 90);

    /*Set the next points on 'ser1'*/
    lv_chart_set_next_value(chart, ser_speed, 1);
    lv_chart_set_next_value(chart, ser_speed, 2);
    lv_chart_set_next_value(chart, ser_speed, 1);
    lv_chart_set_next_value(chart, ser_speed, 3);
    lv_chart_set_next_value(chart, ser_speed, 3);
    lv_chart_set_next_value(chart, ser_speed, 4);
    lv_chart_set_next_value(chart, ser_speed, 3);
    lv_chart_set_next_value(chart, ser_speed, 7);
    lv_chart_set_next_value(chart, ser_speed, 9);
    lv_chart_set_next_value(chart, ser_speed, 15);
    lv_chart_set_next_value(chart, ser_speed, 50);




    lv_obj_t * obj;

    obj = lv_obj_create(cont);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);

    obj = lv_obj_create(cont);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    obj = lv_obj_create(cont);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // obj = lv_obj_create(cont);
    // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    obj = lv_obj_create(cont);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 1, 1);





    // obj = lv_obj_create(cont);
    // lv_obj_set_size(obj, 40, 40);           /*Fix size*/

    // obj = lv_obj_create(cont);
    // lv_obj_set_height(obj, 80);
    // lv_obj_set_flex_grow(obj, 1);           /*1 portion from the free space*/

    // obj = lv_obj_create(cont);
    // lv_obj_set_height(obj, 80);
    // lv_obj_set_flex_grow(obj, 1);           /*1 portion from the free space*/

    // obj = lv_obj_create(cont);
    // lv_obj_set_height(obj, 80);
    // lv_obj_set_flex_grow(obj, 2);           /*2 portion from the free space*/

    // obj = lv_obj_create(cont);
    // lv_obj_set_size(obj, 40, 40);           /*Fix size. It is flushed to the right by the "grow" items*/
}