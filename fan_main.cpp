// C++ code
//    


/*       Name: Joey Yang | Email: joey.yang.j@gmail.com

    
Description:
    This exercise implements AUTO mode functionality. 
    Measure the temperature using the thermistor.
    Display the temperature in degrees Fahrenheit (e.g., 70º F) on the second row of the LCD Screen.
    Update the temperature every 1 second
    Define a temperature threshold that is appropriate to your environment
    Pushing the joystick up increases the temperature threshold by 1º F
    If the joystick is held in the up position, do not increase the temperature threshold
    To increase the temperature threshold, the joystick must return to the idle position before being pressed up again.
    Pushing the joystick down decreases the temperature threshold by 1º F
    If the joystick is held in the down position, do not decrease the temperature threshold
    To decrease the temperature threshold, the joystick must return to the idle position before being pressed down again.
    The DC motor (fan) has a PWM period of 10 ms.
    The duty cycle of the fan depends on the difference between the measured temperature T and the threshold H. 
    The duty cycle is 0 if T < H
    The duty cycle is 10% if T - H = 1º 
    The duty cycle is 20% if T - H = 2º
    ... 
    The duty cycle is 90% if T - H = 9º 
    The duty cycle is 100% if T - H > 10º
              
 *        

 *         I acknowledge all content contained herein, excluding template or example code, is my own original work.

 *

 *         Demo Link: <https://www.youtube.com/watch?v=RGgkL0LvwqI&ab_channel=JoeyYang>

 */

#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "LCD.h"
#include "serialATmega.h"
#include "stdio.h"
#include "stdlib.h"




//TODO: declare variables for cross-task communication
int threshold = 80;
int fdCycle = 0;
int temp = fdCycle;
int i = 0;
int manualPeriod = 10;
int fLow = 0;
int fHigh = 0;
int j = 0;

int temp2 = 0;

int A_or_M = 0; //0 = manual : 1 = auto


int tempReading = 0;/* your code to fetch analog value of thermistor with ADC_read() */

int tempK = log(10000.0 * ((1024.0 / tempReading - 1)));



int tempF =  /* your code convert Kelvin into Fahrenheit */0;










/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 3


