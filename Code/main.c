#include "system.h"
#include "uart.h"
#include "adc.h"
#include "lcd.h"

void delay1 (unsigned long a)
{
   a *= 1000000;
   while (--a!=0); 
}

void main()
{
   unsigned short temp;
   unsigned short level;
   
   IO0DIR = (1<<4);       // for the heater controller, pin 4 will be used
   
   clock_init();   //initialize sytem clock
   
   adc_init();     //initialize ADC0
   
   lcd_init();         //initialize LCD... see function definition in "lcd.c"     
   
   lcd_backlight(ON);  //turn on the backlight
   
   int column = 1;      // set the column to 1
   int roww = 1;        // set the row to 1
   
   
   while(1)
   {
     
     lcd_write("temp: ", roww, column);         // write to the LCD's first row
     temp = adc_read(0);                        // read from port 27 with 0
     temp /= 3.125;                             // convert the thermometer input to degree celcius
     put_int(temp, column + 6, roww);           // print the temp to LCD
     
     if (temp < 55){            // turns on heater for tempratures below 55 degree celcius
       IO0SET = (1<<4);
     } else if(temp > 71){      // turns off heater for tempratures above 71 degree celcius
         IO0CLR = (1<<4);
     }
     
     lcd_write("level: ", roww+1, column);      // write to the LCD's second row
     level = adc_read(1);                       // read analog input from port 27 with 1 => port 28
     level /= 10;                               // maps the level sensor output to values( 0 - 100) 
     put_int(level, roww+1, column + 7);        // print the level to the LCD
     
     if (level < 50){                           // turns the pump ON if level is below half level
        PINSEL0_bit.P0_0 = 1; 
     } else if(level > 100){                    // turns the pump OFF if the level is equal or above full
        PINSEL0_bit.P0_0 = 0; 
     }
     
     delay1(2);                                 // wait for 2 second before refreshing
   }
}


   
   