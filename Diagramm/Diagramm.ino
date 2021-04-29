#include "Record.h"
#include "Graphics.h"
#include "Display.h"
#include "Cloud.h"
#include "Range.h"
#include <PolledTimeout.h>

using namespace std;

using esp8266::polledTimeout::periodicFastMs;
periodicFastMs upload(20000);
periodicFastMs actualize(1000);

void setup() {
  //Serial.begin(115200);
  init_wifi();
  init_cloud();
  init_tft();
  init_bme();
}

void loop() {
  /*
  for (int i = 0; i < 20; ++i) {
    rc::add({
      map({-20,50}, {0,20}, i),
      map({900,1100}, {0,20}, i),
      map({0,100}, {0,20}, i)
    });
    ds::refresh();
    check_for(500);
  }
  */
  if (upload) {
    cl::send(rc::average());
  }
  if (actualize) {
    rc::measure();
    ds::refresh();
  }
  ts::check();
}
