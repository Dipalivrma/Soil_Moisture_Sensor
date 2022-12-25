#include <WiFi.h>
#include <WiFiClient.h>
#include<ThingSpeak.h>
#include"HTTPClient.h"
#include"time.h"
#define CSE_IP "192.168.20.2"
#define CSE_PORT 5089
#define OM2M_ORGIN "admin:admin"
#define OM2M_MN "/~/in-cse/in-name/"
#define OM2M_AE "Plant_Health_Monitoring_Smart_Watering"
#define OM2M_DATA_CONT "Node-1/Data"
HTTPClient http;
WiFiClient client;

unsigned long myChannel_number=1992437;
const char * myWriteAPIKEY= "3LYJJQVFBMAXNJPZ";
const int sensor_pin = 34; 
int moisture,sensor_analog;
const char * ntpServer = "pool.ntp.org";
char ssid[] = "SAINTGITS";
char passw[] = "saintgitswifi";
int status = WL_CONNECTED;
long int prev_millis = 0;
unsigned long ntime;

unsigned long getTime() 
{ 
  time_t now;
  struct tm timeinfo;
    if (!getLocalTime( & timeinfo)) {
      Serial.println("Failed to obtain time");
      return (0);
    }
    time( & now);
    return now;
}

void setup()
{   
  configTime(0, 0, ntpServer);
   Serial.begin(115200);
     //WiFi.begin(ssid, passw);
  WiFi.begin(ssid, passw);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }
  Serial.println(WiFi.localIP());
  configTime(0, 0, ntpServer);
  // pinMode (15,OUTPUT);
  ThingSpeak.begin(client);
 
  }

void loop()
{
   sensor_analog = analogRead(sensor_pin);
  moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
   Serial.print("Moisture = ");
  Serial.print(moisture);  /* Print Temperature on the serial window */
  Serial.println("%");
  delay(1000);   
  
  ThingSpeak.setField(1,moisture);
  ThingSpeak.writeFields(myChannel_number, myWriteAPIKEY);
  
  if (millis() - prev_millis >= 2000) 
  {
   ntime = getTime();
    String data;
    String server = "http://" + String() + CSE_IP + ":" + String() + CSE_PORT + String() + OM2M_MN;
    data = "[" + String(moisture) + "]";
    Serial.println(data);
     
    http.begin(server + String() + OM2M_AE + "/" + OM2M_DATA_CONT + "/");

    http.addHeader("X-M2M-Origin", OM2M_ORGIN);
    http.addHeader("Content-Type", "application/json;ty=4");
    http.addHeader("Content-Length", "100");
    
  String req_data = String() + "{\"m2m:cin\": {"
      +
      "\"lbl\": \"" + "Esp32 humidity and Temperature" + "\","
      +
      "\"con\": \"" + data + "\","
      +
      "\"cnf\":\"text\""
      +
      "}}";

   int code = http.POST(req_data);
   http.end();
   Serial.println(code);
   prev_millis = millis();
 }

  delay(5000);
}
