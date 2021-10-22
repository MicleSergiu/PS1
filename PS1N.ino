#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 13     
#define DHTTYPE DHT11

#define BUTTON_OK 7
#define BUTTON_CANCEL 9
#define BUTTON_PREV 8
#define BUTTON_NEXT 10

DHT dht(DHTPIN, DHTTYPE); //senzor de temperatura

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //display

int h=12;
int m=59;
int s=45;
int isWorking =0;
float tC =0;
int ok=0;

static uint32_t last_time, now = 0;

//declarare pini bec pentru modulul L298N
int Bpin1 = A2;
int Bpin2 = A3;

enum Buttons {
  EV_OK,
  EV_CANCEL,
  EV_NEXT,
  EV_PREV,
  EV_NONE,
  EV_MAX_NUM
};

enum Menus {
  MENU_MAIN = 0,
  MENU_KP,
  MENU_TEMP,
  MENU_START,
  MENU_MAX_NUM
};

double t_set = 30;
double kp = 20;
float temp_q = 0;
Menus scroll_menu = MENU_MAIN;
Menus current_menu =  MENU_MAIN;

void state_machine(enum Menus menu, enum Buttons button);
Buttons GetButtons(void);
void print_menu(enum Menus menu);

typedef void (state_machine_handler_t)(void);

void print_menu(enum Menus menu)
{
  lcd.clear();
  switch(menu)
  {
    case MENU_START:
      isWorking = analogRead(A0);
      tC = dht.readTemperature();
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
      Serial.println(isWorking);
  
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
        if(current_menu != MENU_MAIN)
          {
            lcd.setCursor(15,1);
            lcd.print("M");
          }
          if( ok & tC<t_set)
          {
            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, LOW);
          }
          else
          {
            digitalWrite(Bpin1, LOW);
            digitalWrite(Bpin2, LOW);
          }
        break;
    case MENU_KP:
      lcd.print("KP = ");
      lcd.print(kp);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(0,1);
        lcd.print("modifica");
      }
      break;
    case MENU_TEMP:
      lcd.print("TEMP = ");
      lcd.print(t_set);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(0,1);
        lcd.print("modifica");
      }
      break;
    case MENU_MAIN:
    default:
      lcd.print("PS1N");
      break;
  }
}

void enter_menu(void)
{
  current_menu = scroll_menu;
}

void go_home(void)
{
  scroll_menu = MENU_MAIN;
  current_menu = scroll_menu;
}

void go_next(void)
{
  scroll_menu = (Menus) ((int)scroll_menu + 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void go_prev(void)
{
  scroll_menu = (Menus) ((int)scroll_menu - 1);
  scroll_menu = (Menus) ((int)scroll_menu % MENU_MAX_NUM);
}

void start_program(void)
{
  ok=1;
 //digitalWrite(Bpin1, HIGH);
 //digitalWrite(Bpin2, LOW);
}

void stop_program(void)
{
  ok=0;
 //digitalWrite(Bpin1, LOW);
 //digitalWrite(Bpin2, LOW);
}

void save_kp(void)
{
}

void inc_kp(void)
{
  kp++;
}

void dec_kp(void)
{
  kp--;
}

void save_temp(void)
{
}

void inc_temp(void)
{
  t_set++;
}

void dec_temp(void)
{
  t_set--;
}


state_machine_handler_t* sm[MENU_MAX_NUM][EV_MAX_NUM] = 
{ //events: OK , CANCEL , NEXT, PREV
  {enter_menu, go_home, go_next, go_prev},  // MENU_MAIN
  {go_home, go_home, inc_kp, dec_kp},       // MENU_KP
  {go_home, go_home, inc_temp, dec_temp},   // MENU_TEMP
  {go_home, go_home, start_program, stop_program}, //MENU_START
};

void state_machine(enum Menus menu, enum Buttons button)
{
  sm[menu][button]();
}

Buttons GetButtons(void)
{
  enum Buttons ret_val = EV_NONE;
  if (digitalRead(BUTTON_OK))
  {
    ret_val = EV_OK;
  }
  else if (digitalRead(BUTTON_CANCEL))
  {
    ret_val = EV_CANCEL;
  }
  else if (digitalRead(BUTTON_NEXT))
  {
    ret_val = EV_NEXT;
  }
  else if (digitalRead(BUTTON_PREV))
  {
    ret_val = EV_PREV;
  }
  Serial.print(ret_val);
  return ret_val;
}

void setup()
{
 lcd.begin(16,2);
 now=millis();
 dht.begin();
 pinMode(Bpin1, OUTPUT);
 pinMode(Bpin2, OUTPUT);
 //BUTTON_OK
 pinMode(7, INPUT);
 digitalWrite(7, LOW);
 //BUTTON_CANCEL
 pinMode(9, INPUT);
 digitalWrite(9, LOW);
 //BUTTON_NEXT
 pinMode(8, INPUT);
 digitalWrite(8, LOW);
 //BUTTON_PREV
 pinMode(10, INPUT);
 digitalWrite(10, LOW);
 Serial.begin(9600);
}

void loop()
{
 volatile Buttons event = GetButtons();
 if (event != EV_NONE)
 {
   state_machine(current_menu, event);
 }
   print_menu(scroll_menu);
 delay(1000);
}
