// Final Project Sensor & Measurement | Biomedical Device Network | Signal & System
// Dave Danadiva Agusta Peerera | BE-4 | 12006002


// Template ID, Device Name and Auth Token are provided by the Blynk.Cloud
// See the Device Info tab, or Template settings
#define BLYNK_TEMPLATE_ID "TMPLpKXTy9M0"
#define BLYNK_DEVICE_NAME "LED Switch"
#define BLYNK_AUTH_TOKEN "toISQm1I-u0Sgbvv1Og2xmeVIrYwjqir"


// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial


#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;

// WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Rukita_Laterra_2B";
char pw[] = "hasslefree66";

//char ssid[] = "SGU-LAB";
//char pw[] = "sgunumber1";

WidgetLED led0(V0);

#define ledPin 15         // What digital pin is the LED
#define DHTPIN 4         // What digital pin is the DHT

// Type of DHT used
#define DHTTYPE DHT11     // DHT 11

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

// Configuration for Deep Sleep
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// Control Virtual Pin
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable

  // process received value

  if (pinValue > 0){
    digitalWrite(ledPin, HIGH);
  }
  else {
    digitalWrite(ledPin, LOW);
  }
  
}


void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V3, t);
  Blynk.virtualWrite(V2, f);
  
//-------------------------------- Binning Humidity--------------------------------------
  if (h < 40){
    Blynk.virtualWrite(V4, "Too Dry");
  }
  else if (h > 60) {
    Blynk.virtualWrite(V4, "Too Humid");
  }
  else {
    Blynk.virtualWrite(V4, "Comfort Zone");
  }
//-------------------------------- Binning Temp--------------------------------------
  if (t <25 ){
    Blynk.virtualWrite(V5, "Cold Temperature");
  }

  else if (t >= 25 & t <= 35){
    Blynk.virtualWrite(V5, "Temperate");
  }

  else {
    Blynk.virtualWrite(V5, "Hot Temperature");
  }
  
}


void myTimerEvent()
{
  if (bootCount == 1){ 
    Blynk.virtualWrite(V7, millis() / 1000);
  }

  else {
    Blynk.virtualWrite(V7, (60 * (bootCount-1) +  millis() / 1000));
  }
  Blynk.virtualWrite(V8, bootCount);
}


void setup()
{
  // Debug console
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  
  ++bootCount;
  
  Blynk.begin(auth, ssid, pw);

  dht.begin();

// Condition in which esp will on and sleep
  while(millis()/1000 < 60){
    
    // Setup a function to be called every second
    timer.setInterval(2000L, sendSensor);
    timer.setInterval(1000L, myTimerEvent);
  
    Blynk.run();
    timer.run();

  }

  // Start Deep Sleep
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();

}

void loop()
{
  //No Code
}
