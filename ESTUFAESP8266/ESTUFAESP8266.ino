
#define BLYNK_TEMPLATE_ID           "TMPLx-v5R01w"
#define BLYNK_DEVICE_NAME           "Quickstart Device"
#define BLYNK_AUTH_TOKEN            "auxLKH66_2BBgopOVBjOqara6nVja1zj"

#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = BLYNK_AUTH_TOKEN;

char ssid[] = "NAUTEC";
char pass[] = "nautec2022";

BlynkTimer timer;
BLYNK_WRITE(V0)
{
  
  int value = param.asInt();
  Blynk.virtualWrite(V1, value);
}

BLYNK_CONNECTED()
{

  Blynk.setProperty(V3, "offImageUrl", "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations.png");
  Blynk.setProperty(V3, "onImageUrl",  "https://static-image.nyc3.cdn.digitaloceanspaces.com/general/fte/congratulations_pressed.png");
  Blynk.setProperty(V3, "url", "https://docs.blynk.io/en/getting-started/what-do-i-need-to-blynk/how-quickstart-device-was-made");
}

void myTimerEvent()
{
 
  Blynk.virtualWrite(V2, millis() / 1000);
}

void setup()
{

  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);
  
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run();
  
}
