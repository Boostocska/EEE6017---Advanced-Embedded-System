/*
 * File:   LCD_Erik.c
 * Author: Boostocska
 *
 * Created on 2023. április 10., 14:54
 * 
 * In 4 bit: HIGH 4 bits then LOW 4 bits
 */

#include <xc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "LCD_Erik.h"

/* LCD Initialization with configurable bit mode
 * bitMode = "HIGH" -> 8 bit 
 * bitMode = "LOW" -> 4 bit
 */
void LCD_Init(_Bool bitModeSelect, unsigned int lines, _Bool fontType){
    _bitMode = bitModeSelect;
    __delay_ms(50);     // Minimum delay in Datasheet 40ms on startup
    LCD_RW = 0;
    LCD_RS = 0;
    LCD_EN = 0;
    LCD_setBitMode();
    //LCD_on();
    //LCD_cursor(0);
}

void LCD_setBitMode(void){
    LCD_Write(0x30);    // Display Init
    __delay_us(40);
    
    if(_bitMode){
        LCD_CMD(0x);
    }
    else{
        LCD_Write(0x30);  //Function set - 4 bit
        __delay_us(40);
    }
}

void LCD_Write(unsigned char command){
    if (_bitMode){
        LCD_DATA_0 = command == 0x01 ? 0 : 1;
        LCD_DATA_1 = command == 0x02 ? 0 : 1;
        LCD_DATA_2 = command == 0x04 ? 0 : 1;
        LCD_DATA_3 = command == 0x08 ? 0 : 1;
    }
    
    LCD_DATA_4 = command == 0x10 ? 0 : 1;
    LCD_DATA_5 = command == 0x20 ? 0 : 1;
    LCD_DATA_6 = command == 0x40 ? 0 : 1;
    LCD_DATA_7 = command == 0x80 ? 0 : 1;
    
    LCD_Enable();
}

void LCD_Enable(void){
    LCD_EN = 1;
    __delay_us(5);
    LCD_EN = 0;
}

void LCD_on(void){
    
}

void LCD_off(void){
    
}
    
void LCD_PinMode(unsigned char dataPins){
    
}

void LCD_Output(){
    unsigned char i = 0;
    if(_bitMode){
        for(i = 0; i <= 7; i++){
            
        }
    }
    else{
        for(i = 0; i <= 3; i++){
            
        }
    }
}

/*
 * Splicing 8 bits to 4 bits for 4 bit control
 * Returns array of 2
 */
unsigned char* spliceByteToNibbles(unsigned char data){
    static unsigned char dataSplit[2];
    
    dataSplit[0] = data & 0x0F;         // Low 4 bits
    dataSplit[1] = (data >> 4) & 0x0F;  // High 4 bits
    
    return dataSplit;
}