#include "M5Atom.h"
#include "UDPIO.h"

void setup()
{
  M5.begin(true, true, true);
  udpMode = SEND;  
  setupWifi();
}

void loop()
{
  M5.update();
  loopUdp(true); 
}
