#include <xc.h>

#ifndef LCD_H
    #define LCD_H

    #ifndef _XTAL_FREQ
        #define _XTAL_FREQ 4000000
    #endif

    #ifndef LCD_DATA_0
        #define LCD_DATA_0 RD0
    #endif

    #ifndef LCD_DATA_1
        #define LCD_DATA_1 RD1
    #endif

    #ifndef LCD_DATA_2
        #define LCD_DATA_2 RD2
    #endif

    #ifndef LCD_DATA_3
        #define LCD_DATA_3 RD3
    #endif

    #ifndef LCD_DATA_4
        #define LCD_DATA_4 RD4
    #endif

    #ifndef LCD_DATA_5
        #define LCD_DATA_5 RD5
    #endif

    #ifndef LCD_DATA_6
        #define LCD_DATA_6 RD6
    #endif

    #ifndef LCD_DATA_7
        #define LCD_DATA_7 RD7
    #endif

    #ifndef LCD_RS
        #define LCD_RS RC0
    #endif

    #ifndef LCD_EN
        #define LCD_EN RC1
    #endif

    #ifndef LCD_RW
        #define LCD_RW RC2
    #endif

    _Bool _bitMode; // "HIGH" - 8-bit, "LOW" - 4-bit control

    /*
     * LCD Initialisation
     * @param bitModeSelect Bit Mode Select ("LOW" for 4, "HIGH" for 8 bits)
     * @param lines Display Line Count (1 line or 2 lines display)
     * @param fontType Display Font Type ("LOW" for 5x8, "HIGH" for 5x11 dots)
     * @return void
     */
    void LCD_Init(_Bool bitModeSelect, unsigned int lines, _Bool fontType);
    
    /*
     * Splice Byte to Nibbles
     * @param data Byte to split
     * @return char[2]
     */
    unsigned char* spliceByteToNibbles(unsigned char data);
    
    void LCD_setBitMode(void);
    void LCD_on(void);
    void LCD_off(void);
    void LCD_Write(unsigned char command);

#endif
    
    /* Min delays:
     * Function set: 37us
     * Display on/off: 37 us
     * Display clear: 1.52 ms
     * Entry mode set: 37 us 
     */
    