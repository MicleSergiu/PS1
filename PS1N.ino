#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 13     
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE); //senzor de temperatura

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //display

unsigned int temp;
int h=12;
int m=59;
int s=45;

static uint32_t last_time, now = 0;

//declarare pini bec pentru modulul L298N
int Bpin1 = A2;
int Bpin2 = A3;

void setup()
{
 lcd.begin(16,2);
 now=millis();
 dht.begin();
 pinMode(Bpin1, OUTPUT);
 pinMode(Bpin2, OUTPUT);
}

void loop()
{
 float tC = dht.readTemperature();
 lcd.setCursor(0, 0);
 lcd.print("Temp:");
 lcd.setCursor(5, 0);
 lcd.print(tC);
 lcd.print(" C");
 lcd.setCursor(0,1);
 lcd.print("Ora:");
 lcd.setCursor(4,1);
 if(h<10)lcd.print("0");
 lcd.print(h);
 lcd.print(":");
 if(m<10)lcd.print("0");
 lcd.print(m);
 lcd.print(":");
 if(s<10)lcd.print("0");
 lcd.print(s);
  
 for ( int i=0 ;i<5 ;i++)
{
  	while ((now-last_time)<200)
  	{ 
     now=millis();
  	}
  
 	 last_time=now;

 	if(s==60)
 	{
  	 s=0;
  	 m=m+1;
 	}
 	if(m==60)
 	{
  	 m=0;
 	 h=h+1;
 	}
 	if(h==24)
 	{
  	 h=0;
 	}  
 }
  s++;
  if(s==60)
 	{
  	 s=0;
  	 m=m+1;
 	}
 	if(m==60)
 	{
  	 m=0;
 	 h=h+1;
 	}
 	if(h==24)
 	{
  	 h=0;
 	}
 digitalWrite(Bpin1, HIGH);
 digitalWrite(Bpin2, LOW);
}
