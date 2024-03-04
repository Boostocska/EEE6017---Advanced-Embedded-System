/*
 * File:            main.c
 * Student Name:    Erik Zsolt Kis-Varga
 * Student Number:  2012443
 */

#include <p18cxxx.h>
#include <xc.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libs/LCD.h"

#pragma config WDT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF
#pragma config OSC = HS

#define _XTAL_FREQ 4000000
#define LED1 PORTDbits.RD7
#define LED2 PORTDbits.RD6
#define BUZZER PORTDbits.RD1
#define BTN_ER PORTDbits.RD0
#define BTN_RST PORTCbits.RC3
#define BTN_SYS PORTCbits.RC2
#define PIR PORTCbits.RC0
#define STATE_DEACTIVATED   0
#define STATE_ACTIVATED     1
#define STATE_DEFAULT       2
#define STATE_OPENED        3
#define STATE_EMERGENCY     4

typedef _Bool Bool;

const int PWM_CLOSE = 250;
const int PWM_OPEN = 475;
const unsigned char keypad[4][4] = {
    { 1, 2, 3, 0x41},
    { 4, 5, 6, 0x42},
    { 7, 8, 9, 0x43},
    { 0x2A, 0x30, 0x23, 0x44}
};
const unsigned char activationCode[5] = {1,2,3,4,5};
const unsigned char resetCode[5] = {5,4,3,2,1};
unsigned long preciseTimer;

Bool gateState = 0;     // 0 - Closed | 1 - Open
char currentState = STATE_DEACTIVATED;

/*
 *  Function Prototypes
 */
void init();
void setState(int targetState);
void handleGate(Bool toOpen, Bool *isOpen, void (*callback)(int));
void closeGate();
void openGate();
void enableBuzzer(Bool isEmergency);
void disableBuzzer();
void toggleBuzzer(Bool isEmergency);
void pwmSet(int duty);
char keypadPolling();
Bool erPolling();
Bool pirPolling();
Bool sysBtnPolling();
Bool rstBtnPolling();
char readMatchCode(char* codeToMatch, Bool** isEnabled);

void init(){
    
    /*
     * Input/Output Configuration
     */
    TRISB = 0xF0;   // 11110000
    TRISC = 0x0D;   // 00001101
    TRISD = 0x21;   // 00100001
    
    /*
     * Output Defaults
     */
    PORTB = 0x0F;
    PORTC = 0x00;   // 00000000
    PORTD = 0x80;   // 00000000
    
    /*
     * Timer Configuration
     * Timer 0 - Precise 1 Second Timer, Disabled
     * Timer 1 - Customizable Counter for Timing
     * Timer 2 - PWM Control, Enabled
     */
    T0CON = 0x48;   // 01001000
    T1CON = 0x00;   // 00000000
    T2CON = 0x06;   // 00000110
    PR2 = 0xFF;     // 11111111
    CCP2CON = 0x0C; // 00001100
    
    /*
     * Interrupt Configuration
     */
    
    INTCON = 0xA0;  // 10100000
    INTCON2 = 0x80; // 10000000
    INTCON3 = 0x00; // 00000000
    RCON = 0x00;    // 00000000
    
    pwmSet(PWM_CLOSE);     // Gate Default Position
    LCD_Init();             // LCD Initialization
    setState(currentState); // Initialized State
}

