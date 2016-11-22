#include "ESP8266WiFi.h"
#include <ESP8266mDNS.h>
#include <WiFiClient.h>
#include <EEPROM.h>

MDNSResponder mdns;
WiFiServer server(80);

const char* ssid = "Cloud Setup";
String st;
bool esidStored;
int webtype;

int WifiStatus = WL_IDLE_STATUS;
IPAddress ip; 


float humdht, tempdht, lum;  // Values read from sensor
// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 2000;              // interval at which to read sensor

#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);


//This function simply checks that a Wifi network has been established
int testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  Serial.print("Wifi Status: ");
  while ( c < 20 ) {
    //if connection is successful, return "20" for success and exit function
    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(0, HIGH);
      return(20);
    } 
    delay(500);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("Connect timed out, opening AP");
  digitalWrite(0, LOW);
  return(10);
} 


void clearEEPROM() {
  for (int i = 0; i < 365; ++i) { EEPROM.write(i, 0); }
  EEPROM.commit();
  return;
}

void readEEPROM() {


}

void writeEEPROM(int debut,String valeur) {
  //Serial.println(valeur);
  Serial.println("");
  for (int i = debut; i < debut+valeur.length(); ++i)
    {
    EEPROM.write(i, valeur[i-debut]);
    Serial.print("Wrote: ");
    Serial.println(valeur[i-debut]); 
    } 
  EEPROM.commit();
  return;
}

//Manage HTTP requests
int mdns1() {
  // Check for any mDNS queries and send responses
  //mdns.update();
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    //Serial.println("No client connected, running the mdns function");
    return(20);
  }
  Serial.println("");
  Serial.println("New client");

  // Wait for data from client to become available
  while(client.connected() && !client.available()){
    delay(1);
   }
  
  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');
  
  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return(20);
   }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush(); 
  String s;
  if ( webtype == 0 ) {
      if (req == "/")
      {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
        s += ipStr;
        s += "<p>";
        s += st;
        s += "<form method='get' action='a'><label>SSID: </label><input name='ssid' length=32><input type=password name='pass' length=128>";
    s += "<br><label>Ip server Domoticz : </label><input name='domIP' length=32><br><label>Port server domoticz</label><input name='domPORT' length=32>";
    s += "<br><label>Login domoticz: </label><input name='domLOGIN' length=32><input type=password name='domPW' length=128>";
    s += "<br><label>IDX domoticz DHT: </label><input name='domIDX1' length=10><label>IDX domoticz Lum: </label><input name='domIDX2' length=10>";
        s += "<br><input type='submit'></form></html>\r\n\r\n";
        Serial.println("Sending 200");
      }
      else if ( req.startsWith("/a?ssid=") ) {
        // /a?ssid=blahhhh&pass=poooo
    int pos;
    
        Serial.println("clearing eeprom");
        clearEEPROM();
        String qsid; 
        qsid = req.substring(req.indexOf('=')+1,req.indexOf('&'));
    pos = req.indexOf('&');
        Serial.println(qsid);
        Serial.println("");
        Serial.println("writing eeprom ssid:");
    writeEEPROM(0,qsid);
    
        String qpass;
    qpass = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);   
        qpass.replace('+', ' ');
        Serial.println(qpass);
        Serial.println("");
        Serial.println("writing eeprom pass:");
    writeEEPROM(32,qpass);    
        
        String qdomIP; 
        qdomIP = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);
        Serial.println(qdomIP);
        Serial.println(""); 
        Serial.println("writing eeprom domoticz IP:");
    writeEEPROM(160,qdomIP);    
    
        String qdomPORT; 
        qdomPORT = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);
        Serial.println(qdomPORT);
        Serial.println(""); 
        Serial.println("writing eeprom domoticz Port:");
    writeEEPROM(175,qdomPORT);    

        String qdomLOGIN; 
        qdomLOGIN = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);
        Serial.println(qdomLOGIN);
        Serial.println("");
        Serial.println("writing eeprom domoticz Login:");
    writeEEPROM(180,qdomLOGIN);

        String qdomPW; 
        qdomPW = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);
        Serial.println(qdomPW);
        Serial.println("");
        Serial.println("writing eeprom domoticz password:");
    writeEEPROM(212,qdomPW);
    
        String qdomIDX1; 
        qdomIDX1 = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));
    pos = req.indexOf('&',pos+1);
        Serial.println(qdomIDX1);
        Serial.println("");
        Serial.println("writing eeprom domoticz IDX DHT:");
    writeEEPROM(340,qdomIDX1);

        String qdomIDX2; 
        qdomIDX2 = req.substring(req.indexOf('=',pos)+1,req.indexOf('&',pos+1));  //req.substring(req.lastindexOf('domIDX2=')+1);
    pos = req.indexOf('&',pos+1); 
        Serial.println(qdomIDX2);
        Serial.println("");
        Serial.println("writing eeprom domoticz IDX lumens:");
    writeEEPROM(345,qdomIDX2);  

    writeEEPROM(350,"00000");
    writeEEPROM(355,"00000");
    writeEEPROM(360,"00000");
        
        esidStored = true;

        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 ";
        s += "Found ";
        s += req;
        s += "<p> saved to eeprom... reset to boot into new wifi</html>\r\n\r\n";

      }
      else
      {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 404");
      }
  } 
  else
  {
      if (req == "/")
      {
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP8266";
        s += "<p>";
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");
      }
      else if ( req.startsWith("/cleareeprom") ) {
        s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ESP8266";
        s += "<p>Clearing the EEPROM<p>";
        s += "</html>\r\n\r\n";
        Serial.println("Sending 200");  
        Serial.println("clearing eeprom");
    clearEEPROM();
        esidStored = false;
        WiFi.disconnect();      //after EEPROM is cleared, disconnect from the current wifi access
        setup();        // restart from setup
      }
      else
      {
        s = "HTTP/1.1 404 Not Found\r\n\r\n";
        Serial.println("Sending 200");
      }       
  }
  client.print(s);
  Serial.println("Done with client");
  return(20);
}