//Task struct for concurrent synchSMs implmentations
typedef struct _task{
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;


//TODO: Define Periods for each task
// e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long TASK1_PERIOD = 1000;
const unsigned long TASK2_PERIOD = 100;
const unsigned long TASK3_PERIOD = 1;
const unsigned long GCD_PERIOD = /*findGCD(TASK1_PERIOD, TASK2_PERIOD)*/ 1; /* TODO: Calulate GCD of tasks */ 

task tasks[NUM_TASKS]; // declared task array with 5 tasks

//TODO: Define, for each task:
// (1) enums and
// (2) tick functions

enum LCD_screen {LCD_Start, MANUAL, MANUALPRESS, AUTO, AUTOPRESS};
int LCD_TICK(int state);

int LCD_TICK(int state)
{
    tempReading = ADC_read(0);


    int ones = (fdCycle / 1) % 10;
    int tens = (fdCycle / 10) % 10;
    int hundreds = (fdCycle / 100) % 10;

    char ones_char = ones + '0';
    char tens_char = tens + '0';
    char hundreds_char = hundreds + '0';

    int tempOnes = (tempF / 1) % 10;
    int tempTens = (tempF / 10) % 10;

    char tempRead_oneschar = tempOnes + '0';
    char tempRead_tenschar = tempTens + '0';


    int thOnes = (threshold / 1) % 10;
    int thTens = (threshold / 10) % 10;

    char th_oneschar = thOnes + '0';
    char th_tenschar = thTens + '0';




    switch(state) { //state transitions
        case LCD_Start:
            state = MANUAL;
            lcd_clear();
            
        break;

        case MANUAL:
        if(ADC_read(2) >= 0 && ADC_read(2) <= 200) {
            state = MANUALPRESS;
        }
        break;

        case MANUALPRESS:
        if(ADC_read(2) <= 1023 && ADC_read(2) >= 800)
        {
            state = AUTO;
            A_or_M = 1;
            lcd_clear();
        }
        break;

        case AUTO:
        if(ADC_read(2) >= 0 && ADC_read(2) <= 200) {
            state = AUTOPRESS;
            
        }
        break;
        
        case AUTOPRESS:
        if(ADC_read(2) <= 1023 && ADC_read(2) >= 800)
        {
            state = MANUAL;
            A_or_M = 0;
            lcd_clear();
        }
        
        break;
    
        default:
         state = LCD_Start;

    }

    switch(state) { //state actions

        case MANUAL:
        
       if(fdCycle == 100)
        {
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: MANUAL");
            lcd_goto_xy (1, 0);
            lcd_write_str("FAN: ");
            lcd_write_character(hundreds_char);
            lcd_write_character(tens_char);
            lcd_write_character(ones_char);
            lcd_write_character('%');
            A_or_M = 0;
        }
        else if(fdCycle == 0)
        {
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: MANUAL");
            lcd_goto_xy (1, 0);
            lcd_write_str("FAN: ");
            lcd_write_character(ones_char);
            lcd_write_character('%');
            lcd_goto_xy (1, 7);
            lcd_write_character(' ');
            A_or_M = 0;
        }
        else{
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: MANUAL");
            lcd_goto_xy (1, 0);
            lcd_write_str("FAN: ");
            lcd_write_character(tens_char);
            lcd_write_character(ones_char);
            lcd_write_character('%');
            lcd_goto_xy (1, 8);
            lcd_write_character(' ');
            A_or_M = 0;
        }


            break;

        case MANUALPRESS:
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: MANUAL");
            break;


        case AUTO:
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: AUTO");
            lcd_goto_xy (1, 0);
            lcd_write_str("Curr:");
            lcd_write_character(tempRead_tenschar);
            lcd_write_character(tempRead_oneschar);
            lcd_write_character('F');
            lcd_write_character(' ');
            lcd_write_str("Th:");
            lcd_write_character(th_tenschar);
            lcd_write_character(th_oneschar);
            lcd_write_character('F');

            A_or_M = 1;
            break;

            
        
        case AUTOPRESS:
            lcd_goto_xy (0, 0);
            lcd_write_str("Mode: AUTO");
            
            break;

           default:
            break;
    }
    return state;
}

enum buzz {Buzz_Start, buzz_press, buzz_release, buzz_up, buzz_down};
int buzz_tick(int state);

int buzz_tick(int state)
{

    switch(state)
    {
        case Buzz_Start:
            state = buzz_release;
        break;

        case buzz_release:
        if(ADC_read(2) >= 0 && ADC_read(2) <= 200) {
                state = buzz_press;
                i = 0;
        }
         else if((ADC_read(1) >= 900 && ADC_read(1) <= 1023))
        {
            state = buzz_up;
            i = 0;
        }
        else if((ADC_read(1) >= 0 && ADC_read(1) <= 200))
        {
            state = buzz_down;
            i = 0;
        }
        break;

        case buzz_press:
            if(ADC_read(2) <= 1023 && ADC_read(2) >= 800){
                state = buzz_release;
                i = 0;
            }
        break;

        case buzz_up:
            if(ADC_read(1) >= 500 && ADC_read(1) <= 600)
            {
                state = buzz_release;
                fdCycle = fdCycle + 10;
                threshold = threshold + 1;
                i = 0;
            }
           
        break;

        case buzz_down:
            if(ADC_read(1) >= 500 && ADC_read(1) <= 600)
            {
                state = buzz_release;
                fdCycle = fdCycle - 10;
                threshold = threshold - 1;
                i = 0;
            }
           
        break;

        default:
            state = Buzz_Start;

    }

    switch(state)
    {
        case buzz_press:
            if(i < 2)
            {
              //  serial_println("buzzon");
                PORTB = SetBit(PORTB, 0 , 1);
                i++;
            }
            else
            {
                PORTB = SetBit(PORTB, 0 , 0);
               // serial_println("buzzoff");
            }
            
        break;

        case buzz_up:
            if(i < 2)
            {
               // serial_println("buzzon");
                PORTB = SetBit(PORTB, 0 , 1);
                i++;
            }
            else
            {
                PORTB = SetBit(PORTB, 0 , 0);
               // serial_println("buzzoff");
            }
        break;

        case buzz_down:
            if(i < 4)
            {
                //serial_println("buzzon");
                PORTB = SetBit(PORTB, 0 , 1);
                i++;
            }
            else
            {
                PORTB = SetBit(PORTB, 0 , 0);
               // serial_println("buzzoff");
            }
        break;

        case buzz_release:
                 PORTB = SetBit(PORTB, 0, 0);
            break;


            default:
                break;
    }

    return state;
}


enum fan {fan_start, fanHigh, fanLow};
int fan_tick(int state);

int fan_tick(int state)
{

      double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));

        tempK = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);

        tempF = ((tempK - 273.15)*(9.0/5)) + 32.0;/* your code convert Kelvin into Fahrenheit */

    if(A_or_M == 0)
    {
        if(fdCycle < 0)
        {
            fdCycle = 0;
        }
        else if(fdCycle > 100)
        {
            fdCycle = 100;
        }

        temp = fdCycle / 10;

        fHigh = temp;
        fLow = (manualPeriod - (temp)); 
        temp2 = fdCycle;
    }
    else if(A_or_M == 1)
    {
      if(tempF <= threshold)
      {
        fdCycle = 0;
      }
        else if(tempF - threshold == 1)
        {
          fdCycle = 10;
        }
        else if(tempF - threshold == 2)
        {
          fdCycle = 20;
        }
        else if(tempF - threshold == 3)
        {
          fdCycle = 30;
        }
        else if(tempF - threshold == 4)
        {
          fdCycle = 40;
        }
        else if(tempF - threshold == 5)
        {
          fdCycle = 50;
        }
        else if(tempF - threshold == 6)
        {
          fdCycle = 60;
        }
        else if(tempF - threshold == 7)
        {
          fdCycle = 70;
        }
        else if(tempF - threshold == 8)
        {
          fdCycle = 80;
        }
        else if(tempF - threshold == 8)
        {
          fdCycle = 80;
        }
        else if(tempF - threshold == 9)
        {
          fdCycle = 90;
        }
        else {
          fdCycle = 100;
        }

        temp = fdCycle / 10;

        fHigh = temp;
        fLow = (manualPeriod - (temp)); 
        fdCycle = temp2;
    }

    switch(state)
    {
        case fan_start:
            state = fanHigh;
        break;

        case fanHigh:
        if(j < fHigh){
            state = fanHigh;
        }
        else if(j >= fHigh){
            state = fanLow;
            j = 0;
          }
        break;

        case fanLow:
        if(j < fLow)
        {
            state = fanLow;
        }
        else if(j >= fLow)
        {
            state = fanHigh;
            j = 0;
        }
        break;

        

        default:
         state = fan_start;
    }


    switch(state)
    {
        case fanHigh:
            PORTB = SetBit(PORTB, 1, 1);
            j++;
        break;

        case fanLow:
            PORTB = SetBit(PORTB, 1, 0);
            j++;
        break;

        default:
            break;
    }
    return state;
}



