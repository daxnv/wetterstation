#include "Plot.h"
#include "Graphics.h"

using namespace std;
using namespace gx;

namespace bar {
    void draw(int y, int dx, uint16_t color) {
        tft.fillRect(left_bound, y, dx, 20, color); //Der eigentliche Balken
        tft.fillRect(left_bound+dx, y, right_bound-left_bound-dx, 20, TFT_BLACK); //Schwarze Überdeckung falls der Balken vorher größer war
        tft.drawFastHLine(left_bound+dx, y+19, right_bound-left_bound-dx, TFT_WHITE);  //Der weiße Strich zur Orientierung
    }

    int mapx(Range origin, int value) {
        return map({1, right_bound - left_bound}, origin, value);
    }
}

namespace gph {
    constexpr uint16_t upper_bound = 10 + 16*3;
    constexpr uint16_t lower_bound = 235;

    void drawAxis() {
        int zero = map({lower_bound,upper_bound}, {0,7}, 1); //Höhe der x-Achse

        tft.fillRect(left_bound + 3, upper_bound, right_bound-left_bound - 3, zero-upper_bound, TFT_BLACK); //Hintergrund übermalen
        tft.fillRect(left_bound + 3, zero + 1, right_bound-left_bound - 3, lower_bound-zero, TFT_BLACK);

        tft.drawFastVLine(left_bound + 2, upper_bound, lower_bound-upper_bound+2, TFT_WHITE); //y-Achse
        tft.fillTriangle(left_bound + 2, upper_bound, left_bound, upper_bound + 5, left_bound + 4, upper_bound + 5, TFT_WHITE); //Pfeilpitze

        tft.drawFastHLine(left_bound, zero, right_bound-left_bound, TFT_WHITE); //x-Achse
        tft.fillTriangle(right_bound, zero, right_bound - 5, zero - 2, right_bound - 5, zero + 2, TFT_WHITE); //Pfeilspitze

        //Zeichnet sieben Striche von -20n bis 120n ein, wobei n 1°C bzw. 10hPa bzw. 1% entspricht
        for (int i = 0; i < 7; ++i) {
            tft.drawFastHLine(left_bound,
                              map({lower_bound,upper_bound}, {0,7}, i),
                              5, TFT_WHITE);
        }
    }

    void drawGraph(Range origin, std::vector<int> values, uint32_t color) {
        //Zeichnet immer Striche von Wert zu Wert
        //Die Menge der Werte bestimmt die Verzerrung in x-Richtung
        for (int i = 1; i < values.size(); ++i) {
            tft.drawLine(map({left_bound+2, right_bound}, {0, values.size()}, i-1), //x-Position, gemappt von der Länge der Werteliste
                         map({lower_bound, upper_bound}, origin, values[i-1]), //Höhe des ersten Punktes je nach 'origin'

                         map({left_bound+2, right_bound}, {0, values.size()}, i), //Zweiter Punkt
                         map({lower_bound, upper_bound}, origin, values[i]),
                         
                         color);
        }
    }
}
