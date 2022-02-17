#include "NXP/iolpc2148.h"
#include "lcd.h"

void delay(unsigned long a);

/*-------------------------------------------------------------------------
   Function Name: lcd_command

   Parameters: Command to be sent to LCD
 
   Return:  None
 
   Description: Sends a command byte to the LCD COG controller (Hitachi HD4878)

   Note: See the LCD datasheet 
 ---------------------------------------------------------------------------*/
void lcd_command(char cmd)
{
   IO1CLR_bit.P1_24 = 1;    //command mode (RS = 0)
   
   IO1PIN = cmd<<16;        //send command
   
   IO1SET_bit.P1_25 = 1;    //Set the enable input (E)
   delay(10);
   
   IO1CLR_bit.P1_25 = 1;  //toggle the E input to latch the command to the LCD controller (falling edge)
   delay(100);            //delay a little for the LCD to settle
}

/*-------------------------------------------------------------------------
   Function Name: lcd_data

   Parameters: data to be sent to LCD
 
   Return:  None
 
   Description: Sends a command byte to the LCD COG controller (Hitachi HD4878)

   Note: See the LCD datasheet 
 ---------------------------------------------------------------------------*/
void lcd_data(char data)
{
   IO1SET_bit.P1_24 = 1;  //data mode (RS = 1)
   
   //FIO1MASK = 0xFF00FFFF;   //mask the unused bits of the PIO (to keep them unaffected)
   IO1PIN =  ((data<<16)|0x01000000);        //send data
   //FIO1MASK = 0xF0FFFFFF;   //unmask RS and E
   
   IO1SET_bit.P1_25 = 1;   //Set the enable input (E)
   delay(10);
   IO1CLR_bit.P1_25 = 1;   //toggle the E input to latch the command to the LCD controller (falling edge)
   delay(50000);              //delay a little for the LCD to settle
}

/*-------------------------------------------------------------------------
   Function Name: lcd_gotoXY

   Parameters: row and column address of LCD character
 
   Return:  None
 
   Description: put the cursor to address (row,col)
 ---------------------------------------------------------------------------*/
void lcd_gotoXY(char row, char col)
{
  char address;
  
  if((row > 2)||(col>16)) return;     //address out of range, just return
  
  if(row == 1) address = 0x80;        //first line
  else address = 0xC0;                //second line
  
  lcd_command(address + col);         //set address (row and column) 
}

/*-------------------------------------------------------------------------
   Function Name: lcd_put

   Parameters: the character to be put on LCD and the position on the screen
 
   Return:  None
 
   Description: put a character on LCD at address specified by (row,col)
 ---------------------------------------------------------------------------*/
void lcd_put(char character, char row, char col)
{
   lcd_gotoXY(row,col);
   lcd_data(character);
}

/*-------------------------------------------------------------------------
   Function Name: lcd_write

   Parameters: the string to be printed on LCD and the starting position on the screen
 
   Return:  None
 
   Description: prints a character string on LCD starting at address specified by (row,col)
 ---------------------------------------------------------------------------*/
void lcd_write(char *buffer,char row,char col)
{
   lcd_gotoXY(row,col);
   
   while(*buffer != 0)
   {
      lcd_data(*buffer++);
   }
}

/*-------------------------------------------------------------------------
   Function Name: lcd_init

   Parameters: None
 
   Return:  None
 
   Description: initializes the LCD module (see initialization process on datasheet)
 ---------------------------------------------------------------------------*/
void lcd_init(void)
{
   IO1DIR |= 0xFFFFFFFF;  //set direction to output
   IO0DIR |= 0x40400000;  //set P0_22 and P0_30 as output (LCD_RW and LCD_BKL resp)
      
   delay(160000);         //around 20ms
   
   IO0CLR_bit.P0_22 = 1;  //LCD write mode
   
   lcd_command(0x01);     //clear display
   delay(8000);           //around 1ms
   
   lcd_command(0x38);     //LCD function set, 8-bit mode, 5x7 font, 2 lines display
   delay(40000);          //around 5ms
   
   lcd_command(0x38);     //LCD function set, 8-bit mode, 5x7 font, 2 lines display
   delay(40000);          //around 5ms
   
   lcd_command(0x08);     //turn off LCD
   delay(40000);          //around 5ms
   
   lcd_command(0x0E);     //turn on LCD, cursor on, blink off
   delay(160000);         //around 20ms
      
   lcd_command(0x06);     //Entry mode set, auto-increment address when writing
   delay(40000);          //around 5ms
}

/*-------------------------------------------------------------------------
   Function Name: lcd_backlight

   Parameters: option, either ON or OFF
 
   Return:  None
 
   Description: Turns the LCD backlight ON or OFF (only on the EduBoard)

   Note: See the EduBoard schematic (page 5)
 ---------------------------------------------------------------------------*/
void lcd_backlight(char option)
{
   IO0DIR_bit.P0_30 = 1;
   
   if(option == ON)
      IO0SET_bit.P0_30 = 1;    //backlight ON
   
   else
      IO0CLR_bit.P0_30 = 1;    //backlight OFF
}
void put_int(unsigned short integer, char row, char col)
{
   char digit[4];
     
   digit[0] = integer/1000;   //first digit
   
   integer %= 1000;           //The remainder after dividing by 1000
   
   digit[1] = integer/100;   //the second digit
   
   integer %= 100;           //The remainder after dividing by 100
   
   digit[2] = integer/10;    //third digit
   
   digit[3] = integer%10;    //fourth digit
   
   //change digits in to ASCII and print (if not zero) (add 0x30 or 48 to change to ASCII)
   for(int i=0; i<4; i++)
   {
      lcd_put(digit[i]+48, row, col+i);
   }
}

//private function for a simple delay
void delay(unsigned long a)
{
   while(--a != 0);
}