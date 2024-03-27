#include "lvgl.h"
#include "demos/lv_demos.h"
#include "display/fbdev.h"
#include "indev/evdev.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <ArduinoJson.h>

#include "config.h"
#include "serialPort.hpp"
#include "mock_pb_ui.h"
#include "widgets/vertical_bar.h"
#include "widgets/numericalValue.h"
#include "widgets/gauge.h"
#include "widgets/graph.h"
#include "dash.hpp"
#include "util.h"
#include "vesc.h"
#include "mqtt.hpp"

#define DISP_BUF_SIZE (128 * 1024)

SerialPort *serial;

uint8_t serBuf[4096] = {0};
size_t serBufPos = 0;

std::string curIpStr, curSsidStr;
std::mutex mutexIpSsid;
size_t curMqttQueued;

Mqtt mq;

ValuesSetupPackage_t vals;

// dirty hacked together serial handling. TODO: make better
void handleSerial() {
    int available = serial->available();
    if (available + serBufPos < sizeof(serBuf)) {
        serial->readBytes(serBuf + serBufPos, available);
        serBufPos += available;
    }

    std::string serBufStr((char*)serBuf, serBufPos);
    if (serBufStr.find('\n') != std::string::npos) {

        curMqttQueued = mq.getQueued();

        JsonDocument doc;
        deserializeJson(doc, serBuf);

        if (doc["system"] == "vesc" && doc["type"] == "setup_values") {
            vals = doc["data"];
            dashUpdate(vals.batteryLevel, vals.tempMosfet, vals.inpVoltage, vals.inputCurrent, vals.speed, vals.rpm, vals.tempMotor, vals.distance, vals.motorCurrent, vals.wattHours);
        }

        // forward JSON packet to MQTT
        mq.publishJson(doc);

        memset(serBuf, 0, sizeof(serBuf));
        serBufPos = 0;
    }
}

// dirty hacked together IP/SSID polling. TODO: make better
void updateIpSsidStrThread() {
        while (true) {
        // Update the global string
        {
            std::lock_guard<std::mutex> lock(mutexIpSsid);
            // globalString = "Updated string at " + std::to_string(std::time(nullptr));
            curIpStr = Util::getIPAddress();
            curSsidStr = Util::getCurrentSSID();
        }

        // Sleep for 1 second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

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

    serial = new SerialPort(serialPortPath, serialPortBaud);

    // const char testStr[] = "asdf1234!...";
    // printf("Serial bytes available before sending: %d\n", serial.available());
    // serial.writeBytes((uint8_t*)testStr, sizeof(testStr));
    // usleep(100000);
    // printf("Serial bytes available after sending: %d\n", serial.available());
    // uint8_t rcvBuf[serial.available()] = {0};
    // serial.readBytes(rcvBuf, serial.available());
    // printf("Serial bytes received: %s\n", rcvBuf);



    lv_theme_t * dark = lv_theme_default_init(lv_disp_get_default(), lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
    dark->font_small = &lv_font_montserrat_10;
    dark->font_large = &lv_font_montserrat_28;
    // lv_disp_set_theme(lv_disp_get_default(), dark);

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

    dashInit(cont);

    std::cout << "IP addr: " << Util::getIPAddress() << std::endl;
    std::cout << "SSID: " << Util::getCurrentSSID() << std::endl;
    // GPS Fix etc

    // Start a thread to periodically update SSID / IP
    std::thread updaterThread(updateIpSsidStrThread);



    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_timer_handler();
        usleep(5000);
        handleSerial();
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