void main() {
    init();     // Initialization
    
    while(1){   // Main Program
        Bool isSysBtnPressed = 0;
        Bool* isSysBtnPressedPtr = &isSysBtnPressed;
        switch(currentState){
            
            /*
             * Deactivated State
             */
            case STATE_DEACTIVATED:
                while(!isSysBtnPressed){
                    for(int i = 0; i < 3000; i += 50){
                        if(erPolling()){
                            LCD_Set_Cursor(1,1);
                            LCD_Write_String("No ER:Deactivate");
                            __delay_ms(3000);
                            LCD_Clear();
                            LCD_Write_String("Deactivated");
                        }
                        if(!sysBtnPolling()){
                            i = 0;
                        }
                        __delay_ms(50);
                    }
                    LCD_Set_Cursor(2,16);
                    LCD_Write_Char(0xFF);
                    isSysBtnPressed = 1;
                }
                char matchCodeResponse = readMatchCode(activationCode, &isSysBtnPressedPtr);
                if(matchCodeResponse == 2){         // Emergency Button Pressed
                    LCD_Set_Cursor(1,1);
                    LCD_Write_String("No ER:Deactivate");
                    __delay_ms(3000);
                    LCD_Clear();
                    LCD_Write_String("Deactivated");
                }
                else if(matchCodeResponse == 1){    // Correct Code Entered
                    setState(STATE_ACTIVATED);
                }
                else{
                    LCD_Set_Cursor(2,16);
                    LCD_Write_Char(" ");
                }
                break;
                
            /*
             * Activated State
             */
            case STATE_ACTIVATED:
                for(int i = 0; i < 5000; i+= 50){
                    if(erPolling()){
                        break;
                    }
                    __delay_ms(50);
                }
                if(erPolling()){
                    setState(STATE_EMERGENCY);
                    break;
                }
                setState(STATE_DEFAULT);
                break;
                
                
            /*
             * Default / Closed State
             */
            case STATE_DEFAULT:
                while(1){
                    for(int i = 0; i < 3000; i += 50){
                        if(erPolling() || pirPolling()){
                            break;
                        }
                        if(!sysBtnPolling()){
                            i = 0;
                        }
                        __delay_ms(50);
                    }
                    if(erPolling()){        // If ER Pressed
                        setState(STATE_EMERGENCY);
                        break;
                    }
                    else if(pirPolling()){  // If PIR Triggered
                        setState(STATE_OPENED);
                        break;
                    }
                    else{
                        LCD_Set_Cursor(2,16);
                        LCD_Write_Char(0xFF);
                        isSysBtnPressed = 1;
                    }
                    
                    if(isSysBtnPressed){
                        char matchCodeResponse = readMatchCode(activationCode, &isSysBtnPressedPtr);
                        if(matchCodeResponse == 2){ // Emergency Button Pressed
                            setState(STATE_EMERGENCY);
                            break;
                        }
                        else if(matchCodeResponse == 1){    // Correct Code
                            setState(STATE_DEACTIVATED);
                            break;
                        }
                        else{
                            LCD_Set_Cursor(2,16);
                            LCD_Write_Char(" ");
                        }
                    }
                    break;
                }
                break;
            
            /*
             * Opened State
             */
            case STATE_OPENED:
                while(1){
                    //If no PIR Detection for 15s
                    for(int i = 0; i < 15000 ; i += 50){
                        if(erPolling()){
                            break;
                        }
                        if(pirPolling()){
                            i = 0;
                        }
                        __delay_ms(50);
                    }
                    if(erPolling()){
                        setState(STATE_EMERGENCY);
                    }
                    else{
                        setState(STATE_DEFAULT);
                    }
                    break;
                }
                break;
                
             /*
             * Emergency State
             */
            case STATE_EMERGENCY:
                while(1){
                    for(int i = 0; i < 8000; i += 50){
                        if(!rstBtnPolling()){
                            i = 0;
                        }
                        __delay_ms(50);
                    }
                    LCD_Set_Cursor(2,16);
                    LCD_Write_Char(0xFF);
                    Bool isRstBtnPressed = 1;
                    Bool* isRstBtnPressedPtr = &isRstBtnPressed;
                    char matchCodeResponse = readMatchCode(&resetCode, &isRstBtnPressedPtr);
                    if(matchCodeResponse == 1){    // Correct Code Entered
                        preciseTimer = 0;
                        setState(STATE_DEACTIVATED);
                        break;
                    }
                        else{
                            LCD_Set_Cursor(2,16);
                            LCD_Write_Char(" ");
                        }
                }
                break;
                
             /*
             * Error State
             */
            default:    // On Unexpected Error Fall Back to Deactivated State
                setState(STATE_DEACTIVATED);
                break;
        }
    }
}

/*
 * PWM Set Function
 */
void pwmSet(int duty){
    CCP2CONbits.DC2B = (char)(duty & 0x03);
    CCPR2L = (char)(duty >> 2);
}

/*
 * Set State Function
 */
