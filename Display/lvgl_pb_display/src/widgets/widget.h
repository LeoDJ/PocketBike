#pragma once

#include "lvgl.h"

class Widget {
  public:
    Widget(lv_obj_t *parentGrid, uint8_t xPos, uint8_t yPos, uint8_t xSpan = 1, uint8_t ySpan = 1)
        : _parent(parentGrid), _xPos(xPos), _yPos(yPos), _xSpan(xSpan), _ySpan(ySpan) {};
    void setValue(float val);
    virtual void draw() = 0;

  protected:
    lv_obj_t *_parent;
    uint8_t _xPos, _xSpan, _yPos, _ySpan;
};