#include "mock_pb_ui.h"

static lv_obj_t * meter_speed;

static const lv_palette_t color_primary_y = LV_PALETTE_RED;
static const lv_palette_t color_secondary_y = LV_PALETTE_BLUE;



static void chart_draw_labels_event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        lv_snprintf(dsc->text, dsc->text_length, "%ds", -60 + dsc->value * 10);  // Print X axis labels. TODO: make dynamic
    }

    if(dsc->id == LV_CHART_AXIS_PRIMARY_Y) {
        if (dsc->text) {
            lv_snprintf(dsc->text, dsc->text_length, "%dA", dsc->value);  // Print Y axis unit
        }
        if (dsc->label_dsc) {
            dsc->label_dsc->color = lv_color_darken(lv_theme_default_get()->color_primary, 4);
        }
    }

    if(dsc->id == LV_CHART_AXIS_SECONDARY_Y) {
        if (dsc->text) {
            lv_snprintf(dsc->text, dsc->text_length, "%dkm/h", dsc->value);  // Print secondary Y axis unit
        }
        if (dsc->label_dsc) {
            dsc->label_dsc->color = lv_color_darken(lv_theme_default_get()->color_secondary, 4);
        }
    }

}

static void chart_draw_horz_lines_event_cb(lv_event_t * e) {
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_DIV_LINE_HOR)) return;

    // make zero line thicker
    if(dsc->type == LV_CHART_DRAW_PART_DIV_LINE_HOR) {
        if (dsc->id == 2)   // TODO: make dynamic
            dsc->line_dsc->width = 2;
        else 
            dsc->line_dsc->width = 1;
    }
}

