# MG82F6D17 I2C LCD 
This project was written in Keil uVision 5

It aims to provide a somewhat easy to use library for interfacing with I2C LCDs. Only supports 1 LCD for now because C and I am pretty new to writing libraries.
All header files required for this MCU are provided in the [include](https://github.com/fungus-dev/MG82F6D17_I2C_LiquidCrystal/tree/05296d5cfaa607194ab0b08b77f79535955f0087/include) folder. 

## Usage: 

All functions are exposed via the LCD object.
Initialize the LCD by `LCD.init(address,backlight on/off)`
then call functions as needed. A list of all functions is given [below](#Functions).  
Or you could flash [LCD.hex](https://github.com/fungus-dev/MG82F6D17_I2C_LiquidCrystal/blob/05296d5cfaa607194ab0b08b77f79535955f0087/Objects/I2C.hex) to the mcu and connect LCD according to the mapping for a simple test
```
SDA -> P61 (pin 1)
SCL -> P60 (pin 2)
```
Pin mapping for I2C can be changed by using the `TWI0_SetUsePxxPxx()` macros in [API_Macro_MG82F6D17.H](https://github.com/fungus-dev/MG82F6D17_I2C_LiquidCrystal/blob/05296d5cfaa607194ab0b08b77f79535955f0087/include/API_Macro_MG82F6D17.H#L723-L726)
## Functions

```
init(addr,backlight)                  // Start the LCD with given address and backlight choice
Hello(void)                           // Print simple Hello message message
clear(void)                           // Clear display by writing all spaces
clearCommand(void)                    // Clear display via command
home(void)                            // Return cursor to start
setCursor(u8 row,u8 column)           // Set cursor position to row,column. Uses 1 indexing

printstr(char* str)                   // Print string
write(u8 chara)                       // Write a single character 
printInt(long n)                      // Print integer

leftToRight(void)                     // Print in left to right direction. Default
rightToLeft(void)                     // Print in right to left direction
autoscroll(void)                      // Scrolling effect 
noAutoscroll(void)

backlight(void)                       // Turn on backlight
noBacklight(void)                     // Turn it off 
displayOn(void)                       
displayOff(void)                      
cursor(void)                         
noCursor(void)
blink(void)
noBlink(void)
```
