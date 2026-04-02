#include "REG_MG82F6D17.H"
#include "TYPE.h"
#include "Delay.h"



// TO DO: FIX AND SYNC ALL DELAYS


/**
Somewhat ported from:
https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library/blob/master/LiquidCrystal_I2C.h

**/
// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define LCD_BACKLIGHT    0x08
#define LCD_ENABLE       0x04
#define LCD_RW           0x00
#define LCD_RS           0x01	

u8	TWI0OvTime;


//LCD configuration 
typedef struct{
	u8 _backlight;
	u8 _addr;
	u8 _displayctrl;
	u8 _displaymode;
	u8 _displayfunction;
} LCD_options;

static LCD_options lcd;  


	
/**
	Honestly I don't even want to mess with this. It was taken from the manufacturer's sample and the datasheet is too scary for me to figure out and optimize;
**/
u8 TWI0_WriteBuf(u8 DevAddr,u16 RegStartAddr,u8 *pBuf,u8 Len){   
	u8 i;
	u8 Flag;
	Flag=1;

	SICON |=STA;						// Send START
	TWI0OvTime=5;
	SICON &=~SI;			
	while((SICON&SI)!=SI){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.
	SICON &=~STA;

	SICON=SICON|(AA);	   

	Flag++;
	SIDAT = DevAddr&0xFE;				// send Slave Device address  
	TWI0OvTime=5;
	SICON &=~SI;			
	while((SICON&SI)!=SI){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.

	Flag++;
	SIDAT = HIBYTE(RegStartAddr);		// send Slave Data address high
	TWI0OvTime=5;
	SICON &=~SI;			
	while((SICON&SI)!=SI){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.

	Flag++;
	SIDAT = LOBYTE(RegStartAddr);		// send Slave Data address low
	TWI0OvTime=5;
	SICON &=~SI;			
	while((SICON&SI)!=SI){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.


	Flag++;
	i=0;
	while(i<Len)
	{
		if(i==(Len-1))
		{
			SICON=SICON&(~AA);	   
		}
		else
		{
			SICON=SICON|(AA);	   
		}
		SIDAT=pBuf[i];			// send  Data 
		TWI0OvTime=5;
		SICON &=~SI;			
		while((SICON&SI)!=SI){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.
		i++;
	}
					
	Flag++;
	SICON |= STO;				// Send STOP
	TWI0OvTime=5;
	SICON &=~SI;			
	while((SICON&STO)==STO){if(TWI0OvTime==0) goto TWI0_WRITE_ERR;}		// wait completed, if time overflow,then return fail.
	SICON &=~STO;

	SICON = SICON &(~SI);		

	return 0;
	
TWI0_WRITE_ERR:
	return Flag;
}





static void PCF8574_Write(u8 dataC) {
	dataC |= lcd._backlight;
  if(TWI0_WriteBuf(lcd._addr, 0x0000, &dataC, 1)){
		//I2C write error, halt
		while(true){}
	}
}

static void pulseWrite(u8 dataC){
		PCF8574_Write(dataC | LCD_ENABLE);
		DelayXus(1);
		PCF8574_Write(dataC & ~LCD_ENABLE);
}

static void write4bit(u8 dataC){
		PCF8574_Write(dataC);
		pulseWrite(dataC);
}


//Wrapper function for sending 8 bit data and commands
static void send(u8 dataC, u8 mode){
		u8 highNibble = dataC & 0xF0; 
		u8 lowNibble = (dataC << 4) & 0xF0;   //extract 4 bit data
		write4bit((highNibble) | mode);
		write4bit((lowNibble) | mode);
}

static void command(u8 dataC){
		send(dataC,0);
		DelayXus(37); //KEEP THIS DELAY OR ELSE SOME COMMANDS FAIL TO FINISH AND CAN MESS UP THE OUTPUT COMPLETELY
}	
static void write(u8 dataC){
		send(dataC,1);
		DelayXus(41); 
}
static void lcd_init(u8 addr, u8 backlight){
		
	lcd._backlight = backlight ? LCD_BACKLIGHT : LCD_NOBACKLIGHT;
	lcd._addr = addr << 1; //i2c uses 7 bits for address 
	PCF8574_Write(lcd._backlight);
	DelayXms(10);
	
	//Start sequence of the LCD controller
	write4bit(0x30); DelayXus(450); 
	write4bit(0x30); DelayXus(150); 
	write4bit(0x30); DelayXus(150);	
	write4bit(0x20); //set to 4bit mode
	DelayXus(150);
	
	
	
	lcd._displayfunction = LCD_FUNCTIONSET | LCD_5x8DOTS | LCD_2LINE;//2 LINE, 5X8 DOTS 
	command(lcd._displayfunction);  
	
	lcd._displayctrl = LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF	; //display on, cursor off, blink off
	command(lcd._displayctrl);  
	
	command(LCD_CLEARDISPLAY);
	DelayXms(2); //clearing takes time 
	
	lcd._displaymode = LCD_ENTRYMODESET| LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	command(lcd._displaymode);

	command(LCD_RETURNHOME);	
	DelayXms(2);
	
}
static void writeHello(){
	write('H');
	write('e');
	write('l');
	write('l');
	write('o');
}


//high lvl functions

static void clearCommand(){
	command(LCD_CLEARDISPLAY); 
	DelayXms(2);	
}

static void home(){
	command(LCD_RETURNHOME);	
	DelayXms(2);
}
static void setCursor(u8 row,u8 col){		
	u16 rowoff[] = {0x00,0x40};
	command(LCD_SETDDRAMADDR | (col+rowoff[row-1]-1)); // using 1 indexing
}
static void backlight(){
		lcd._backlight = LCD_BACKLIGHT;
		PCF8574_Write(0);
}
static void noBacklight(){
		lcd._backlight = LCD_NOBACKLIGHT;
		PCF8574_Write(0);
}
static void displayOn(){
		lcd._displayctrl |= LCD_DISPLAYON;
		command(lcd._displayctrl);
}
static void displayOff(){
		lcd._displayctrl &= ~LCD_DISPLAYON;
		command(lcd._displayctrl);
}
static void cursor() {
		lcd._displayctrl |= LCD_CURSORON;
		command(lcd._displayctrl);
}
static void noCursor() {
		lcd._displayctrl &= ~LCD_CURSORON;
		command(lcd._displayctrl);
}
static void noBlink() {
		lcd._displayctrl &= ~LCD_BLINKON;
		command(lcd._displayctrl);
}
static void blink() {
		lcd._displayctrl |= LCD_BLINKON;
		command(lcd._displayctrl);
}
static void leftToRight(){
		lcd._displaymode |= LCD_ENTRYLEFT;
		command(lcd._displaymode);
}
static void rightToLeft(){
		lcd._displaymode &= ~LCD_ENTRYLEFT;
		command(lcd._displaymode);
}
static void autoscroll(){
		lcd._displaymode |= LCD_ENTRYSHIFTINCREMENT;
		command(lcd._displaymode);
}
static void noautoscroll(){
		lcd._displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
		command(lcd._displaymode);
}
static void printstr(char *str){
	//u8 len = 1;
		while(*str){
			//if(len==16) setCursor(2,1);
			write(*str++);
			//len++;
		}
}

static void clear(){
//	setCursor(1,1);
//	printstr("                ");
//	setCursor(2,1);
//	printstr("                ");
//	setCursor(1,1);
	//sometimes very slow the above method
		clearCommand();
	
	
}


static void printInt(long n){
//		char str[11]; u8 t; u8 i = 0;
//		
//		if(n==0){ write('0'); return;}
//		if(n<10){ str[1]= '0';}
//		while(n>0){
//			t = n%10;
//			str[i++] = (u8)(t+'0');
//			n/=10;
//		}
//		//printstr(str);
//		while (i >0) {
//        write(str[--i]);
//        DelayXms(1);  // Delay for stability, adjust as needed
//    }
	
	//new method
	char buff[8*sizeof(n)+1];
	char *i = &buff[sizeof(buff)-1];   //pointer to last member of array, so it iterates from last which correlates to our while loop starting from the last number
	if(n==0){ write('0'); return; }
	if(n<0){ 
		write('-');
		n = -n;
	}
	*i='\0'; //NULL terminator 
	do{
		char c = n%10;
		n=n/10;
		*--i = c+'0';  //adding ascii code of 0 as our starting ascii value for numbers 0-9, so for 0 => 0 + '0' = 48, then 1 = 1+'0' = 49 and so on 
	} while(n);
	printstr(i);
		
}


//include user functions 
typedef struct {
		void (*init)(u8 addr,u8 backlight);
		void (*Hello)(void);
		void (*clear)(void);
		void (*home)(void);
		void (*setCursor)(u8 row,u8 column);
		void (*backlight)(void);
		void (*noBacklight)(void);
		void (*displayOn)(void);
		void (*displayOff)(void);
		void (*cursor)(void);
		void (*noCursor)(void);
		void (*blink)(void);
		void (*noBlink)(void);
		void (*leftToRight)(void);
		void (*rightToLeft)(void);
		void (*autoscroll)(void);
		void (*noAutoscroll)(void);
		void (*printstr)(char* str);
		void (*write)(u8 chara);
		void (*printInt)(u32 n);
		void (*clearCommand)(void);

} I2C_LCD;

const I2C_LCD LCD = {  //acts like a singleton 
			lcd_init,writeHello,clear,home,setCursor,backlight,noBacklight,displayOn,displayOff,cursor,noCursor,blink,noBlink,leftToRight,rightToLeft,autoscroll,noautoscroll,printstr,write,printInt
};


