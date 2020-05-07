//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//**************************************************************************************************************************************

//**********************************************
/*
 * Julio Lazo
 * Javier Archila
 * Juego programado en Tiva C para la clase de digital 2
 */
//**********************************************

#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"


#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  
//variables para case de movimiento de carros 1 y 2
int car_1 = 0;
int car_2 = 0;
int x_car_1 = 80;
int x_car_2 = 240;
int anim = 0;
int x = 0;
//variables en "y" de carros enemigos
int y_o_1 = 0;
int y_o_2 = 0;
int y_o_3 = 0;
int y_o_4 = 0;
int y_o_5 = 0;
int y_o_6 = 0;
int y_o_7 = 0;
int y_o_8 = 0;
int y_o_9 = 0;
int y_o_10 = 0;
int faster_1 = 0;
int faster_2 = 0;
int faster_3 = 0;
int faster_4 = 0;
int faster_5 = 0;
int faster_6 = 0;
int faster_7 = 0;
int faster_8 = 0;
int faster_9 = 0;
int faster_10 = 0;
//variable para parar el juego cuando se choca
int game_on = 1;

//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

//funciones de carros de jugadores
void red_car_animation(void);
void yellow_car_animation(void);

//funciones para los carros enemigos
void rand_car_1(void);
void rand_car_2(void);
void rand_car_3(void);
void rand_car_4(void);
void rand_car_5(void);
void rand_car_6(void);
void rand_car_7(void);
void rand_car_8(void);
void rand_car_9(void);
void rand_car_10(void);

//función para incializar variables
void vars_init(void);
//función para pantalla de inicio
void start_screen(void);

//sprites y bitmaps
extern uint8_t red_car_fwd [];
extern uint8_t red_car_right [];
extern uint8_t red_car_left [];
extern uint8_t yellow_car_fwd [];
extern uint8_t yellow_car_left [];
extern uint8_t yellow_car_right [];
extern uint8_t onco_car_1 [];
extern uint8_t onco_car_2 [];
extern uint8_t onco_car_3 [];
extern uint8_t onco_car_4 [];
extern uint8_t onco_car_5 [];
extern uint8_t onco_car_6 [];
extern uint8_t onco_car_7 [];
extern uint8_t onco_car_8 [];
extern uint8_t onco_car_9 [];
extern uint8_t onco_car_10 [];
extern uint8_t boom[];
extern uint8_t lasnaga[];
extern uint8_t inicio[];
extern uint8_t jug1_wins[];
extern uint8_t jug2_wins[];