//webtype 0 - AP mode, webtype 1 - client mode
//MDNS only works for Client mode at this time!
void launchWebServer() {
          Serial.println("");
          Serial.println("Starting Web Server...");
          // Start the server
          //AP Mode
          if (webtype == 0){
              Serial.println("AP IP Address: ");
              Serial.println(WiFi.softAPIP());
            
          }
          //Client mode
          else if (webtype == 1){
                Serial.println("Client IP Address ");
                Serial.println(WiFi.localIP()); 
                if (!mdns.begin("Cloud", WiFi.localIP())) {
                  Serial.println("Error setting up AP MDNS responder!");
                  while(1) { 
                    delay(1000);
                  }
              }
                Serial.println("mDNS responder started");

               
          }
          
          server.begin();
          Serial.println("Server started");
          digitalWrite(0, HIGH);

          //call mdns1 once, then exit to main loop and call when it is necessary
          mdns1();
//          int b = 20;       //local variable which detects if a client is connected
//          int c = 0;
//          while(b == 20) { 
//             b = mdns1(webtype);
//           }
}


//This function searches for networks and stores them in the WiFi struct.  Called when 
//stored SSID connection fails or if there is no stored SSID
void setupAP(void) {
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ul>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st +=i + 1;
      st += ": ";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ul>";
  delay(100);
  //create an AP with SSID as declared in the global vars
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  Serial.println("Soft AP Mode");
  Serial.print("SSID: ");
  Serial.println(ssid);
  webtype = 0;
  launchWebServer();   //Call launch web, but let it know that this is in AP mode
  Serial.println("over");
}


void senddomoticz(char edomIP, char edomPORT, char domIDX) {

// Domoticz format /json.htm?type=command&param=udevice&idx=IDX&nvalue=0&svalue=TEMP;HUM;HUM_STAT

if (server.connect(edomIP,edomPORT)) {
  
    server.print("GET /json.htm?type=command&param=udevice&idx=");
    server.print(domIDX);
    server.print("&nvalue=0&svalue=");
    server.print(tempdht);
    server.print(";");
    server.print(humdht);
    server.print(";0"); //Value for HUM_STAT. Can be one of: 0=Normal, 1=Comfortable, 2=Dry, 3=Wet
    server.println(" HTTP/1.1");
    server.print("Host: ");
    server.print(edomIP);
    server.print(":");
    server.println(edomPORT);
    server.println("User-Agent: ESP8266WiFi");
    server.println("Connection: close");
    server.println();

    server.stop();
    }
  
}


void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor 
    previousMillis = currentMillis;   

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    lum = analogRead(A0);
    humdht = dht.readHumidity();          // Read humidity (percent)
    tempdht = dht.readTemperature();     // Read temperature as celsius
    // Check if any reads failed and exit early (to try again).
    if (isnan(humdht) || isnan(tempdht)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}


