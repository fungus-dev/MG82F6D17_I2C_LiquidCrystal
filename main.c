
#define MCU_SYSCLK       24000000
#define MCU_CPUCLK       MCU_SYSCLK

#include <Intrins.h>
#include ".\include\API_Macro_MG82F6D17.H"
#include ".\include\REG_MG82F6D17.H"
#include ".\include\Delay.h"	
#include ".\include\CLKCONF.H"
#include ".\include\TYPE.h"
#include <stdlib.h>

#include ".\include\LCD_I2C.c"




void InitTWI0(void) {
    TWI0_Clear();
    TWI0_SetClock(TWI0_CLK_SYSCLK_64);  
    TWI0_SetUseP60P61(); // Set to P60 (SCL) and P61 (SDA)
    TWI0_Enable();
}

void initSystem(){
	InitClock();
	InitTWI0();
}

int main(){
	initSystem();
	LCD.init(0x27,1);
	LCD.clear();
	LCD.printstr("Hello");
	
	
	while(true){		
	}
}

