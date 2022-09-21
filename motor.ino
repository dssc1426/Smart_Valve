#include <WiFi.h>//including wifi library.
#include "ThingSpeak.h" //including ThingSpeak library.
#include <Servo.h> //including Servo library.
int pos = 0; // intializising the motor position as 0 .

Servo myservo; // created the variable for servo module.


const char* ssid = "SG";   // your Wifi/Hotspot SSID(name) .
const char* password = "Gs@14261192";   // your Wifi/Hotspot password.

WiFiClient  client;//created the variable for Wifi module.

unsigned long myChannelNumber =1627736; //ThingSpeak channel number in website.
const char * myWriteAPIKey = "YV8JIR8DZE9OVCYN";//ThingSpeak Api_Key in website.

// Timer variables.
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
// Timer variables.

int device1=23;//valve1(servo motor) is connected to pin 23(G23) in ESP32.
int last_status;//last_status for sending the previous status to thinkspeak.

#define DEVICEON 0//Defining boolean value for on and off. 
#define DEVICEOFF 1//Defining boolean value for on and off. 
int analogPin1= 36;//Soil_moisture1 pin is connected to pin 36(SP) in ESP32.
int analogPin2= 39;//Soil_moisture2 pin is connected to pin 39(SN) in ESP32.
int analogPin3= 34;//Soil_moisture3 pin is connected to pin 34(G34) in ESP32.
int relay1=18;//Relay1(valve1) is connected to pin 18(G18) in ESP32.
int relay2=19;//Relay2(valve2) is connected to pin 19(G19) in ESP32.
int relay3=21;//Relay3(valve3) is connected to pin 21(G21) in ESP32.
int relay4=22;//Relay4(main_motor) is connected to pin 22(G22) in ESP32.
int moisture1, moisture2, moisture3;//intializing the mositure variables to read/write the values.
int flag=0;//intializing flag as 0.

