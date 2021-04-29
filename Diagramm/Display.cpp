#include "Display.h"
#include "Record.h"
#include "Graphics.h"
#include "Plot.h"
#include "MultiGradient.h"

using namespace std;

ts::Selection ts::last;

void ts::check() {
    uint16_t x = 320, y = 240; //Liegt im Nirvana
    tft.getTouch(&x,&y);
    Selection now;
    if (x < 40) { //Überprüfen, ob der Punkt auf den Pfeilen liegt
        if (y < 40)
            now = Prev;
        else if (y > 200)
            now = Next;
    }

    if (last != now) //Wenn der Fingerdruck sich vom Display löst
        change(last);
    
    last = now;
}

void ts::change(Selection s) {
    if (s == Next)
        ds::next();
    else if (s == Prev)
        ds::prev();
}

void ds::next() {
    if (++curr >= displays.size())
        curr = 0;
    tft.fillScreen(TFT_BLACK);
    refresh();
}

void ds::prev() {
    if (--curr < 0)
        curr = displays.size() - 1;
    tft.fillScreen(TFT_BLACK);
    refresh();
}

void ds::refresh() {
    gx::drawButtons();
    displays[curr]();
}

int ds::curr = 0;

void overview();
void history();

array<function<void()>, 2> ds::displays = {
    overview,
    history
};

//Farben für die Balken
#define BLUE {0,0,255}
#define GREEN {0,255,0}
#define RED {255,0,0}
#define YELLOW {255,255,0}
#define MAGENTA {255,0,255}
#define CYAN {0,255,255}
#define WHITE {255,255,255}

const MultiGradient TG({BLUE, GREEN, RED});
const MultiGradient PG({MAGENTA, BLUE, GREEN, RED});
const MultiGradient HG({WHITE, CYAN, BLUE});

void single_overview(const char name[], function<void(int)> unit, float aver, float last, float second, const char fill[]) {
    //tx::tab() -- vorher gemacht
      tft.print(name); tft.print(": "); tx::average(); unit(aver); tft.println(fill); //Erste Zeile mit Durchschnitt
    tx::tab();
      if (last < second) //Zweite Zeile mit Wert und Tendenz
        tx::tendencyDown();
      else
        tx::tendencyUp();
     unit(last); tft.println(fill);
}

void overview() {
    Record last = rc::at(rc::length() - 1);
    Record second = rc::at(rc::length() - 2);
    Record aver = rc::average();

    tft.setTextSize(2);
    tft.setCursor(0,0);

    tx::tab(5);
    single_overview("Temperatur", tx::celsius, aver.temp, last.temp, second.temp, "  ");
    bar::draw(tft.getCursorY()+5, bar::mapx({-20, 50}, last.temp), TG.map({-20, 50}, last.temp).convert565().bytes());

    tx::tab(30);
    single_overview("Luftdruck", tx::hpascal, aver.press, last.press, second.press, " ");
    bar::draw(tft.getCursorY()+5, bar::mapx({900,1100}, last.press), PG.map({900,1100}, last.press).convert565().bytes());

    tx::tab(30);
    single_overview("Feuchtigkeit", tx::percent, aver.humid, last.humid, second.humid, "  ");
    bar::draw(tft.getCursorY()+5, bar::mapx({0,100}, last.humid), HG.map({0,100}, last.humid).convert565().bytes());
}

//Farben für die Graphen
#define TC TFT_RED
#define PC TFT_GREEN
#define HC TFT_BLUE

void single_legend(const char name, function<void(int)> unit, float min, float max, const char fill[], uint16_t line_c, uint16_t text_c) {
    tx::tab();

    tft.print(name); tft.print(": ");
    unit(min);
    tft.setTextColor(line_c, tft.textbgcolor); tft.print("--");
    tft.setTextColor(text_c, tft.textbgcolor); unit(max);
    tft.println(fill);
}

void history() {
    Record max = rc::max();
    Record min = rc::min();
    uint32_t text_c = tft.textcolor;

    tft.setTextSize(2);
    tft.setCursor(0, 5);
    
    single_legend('T', tx::celsius, min.temp, max.temp, "   ", TC, text_c);

    single_legend('D', tx::hpascal, min.press, max.press, " ", PC, text_c);

    single_legend('F', tx::percent, min.humid, max.humid, "  ", HC, text_c);

    std::vector<int> temp, press, humid;
    for (int i = 0; i < rc::length(); ++i) { //Die drei Werte je in eine eigene Liste entzippen
        temp.emplace_back(rc::at(i).temp);
        press.emplace_back(rc::at(i).press);
        humid.emplace_back(rc::at(i).humid);
    }
    gph::drawAxis();
    gph::drawGraph({-20, 120}, temp, TC);
    gph::drawGraph({-200, 1200}, press, PC);
    gph::drawGraph({-20, 120}, humid, HC);
}
