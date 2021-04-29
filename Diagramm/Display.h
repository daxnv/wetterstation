#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <array>
#include <functional>

class ts { //Touchscreen
public:
    static void check(); //liest den Status des Touchscreens
private:
    static enum Selection {None, Prev, Next} last;
    static void change(Selection s); //ändert die Anzeige entsprechend des Parameters
};

class ds { //Display
public:
    static void next(); //verändern den Index
    static void prev(); // " "
    static void refresh(); //aktualisiert die Anzeige mit neusten Daten
private:
    static std::array<std::function<void()>, 2> displays; //die Liste der verfügbaren Anzeigen
    static int curr; //Index der aktuellen Anzeige
};

#endif //_DISPLAY_H
