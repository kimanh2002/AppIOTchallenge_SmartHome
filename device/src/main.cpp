
#include < Ardunio.h>
#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "DHT.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"          //https://github.com/tzapu/WiFiManager

#define DHTPIN 5  // Pin D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


#define FIREBASE_HOST "iotapp-da5d9-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyC-S2gxQ6EVqi6LcargcUhWwwGbixl_Txo"
FirebaseData firebaseData;

float h, hO = 0;
float t, tO = 0;    

unsigned long timer = millis();


void setDataMyHome(float h, float t, ) {
  if (Firebase.setFloat(firebaseData, "/Humidity", h))
  {
    Serial.println("PASSED Humidity");
    Serial.print("VALUE: ");
    Serial.print(firebaseData.floatData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.setFloat(firebaseData, "/Temperature", t))
  {
    Serial.println("PASSED Temperature");
    Serial.print("VALUE: ");
    Serial.print(firebaseData.floatData());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setup()
{
  //pinMode(GasPin, INPUT);   
  //pinMode(Motion, INPUT);

  Serial.begin(115200);
  WiFiManager wifiManager;
  //có thểreset các cài đặt cũ bằng cách gọi hàm:
  // wifiManager.resetSettings();

  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    ESP.reset();
    delay(1000);
  }
  Serial.println("connected...yeey :");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println("Start DHT11");
  dht.begin();
}


void loop() {
  if ((unsigned long)(millis() - timer) > 1000) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    //hic = dht.computeHeatIndex(t, h, false);

   // gasSensorAnalog = analogRead(GasPin);

    //isMotion = digitalRead(Motion);

    if (isnan(h) || isnan(t)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  }

  // đẩy giá trị lên firebase
  if (abs(t - tO) > 0.1 || abs(h - hO) > 0.5 ) {
    setDataMyHome(h, t);
    hO = h;
    tO = t;
    //gasSensorAnalog0 = gasSensorAnalog;

  }
}