int j1izq;
int j1der;
int j2izq;
int j2der;
int reset_btn;
int start_btn;
int end_to_start = 0;
int x_s = 1;
int explosion = 0;
int jug1_loser = 0;
int jug2_loser = 0;
int var1 = 0;
int var2 = 0;
int var3 = 0;
int init_snd = 0;
int fwd_snd = 0;
int turn_snd = 0;
int boom_snd = 0;

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  

  //PA2(JUG2, IZQUIERDA), PA3(JUG2, DERECHA), PA6(JUG1, IZQUIERDA), PA7(JUG1, DERECHA)
  pinMode (PA_2, INPUT);
  pinMode (PA_3, INPUT);
  pinMode (PA_6, INPUT);
  pinMode (PA_7, INPUT);
  pinMode (PF_4, INPUT);

  pinMode(PC_6, OUTPUT); //pin para música de inicio
  pinMode(PC_7, OUTPUT); //pin para sonido de giro
  pinMode(PD_6, OUTPUT); //pin para sonido de ir adelante
  pinMode(PD_7, OUTPUT); //pin para sonido de explosión
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  LCD_Init();
  vars_init();
  start_screen();
  LCD_Clear(0x8410);
  
  FillRect(155, 0, 3, 240,0xFFFF);
  FillRect(162, 0, 3, 240,0xFFFF);
  
  digitalWrite(PD_6, LOW);
  digitalWrite(PD_7, LOW);
  digitalWrite(PC_6, LOW);
  digitalWrite(PC_7, LOW);
  
}
//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
  reset_btn = digitalRead(PF_4);

  if(game_on==0 && jug1_loser == 1 || jug2_loser ==1){
    digitalWrite(PD_7, LOW);
    for(var1 = 0; var1 ==0; var1++){
      FillRect(0, 0, 320, 240, 0x0000);
      FillRect(0, 0, 320, 10, 0xffff);
      FillRect(0, 0, 10, 240, 0xffff);
      FillRect(0, 230, 320, 10, 0xffff);
      FillRect(310, 0, 10, 240, 0xffff);
      LCD_Bitmap(130, 100, 60, 60, lasnaga);
      LCD_Print("Press start to play again", 70, 180, 1, 0xffff, 0x0000);
    }
      
    if(jug1_loser == 1){
      jug1_loser = 0;
      for(var2 = 0; var2<=1; var2++){
        LCD_Bitmap(85, 50, 150, 20, jug2_wins);
      }
    }

    if(jug2_loser == 1){
      jug2_loser = 0;
      for(var3=0; var3<=1; var3++){
        LCD_Bitmap(85, 50, 150, 20, jug1_wins);
      }
    }
  }

  else if(game_on==0 && reset_btn==HIGH){
    game_on = 1;
    setup();
  }
  
  while(game_on){
    //PA2(JUG2, IZQUIERDA), PA3(JUG2, DERECHA), PA6(JUG1, IZQUIERDA), PA7(JUG1, DERECHA)
    j2izq = digitalRead(PA_2);
    j2der = digitalRead(PA_3);
    j1izq = digitalRead(PA_6);
    j1der = digitalRead(PA_7);

//    digitalWrite(PC_6, LOW);
//    digitalWrite(PC_7, LOW);
//    digitalWrite(PD_6, LOW);
//    digitalWrite(PD_7, LOW);

    if(car_1!=0 || car_2!=0){
      digitalWrite(PC_7, HIGH); //giro, arduino pin 5
      digitalWrite(PD_6, LOW); //recto, arduino pin 3
    }
    else if(car_1==0 && car_2==0){
      digitalWrite(PC_7, LOW);
      digitalWrite(PD_6, HIGH);
    }
    
  
    
    
    //////////////////////////////CARRO 1/////////////////////////////////
    if(x_car_1>=0 && (x_car_1+18)<=155){
      red_car_animation();
    }
    if(x_car_1<=0){
      x_car_1++;
    }
    if((x_car_1+18)>=155){
      x_car_1--;
    }
      
    //////////////////////////////CARRO 2//////////////////////////////////
    if(x_car_2>=166 && (x_car_2+18)<=320){
      yellow_car_animation();
    }
    if(x_car_2<=166){
      x_car_2++;
    }
    if((x_car_2+18)>=320){
      x_car_2--;
    }

    rand_car_1();
    rand_car_2();
    rand_car_3();
    rand_car_4();
    rand_car_5();
    rand_car_6();
    rand_car_7();
    rand_car_8();
    rand_car_9();
    rand_car_10();
    
  }
  
  
}
//PC_6 = init_snd


void start_screen(void){
  digitalWrite(PC_6, HIGH);
  while(end_to_start==0){
    if(x_s==1){
      LCD_Bitmap(0, 0, 320, 240, inicio);
      
    }
    x_s = 0;
    start_btn = digitalRead(PF_4);
    
    if(start_btn == HIGH){
      end_to_start = 1;
      digitalWrite(PC_6, LOW);
      setup();
    }
  }
}

//Función de inicialización de variables en cada juego (puesta en setup)
void vars_init(void){
  x_car_1 = 80;
  x_car_2 = 240;
  car_1 = 0;
  car_2 = 0;
  y_o_1 = -40;
  y_o_2 = -80;
  y_o_3 = -120;
  y_o_4 = -160;
  y_o_5 = -200;
  y_o_6 = -240;
  y_o_7 = -280;
  y_o_8 = -320;
  y_o_9 = -360;
  y_o_10 = -400;
  jug1_loser = 0;
  jug2_loser = 0;
  var1 = 0;
  var2 = 0;
  var3 = 0;
}


//función de movimiento de carro 1

