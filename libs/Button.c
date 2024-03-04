#include <p18cxxx.h>
#include "Button.h"
#include <xc.h>

void InitialiseButtons(void)
{
	TRISA |= 0x10;

}	


char SimpleUpDownButton(void)
{
	char ButtonUpDown;
	
	if(PORTA & 0x10)
		ButtonUpDown = UP;
	else
		ButtonUpDown = DOWN;
	
	return ButtonUpDown;
}