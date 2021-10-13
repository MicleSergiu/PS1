#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned int temp;

void setup()
{
 lcd.begin(16,2);
 adc_init();
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
 delay(1000);
 lcd.clear();
}