#include <TinyGPS++.h> 
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
String myAPIKey = "0LHOGWV12DIUNCMI";
#define DHTPIN A1
#define DHTTYPE    DHT11

TinyGPSPlus gps;
 
#define S_RX    12                // Define software serial RX pin
#define S_TX    11                // Define software serial TX pin
 
SoftwareSerial ss(S_RX, S_TX); 

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;
SoftwareSerial ser(7, 8); // RX, 
SoftwareSerial cer(9, 10);
SoftwareSerial ldrpir(9,2); /////doubt

#define DHT11_PIN 4
int humi,temp;
float humidity,temp_f;
long writingTimer=17;
long startTimer=0;
long waitTime=0;

int ldr = A0;
int ldr_reading = 0; 
int ldr_thresold = 800;
int ldr_value=0;
int pir_value=0;
int pir;
float temp_value;
float hum_value;
float my_lat,my_lon;
float sys_lat,sys_lon;
float diff_lat_gps,diff_lon_gps;

boolean relay1_st=false;
boolean relay2_st=false;
unsigned char check_connection=0;
unsigned char times_check=0;
boolean error;

String voice;
void setup() {
Serial.begin(9600);
ss.begin(9600);  
ser.begin(9600);
cer.begin(9600);
ldrpir.begin(9600);
pinMode(13,INPUT);

pinMode(6, OUTPUT);
pinMode(5, OUTPUT);
pinMode(4, OUTPUT);
pinMode(3, OUTPUT);
pinMode(2, OUTPUT);
 dht.begin();
 ser.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  ser.println(F("------------------------------------"));
  ser.println(F("Temperature Sensor"));
  ser.print  (F("Sensor Type: ")); ser.println(sensor.name);
  ser.print  (F("Driver Ver:  ")); ser.println(sensor.version);
  ser.print  (F("Unique ID:   ")); ser.println(sensor.sensor_id);
  ser.print  (F("Max Value:   ")); ser.print(sensor.max_value); ser.println(F("°C"));
  ser.print  (F("Min Value:   ")); ser.print(sensor.min_value); ser.println(F("°C"));
  ser.print  (F("Resolution:  ")); ser.print(sensor.resolution); ser.println(F("°C"));
  ser.println(F("------------------------------------"));
 // Print humidity sensor details.
 dht.humidity().getSensor(&sensor);
 ser.println(F("Humidity Sensor"));
 ser.print  (F("Sensor Type: ")); ser.println(sensor.name);
 ser.print  (F("Driver Ver:  ")); ser.println(sensor.version);
 ser.print  (F("Unique ID:   ")); ser.println(sensor.sensor_id);
 ser.print  (F("Max Value:   ")); ser.print(sensor.max_value); ser.println(F("%"));
 ser.print  (F("Min Value:   ")); ser.print(sensor.min_value); ser.println(F("%"));
 ser.print  (F("Resolution:  ")); ser.print(sensor.resolution); ser.println(F("%"));
 ser.println(F("------------------------------------"));
////  // Set delay between sensor readings based on sensor details.
 delayMS = sensor.min_delay / 1000;
 cer.println (F("Location Details:"));
 cer.println(F("------------------------------------"));
 Serial.print(F("Give latitude and longitude location of your phone"));
 my_lat=Serial.read();
 my_lon=Serial.read();
}

void loop() {

while(Serial.available()){
  ldr_reading=analogRead(ldr);
  ldrpir.print(F("The value of LDR reading is....."));
  ldrpir.print(ldr_reading);
  
  
  pir=digitalRead(13);


  sensors_event_t events;
  dht.temperature().getEvent(&events);
  temp_value=events.temperature;

  
  

  sys_lat=gps.location.lat();
  sys_lon=gps.location.lng();

  if(sys_lat>my_lat)
  {
    diff_lat_gps=sys_lat-my_lat;
  }
  else
  {
    diff_lat_gps=my_lat-sys_lat;
  }

  if(sys_lon>my_lon)
  {
    diff_lon_gps=sys_lon-my_lon;
  }
  else
  {
    diff_lon_gps=my_lon-sys_lon;
  }
  
  if(ldr_reading>ldr_thresold)
  {
    ldr_value=1;
  }
  else
  {
    ldr_value=0;
  }

  if(pir==HIGH)
  {
    ldrpir.print(F("Motion is detected"));
    pir_value=1;
  }
  else
  {
    ldrpir.print(F("Motion is not detected"));
    pir_value=0;
  }
  


  
  delay(3);
  char c = Serial.read();
  voice+=c;}

if(voice.length() >0){
  Serial.println(voice);
  if(voice == "light on" || (pir_value==1 && ldr_value==0))
  {digitalWrite(6, HIGH);}
  else if(voice == "light off" || pir_value==0 || ldr_value==1)
  {digitalWrite(6, LOW);}
  else if(voice == "fan on" || pir_value==1)
  {digitalWrite(5, HIGH);}
  else if(voice == "fan off" || pir_value==0)
  {digitalWrite(5, LOW);}
  else if(voice == "yellow night lamp on")
    {digitalWrite(4,HIGH);}
    else if(voice == "yellow night lamp off")
    {digitalWrite(4,LOW);}
    else if(voice == "ac on" || (pir_value==1 && temp_value>27) || (temp_value>27 && (diff_lat_gps<0.0055 || diff_lon_gps<0.0045)))
    {digitalWrite(3,HIGH);}
    else if(voice == "ac off" || (pir_value==0 || temp_value<15))
    {digitalWrite(3,LOW);}
    else if(voice == "heater on" || (pir_value==1 && temp_value<=15) || (temp_value<=15 && (diff_lat_gps<0.0055 || diff_lon_gps<0.0045)))
    {digitalWrite(2,HIGH);}
    else if(voice == "heater off" || (pir_value==0 || temp_value>=27))
    {digitalWrite(2,LOW);}
    else if(voice == "read temperature")
    {
      sensors_event_t event;
      dht.temperature().getEvent(&event);
      if (isnan(event.temperature)) {
        ser.println(F("Error reading temperature!"));
        }
      else {
        ser.print(F("Temperature: "));
        ser.print(event.temperature);
        ser.println(F("°C"));
        }
    }
    else if(voice == "read humidity")
    {
      sensors_event_t event;
      dht.humidity().getEvent(&event);
      if (isnan(event.relative_humidity)) {
          ser.println(F("Error reading humidity!"));
       }
      else {
          ser.print(F("Humidity: "));
          ser.print(event.relative_humidity);
          ser.println(F("%"));
       }
    }
    
    
    else if(voice == "read location")
    {
        if (gps.location.isValid()) {
        cer.print("Latitude   = ");
        cer.println(gps.location.lat(), 6);
        cer.print("Longitude  = ");
        cer.println(gps.location.lng(), 6);}
       else
        cer.println("Location Invalid");
  
    }
   
    else if(voice == "all on")
    {
      digitalWrite(6,HIGH);
      digitalWrite(5,HIGH);
      digitalWrite(4,HIGH);
      digitalWrite(2,HIGH);
      digitalWrite(3,HIGH);
    }
    else if(voice == "all off")
    {
      digitalWrite(6,LOW);
      digitalWrite(5,LOW);
      digitalWrite(4,LOW);
      digitalWrite(2,LOW);
      digitalWrite(3,LOW);
    }
    
  voice = "";}
}