void mock_pb_ui() {    

    static lv_style_t style_less_padding;
    lv_style_init(&style_less_padding);
    lv_style_set_pad_all(&style_less_padding, 3);
    
    // static lv_style_t style_invisible;
    // lv_style_init(&style_invisible);
    // lv_style_set_bg_opa(&style_invisible, LV_OPA_TRANSP);
    // lv_style_set_border_opa(&style_invisible, LV_OPA_TRANSP);
    


    const int column_segments = 24;
    const int row_segments = 4;

    static lv_coord_t col_dsc[column_segments + 1];
    static lv_coord_t row_dsc[row_segments + 1];

    for (int i = 0; i < column_segments; i++) {
        col_dsc[i] = LV_GRID_FR(1);
    }
    col_dsc[column_segments] = LV_GRID_TEMPLATE_LAST;
    for (int i = 0; i < row_segments; i++) {
        row_dsc[i] = LV_GRID_FR(1);
    }
    row_dsc[row_segments] = LV_GRID_TEMPLATE_LAST;
    
    lv_obj_t * cont = lv_scr_act();
    lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(cont, 2, 0);
    lv_obj_set_style_pad_row(cont, 2, 0);
    lv_obj_set_style_pad_all(cont, 2, 0);




    // Speed meter
    meter_speed = lv_meter_create(cont);
    lv_obj_set_grid_cell(meter_speed, LV_GRID_ALIGN_CENTER, 0, 4, LV_GRID_ALIGN_CENTER, 0, 4);
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

    // unit label
    lv_obj_t * label_kmh = lv_label_create(meter_speed);
    lv_obj_align(label_kmh, LV_ALIGN_BOTTOM_MID, 0, LV_PCT(-30));
    lv_obj_set_style_text_font(label_kmh, lv_theme_default_get()->font_small, LV_PART_MAIN);
    lv_label_set_text(label_kmh, "km/h");

    // digital reading
    lv_obj_t * label_speed = lv_label_create(meter_speed);
    lv_obj_set_style_text_font(label_speed, lv_theme_default_get()->font_large, LV_PART_MAIN);
    lv_obj_align(label_speed, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_label_set_text_fmt(label_speed, "%4.1f", 42.0);

    // Add actual needle indicator
    indic = lv_meter_add_needle_line(meter_speed, scale, 4, lv_palette_main(LV_PALETTE_GREY), -10);
    lv_meter_set_indicator_value(meter_speed, indic, 42);
    



    // Numerical value
    lv_obj_t * cont_label_current = lv_obj_create(cont);
    lv_obj_set_grid_cell(cont_label_current, LV_GRID_ALIGN_STRETCH, 4, 3, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_set_style_pad_all(cont_label_current, 3, LV_PART_MAIN);
    lv_obj_set_style_opa(cont_label_current, LV_OPA_TRANSP, LV_PART_SCROLLBAR);

    lv_obj_t * label_current = lv_label_create(cont_label_current);
    lv_obj_set_style_text_font(label_current, lv_theme_default_get()->font_large, LV_PART_MAIN);
    lv_obj_align(label_current, LV_ALIGN_LEFT_MID, 0, 0);

    lv_obj_t * label_current_unit = lv_label_create(cont_label_current);
    lv_obj_set_style_text_font(label_current_unit, lv_theme_default_get()->font_small, LV_PART_MAIN);
    lv_obj_set_style_text_align(label_current_unit, LV_TEXT_ALIGN_RIGHT, LV_PART_MAIN);
    lv_obj_align(label_current_unit, LV_ALIGN_BOTTOM_RIGHT, 0, -1);
    lv_label_set_text(label_current_unit, "kWh/\n100km");
    lv_label_set_text_fmt(label_current, "%5.1f", 23.42);





    // Battery meter
    lv_obj_t * cont_battery = lv_obj_create(cont);
    lv_obj_set_grid_cell(cont_battery, LV_GRID_ALIGN_STRETCH, 12, 1, LV_GRID_ALIGN_STRETCH, 0, 4);
    lv_obj_set_style_pad_all(cont_battery, 3, LV_PART_MAIN);
    lv_obj_set_style_opa(cont_battery, LV_OPA_TRANSP, LV_PART_SCROLLBAR);
    lv_obj_set_flex_flow(cont_battery, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont_battery, 3, LV_PART_MAIN);

    // Title
    lv_obj_t * label_battery_title = lv_label_create(cont_battery);
    lv_obj_set_style_text_font(label_battery_title, lv_theme_default_get()->font_small, LV_PART_MAIN);
    lv_label_set_text(label_battery_title, "Batt.");

    // Indicator
    lv_obj_t * bar_battery = lv_bar_create(cont_battery);
    // lv_obj_set_align(bar_battery, LV_ALIGN_BOTTOM_MID);
    // lv_obj_set_size(bar_battery, LV_PCT(95), LV_PCT(80));
    lv_obj_set_width(bar_battery, LV_PCT(95));
    lv_obj_set_flex_grow(bar_battery, 1);
    lv_obj_set_style_radius(bar_battery, 3, LV_PART_MAIN);
    lv_obj_set_style_radius(bar_battery, 3, LV_PART_INDICATOR);
    lv_bar_set_value(bar_battery, 90, LV_ANIM_ON);

    // Numerical value
    lv_obj_t * label_battery_value = lv_label_create(cont_battery);
    lv_label_set_text_fmt(label_battery_value, "%d%%", 90);

    // Coloring of indicator
    const int bat_yellow_begin = 30;    // %, size of yellow area
    const int bat_red_begin = 10;       // %, size of red area
    const int gradient_size = 5;        // %, overlapping area of gradient

    static lv_grad_dsc_t grad_battery;
    grad_battery.dir = LV_GRAD_DIR_VER;
    grad_battery.stops_count = 4;
    grad_battery.stops[0].color = lv_palette_main(LV_PALETTE_GREEN);
    grad_battery.stops[1].color = lv_palette_main(LV_PALETTE_YELLOW);
    grad_battery.stops[2].color = lv_palette_main(LV_PALETTE_YELLOW);
    grad_battery.stops[3].color = lv_palette_main(LV_PALETTE_RED);
    grad_battery.stops[0].frac = ((100 - bat_yellow_begin) * 255 / 100);
    grad_battery.stops[1].frac = ((100 - bat_yellow_begin + gradient_size) * 255 / 100);
    grad_battery.stops[2].frac = ((100 - bat_red_begin - gradient_size) * 255 / 100);
    grad_battery.stops[3].frac = ((100 - bat_red_begin) * 255 / 100);
    grad_battery.dither = LV_DITHER_GRADIENT;
    lv_obj_set_style_bg_grad(bar_battery, &grad_battery, LV_PART_INDICATOR);
    



    // Chart
    lv_obj_t * cont_chart = lv_obj_create(cont);
    lv_obj_set_style_pad_all(cont_chart, 0, 0);
    lv_obj_set_style_bg_opa(cont_chart, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_opa(cont_chart, LV_OPA_TRANSP, LV_PART_MAIN);
    

    lv_obj_t * chart = lv_chart_create(cont_chart);
    lv_obj_set_grid_cell(cont_chart, LV_GRID_ALIGN_STRETCH, 4, 8, LV_GRID_ALIGN_STRETCH, 0, 2);
    lv_obj_center(chart);
    lv_obj_align(chart, LV_ALIGN_TOP_MID, -5, 3);
    lv_obj_set_size(chart, lv_pct(65), lv_pct(75));
    
    lv_obj_set_style_pad_all(chart, 0, LV_PART_MAIN);   // removes padding around actual data
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR); // no dots
    lv_obj_set_style_text_font(chart, &lv_font_montserrat_10, LV_PART_TICKS);   // TODO: investigate why setting font via (lv_theme_default_get()->font_small) draws text with default font and small font overlayed
    lv_chart_set_point_count(chart, 60);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -100, 100);
    lv_chart_set_range(chart, LV_CHART_AXIS_SECONDARY_Y, 0, 50);
    lv_chart_set_div_line_count(chart, 5, 7);
    lv_obj_add_event_cb(chart, chart_draw_labels_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);
    lv_obj_add_event_cb(chart, chart_draw_horz_lines_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);

    lv_chart_series_t * ser_current = lv_chart_add_series(chart, lv_theme_default_get()->color_primary, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * ser_speed = lv_chart_add_series(chart, lv_theme_default_get()->color_secondary, LV_CHART_AXIS_SECONDARY_Y);

    // add ticks and labels. TODO: place inside grid cell
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_X, 4, 2, 7, 4, true, 40);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_PRIMARY_Y, 4, 2, 5, 2, true, 20);
    lv_chart_set_axis_tick(chart, LV_CHART_AXIS_SECONDARY_Y, 4, 2, 3, 5, true, 50);

    
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

    // obj = lv_obj_create(cont);
    // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 10, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    // obj = lv_obj_create(cont);
    // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 11, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    // obj = lv_obj_create(cont);
    // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 10, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    // // obj = lv_obj_create(cont);
    // // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    // obj = lv_obj_create(cont);
    // lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 11, 1, LV_GRID_ALIGN_STRETCH, 1, 1);





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