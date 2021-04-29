#include "Graphics.h"

TFT_eSPI tft;
void init_tft() {
    Serial.println("Initialisiere Display");
    tft.init();
    tft.setRotation(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.fillScreen(TFT_BLACK);
}

namespace gx {
    void drawArrowUp(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
        tft.fillTriangle(x,y+l, x+l,y+l, x+l/2,y, color);
    }

    void drawArrowDown(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
        tft.fillTriangle(x,y, x+l,y, x+l/2,y+l, color);
    }

    void drawButtonUp(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
        tft.drawRect(x,y,l+1,l+1,color);
        drawArrowUp(x, y, l, color);
    }

    void drawButtonDown(uint16_t x, uint16_t y, uint16_t l, uint16_t color) {
        tft.drawRect(x,y,l+1,l+1,color);
        drawArrowDown(x, y, l, color);
    }

    void drawButtons() {
        drawButtonUp(0,0,40,TFT_DARKGREY);
        drawButtonDown(0,199,40,TFT_DARKGREY);
    }

    void drawArrowMore(uint16_t x, uint16_t y, uint16_t l) {
        drawArrowUp(x, y, l, TFT_GREEN);
        x += 2, y+=4, l -= 4;
        drawArrowUp(x, y, l, TFT_BLACK);
    }

    void drawArrowLess(uint16_t x, uint16_t y, uint16_t l) {
        drawArrowDown(x, y, l, TFT_RED);
        x += 1, y-=2, l -= 2;
        drawArrowDown(x, y, l, TFT_BLACK);
    }
}

namespace tx {
    void celsius(int degrees) {
        tft.print(degrees);

        int x = tft.getCursorX();   //Überschreibe Hintergrund für Gradzeichen
        tft.print(' ');
        tft.setCursor(x, tft.getCursorY());

        char size = tft.textsize;
        tft.setTextSize(size-1);
        tft.print(" o");
        tft.setTextSize(size);
        tft.print('C');
    }

    void hpascal(int hpascal) {
        tft.print(hpascal);
        tft.print("hPa");
    }

    void percent(int percent) {
        tft.print(percent);
        tft.print('%');
    }

    void average() {
        //sammle Daten
        int x = tft.getCursorX();
        int y = tft.getCursorY();
        int fontWidth = tft.textWidth(" ");
        //Platz zum Zeichnen
        tft.print("  ");
        //Zeichnen
        tft.drawCircle(x + fontWidth/2, y + tft.fontHeight()/2, fontWidth/2, tft.textcolor);
        tft.drawLine(x, y + tft.fontHeight(), x + fontWidth, y, tft.textcolor);
    }

    void tendencyUp() {
        int x = tft.getCursorX();
        int y = tft.getCursorY();

        tft.print(' ');
        char size = tft.textsize;
        tft.setTextSize(size-1);
        tft.print(' ');

        tft.setTextSize(size);
        gx::drawArrowMore(x, y + tft.fontHeight() - tft.textWidth(" ") - size, tft.textWidth(" ") - size);
    }

    void tendencyDown() {
        int x = tft.getCursorX();
        int y = tft.getCursorY();

        tft.print(' ');
        char size = tft.textsize;
        tft.setTextSize(size-1);
        tft.print(' ');

        tft.setTextSize(size);
        gx::drawArrowLess(x, y + tft.fontHeight() - tft.textWidth(" ") - size, tft.textWidth(" ") - size);
    }

    void tab(int down) {
        tft.setCursor(gx::left_bound, tft.getCursorY() + down);
    }
}