void red_car_animation(void){
  //PA2(JUG2, IZQUIERDA), PA3(JUG2, DERECHA), PA6(JUG1, IZQUIERDA), PA7(JUG1, DERECHA)
  
  if((j1izq==LOW && j1der==LOW) || (j1izq==HIGH && j1der==HIGH)){ //recto
    car_1 = 0;

    }
  else if(j1izq==LOW && j1der==HIGH){//derecha
    car_1=1;

    }
  else if(j1izq=HIGH && j1der==LOW){//izquierda
    car_1=2;

    }

  switch (car_1){
    case 0: 
      anim = (x/5)%4;
      LCD_Sprite(x_car_1, 120, 18, 25, red_car_fwd, 4, anim, 0, 0);
      x++;
      break;
//void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
    case 1: //DERECHA
      anim = (x/5)%2;
      LCD_Sprite(x_car_1, 120, 18, 25, red_car_right, 2, anim, 0, 0);
      
      x++;
      x_car_1 = x_car_1 + 2;
      V_line(x_car_1-1, 120, 25, 0x8410);
      
      
      break;  

    case 2: //IZQUIERDA
      anim = (x/5)%2;
      LCD_Sprite(x_car_1, 120, 18, 25, red_car_left, 2, anim, 0, 0);
      x++;
      x_car_1 = x_car_1 - 2;
      
      break; 
    }
  }


//función de movimiento de carro 2
void yellow_car_animation(void){
  //PA2(JUG2, IZQUIERDA), PA3(JUG2, DERECHA), PA6(JUG1, IZQUIERDA), PA7(JUG1, DERECHA)
  if((j2izq==LOW && j2der==LOW) || (j2izq==HIGH && j2der==HIGH)){
    car_2 = 0;

    }
  else if(j2izq==LOW && j2der==HIGH){//derecha
    car_2=1;

    }
  else if(j2izq=HIGH && j2der==LOW){//izquierda
    car_2=2;

    }

  switch (car_2){
    case 0: 
      anim = (x/5)%4;
      LCD_Sprite(x_car_2, 120, 18, 25, yellow_car_fwd, 4, anim, 0, 0);
      x++;
      break;

    case 1: //DERECHA
      anim = (x/5)%2;
      LCD_Sprite(x_car_2, 120, 18, 25, yellow_car_right, 2, anim, 0, 0);
      x++;
      x_car_2 = x_car_2 + 2;
      V_line(x_car_2-1, 120, 25, 0x8410);
      break;  

    case 2: //IZQUIERDA
      anim = (x/5)%2;
      LCD_Sprite(x_car_2, 120, 18, 25, yellow_car_left, 2, anim, 0, 0);
      x++;
      x_car_2 = x_car_2 - 2;
      
      break; 
    }
  }


  
//void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
//void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
//void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);



