unsigned int milliseconds;
unsigned char seconds, minuts, hours, Tseconds = 0, Tminuts = 0,Cseconds,Cminuts, D;
bool RunTime = true, fast = false, hold, btnState, btnSt;
volatile unsigned int T, save, SAVE;
unsigned char H,F;
bool bClock;
bool bTimer, sTimer, tStop = false;
bool bCounter, START;
bool Sett;
/****************************************************************/
/****************************************************************/
/****************************************************************/

ISR(PCINT0_vect){ // прерывание по вектору INT0
  btnSt = !btnSt;
  btnState = !btnSt;

}

ISR (TIMER0_COMPA_vect){
  TimeControl();
  WatchControl();
  BTN_Controll();
  TimerControl();
  CounterController();
  
}

/****************************************************************/
/****************************************************************/
/****************************************************************/
void CounterController(){
  if(START){
    if(milliseconds > 999){
      Cseconds++;
      if(Cseconds > 59){
        Cseconds = 0;
        Cminuts++;
      }
    }
  }
}
/****************************************************************/
void TimerControl(){
  if(sTimer){
    if(milliseconds > 999){
      if(Tseconds > 0 && Tminuts >= 0){
          Tseconds--;
        }
        if(Tseconds == 0 && Tminuts != 0){
          Tseconds = 59;
          Tminuts--;
        }
      }
    }
  }
/****************************************************************/
void WatchControl(){
  if(RunTime){  
    if(milliseconds > 999){
      seconds++;
    }
    if(seconds > 59){
      seconds = 0;
      minuts++;
    }
    if(minuts > 59){
      minuts = 0;
      hours++;
    }
    if(hours > 23){
      hours = 0;
    }
  }
}

void TimeControl(){
  milliseconds++;
  D++;
  if(milliseconds > 1000){
      milliseconds = 0;


  }
  if(D > 20){
    D = 0;
  }
}

void BTN_Controll(){
    if(btnState){
    save = 0;
    T++;
    }
    else {
      save = T;
      T = 0;
      if(save > 200){
        Sett = true;
        H++;
        if(H>2){
          H=0;
          Sett = false;
        }
      }
      else if(save != 0){
        if(!Sett){
          F++;
          if(F>2){
            F=0;
          }
        }
        else if(bClock){
            switch(H){
            case 0:break;
            case 1:seconds++;break;
            case 2:minuts++;break;
          }
        }
        else if(bTimer){
            switch(H){
            case 0:break;
            case 1:Tseconds++;break;
            case 2:Tminuts++;break;  
          }
        }
        else if(bCounter){
          switch(H){
            case 0:break;
            case 1:START = true;break;
            case 2:START = false;break;  
          }
        }
      }
    }
  }  