void TimerISR() {
	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
		if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
			tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
		}
		tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
	}
}



int main(void) {
    //TODO: initialize all your inputs and ouputs
      DDRD = 0xFF; PORTD = 0x00;
      DDRB = 0xFF; PORTB = 0x00;
      DDRC = 0xF8; PORTC = 0x07; //11111000

    ADC_init();   // initializes ADC
    init_sonar(); // initializes sonar
    lcd_init();
    
    serial_init(9600);

    //TODO: Initialize tasks here
    // e.g. tasks[0].period = TASK1_PERIOD
    // tasks[0].state = ...
    // tasks[0].timeElapsed = ...
    // tasks[0].TickFct = &task1_tick_function;

    tasks[0].period = TASK1_PERIOD;
    tasks[0].state = LCD_Start;
    tasks[0].elapsedTime = TASK1_PERIOD;
    tasks[0].TickFct = &LCD_TICK;

    tasks[1].period = TASK2_PERIOD;
    tasks[1].state = Buzz_Start;
    tasks[1].elapsedTime = TASK2_PERIOD;
    tasks[1].TickFct = &buzz_tick;

    tasks[2].period = TASK3_PERIOD;
    tasks[2].state = fan_start;
    tasks[2].elapsedTime = TASK3_PERIOD;
    tasks[2].TickFct = &fan_tick;


         
        

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {

       // double tempK = log(10000.0 * ((1024.0 / tempReading - 1)));
       // tempK= 1/ (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK)) * tempK);
       // tempF = ((tempK - 273.15)*(9.0/5)) + 32.0;/* your code convert Kelvin into Fahrenheit */
              // serial_println(tempReading);
               //serial_println(tempF);
               //serial_println(fdCycle);
              //serial_println(ADC_read(0));
               //serial_println(tempK);
            //   serial_println(tempReading);

    }

    return 0;
}
