#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <SPI.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
void init_tft();

namespace gx { //Graphics
    constexpr uint16_t left_bound = 50;
    constexpr uint16_t right_bound = 280;

    void drawButtons();
}

namespace tx { //Text und Symbole
    void celsius(int degrees);
    void hpascal(int hpascal);
    void percent(int percent);
    void average();
    void tendencyUp();
    void tendencyDown();
    void tab(int down = 0);
}

#endif //_GRAPHICS_H