void setup() {

  //Setup LED Diagnostic
  pinMode(0, OUTPUT);

  
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;              //string which contains the SSID stored within EEPROM
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  if (esid!=NULL) {
    Serial.println(esid);
  }
  else{
    Serial.println("None Stored");
  }
  
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 160; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  if (epass!=NULL) {
    Serial.println(epass);
  }
  else{
    Serial.println("None Stored");
  }
  
/////////////////////////////////////////////////  
  
  Serial.println("Reading EEPROM domoticz IP");
  String edomIP = "";
  for (int i = 160; i < 175; ++i)
    {
      edomIP += char(EEPROM.read(i));
    }
  Serial.print("Domoticz IP: ");
  if (edomIP!=NULL) {
    Serial.println(edomIP);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM domoticz Port");
  String edomPORT = "";
  for (int i = 175; i < 180; ++i)
    {
      edomPORT += char(EEPROM.read(i));
    }
  Serial.print("Domoticz Port: ");
  if (edomPORT!=NULL) {
    Serial.println(edomPORT);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM domoticz login");
  String edomLOGIN = "";
  for (int i = 180; i < 212; ++i)
    {
      edomLOGIN += char(EEPROM.read(i));
    }
  Serial.print("Domoticz Login: ");
  if (edomLOGIN!=NULL) {
    Serial.println(edomLOGIN);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM domoticz password");
  String edomPW = "";
  for (int i = 212; i < 340; ++i)
    {
      edomPW += char(EEPROM.read(i));
    }
  Serial.print("Domoticz Password: ");
  if (edomPW!=NULL) {
    Serial.println(edomPW);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM domoticz DHT IDX");
  String edomIDX1 = "";
  for (int i = 340; i < 345; ++i)
    {
      edomIDX1 += char(EEPROM.read(i));
    }
  Serial.print("Domoticz DHT IDX: ");
  if (edomIDX1!=NULL) {
    Serial.println(edomIDX1);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM domoticz Lumens IDX");
  String edomIDX2 = "";
  for (int i = 345; i < 350; ++i)
    {
      edomIDX2 += char(EEPROM.read(i));
    }
  Serial.print("Domoticz Lumens IDX: ");
  if (edomIDX2!=NULL) {
    Serial.println(edomIDX2);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM last temperature");
  String elasttemp = "";
  for (int i = 350; i < 355; ++i)
    {
      elasttemp += char(EEPROM.read(i));
    }
  Serial.print("last temperature: ");
  if (elasttemp!=NULL) {
    Serial.println(elasttemp);
  }
  else{
    Serial.println("None Stored");
  } 
  
  Serial.println("Reading EEPROM last humidity");
  String elasthum = "";
  for (int i = 355; i < 360; ++i)
    {
      elasthum += char(EEPROM.read(i));
    }
  Serial.print("last humidity: ");
  if (elasthum!=NULL) {
    Serial.println(elasthum);
  }
  else{
    Serial.println("None Stored");
  }  
  
  Serial.println("Reading EEPROM last lux");
  String elastlux = "";
  for (int i = 360; i < 365; ++i)
    {
      elastlux += char(EEPROM.read(i));
    }
  Serial.print("last temperature: ");
  if (elastlux!=NULL) {
    Serial.println(elastlux);
  }
  else{
    Serial.println("None Stored");
  } 

  
/////////////////////////////////////////////////

  
  //After reading the stored "esid", if it is valid,
  //try to connect to it.  Else, create an AP and search for networks
  if ( esid!=NULL ) {
      Serial.println("Stored SSID length greater than 1");
      // test stored esid
      WiFi.begin(esid.c_str(), epass.c_str());
      Serial.println("Starting connection with ");
      Serial.print("ESID ");
      Serial.println(esid);
      Serial.print("PASS ");
      Serial.println(epass);
      if ( testWifi() == 20 ) { 
          Serial.println("");
          Serial.println("Client Connection Successful");
      
      gettemperature();
      senddomoticz(edomIP,edomPORT,edomIDX1);
      
          webtype = 1;
          launchWebServer();
          return;
      }
  }
  //No stored or failed wireless connection.  Create AP to set up wireless
  Serial.println("SSID did not exist, creating an AP");
  setupAP(); 
}



void loop() {

  mdns1();

}
