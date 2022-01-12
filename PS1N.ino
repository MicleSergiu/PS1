#include <LiquidCrystal.h>
#include "DHT.h"

#define DHTPIN 13     
#define DHTTYPE DHT11

#define BUTTON_OK 7
#define BUTTON_CANCEL 9
#define BUTTON_PREV 8
#define BUTTON_NEXT 10

#define enA 6

DHT dht(DHTPIN, DHTTYPE); //senzor de temperatura

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); //display

long previousMillis = 0;
int h=0;
int m=0;
int s=0;
int isWorking =0;
float tC =0;
int ok=0;
int ti_s=0, ti_m=2,tm_s=0, tm_m=1, tr_s=0, tr_m=5;
int TI, TM, TR;
int TI_DONE = 0, TM_DONE = 0, TR_DONE = 0;
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
  MENU_KI,
  MENU_KD,
  MENU_TI,
  MENU_TM,
  MENU_TR,
  MENU_TEMP,
  MENU_START,
  MENU_MAX_NUM
};

double t_set = 50;
int kp = 90, ki = 14, kd = 80;
float temp_q = 0;
float last_set_temperature = 0;
float PID_error = 0; 
float previous_error = 0;
float elapsedTime, Time, timePrev;
float PID_value = 0;
float PID_p = 0;    float PID_i = 0;    float PID_d = 0;
float last_kp = 0;
float last_ki = 0;
float last_kd = 0;

int PID_values_fixed =0;
Menus scroll_menu = MENU_MAIN;
Menus current_menu =  MENU_MAIN;

void state_machine(enum Menus menu, enum Buttons button);
Buttons GetButtons(void);
void print_menu(enum Menus menu);

typedef void (state_machine_handler_t)(void);

