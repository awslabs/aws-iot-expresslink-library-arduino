#include <ExpressLink.h>

ExpressLink el;

void setup()
{
  // USB Host
  Serial.begin(115200);

  // ExpressLink
  Serial1.end();
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.setFIFOSize(4096);
  Serial1.begin(ExpressLink::BAUDRATE);
  if (!el.begin(Serial1, 10, 11, 12))
  {
    Serial.println("Failed to initialize ExpressLink.");
    while (true) { ;; }
  }

  Serial.println("Setup complete.");
}

void loop()
{
  Serial.println("This sample only works on devices / DevKits with two separate serial connections: one for the USB host and one for the ExpressLink module.");
  Serial.println("Some DevKits and Arduino boards connect the ExpressLink directly to the USB Host serial pins which might cause transmission issues.");
  Serial.println("ExpressLink passthrough activated.");

  el.passthrough();

  Serial.println("This line is never reached.");
}