void VisionController(char x,char h){
  switch(x){
    default:
    SetClock(h);
    bClock = true;
    bTimer = false;
    bCounter = false;
    break;
    /****/
    case 1:
    SetTimer(h);
    bClock = false;
    bTimer = true;
    bCounter = false;
    break;
    /****/
    case 2:
    SetCounter(h);
    bClock = false;
    bTimer = false;
    bCounter = true;
    break;
  }
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void setup() {
  //Прерывание
  TCCR0A |= (1 << WGM01);              //сброс при совпадении
  OCR0A = 0xF9;                        //начало отсчета до переполнения (249)
  TIMSK0 |= (1 << OCIE0A);             //разрешить прерывание при совпадении с регистром А
  TCCR0B |= (1 << CS01) | (1 << CS00); //установить делитель частоты на 64 
  sei();                               //разрешить прерывания
  //Режим пинов
  DDRD |= B11111100;
  DDRB |= B00011111;
 
  //Для кнопки
  PCMSK0 |= (1<<PCINT5);
  PCICR |= (1<<PCIE0);

}

void loop() {
  // put your main code here, to run repeatedly:
  PORTB |= (1 << 5);
  BTN();
  VisionController(F,H);
}
/***********************************************************/
void BTN(){
  if(PORTB5){
    PORTB |= (1<<5);
  }
  else PORTB &= ~(1<<5);
}
/***********************************************************/
void S_1(char number){
  clearLEDs();         
  pickDigit(3); // включаем четвертую цифру (единицы минут)
  pickNumber(number);
}
void S_2(char number){
  clearLEDs();
  pickDigit(2); // включаем третью цифру (десятки минут)
  pickNumber(number);
}
void S_3(char number){
  clearLEDs();         
  pickDigit(1); // включаем вторую цифру (единицы часов)
  pickNumber(number);
}
void S_4(char number){
  clearLEDs();         
  pickDigit(0); // включаем первую цифру (десятки часов)
  pickNumber(number);
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void SetClock(char x){
  switch(x){
    default:   
    RunTime = true;
      if(D == 0){
        S_1(seconds % 10);
      }
      if(D == 5){
        S_2(seconds / 10);
      }
      if(D == 10){
        S_3(minuts % 10);
      }
      if(D == 15){
        S_4(minuts / 10);
      }
      break;
      
    case 1:
      RunTime = false;
      if(seconds > 59){
        seconds = 0;
      }
      if(milliseconds > 500){
        if(D == 10){
            S_1(seconds % 10);
          }
          else if(D == 20){
            S_2(seconds / 10);
          }
        }
        else{
          clearLEDs();
        }
        break;       
    case 2:
      RunTime = false;
      if(minuts > 59){
        minuts = 0;
      }
      if(milliseconds > 500){
        if(D == 10){
            S_3(minuts % 10);
          }
          else if(D == 20){
            S_4(minuts / 10);
          }
        }
        else{
          clearLEDs();
        }
        break;  
  }
}
/****************************************************************/
/****************************************************************/
/***********************TIMEEEER*******************************/
/****************************************************************/
void SetTimer(char x){
  switch(x){
    default:
      sTimer = true;
      if(D == 0){
        S_1(Tseconds % 10);
      }
      if(D == 5){
        S_2(Tseconds / 10);
      }
      if(D == 10){
        S_3(Tminuts % 10);
      }
      if(D == 15){
        S_4(Tminuts / 10);
      }
      break;
    case 1:
      sTimer = false;
      if(Tseconds > 59){
        Tseconds = 0;
      }
      if(milliseconds > 500){
        if(D == 10){
            S_1(Tseconds % 10);
          }
          else if(D == 20){
            S_2(Tseconds / 10);
          }
        }
        else{
          clearLEDs();
        }
        break;       
    case 2:
      sTimer = false;
      if(Tminuts > 59){
        Tminuts = 0;
      }
      if(milliseconds > 500){
        if(D == 10){
            S_3(Tminuts % 10);
          }
          else if(D == 20){
            S_4(Tminuts / 10);
          }
        }
        else{
          clearLEDs();
        }
        break;  
  }
}
/****************************************************************/
/****************************************************************/
/***********************COUNTERR*******************************/
/****************************************************************/
void SetCounter(char x){
  switch(x){
    default:
      Cseconds = 0;
      Cminuts = 0;
      if(D == 0){
        S_1(0);
      }
      if(D == 5){
        S_2(7);
      }
      if(D == 10){
        S_3(7);
      }
      if(D == 15){
        S_4(7);
      }
      break;
    case 1:
      START = true;
      SAVE = milliseconds;
      if(Cseconds > 59){
        Cseconds = 0;
      }
      if(Cminuts > 59){
        Cminuts = 0;
      }
      if(D == 0){
        S_1(SAVE % 10);
      }
      if(D == 5){
        S_2(SAVE / 10);
      }
      if(D == 10){
        S_3(Cseconds % 10);
      }
      if(D == 15){
        S_4(Cseconds / 10);
      }
      break;      
    case 2:
      START = false;
      if(D == 0){
        S_1(SAVE % 10);
      }
      if(D == 5){
        S_2(SAVE / 10);
      }
      if(D == 10){
        S_3(Cseconds % 10);
      }
      if(D == 15){
        S_4(Cseconds / 10);
      }
      break; 
      
 
  }
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void pickNumber(int x){
  switch (x) {
    default: zero(); break;
    case 1: one(); break;
    case 2: two(); break;
    case 3: three(); break;
    case 4: four(); break;
    case 5: five(); break;
    case 6: six(); break;
    case 7: seven(); break;
    case 8: eight(); break;
    case 9: nine(); break;
  }
}
void pickDigit(int x) {
  PORTD |= (1 << 2);
  PORTD |= (1 << 3);
  PORTD |= (1 << 4);
  PORTD |= (1 << 5);
  
  switch (x) {
    case 0: PORTD &= ~(1 << 2); break;
    case 1: PORTD &= ~(1 << 3); break;
    case 2: PORTD &= ~(1 << 4); break;
    case 3: PORTD &= ~(1 << 5); break;
  }
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void clearLEDs() {
  PORTD &= ~(1 << 6);
  PORTD &= ~(1 << 7);
  PORTB &= ~(1 << 0);
  PORTB &= ~(1 << 1);
  PORTB &= ~(1 << 2);
  PORTB &= ~(1 << 3);
  PORTB &= ~(1 << 4);
}
void zero() {
  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB |= (1 << 2);
  PORTB |= (1 << 3);
  PORTB &= ~(1 << 4);
}
void one() {  
  PORTD &= ~(1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB &= ~(1 << 1);
  PORTB &= ~(1 << 2);
  PORTB &= ~(1 << 3);
  PORTB &= ~(1 << 4);
}
void two() {
  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB &= ~(1 << 0);
  PORTB |= (1 << 1);
  PORTB |= (1 << 2);
  PORTB &= ~(1 << 3);
  PORTB |= (1 << 4);
}

void three() {
  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB &= ~(1 << 2);
  PORTB &= ~(1 << 3);
  PORTB |= (1 << 4);
}

void four() {
  PORTD &= ~(1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB &= ~(1 << 1);
  PORTB &= ~(1 << 2);
  PORTB |= (1 << 3);
  PORTB |= (1 << 4);
}

void five() {
  PORTD |= (1 << 6);
  PORTD &= ~(1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB &= ~(1 << 2);
  PORTB |= (1 << 3);
  PORTB |= (1 << 4);
}

void six() {
  PORTD |= (1 << 6);
  PORTD &= ~(1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB |= (1 << 2);
  PORTB |= (1 << 3);
  PORTB |= (1 << 4);
}

void seven() {

  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB &= ~(1 << 1);
  PORTB &= ~(1 << 2);
  PORTB &= ~(1 << 3);
  PORTB &= ~(1 << 4);
}

void eight() {
  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB |= (1 << 2);
  PORTB |= (1 << 3);
  PORTB |= (1 << 4);
}

void nine() {
  PORTD |= (1 << 6);
  PORTD |= (1 << 7);
  PORTB |= (1 << 0);
  PORTB |= (1 << 1);
  PORTB &= ~(1 << 2);
  PORTB |= (1 << 3);
  PORTB |= (1 << 4);
}  