void print_menu(enum Menus menu)
{
  lcd.clear();
  long unsigned int currentMillis = 0;
  switch(menu)
  {
    case MENU_START:
      PID_error = t_set - tC + 3;
      PID_p = 0.01*kp * PID_error;
      PID_i = 0.01*PID_i + (ki * PID_error);

      timePrev = Time;
      Time = millis();
      elapsedTime = (Time - timePrev) / 1000;
      PID_d = 0.01*kd*((PID_error - previous_error)/elapsedTime);
      PID_value = PID_p + PID_i + PID_d;

      if(PID_value < 0)
      {    PID_value = 0;    }
      if(PID_value > 100)  
      {    PID_value = 255;  }
      
      isWorking = analogRead(A0);
      tC = dht.readTemperature();
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.setCursor(5, 0);
      lcd.print(tC);
      lcd.print(" C");
      Serial.println("PID:");
      Serial.println(PID_value);
      
        
        if(current_menu != MENU_MAIN)
          {
            lcd.setCursor(15,0);
            lcd.print("M");
          }
        if(ok)
          {
           currentMillis = millis();
        if(currentMillis - previousMillis > 1000) 
          {
            previousMillis = currentMillis;  
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
            lcd.setCursor(0,1);
            if(h<10)lcd.print("0");
            lcd.print(h);
            lcd.print(":");
            if(m<10)lcd.print("0");
            lcd.print(m);
            lcd.print(":");
            if(s<10)lcd.print("0");
            lcd.print(s);
            if(TI_DONE==0 && TM_DONE==0 && TR_DONE==0)
            {
              if(ti_s==s && ti_m==m)
              {
                s=0; m=0; TI_DONE=1;
              }
              else
              {
                digitalWrite(Bpin1, HIGH);
                digitalWrite(Bpin2, LOW); 
                analogWrite(enA,PID_value);
                lcd.setCursor(14,1);
                lcd.print("TI");
              }
            }
            if(TI_DONE==1 && TM_DONE == 0 && TR_DONE==0)
            {
                if(tm_s==s && tm_m==m)
              {
                s=0; m=0; TM_DONE=1;
              }
              else
              {
                digitalWrite(Bpin1, HIGH);
                digitalWrite(Bpin2, LOW); 
                analogWrite(enA,PID_value);
                lcd.setCursor(14,1);
                lcd.print("TM");
              }
            }
            if(TI_DONE==1 && TM_DONE == 1 && TR_DONE==0)
            {
                if(tr_s==s && tr_m==m)
              {
                s=0; m=0; TR_DONE=1;
              }
              else
              {
                digitalWrite(Bpin1, HIGH);
                digitalWrite(Bpin2, HIGH); 
                lcd.setCursor(14,1);
                lcd.print("TR");
              }
            }
          }
          else if(!ok)
          {
            digitalWrite(Bpin1, HIGH);
            digitalWrite(Bpin2, HIGH);
          }
          previous_error = PID_error;
        break;
    case MENU_KP:
      lcd.print("Constanta KP");
      lcd.setCursor(0,1);
      lcd.print("KP = ");
      lcd.print(kp);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
      case MENU_KI:
      lcd.print("Constanta KI");
      lcd.setCursor(0,1);
      lcd.print("KI = ");
      lcd.print(ki);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
      case MENU_KD:
      lcd.print("Constanta KD");
      lcd.setCursor(0,1);
      lcd.print("KD = ");
      lcd.print(kd);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
    case MENU_TI:
      lcd.print("Timp incalzire");
      lcd.setCursor(0,1);
      lcd.print("TI=");
      if(ti_m<10)
        lcd.print("0");
      lcd.print(ti_m);
      lcd.print(":");
      if(ti_s<10)
        lcd.print("0");
      lcd.print(ti_s);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
    case MENU_TM:
    lcd.print("Timp mentinere");
    lcd.setCursor(0,1);
    lcd.print("TM=");
      if(tm_m<10)
        lcd.print("0");
      lcd.print(tm_m);
      lcd.print(":");
      if(tm_s<10)
        lcd.print("0");
      lcd.print(tm_s);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
    case MENU_TR:
    lcd.print("Timp racire");
    lcd.setCursor(0,1);
    lcd.print("TR=");
      if(tr_m<10)
        lcd.print("0");
      lcd.print(tr_m);
      lcd.print(":");
      if(tr_s<10)
        lcd.print("0");
      lcd.print(tr_s);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
      }
      break;
    case MENU_TEMP:
      lcd.print("Temperatura");
      lcd.setCursor(0,1);
      lcd.print("TEMP = ");
      lcd.print(t_set);
      if(current_menu != MENU_MAIN)
      {
        lcd.setCursor(15,1);
        lcd.print("M");
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
}

void stop_program(void)
{
  ok=0;
}

void save_kp(void)
{
}

void inc_kp(void)
{
  kp+=1;
}

void dec_kp(void)
{
  kp-=1;
}

void inc_ki(void)
{
  ki+=1;
}

void dec_ki(void)
{
  ki-=1;
}

void inc_kd(void)
{
  kd+=1;
}

void dec_kd(void)
{
  kd-=1;
}
void inc_ti(void)
{
  ti_s++;
  if(ti_s==60)
  {
    ti_s=0;
    ti_m+=1;
  }
}

void dec_ti(void)
{
  if(ti_s==0)
  {
    ti_s=60;
    ti_m-=1;
  }
  ti_s--;
}

void inc_tm(void)
{
  tm_s++;
  if(tm_s==60)
  {
    tm_s=0;
    tm_m+=1;
  }
}

void dec_tm(void)
{
  if(tm_s==0)
  {
    tm_s=60;
    tm_m-=1;
  }
  tm_s--;
}

void inc_tr(void)
{
  tr_s++;
  if(tr_s==60)
  {
    tr_s=0;
    tr_m+=1;
  }
}

void dec_tr(void)
{
  if(tr_s==0)
  {
    tr_s=60;
    tr_m-=1;
  }
  tr_s--;
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
  {enter_menu, go_home, inc_kp, dec_kp},       // MENU_KP
  {enter_menu, go_home, inc_ki, dec_ki},       // MENU_KI
  {enter_menu, go_home, inc_kd, dec_kd},       // MENU_KD
  {enter_menu, go_home, inc_ti, dec_ti},       // MENU_TI
  {enter_menu, go_home, inc_tm, dec_tm},       // MENU_TM
  {enter_menu, go_home, inc_tr, dec_tr},       // MENU_TR
  {enter_menu, go_home, inc_temp, dec_temp},   // MENU_TEMP
  {enter_menu, go_home, start_program, stop_program}, //MENU_START
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
 dht.begin();
 pinMode(enA, OUTPUT);
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
 Time = millis();
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
 delay(250);
}
