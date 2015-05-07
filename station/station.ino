#include <SoftwareSerial.h>
#include <DHT.h>

#define DEBUG

#ifdef DEBUG
 #define ECHOLN(x)  monitor.println (x)
 #define ECHO(x)  monitor.print (x)
#else
 #define ECHOLN(x)
 #define ECHO(x)
#endif

//#define SSID "ssidname"
//#define PASS "wifipassword"

#define IP "184.106.153.149"
//#define API_KEY "thingsspeak password"
#define GET "GET"


#define DHTPIN A0     // what pin we're connected to
#define DHTTYPE DHT22   // DHT type

#define LOOP_INTERVAL 300000
#define AVG_INTERVAL 2000

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial monitor(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5000);
  //test if the module is ready
  #ifdef DEBUG
    monitor.begin(9600);
  #endif
  Serial.println("AT");
  ECHOLN("Pavel test");
  delay(5000);
  if (Serial.find("OK"))
  {
    ECHOLN("Connecting to wifi");
    connectWiFi();
  }
  else
  {
    ECHOLN("Error");
    while (1);
  }

}

void loop() {
  float dhtValues[3];
  readDHT(&dhtValues[0]);
  updateWeb(dhtValues[0],dhtValues[1]); 
  delay(LOOP_INTERVAL);
}

void readDHT(float *values){
  values[0] = 0;
  values[1] = 0;
  values[2] = 0;
  delay(AVG_INTERVAL);
  readDHTValue(&values[0]);
  delay(AVG_INTERVAL);
  readDHTValue(&values[0]);
  delay(AVG_INTERVAL);
  readDHTValue(&values[0]);
  
  values[0] = values[0]/values[2];
  values[1] = values[1]/values[2];
  ECHO("Temp: ");
  ECHO(values[0]);
  ECHO("°C, Humidity: ");
  ECHO(values[1]);
  ECHO("%");
}

void readDHTValue(float* values){
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity(); 
  if (isnan(temp) || isnan(humidity)) {
    ECHOLN("Failed to read from DHT sensor!");
    return;
  }
  values[0] += temp;
  values[1] += humidity;
  values[2]++;
   
}


boolean connectWiFi(){
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
    ECHOLN("RECEIVED: OK");
    return true;
  }else{
    ECHOLN("RECEIVED: Error");
    return false;
  }
}

void updateWeb(float tenmp, float humidity){
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += IP;
  cmd += "\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    ECHOLN("RECEIVED: Error");
    return;
  }
  cmd = GET;
  cmd += " /update?key=";
  cmd += API_KEY;
  cmd += "&field1=";
  cmd += tenmp;
  cmd += "&field2=";
  cmd += humidity;
  cmd += "\r\n";
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    ECHO(">");
    ECHO(cmd);
    Serial.print(cmd);
  }else{
    Serial.println("AT+CIPCLOSE");
  }
  if(Serial.find("OK")){
    ECHOLN("RECEIVED: OK");
  }else{
    ECHOLN("RECEIVED: Error");
  }
}
