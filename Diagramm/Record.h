#ifndef _RECORD_H
#define _RECORD_H

#include <BME280I2C.h>
#include <Wire.h>
#include <deque>

extern BME280I2C bme;
void init_bme();

struct Record {
    float temp, press, humid;
};

class rc { //Record
public:
    static constexpr uint8_t recordsize = 30; //Maximale länge der Aufzeichnung

    static void measure(); //Misst mit bme
    static void add(Record r);
    static Record at(int i);
    static int length();
    static Record average(); //Die Werte sind nicht unbedingt zur gleichen Zeit entstanden
    static Record max();     //*
    static Record min();     //*
private:
    static std::deque<Record> records;
};

#endif //_RECORD_H
