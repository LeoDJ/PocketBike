#include "lvgl.h"
#include "demos/lv_demos.h"
#include "display/fbdev.h"
#include "indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#include "mock_pb_ui.h"
#include "widgets/vertical_bar.h"
#include "widgets/numericalValue.h"
#include "widgets/gauge.h"
#include "widgets/graph.h"

#define DISP_BUF_SIZE (128 * 1024)

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_draw_buf_t disp_buf;
    lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = fbdev_flush;
    disp_drv.hor_res    = 960;
    disp_drv.ver_res    = 160;
    lv_disp_drv_register(&disp_drv);

    // evdev_init();
    // static lv_indev_drv_t indev_drv_1;
    // lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
    // indev_drv_1.type = LV_INDEV_TYPE_POINTER;

    /*This function will be called periodically (by the library) to get the mouse position and state*/
    // indev_drv_1.read_cb = evdev_read;
    // lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);


    /*Set a cursor for the mouse*/
    // LV_IMG_DECLARE(mouse_cursor_icon)
    // lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
    // lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
    // lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/


    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_benchmark();
    // lv_demo_stress();

    // lv_theme_t * mono = lv_theme_mono_init(lv_disp_get_default(), true, lv_theme_default_get()->font_normal);
    // lv_disp_set_theme(lv_disp_get_default(), mono);



    // mock_pb_ui();

    lv_theme_t * dark = lv_theme_default_init(lv_disp_get_default(), lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    dark->font_small = &lv_font_montserrat_10;
    dark->font_large = &lv_font_montserrat_28;
    lv_disp_set_theme(lv_disp_get_default(), dark);

    // mock_pb_ui();

    // TODO: make widget manager or sth
    const int column_segments = 24;
    const int row_segments = 4;

    lv_coord_t col_dsc[column_segments + 1];
    lv_coord_t row_dsc[row_segments + 1];

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


    VerticalBar *bat = new VerticalBar(cont, 0, 0, 1, 4);
    bat->init("Batt.");
    bat->draw();
    bat->setValue(90);

    NumericalValue *batVolt = new NumericalValue(cont, 1, 0, 3, 1);
    batVolt->init("Batt\nVolt");
    batVolt->draw();
    batVolt->setValue(3.9*12);

    Gauge *speed = new Gauge(cont, 4, 0, 4, 4);
    speed->init(0, 50, 10, 2, 30, "km/h");
    speed->draw();
    speed->setValue(33.3);

    Graph *graph = new Graph(cont, 8, 0, 8, 2);
    graph->init(60, 
        {.minVal = -60,  .maxVal = 0,   .majorIncrementValue = 10, .minorIncrementNumber = 3, .axisUnitText = "s"}, 
        {.minVal = -100, .maxVal = 100, .majorIncrementValue = 50, .minorIncrementNumber = 1, .axisUnitText = "A"}, 
        {.minVal = 0,    .maxVal = 50,  .majorIncrementValue = 25, .minorIncrementNumber = 1, .axisUnitText = "km/h"});
    graph->draw();

    for (int i = 0; i < 60; i++) {
        graph->setValue(i*i/32-50, i*i/32);
    }




    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}

extern "C" {

/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}

}