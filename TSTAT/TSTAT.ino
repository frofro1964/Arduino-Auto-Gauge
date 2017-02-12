

// include the library code:
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <MQTT.h>
#include <stdlib.h>

LiquidCrystal_I2C lcd(0x3F,16,2); //Addr: 0x3F, 20 chars & 4 lines

#define CLIENT_ID "FROSTAT_2"
#define SET_POINT "/frostat_2/setpoint"
#define T_AMB     "/frostat_2/tamb"
#define H_AMB     "/frostat_2/hamb"
#define STATUS    "/frostat_2/status"

#define TRUE 1
#define FALSE 0

#define USER    "pi"
#define PWD     "Michid24"

//Outputs
#define CALL_HEAT 14
#define CALL_AC   12
#define CALL_FAN  13
#define CALL_AUX  15

//Inputs
#define KEY_UP    10
#define KEY_DOWN  9
#define KEY_MENU 10





MQTT myMqtt(CLIENT_ID, "fronet.mooo.com", 1883);


const char* ssid     = "Sneaky Cat";
const char* password = "michid24";

const int Hysterisis_Set = 1;


byte fetch_humidity_temperature(unsigned int *p_Humidity, unsigned int *p_Temperature);
void print_float(float f, int num_digits);

float hamb, tamb;
unsigned int H_dat, T_dat;

int set_point = 24;

int key_up_state = 0;
int key_down_state = 0;
int key_menu_state = 0;
 
int count = 0;

String last = "NaN";
String Status;



// Connect via i2c, default address #0 (A0-A2 not jumpered)


void setup() {
  
  //Setup relay outputs
  pinMode(CALL_HEAT, OUTPUT);
  digitalWrite(CALL_HEAT,HIGH);
  pinMode(CALL_AC, OUTPUT);
  digitalWrite(CALL_AC,HIGH);
  pinMode(CALL_FAN, OUTPUT);
  digitalWrite(CALL_FAN,HIGH);
  pinMode(CALL_AUX, OUTPUT);
  digitalWrite(CALL_AUX,HIGH);

  //Setup input pins
  pinMode(KEY_UP, INPUT);
  pinMode(KEY_DOWN, INPUT);
  pinMode(KEY_MENU, INPUT);

  //Setup LCD
  Wire.begin(2,0);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);

  //Setup Wifi
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
  }

  //Print ip
  lcd.setCursor(0, 0);
  lcd.print("IP address:   ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(2000);
  

  //Setup MQTT
  myMqtt.onConnected(myConnectedCb);
  myMqtt.onDisconnected(myDisconnectedCb);
  myMqtt.onPublished(myPublishedCb);
  myMqtt.onData(myDataCb);
  myMqtt.connect();

  
  //Clear for main loop
  lcd.clear();
}

void loop() {

   byte _status;
   unsigned int H_dat, T_dat;
   float RH, T_C;
   char buff[6];
   String Tamb, Hamb, Set_Point_str;
   int Hysterisis = Hysterisis_Set;

   if (last == "NaN"){
    Hysterisis = 0;    
   }

   myMqtt.subscribe(SET_POINT);

   key_up_state = digitalRead(KEY_UP);
   key_down_state = digitalRead(KEY_DOWN);
   key_menu_state = digitalRead(KEY_MENU);
  


  _status = fetch_humidity_temperature(&H_dat, &T_dat);
  
     


      if(key_up_state == 0){
        set_point = set_point + 1;
        itoa(set_point, buff, 10);
        Set_Point_str = buff;
        myMqtt.publish(SET_POINT, Set_Point_str);
        
      }
      if(key_down_state == 0){
         set_point = set_point - 1;
         itoa(set_point, buff, 10);
         Set_Point_str = buff;
         myMqtt.publish(SET_POINT, Set_Point_str);
      }

      

      
      

    
      RH = (float) H_dat * 6.10e-3;
      Hamb = dtostrf(RH, 4, 2, buff);
      T_C = (float) T_dat * 1.007e-2 - 40.0;
      Tamb = dtostrf(T_C,4,2,buff);



      if((set_point - Hysterisis) > T_C ){
        digitalWrite(CALL_HEAT,LOW);
        Status = "Heating";
        
      }
      if((set_point + Hysterisis) < T_C){
        digitalWrite(CALL_HEAT,HIGH);
        Status = "Idle";
      }

      
      if (Status != last){
          myMqtt.publish(STATUS,Status);
      }
      
      if (count >= 10){

         myMqtt.publish(T_AMB, Tamb);
         myMqtt.publish(H_AMB, Hamb);
         count = 0;
      }


      lcd.setCursor(0,0);
      lcd.print("Set Temp: ");
      lcd.print(set_point);
      lcd.print((char)223);
      lcd.print("C ");

      lcd.setCursor(0, 1);
      print_float(T_C, 2);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(8, 1);   
      print_float(RH, 1);
      lcd.print("%H");
      delay(300);

      count++;


      
      last = Status;
}



/*
 * 
 */
void myConnectedCb() {
  //Serial.println("connected to MQTT server");
}

void myDisconnectedCb() {
  //Serial.println("disconnected. try to reconnect...");
  delay(500);
  myMqtt.connect();
}

void myPublishedCb() {
  //Serial.println("published.");
}

void myDataCb(String& topic, String& data) {

      String buff;
      int len = 8;
  
      if(topic == SET_POINT){
       
       set_point = atoi(data.c_str());

       
      }


}


byte fetch_humidity_temperature(unsigned int *p_H_dat, unsigned int *p_T_dat)
{
      byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
      unsigned int H_dat, T_dat;
      address = 0x27;;
      Wire.beginTransmission(address); 
      Wire.endTransmission();
      delay(100);
      
      Wire.requestFrom((int)address, (int) 4);
      Hum_H = Wire.read();
      Hum_L = Wire.read();
      Temp_H = Wire.read();
      Temp_L = Wire.read();
      Wire.endTransmission();
      
      _status = (Hum_H >> 6) & 0x03;
      Hum_H = Hum_H & 0x3f;
      H_dat = (((unsigned int)Hum_H) << 8) | Hum_L;
      T_dat = (((unsigned int)Temp_H) << 8) | Temp_L;
      T_dat = T_dat / 4;
      *p_H_dat = H_dat;
      *p_T_dat = T_dat;
      return(_status);
}
   


   
void print_float(float f, int num_digits)
{
    int f_int;
    int pows_of_ten[4] = {1, 10, 100, 1000};
    int multiplier, whole, fract, d, n;

    multiplier = pows_of_ten[num_digits];
    if (f < 0.0)
    {
        f = -f;
        lcd.print("-");
    }
    whole = (int) f;
    fract = (int) (multiplier * (f - (float)whole));

    lcd.print(whole);
    lcd.print(".");

    for (n=num_digits-1; n>=0; n--) // print each digit with no leading zero suppression
    {
         d = fract / pows_of_ten[n];
         lcd.print(d);
         fract = fract % pows_of_ten[n];
    }
}  
