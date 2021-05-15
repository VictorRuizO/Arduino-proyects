#define ESPALEXA_ASYNC //it is important to define this before #include <Espalexa.h>!
#include <Espalexa.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>  

#define PIN_R 0
#define PIN_G 1
#define PIN_B 2

#define deviceName  "cama"
#define PARAM_MESSAGE  "message"

AsyncWebServer server(80);
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
Espalexa espalexa;

//callback functions
void firstLightChanged(uint8_t brightness);

byte red;
byte green;
byte blue;
byte state;
byte vel;
byte anim;
byte bright;

int auxR;
int auxG;
int auxB;
int auxBrigth;
int timer;
int multp = 100;

unsigned long tiempo;
bool cambios,first;
String cmd;


void setup() {
  // put your setup code here, to run once:
  
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);//GPIO 1 (TX)
  pinMode(PIN_B, OUTPUT);
  asigColor(0,0,0,0);
  first=true;
  /*
  EEPROM.write (0, 0);
  EEPROM.write (1, 255);
  EEPROM.write (2, 10);
  EEPROM.write (3, 1);
  EEPROM.write (4, 128);
  EEPROM.write (5, 128);
  EEPROM.write (6, 128);*/
  EEPROM.begin(512);
  anim = EEPROM.read(0);
  bright = EEPROM.read(1);
  vel = EEPROM.read(2);
  state = EEPROM.read(3);
  red = EEPROM.read(4);
  green = EEPROM.read(5);
  blue = EEPROM.read(6);
  cambios = false;
  
  /*
  anim = 0;
  bright = 255;
  vel = 15;
  state = 1;
  red = 128;
  green = 128;
  blue = 128;
  cambios = false;*/


 
  // Descomentar para resetear configuración
  //wifiManager.resetSettings();
 
  // Cremos AP y portal cautivo
  wifiManager.autoConnect("Bed-RGB");

  // Send a GET request to <IP>/get?message=<message>
    server.on("/bed", HTTP_POST, [] (AsyncWebServerRequest *request) {
        String str;
        if (request->hasParam(PARAM_MESSAGE)) {
            str = request->getParam(PARAM_MESSAGE)->value();

            if(str.equals("GET")){              
              request->send(200, "text/plain", String(anim)+"."+String(bright)+"."+String(vel)+"."+String(state)+"."+String(red)+"."+String(green)+"."+String(blue));
            }else{

            cmd=getValue(str, '.', 0);
            if(cmd.equals("a")){
              anim = getValue(str, '.', 1).toInt();;
              EEPROM.write (0, anim);
              first=true;
            }else if(cmd.equals("r")){
              vel = getValue(str, '.', 1).toInt();;
              EEPROM.write (2, vel);
            }else if(cmd.equals("c")){
              red = getValue(str, '.', 1).toInt();
              green = getValue(str, '.', 2).toInt();
              blue = getValue(str, '.', 3).toInt();
              EEPROM.write (4, red);
              EEPROM.write (5, green);
              EEPROM.write (6, blue);
            }else if(cmd.equals("b")){
              bright = getValue(str, '.', 1).toInt();
              EEPROM.write (1, bright);
            }
            else if(cmd.equals("s")){
              state = getValue(str, '.', 1).toInt();
              EEPROM.write (3, state);
              asigColor(red, green, blue, bright*state);
            }
            EEPROM.commit();
            request->send(200, "text/plain", "OK");
            }
            
        } else {
            request->send(200, "text/plain", "No message sent");
        }
        
    });

    server.onNotFound([](AsyncWebServerRequest *request){
      if (!espalexa.handleAlexaApiCall(request)) //if you don't know the URI, ask espalexa whether it is an Alexa control request
      {
        //whatever you want to do with 404s
        request->send(404, "text/plain", "Not found");
      }
    });

    espalexa.addDevice("cama", firstLightChanged);
    espalexa.begin(&server);
  //server.begin();

}

