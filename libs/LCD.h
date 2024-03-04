/* File: LCD.H
 * PIC: 18F452 w/X OSC @ 4MHz, 5v
 * Program Version 1.0
 * Program Description: This program header provides routines for controlling
 * a 16x2 LCD display.
 * Adapted by Microchip and Kyle Wallder (1909160)
*/

// Libraries
#include <xc.h>

// Definitions
#define _XTAL_FREQ 4000000      // 4MHz External Clock
#define LCD_EN_Delay 500        // LCD Enable Pin delay
#define LCD_RS_D TRISD4         // Assign RS Pin to Port D bit 4 (Output)
#define LCD_EN_D TRISC6         // Assign EN Pin to Port D bit 6 (Output)
#define RS RD4                  // RS = RD2
#define RW RC7                  // RW = RD3
#define EN RC6                  // EN = RC4
#define D4 RC5                  // D4 = RC5
#define D5 RC4                  // D5 = RC6
#define D6 RD3                  // D6 = RC7
#define D7 RD2                  // D7 = RD4


// ********* Call Functions from corresponding class ********* //
void LCD_Init();                                    // Initialise The LCD For 4-Bit Interface
void LCD_Clear();                                   // Clear The LCD Display
void LCD_CMD(unsigned char);                        // Send Command To LCD
void LCD_DATA(unsigned char);                       // Send 4-Bit Data To LCD
void LCD_Set_Cursor(unsigned char, unsigned char);  // Set Cursor Position
void LCD_Write_Char(char);                          // Write Character To LCD At Current Position
void LCD_Write_String(char*);                       // Write A String To LCD
void LCD_Shift_Right();                             // Shift text on LCD right
void LCD_Shift_Left();                              // Shift text on LCD left
void LCD_Write_Integer(int);                        // Convert integer to string and display on LCD
void LCD_Write_Float(float);                        // Convert float to string and display on LCD


// ****** End of LCD Header ****** //