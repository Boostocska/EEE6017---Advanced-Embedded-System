/* File: LCD.C
 * PIC: 18F452 w/X OSC @ 4MHz, 5v
 * Program Version 1.0
 * Program Description: This Library allows you to interface with compatible LCDs.
 * Adapted by Microchip and Kyle Wallder (1909160)
*/

// Libraries
#include "LCD.h"
#include <xc.h>
#include <p18cxxx.h>
#include <stdlib.h>
#include <stdio.h>

// Definitions
#define _XTAL_FREQ 4000000      // External Clock of 4MHz

// ********* Alphanumeric LCD 16x2 Routines ********* //

// This function computes the LCD received data on D4 to D7, in 4 bit mode.  
// This is what is used to create the character displays as shown on data sheet
void LCD_DATA(unsigned char Data)
{
    if(Data & 1) 
        D4 = 1;
    else
        D4 = 0;
    if(Data & 2)
        D5 = 1;
    else
        D5 = 0;
    if(Data & 4)
        D6 = 1;
    else
        D6 = 0;
    if(Data & 8) 
        D7 = 1;
    else
        D7 = 0;
}

// This function controls the commands of the LCD as specified within the datasheet
// It is a series of ones and zeros sent to control the enable and RS pin of the LCD
void LCD_CMD(unsigned char CMD)
{
    // Select Command Register
    RS = 0;
    RW = 0;
    
    // Move The Command Data To LCD
    LCD_DATA(CMD);
    
    // Send The EN Clock Signal
    EN  = 1;         
    __delay_us(LCD_EN_Delay);
    EN  = 0;         
}

// This function simply clears the LCD
// Note the 50mS delay.  
// This is required to ensure the LCD has time to clear
void LCD_Clear()
{
    LCD_CMD(0);
    LCD_CMD(1);
    __delay_ms(50);
}

// This function sets the cursor location of the LCD
// It is used in conjunction with the LCD CMD and provides an instruction
void LCD_Set_Cursor(unsigned char r, unsigned char c)
{
    unsigned char Temp,Low4,High4;
    if(r == 1)
    {
      Temp  = 0x80 + c - 1;         //0x80 is used to move the cursor
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
    if(r == 2)
    {
      Temp  = 0xC0 + c - 1;
      High4 = Temp >> 4;
      Low4  = Temp & 0x0F;
      LCD_CMD(High4);
      LCD_CMD(Low4);
    }
}

// LCD Initialisation Function
// This is specified within the data sheet
// Set up for 4 bit operation
void LCD_Init()
{
  // IO Pin Configurations
  LCD_RS_D = 0;
  LCD_EN_D = 0;
  
  // The Init. Procedure As Described In The Datasheet
  LCD_DATA(0x00);
  __delay_ms(30);
  __delay_us(LCD_EN_Delay);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_us(150);
  LCD_CMD(0x03); 
  LCD_CMD(0x02);
  LCD_CMD(0x02);
  LCD_CMD(0x08);
  LCD_CMD(0x00);
  LCD_CMD(0x0C);
  LCD_CMD(0x00);
  LCD_CMD(0x06);
}

// This function allows for the writing of an individual character to the display
// Is required for displaying keypad inputs on the LCD
void LCD_Write_Char(char Data)
{
   char Low4,High4;
   Low4  = Data & 0x0F;
   High4 = Data & 0xF0;
   RS = 1;        
   LCD_DATA(High4>>4);  
   EN = 1;
   __delay_us(LCD_EN_Delay); 
   EN = 0;
   __delay_us(LCD_EN_Delay); 
   LCD_DATA(Low4); 
   EN = 1;
   __delay_us(LCD_EN_Delay); 
   EN = 0;
   __delay_us(LCD_EN_Delay); 
}

// This function is used to write strings to the LCD
// Uses a simple for loop to increment each char
void LCD_Write_String(char *str)
{
    int i;
    for(i=0;str[i]!='\0';i++)
       LCD_Write_Char(str[i]);
}

// This function shifts text on the LCD right
// Function returns nothing.
void LCD_Shift_Right()
{
    LCD_CMD(0x01);
    LCD_CMD(0x0C);
}
// This function shifts text on the LCD left
// Function returns nothing.
void LCD_Shift_Left()
{
    LCD_CMD(0x01);
    LCD_CMD(0x08);
}

// This function converts an integer to string and display
// Function returns nothing.
void LCD_Write_Integer(int v)
{
    unsigned char buf[8];
    sprintf(buf,"%d",v);
    //LCD_Write_String(itoa(buf, v, 10));
    LCD_Write_String(buf);
}

// This function converts a float to string and display
// Function returns nothing.


// ****** End of LCD Class ****** //
void LCD_Write_Float(float f)
{
    char buf11[100];
    sprintf(buf11,"%f",f);
    LCD_Write_String(buf11);
}