void setup() 
{
  pinMode(relay1,OUTPUT);//sets the variable relay1(digital_pin = 18) as output.(syntax = pinMode(pin,Mode))
  pinMode(relay2,OUTPUT);//sets the variable relay2(digital_pin = 19) as output.
  pinMode(relay3,OUTPUT);//sets the variable relay3(digital_pin = 21) as output.
  pinMode(relay4,OUTPUT);//sets the variable relay4(digital_pin = 22) as output.
  pinMode(analogPin1,INPUT);//sets the variable analogPin1(valve1)(digital_pin = 36) as Input.
  pinMode(analogPin2,INPUT);//sets the variable analogPin2(valve1)(digital_pin = 39) as Input.
  pinMode(analogPin3,INPUT);//sets the variable analogPin3(valve1)(digital_pin = 34) as Input.
  myservo.attach(23);//servo motor pin is attached to pin 23 in ESP32(Syntax = variable.attach(pin)).
  pinMode(device1,OUTPUT);//sets the variable device1(valve1)(servo motor)(digital_pin = 23) as gives output.
  digitalWrite(device1,DEVICEOFF);//function used to write value to digitial pin, if it is output its voltage will be sent to corresponding value.
                                  //(syntax = digitalWrite(pin,value)).
                                 // sets the digital pin 13 on
  Serial.begin(115200);//Initialize serial opens serial port, sets data rate to 115200bps.
  WiFi.mode(WIFI_STA);//STA mode is to get ESP32 connected to wifi network(access point).
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() 
{
  int statusCode=0;
  if(flag==1)
  {
    check_water();//this function checks all the moistures percentages to turn off the relays.
  }
  else
  {
    moisture1= map(analogRead(analogPin1),200,4095,100,0);//analogRead is to read the value from certain pin(syntax = analogRead(pin)).
                                              //map(value<|analogRead()|>, fromLow, fromHigh, toLow, toHigh) fromLow-->lower bound of value's current range,
                                              //toHigh-->upper bound of value's target range
    moisture2= map(analogRead(analogPin2),200,4095,100,0);  
    moisture3= map(analogRead(analogPin3),200,4095,100,0);
    ThingSpeak.setField(1,moisture1);// updating the mositure_values into field1 in thinkspeak channel.
    ThingSpeak.setField(2,moisture2);// updating the mositure_values into field2 in thinkspeak channel.
    ThingSpeak.setField(3,moisture3);// updating the mositure_values into field3 in thinkspeak channel.
    int x = ThingSpeak.writeFields(1627688,"WGT8FT9WGWM69QA5");//(Syntax = ThingSpeak.writeFields(channel_Id, Api_key))
    if(x == 200)// if (value of mositure sensors is 200 ).
    {
      Serial.println("Channel update successful.");//print this statement in serial monitor "Channel update successful.".
    }
    else
    {
      Serial.println("Problem updating channel. HTTP error code " + String(x));//print this statement in serial monitor "Problem updating channel. HTTP error code".
    }
  }
  if ((millis() - lastTime) > timerDelay) // if((millis() - 0) > 5000)
                                          //wifi connects after 5 seconds.
  {  
    // Connect or reconnect to WiFi
    if(WiFi.status() != WL_CONNECTED)//attempt to connect to Wifi.
    {
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED)
      {
        WiFi.begin(ssid, password); //connecting with given Wifi name and password.
        delay(5000);     
      } 
      Serial.println("\nConnected.");//if connected it shows Connected.
    }
    int device1Status = ThingSpeak.readIntField(myChannelNumber, 1,"M79CSP54ZUG8T08J");// checking valve1 status in thinkspeak
    statusCode = ThingSpeak.getLastReadStatus();
    if(statusCode == 200)
    {
      
     if(device1Status==1)
     {
      digitalWrite(relay4,0);//main motor
      for(pos = 0; pos <= 90; pos += 1) 
      {                                  
        myservo.write(pos);//turn the servo motor to 90degrees.    
        delay(15);                    
      } 
      Serial.println("Device1 ON");// after turning on the valve printing the statemnet.
      xx://goto function
      int n=random(10,20);
      ThingSpeak.setField(1,n);
      int x=ThingSpeak.writeField(myChannelNumber,1,n,"YV8JIR8DZE9OVCYN");// write_Api_key should be used.
      if(x==200)//doing this to only turn motor once without repeating....so that once status sent, the channel will be not take nxt values.
      {
        Serial.println("Channel Cleared");
      }
      else
      {
        delay(5000);
        goto xx;
      }
      flag=1;
      delay(1000);
     } 
     else if(device1Status==0)//if device is already on then this loop works.
     {
      digitalWrite(relay4,1);//offing all the relays.
      digitalWrite(relay1,1);//offing all the relays.
      digitalWrite(relay2,1);//offing all the relays.
      digitalWrite(relay3,1);//offing all the relays.
      flag=0;
      for(pos = 90; pos>=0; pos-=1)//and then turning the valve1 off.
      {                                
        myservo.write(pos);//and then turning the valve1 off.
        delay(15);                     
      } 
      Serial.println("Device1 OFF");// and then printing statement offed is printed.
      xy://again goto function for not repeating the function of motor
      int n=random(10,20);
      ThingSpeak.setField(1,n);
      int x=ThingSpeak.writeField(myChannelNumber,1,n,"YV8JIR8DZE9OVCYN");
      if(x==200)//for not repeating the function of motor
      {
        Serial.println("Channel Cleared");
      }
      else
      {
        delay(5000);
        goto xy;
      }
     }
    }
    else
    {
     Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
    }
  }
    delay(15000); // No need to read the temperature too often.

}
void check_water()//check_water function checks all the mositure sensors values.
{
  moisture1= map(analogRead(analogPin1),200,4095,100,0);//Reading values from the pin and stores it in thinkSpeak.
  //percentage=map(analogRead(analogPin1), map_low[686], map_high[350], [0], [100]);
  moisture2= map(analogRead(analogPin2),200,4095,100,0);//Reading values from the pin and stores it in thinkSpeak.
                                                        //percentage=map(analogRead(analogPin1), map_low, map_high, 0, 100);
  moisture3= map(analogRead(analogPin3),200,4095,100,0);//Reading values from the pin and stores it in thinkSpeak.
                                                        //percentage=map(analogRead(analogPin1), map_low, map_high, 0, 100);
                                                        //print(" | Percentage: ");
                                                        //print(percentage);
                                                        //print("%")
  ThingSpeak.setField(1,moisture1);//Updating field1 value in thinkSpeak.
  ThingSpeak.setField(2,moisture2);//Updating field2 value in thinkSpeak.
  ThingSpeak.setField(3,moisture3);//Updating field3 value in thinkSpeak.
  int x = ThingSpeak.writeFields(1627688,"WGT8FT9WGWM69QA5");// write_Api_key should be used.
  if(x == 200)
  {
    Serial.println("Channel update successful.");//After updating values statement gets print.
  }
  else
  {
    Serial.println("Problem updating channel. HTTP error code " + String(x));//if error occurs statement if get print.
  }
  
  //checking values of 3 mositures if all are above 95 or how many are above 95 so that the mositure value is less than 95 the respective valves will be opened.
  
  if(moisture1>95 && moisture2>95 && moisture3>95)//checking values of 3 mositures if all are above 95 then this loop.
  {
    digitalWrite(relay4,1);//all relays will be offed.(Syntax = digitalWrite(pin,value))
    digitalWrite(relay1,1);//all relays will be offed.(Syntax = digitalWrite(pin,value))
    digitalWrite(relay2,1);//all relays will be offed.(Syntax = digitalWrite(pin,value))
    digitalWrite(relay3,1);//all relays will be offed.(Syntax = digitalWrite(pin,value))
    flag=0;//and flag value will become 0.
  }
  else if(moisture1>95 && moisture2<95)//if mositure1>95 and mositure2<95 values, then this loop.
  {
    digitalWrite(relay1,1);//relay1 will be closed.(Syntax = digitalWrite(pin,value))
    digitalWrite(relay2,0);//relay2 will be opened.
  }
  else if(moisture2>95 && moisture3<95)//if mositure2>95 and mositure3<95 values, then this loop.
  {
   digitalWrite(relay2,1);//relay2 will be closed.
   digitalWrite(relay3,0);//relay3 will be closed.
  }
  else if(moisture3>95)//if mositure3>95 value, then this loop.
  {
    digitalWrite(relay3,1);//relay3 will be closed.
  }
  else
  {
    digitalWrite(relay1,0);//relay1 will be opened.
  }
  delay(5000);
}
