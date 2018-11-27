/*
 * State Machine for bank door.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

void delay(int ms){

    for(int i=0;i<ms;i++)
        _delay_ms(1);
}

void greenOn(void){
    //Set PORTB output high
    PORTB |= (1<<PB2);

}

void greenOff(void){
    //Set PORTB output low
    PORTB &= ~(1<<PB2);
}

void blueOn(void){
    //Set PORTB output high
    PORTB |= (1<<PB1); 

}

void blueOff(void){
    //Set PORTB output low
    PORTB &= ~(1<<PB1);
}

void redOn(void){
    //Set PORTB output high
    PORTB |= (1<<PB3);

}

void redOff(void){
    //Set PORTB output low
    PORTB &= ~(1<<PB3);
}

void blinkRed(void){
    // Blinking
    DDRB |= (1<<DDB3);
    redOn();
    delay(500);
    redOff();
    DDRB |= (0<<DDB3);
    delay(500);
}

void blinkGreen(void){
    greenOn();
    DDRB |= (1<<DDB2);
    delay(500);
    greenOff();
    DDRB |= (0<<DDB2);
    delay(500);
}

void blinkBlue(void){
    DDRB |= (1<<DDB1);
    blueOn();
    delay(500);
    blueOff();
    DDRB |= (0<<DDB1);
    delay(500);
}

void displayNumber(int number, int display){
    
    // Configureer alle display selectie pinnen als een uitgang
    DDRC = 0b00001111;
    
    //Altijd een uitgang voor de D pinnen connected to display
    DDRD = 0b11111100;
    
    //display selectie
    switch(display){
        case 3:
        PORTC = 0b00001000;
        break;
        case 2:
        PORTC = 0b00000100;
        break;
        case 1:
        PORTC = 0b00000010;
        break;
        case 0:
        PORTC = 0b00000001;
        break;
        default:
        PORTC = 0b00000000;
        break;
    }
    
    //number to display
    switch(number){
        case 9:
            PORTB = 0b00000001;
            PORTD = 0b11101100;
            break;
        case 8:
            PORTB = 0b00000001;
            PORTD = 0b11111100;
            break;
        case 7:
            PORTB = 0b00000001;
            PORTD = 0b11000000;
            break;
        case 6:
            PORTB = 0b00000001;
            PORTD = 0b01111100;
            break;
        case 5:
            PORTB = 0b00000001;
            PORTD = 0b01101100;
            break;
        case 4:
            PORTB = 0b00000000;
            PORTD = 0b11101000;
            break;
        case 3:
            PORTB = 0b00000001;
            PORTD = 0b11100100;
            break;
        case 2:
            PORTB = 0b00000001;
            PORTD = 0b10110100;
            break;
        case 1:
            PORTB = 0b00000000;
            PORTD = 0b11000000;
            break;
        case 0:
            PORTB = 0b00000001;
            PORTD = 0b11011100;
            break;
        default:
            PORTB = 0b00000000;
            PORTD = 0b00000000;
            break;
    }
    
}


int main(void)
{

    // Registers voor leds en switches
    DDRB = 0b00001111;
    PORTB = 0b00000000;


    
                             /* These are the variables used: */
    //uint8_t input = 0;     /* 0 or 1 depending on the input signal */
	uint8_t output = 0;
    uint16_t integrator = 0; /* Will range from 0 to MAXIMUM */
                             /* Choose value so integrator reaches MAXIMUM after 20 ms */
                             /* Clock is 8 MHz. Every step is 0.00125 ms. This x 16000 = 20 ms */
    const uint16_t MAXIMUM = 16000;             // Make it higher, A response within 200ms suffices in most applications
    int no = 0;                                     // for countdown buttonpress show numbers
    volatile uint8_t switch1pressed = 0;
	
	// State machine
	typedef enum{SLOT,DICHT,OPEN}state;
	state currentState = SLOT;
	int switch2 = 0;
	int switch3 = 0;
	volatile uint8_t switch3pressed = 0;
	uint16_t integrator2 = 0;

    while(1)
    {
		// Switch 3
        if ((PINB & (1<<PB4))==0){  // Button is NOT pressed.
            //blinkBlue();
            if (integrator > 0) {
                integrator--;
            }
            else{   // if the button is not pressed anymore and 'integrator' has become zero only then
                    // the displayNumber() may be called
                switch1pressed = 1;
            }
        }
        else if (integrator < MAXIMUM){   // Button IS pressed.
            //blinkRed();
            integrator++;
        }
        
        // ------ ACTIONS if debouncing is certain ------
        if (integrator == 0){
            output = 0;
        }
        else if (integrator == MAXIMUM && switch1pressed == 1){
           switch2 = 1;
		   switch1pressed = 0;
		   //blueOn();
        }
		
		
		// Switch 2
		if ((PINB & (1<<PB5))==0){  // Button is NOT pressed.
			//blinkBlue();
			if (integrator2 > 0) {
				integrator2--;
			}
			else{   // if the button is not pressed anymore and 'integrator' has become zero only then
				// the displayNumber() may be called
				switch3pressed = 1;
			}
		}
		else if (integrator2 < MAXIMUM){   // Button IS pressed.
			integrator2++;
		}
		
		// ------ ACTIONS if debouncing is certain ------
		if (integrator2 == 0){
			output = 0;
		}
		else if (integrator2 == MAXIMUM && switch3pressed == 1){
			switch3 = 1;
			switch3pressed = 0;
			//greenOn();
		}
		
		
		switch(currentState){
			case SLOT:
				blueOff();
				greenOff();
				if (switch3) {
					currentState = DICHT;
					switch3=0;
					switch2=0;
				}
				redOn();
				break;
			case DICHT:
				redOff();
				greenOff();
				if (switch2) {
					currentState = OPEN;
					switch2 = 0;
				}
				else if (switch3) {
					currentState = SLOT;
					switch3 = 0;
				}
				blueOn();
				break;
			case OPEN:
				redOff();
				blueOff();
				if (switch2) {
					currentState = DICHT;
					switch2 = 0;
					switch3 = 0;
				}
				greenOn();
				break;
		}
		
    }
}