//Función de carro enemigo 1
void rand_car_1(void){
  if(y_o_1 >= 0){
    LCD_Bitmap(55, y_o_1, 20, 40, onco_car_1);
    H_line(55, y_o_1, 20, 0x8410);
    y_o_1++;
  }
  if(y_o_1>=240){
    y_o_1 = -40;
  }
  if(y_o_1<=0){
    y_o_1++;
  }  
  if(y_o_1+40>=120 && y_o_1<=145 && x_car_1+18>=55 && x_car_1<=75){
    game_on = 0;
    jug1_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_1-20, y_o_1+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}
//Función de carro enemigo 2
void rand_car_2(void){
  if(y_o_2 >= 0){
    LCD_Bitmap(170, y_o_2, 20, 40, onco_car_2);
    H_line(170, y_o_2, 20, 0x8410);
    y_o_2++;
  }
  if(y_o_2==240){
    y_o_2=-80;
  }
  if(y_o_2<=0){
    y_o_2++;
  }    
//if(y_o_2+alto_bitmap>=120 && y_o_2<=145 && x_car_2+18>=pos inicial del bitmap && x_car_2<=pos inicial del bitmap + ancho bitmap){
  if(y_o_2+40>=120 && y_o_2<=145 && x_car_2+18>=170 && x_car_2<=190){
    game_on = 0;
    jug2_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_2-20, y_o_2+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 3
void rand_car_3(void){
  if(y_o_3 >= 0){
    LCD_Bitmap(85, y_o_3, 20, 40, onco_car_3);
    H_line(85, y_o_3, 20, 0x8410);
    y_o_3++;
  }
  if(y_o_3==240){
    y_o_3=-120;
  }
  if(y_o_3<=0){
    y_o_3++;
  }  
  if(y_o_3+40>=120 && y_o_3<=145 && x_car_1+18>=85 && x_car_1<=105){
    game_on = 0;
    jug1_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_1-20, y_o_3+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 4
void rand_car_4(void){
  if(y_o_4 >= 0){
    LCD_Bitmap(260, y_o_4, 20, 40, onco_car_4);
    H_line(260, y_o_4, 20, 0x8410);
    y_o_4++;
  }
  if(y_o_4==240){
    y_o_4=-160;
  }
  if(y_o_4<=0){
    y_o_4++;
  }  
  if(y_o_4+40>=120 && y_o_4<=145 && x_car_2+18>=260 && x_car_2<=280){
    game_on = 0;
    jug2_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_2-20, y_o_4+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 5
void rand_car_5(void){
  if(y_o_5 >= 0){
    LCD_Bitmap(25, y_o_5, 20, 40, onco_car_5);
    H_line(25, y_o_5, 20, 0x8410);
    y_o_5++;
  }
  if(y_o_5==240){
    y_o_5=-200;
  }
  if(y_o_5<=0){
    y_o_5++;
  }  
  if(y_o_5+40>=120 && y_o_5<=145 && x_car_1+18>=25 && x_car_1<=45){
    game_on = 0;
    jug1_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_1-20, y_o_5+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 6
void rand_car_6(void){
  if(y_o_6 >= 0){
    LCD_Bitmap(200, y_o_6, 20, 40, onco_car_6);
    H_line(200, y_o_6, 20, 0x8410);
    y_o_6++;
  }
  if(y_o_6==240){
    y_o_6=-240;
  }
  if(y_o_6<=0){
    y_o_6++;
  }  
  if(y_o_6+40>=120 && y_o_6<=145 && x_car_2+18>=200 && x_car_2<=220){
    game_on = 0;
    jug2_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_2-20, y_o_6+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 7
void rand_car_7(void){
  if(y_o_7 >= 0){
    LCD_Bitmap(115, y_o_7, 20, 40, onco_car_7);
    H_line(115, y_o_7, 20, 0x8410);
    y_o_7++;
  }
  if(y_o_7==240){
    y_o_7=-280;
  }
  if(y_o_7<=0){
    y_o_7++;
  }  
  if(y_o_7+40>=120 && y_o_7<=145 && x_car_1+18>=115 && x_car_1<=135){
    game_on = 0;
    jug1_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_1-20, y_o_7+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 8
void rand_car_8(void){
  if(y_o_8 >= 0){
    LCD_Bitmap(230, y_o_8, 20, 40, onco_car_8);
    H_line(230, y_o_8, 20, 0x8410);
    y_o_8++;
  }
  if(y_o_8==240){
    y_o_8=-320;
  }
  if(y_o_8<=0){
    y_o_8++;
  }  
  if(y_o_8+40>=120 && y_o_8<=145 && x_car_2+18>=230 && x_car_2<=250){
    game_on = 0;
    jug2_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_2-20, y_o_8+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 9
void rand_car_9(void){
  if(y_o_9 >= 0){
    LCD_Bitmap(135, y_o_9, 20, 40, onco_car_9);
    H_line(135, y_o_9, 20, 0x8410);
    y_o_9++;
  }
  if(y_o_9==240){
    y_o_9=-360;
  }
  if(y_o_9<=0){
    y_o_9++;
  }  
  if(y_o_9+40>=120 && y_o_9<=145 && x_car_1+18>=135 && x_car_1<=155){
    game_on = 0;
    jug1_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_1-20, y_o_9+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}

//Función de carro enemigo 10
void rand_car_10(void){
  if(y_o_10 >= 0){
    LCD_Bitmap(290, y_o_10, 20, 40, onco_car_10);
    H_line(290, y_o_10, 20, 0x8410);
    y_o_10++;
  }
  if(y_o_10==240){
    y_o_10=-400;
  }
  if(y_o_10<=0){
    y_o_10++;
  }  
  if(y_o_10+40>=120 && y_o_10<=145 && x_car_2+18>=290 && x_car_2<=310){
    game_on = 0;
    jug2_loser = 1;
    digitalWrite(PD_7, HIGH);
    for(explosion=0; explosion<=10; explosion++){
      delay(10);
      LCD_Sprite(x_car_2-20, y_o_10+10, 50, 50, boom, 7, explosion, 0, 0);
    }
    explosion=0;
  }
}


/*
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * *****************************************************************************************************************************************************************************************
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * *****************************************************************************************************************************************************************************************
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * *****************************************************************************************************************************************************************************************
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * *****************************************************************************************************************************************************************************************
 * /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
 * *****************************************************************************************************************************************************************************************
 * 
 */
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}

  
