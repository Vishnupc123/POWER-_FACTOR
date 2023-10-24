/*
 * File:   newmain.c
 * Author: VISHNUPRASAD.C
 *
 * Created on 24 October, 2023, 4:20 PM
 */



#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<math.h>
#define _XTAL_FREQ 20000000 

#define RS RB7
#define EN RB6
#define D4 RC4
#define D5 RC5
#define D6 RC6
#define D7 RC7

#define B1 RD3
#define B2 RD2
#define B3 RD6
#define B4 RD7
int i;

void LCD_Init();
void LCD_Command(unsigned char cmd);
void LCD_Char(unsigned char data);
void LCD_String(const char *str);
void LCD_Clear();
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_value_float(float a);
void LCD_value(uint16_t a);

void CCP1_Init() {
    // Configure CCP1 module in Capture mode with both rising and falling edge detection
    CCP1CON = 0b00000101; // Capture mode, every rising edge; CCP1IF bit is set on every rising edge
    T1CON = 0b00000001; // Timer1 configuration (prescaler 1:1)
}

void main() {
    // Initialize CCP module for pulse width measurement
    CCP1_Init();
    LCD_Init();
    TRISD=0x00;
    
    
    
    // Display a message on the LCD
    LCD_Clear();

    // Variables
    uint16_t pulseWidth;

    while(1) {
   
        // Wait for the first rising edge (start of the pulse)
        CCP1CON = 0b00000101; 
        CCP1IF = 0; // 
        while(!CCP1IF); // Wait until the first rising edge occurs
//       Clear the CCP1 interrupt flag

        // Capture the first rising edge time
        uint16_t startTime = CCPR1;

        // Wait for the falling edge (end of the pulse)
        CCP1CON = 0b00000100; 
        CCP1IF = 0; 
        while(!CCP1IF); // Wait until the falling edge occurs
       // Clear the CCP1 interrupt flag

        // Capture the falling edge time
        uint16_t endTime = CCPR1;

        // Calculate pulse width in timer ticks
        if(endTime>startTime)
            pulseWidth = endTime - startTime;
        else
           pulseWidth= 65536-startTime+endTime;
        
        
        LCD_Command(0xc0);
      //  LCD_value_float((float)pulseWidth);
     //   LCD_value(pulseWidth);
        __delay_ms(100);
        float dt =(4*1*(pulseWidth)/20000000.0)*3-0.00015;
        float angle = 360*50*dt;
        LCD_Command(0x80);
        LCD_String("ANGLE=");
        LCD_value_float(angle);
         LCD_Command(0xC0);
        LCD_String("PF=");
        LCD_value_float(-cos(angle)+0.50);
       
        

        
        
        
        // Do something with pulse width value, for example, convert it to milliseconds
        // pulseWidth in timer ticks, assuming 20 MHz clock and 1:1 prescaler
        // To convert it to milliseconds for a 50 Hz signal: pulseWidth * 0.05 (20 ms period)
        
        
            for(int i=0;i<6&&(-cos(angle)+0.50)<0.99;i++){
         LCD_Command(0x94);
        LCD_String("PF_correction started");
       __delay_ms(100);
           switch(i){
                case 0:
                B1=0;
                B2=0;
                B3=0;
                B4=0;
                   break; 
               case 1:
                B1=1;
                B2=0;
                B3=0;
                B4=0;
                   break;
                case 2:
                B1=1;
                B2=1;
                B3=0;
                B4=0;
                   break;
               case 3:
                B1=1;
                B2=1;
                B3=1;
                B4=0;
                   break;
               case 4:
                B1=1;
                B2=1;
                B3=1;
                B4=1;
                
                   break; 
                   
                case 5:
                B1=0;
                B2=0;
                B3=0;
                B4=0;
                
                   break; 
                
           
           }
           __delay_ms(100);
       }i++;
       if(i>5){
           i=0;
        LCD_Clear();}
    }
}
void LCD_Init() {
    // Set LCD pins as outputs
    TRISB7 = 0;
    TRISB6 = 0;
    TRISC = 0X07;

    
    // Wait for LCD to power up (15ms delay after VDD rises to 4.5V)
    __delay_ms(15);
    
    // Initialize LCD in 4-bit mode
    LCD_Command(0x02); // Initialize in 4-bit mode
    LCD_Command(0x28); // 4-bit mode, 2 lines, 5x7 font
    LCD_Command(0x0C); // Display ON, Cursor OFF
    LCD_Command(0x06); // Increment cursor, no shift
    LCD_Command(0x01); // Clear display
    __delay_ms(2);
}

void LCD_Command(unsigned char cmd) {
    RS = 0; // Select command register
    __delay_ms(1);
    EN = 1; // Enable pulse start
    __delay_ms(1);
    D4 = (cmd >> 4) & 1; // Send higher nibble
    D5 = (cmd >> 5) & 1;
    D6 = (cmd >> 6) & 1;
    D7 = (cmd >> 7) & 1;
    __delay_ms(1);
    EN = 0; // Enable pulse end
    __delay_ms(1);
    EN = 1; // Enable pulse start
    __delay_ms(1);
    D4 = cmd & 1; // Send lower nibble
    D5 = (cmd >> 1) & 1;
    D6 = (cmd >> 2) & 1;
    D7 = (cmd >> 3) & 1;
    __delay_ms(1);
    EN = 0; // Enable pulse end
    __delay_ms(1);
}

void LCD_Char(unsigned char data) {
    RS = 1; // Select data register
    __delay_ms(1);
    EN = 1; // Enable pulse start
    __delay_ms(1);
    D4 = (data >> 4) & 1; // Send higher nibble
    D5 = (data >> 5) & 1;
    D6 = (data >> 6) & 1;
    D7 = (data >> 7) & 1;
    __delay_ms(1);
    EN = 0; // Enable pulse end
    __delay_ms(1);
    EN = 1; // Enable pulse start
    __delay_ms(1);
    D4 = data & 1; // Send lower nibble
    D5 = (data >> 1) & 1;
    D6 = (data >> 2) & 1;
    D7 = (data >> 3) & 1;
    __delay_ms(1);
    EN = 0; // Enable pulse end
    __delay_ms(1);
}

void LCD_String(const char *str) {
    uint8_t i = 0;
    while (str[i] != '\0') {
        LCD_Char(str[i]);
        i++;
    }
}

void LCD_Clear() {
    LCD_Command(0x01); // Clear display command
    __delay_ms(2); // Delay for clear command to complete
}

void LCD_SetCursor(uint8_t row, uint8_t column) {
    uint8_t position = 0;
    if (row == 1) {
        position = 0x80 + column; // Command to set cursor to the first row address
    } else if (row == 2) {
        position = 0xC0 + column; // Command to set cursor to the second row address
    }
    LCD_Command(position);
}
void LCD_value_float(float a){
    char r[10];
    sprintf(r,"%0.2f",a);
    LCD_String(r);

}
void LCD_value(uint16_t a){
    char r[10];
    sprintf(r,"%d",a);
    LCD_String(r);

}