void setState(int targetState){
    switch(targetState){
        case STATE_ACTIVATED:   // Set Activated State
            currentState = STATE_ACTIVATED;
            LCD_Clear();
            LCD_Write_String("Activated");
            break;

        case STATE_DEFAULT:     // Set Default / Closed State
            currentState = STATE_DEFAULT;
            LED2 = 0;
            closeGate();
            LCD_Clear();
            LCD_Write_String("Gate Closed");
            break;
            
        case STATE_OPENED:      // Set Opened State
            currentState = STATE_OPENED;
            LCD_Clear();
            LCD_Write_String("Gate Opened");
            LED2 = 1;
            openGate();
            break;
            
        case STATE_EMERGENCY:   // Set Emergency State
            currentState = STATE_EMERGENCY;
            toggleBuzzer(1);
            T0CONbits.TMR0ON = 1;
            LCD_Clear();
            LCD_Write_String("99: Gate Open");
            openGate();
            break;

        default:                // Set Deactivated State
            currentState = STATE_DEACTIVATED;
            LED2 = 1;
            closeGate();
            LCD_Clear();
            LCD_Set_Cursor(1,1);
            LCD_Write_String("Deactivated");
            break;
    }
}

void closeGate(){
    handleGate(0, &gateState, &pwmSet);
}

void openGate(){
    handleGate(1, &gateState, &pwmSet);
}

void disableBuzzer(){
    BUZZER = 0;
}

void toggleBuzzer(Bool isEmergency){
    BUZZER =! BUZZER;
    if(isEmergency){
        LED2 = BUZZER;
    }
}

/*
 * Gate Opening / Closing Function
 */
void handleGate(Bool toOpen, Bool *isOpen, void (*callback)(int)){
    // If Gate is in Requested State -> Do Nothing
    //  Open Gate from Closed State
    if(toOpen && !*isOpen){
        for(int i = PWM_CLOSE; i < PWM_OPEN; i++){
            __delay_ms(35);
            callback(i);
        }
        *isOpen = 1;
    }
    
    //  Close Gate from Open State
    else if(!toOpen && *isOpen){
        LCD_Clear();
        LCD_Set_Cursor(1,1);
        LCD_Write_String("Close");
        T0CONbits.TMR0ON = 1;
        for(int i = PWM_OPEN; i > PWM_CLOSE; i--){
            __delay_ms(50);
            callback(i);
        }
        T0CONbits.TMR0ON = 0;
        disableBuzzer();    // Buzzer Off when Closed
        *isOpen = 0;        // Set Gate State to Closed
    }
}

char readMatchCode(char* codeToMatch, Bool** isEnabled){
    int codePosition = 0;
    while(*isEnabled){
        char pressedKey = keypadPolling();
        if(erPolling()){
            *isEnabled = 0;
            return 2;
        }

        if(pressedKey == codeToMatch[codePosition]){ // Correct
            codePosition++;
            if(codePosition == sizeof(activationCode)){
                *isEnabled = 0;
                return 1;
            }
        }
        else if(pressedKey == 99){} // No Button Pressed
        else if(pressedKey == 98){} // Multiple Buttons Pressed
        else{                       // Incorrect Code
            *isEnabled = 0;
        }
        __delay_ms(50);
    }
    return 0;
}

char keypadPolling(){
    Bool keyDetected = 0;
    unsigned char pressedKey = 99;  // Returns 99 if No Key Pressed
    for(int i = 0; i < 4; i++){
        PORTB = (0x08 >> i);
        for(int j = 0; j < 4; j++){
            if(PORTB & 0x10 << j){
                if(!keyDetected){ // Sets key pressed flag
                    pressedKey = keypad[i][3-j];
                    keyDetected = 1;
                }
                else{   // If multiple key presses detected returns 98;
                    return 98;
                }
            }
        }
    }
    PORTB = 0x0F;
    while(PORTB & 0xF0){}   // Wait for Key Release
    PORTB = 0x00;
    __delay_ms(50);         // Debounce Timing
    return pressedKey;
}

Bool erPolling(){
    if(BTN_ER){
        return 1;
    }
    return 0;
}

Bool pirPolling(){
    if(PIR){
        return 1;
    }
    return 0;
}

Bool sysBtnPolling(){
    if(BTN_SYS){
        return 1;
    }
    return 0;
}

Bool rstBtnPolling(){
    if(BTN_RST){
        return 1;
    }
    return 0;
}

void __interrupt() isr(void){
    INTCONbits.GIE = 0;
    INTCONbits.TMR0IF = 0;
    preciseTimer += 256;
    if(preciseTimer >= 1000000 && currentState != STATE_EMERGENCY){
        preciseTimer -= 1000000;
        toggleBuzzer(0);
    }
    if(preciseTimer >= 3000000 && currentState == STATE_EMERGENCY){
        preciseTimer -= 3000000;
        toggleBuzzer(1);
    }
    INTCONbits.GIE = 1;
}