void firstLightChanged(uint8_t brightness) {
    
    //do what you need to do here

    //EXAMPLE
    if (brightness == 255) {
      state=true;
    }
    else if (brightness == 0) {
      state=false;
    }
    else {
      bright = brightness;
    }
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void asigColor(byte r, byte g, byte b, byte brig){
  double mul = brig/255.0;
  analogWrite(PIN_R, (byte)r*mul);
  analogWrite(PIN_G, (byte)g*mul);
  analogWrite(PIN_B, (byte)b*mul);
}


bool aux_anim;
byte steps_anim;
byte count_anim;
byte *aux_anim_wheel;
byte *aux_anim_wheel_2;
double axu_r,axu_g,axu_b;
double mom_r, mom_g, mom_b;

void loop() {
  wifiManager.loop();
  espalexa.loop();
  if(!state) return;
  
  cambios=false;
  
  switch (anim) {
    case 0:
    asigColor(red, green, blue, bright);
    break;

    /********************************************************************************/
    case 1:
    
    if(first){
      auxBrigth = 0;
      timer=0;
      aux_anim=false;
      steps_anim=1;
      count_anim=0;
      asigColor(red, green, blue, auxBrigth);
      first=false;
    }    
    
    
    if (timer < multp * (vel/10)) {
      timer++;
    }else if(steps_anim==1){
        
      if(aux_anim){
        auxBrigth--;
      }else{
        auxBrigth++;
      }
      if(auxBrigth==255) {
        aux_anim=true;
        steps_anim=2;
      }
      if(auxBrigth==0) {
        aux_anim=false;
        steps_anim=2;
      }
      asigColor(red, green, blue, auxBrigth);
      timer=0;
      
    }else if(steps_anim==2){
      if(count_anim==100){
        steps_anim=1;
        count_anim=0;
      }else{
        count_anim++;
      }
      timer=0;
    }
    break;

    /********************************************************************************/
    case 2:

    if(first){
     count_anim=0;
     timer=0;
     first=false;
    }   

    if (timer < multp * (vel)) {
      timer++;
    }else{
      aux_anim_wheel = Wheel(count_anim);
      asigColor(*aux_anim_wheel, *(aux_anim_wheel + 1), *(aux_anim_wheel + 2), bright);  
        
      if(count_anim==255) count_anim=0;
      else                count_anim++;
  
      timer=0;
     }
    break;

    /********************************************************************************/        
    case 3:

    if(first){
     timer =0;
     first=false;
    }   

    if (timer < multp * (vel)*50) {
      timer++;
    }else{
      aux_anim_wheel = Wheel(random(255));
      asigColor(*aux_anim_wheel, *(aux_anim_wheel + 1), *(aux_anim_wheel + 2), bright); 
  
      timer=0;
     }
    break;

    /********************************************************************************/

    case 4:
    
    if(first){
      auxBrigth = 0;
      timer=0;
      aux_anim=false;
      steps_anim=1;
      count_anim=0;
      aux_anim_wheel = Wheel(random(256));
      asigColor(*aux_anim_wheel, *(aux_anim_wheel + 1), *(aux_anim_wheel + 2), auxBrigth); ;
      first=false;
    }    
    
    
    if (timer < multp * (vel/10)) {
      timer++;
    }else if(steps_anim==1){
        
      if(aux_anim){
        auxBrigth--;
      }else{
        auxBrigth++;
      }
      if(auxBrigth==255) {
        aux_anim=true;
        steps_anim=2;
      }
      if(auxBrigth==0) {
        aux_anim=false;
        steps_anim=2;
        aux_anim_wheel = Wheel(random(256));
      }
      asigColor(*aux_anim_wheel, *(aux_anim_wheel + 1), *(aux_anim_wheel + 2), auxBrigth); ;
      timer=0;
      
    }else if(steps_anim==2){
      if(count_anim==100){
        steps_anim=1;
        count_anim=0;
      }else{
        count_anim++;
      }
      timer=0;
    }
    break;

    /********************************************************************************/
  }
  //delay(1);
}


byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}
