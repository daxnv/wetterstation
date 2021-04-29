#ifndef _PLOT_H
#define _PLOT_H

#include "Range.h"
#include <stdint.h>
#include <vector>

namespace bar { //Bar
    void draw(int y, int dx, uint16_t color);
    int mapx(Range origin, int x); //Hilfe, um richtig auf die richtige Länge 'dx' zu mappen
}

namespace gph { //Graph
    void drawAxis();
    void drawGraph(Range origin, std::vector<int> values, uint32_t color);
}

#endif //_PLOT_H
