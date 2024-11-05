#pragma once
void hdc_init(const int bgcolor, const int fgcolor, const int width, const int high);

void hdc_release();

void hdc_set_pencolor(const int RGB_value);

void hdc_set_pencolor(const unsigned char red, const unsigned char green, const unsigned char blue);

void hdc_cls();

void hdc_base_point(const int x, const int y);