#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned int temp;
int h=12;
int m=59;
int s=45;

static uint32_t last_time, now = 0;

void setup()
{
 lcd.begin(16,2);
 adc_init();
 now=millis();
}

void adc_init()
{
  ADCSRA |= ((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));
  
  ADMUX |= (1<<REFS0);
  ADCSRA |= (1<<ADEN);
  ADCSRA |= (1<<ADSC);
}
           
uint16_t read_adc(uint8_t channel)
{
 ADMUX &= 0xF0;
 ADMUX |= channel;
 ADCSRA |= (1<<ADSC);
 while(ADCSRA & (1<<ADSC));
 return ADC;
}

void loop()
{
 float t, tC;
 temp=read_adc(0);
 t=float(temp*5)/1023;
 tC=float(t-0.5)*100;
 lcd.setCursor(0, 0);
 lcd.print("Temp:");
 lcd.setCursor(5, 0);
 lcd.print(tC);
 lcd.print((char)178);
 lcd.print("C");
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
}