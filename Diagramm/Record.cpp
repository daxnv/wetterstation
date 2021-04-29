#include "Record.h"

using namespace std;

BME280I2C bme;
void init_bme() {
    Serial.println("Initialisiere BME");
    Wire.begin(3,1);
    while(!bme.begin())
    {
        Serial.println("BME nicht gefunden!");
        delay(1000);
    }
}

std::deque<Record> rc::records = {
    {20,1000,50},
    {22,1001,60},
    {23,1002,72},
    {24,1034,40},
    {23,1002,45},
    {25,1000,43}
    //,{50,1100,100}
    //,{-20,900,0}
};

void rc::add(Record r) {
    if (records.size() >= recordsize) //Löscht alte Werte, wenn die Liste zu lang wird
        records.pop_front();
    records.emplace_back(r);
}

void rc::measure() {
    Record r;
    bme.read(r.press, r.temp, r.humid);
    add(r);
}

Record rc::at(int i) {
    if (i < 0 || records.size() - 1 < i) //Bei Zugriff außerhalb der Grenzen
        return {NAN,NAN,NAN};
    return records[i];
}

int rc::length() {
    return records.size();
}

Record rc::average() {
    Record acc{0,0,0};
    for (auto &r : records) {
        acc.temp += r.temp;
        acc.press += r.press;
        acc.humid += r.humid;
    }
    acc.temp /= records.size();
    acc.press /= records.size();
    acc.humid /= records.size();

    return acc;
}

Record rc::max() {
    Record max = at(0);
    for (auto &r : records) {
        if (max.temp < r.temp)
            max.temp = r.temp;
        if (max.press < r.press)
            max.press = r.press;
        if (max.humid < r.humid)
            max.humid = r.humid;
    }
    return max;
}

Record rc::min() {
    Record min = at(0);
    for (auto &r : records) {
        if (min.temp > r.temp)
            min.temp = r.temp;
        if (min.press > r.press)
            min.press = r.press;
        if (min.humid > r.humid)
            min.humid = r.humid;
    }
    return min;
}
