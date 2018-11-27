/*
 * Button debouncing.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

/* These are the variables used for rising edge: */
//uint8_t input = 0;     /* 0 or 1 depending on the input signal */
uint16_t integrator = 0; /* Will range from 0 to MAXIMUM */
uint8_t output;          /* Cleaned-up version of input signal */
/* Choose value so integrator reaches MAXIMUM after 20 ms */
/* Clock is 8 MHz. Every step is 0.00125 ms. This x 16000 = 20 ms */
const uint16_t MAXIMUM = 16000;             // Make it higher, A response within 200ms suffices in most applications
int no = 0;                                     // for countdown buttonpress show numbers
volatile uint8_t switch1pressed = 0;
volatile uint8_t raisedEdge = 0;

/* These are the variables used for falling edge: */
//uint8_t input = 0;     /* 0 or 1 depending on the input signal */
uint16_t integratorfall = 0; /* Will range from 0 to MAXIMUM */
//uint8_t output;          /* Cleaned-up version of input signal */
/* Choose value so integrator reaches MAXIMUM after 20 ms */
/* Clock is 8 MHz. Every step is 0.00125 ms. This x 16000 = 20 ms */
//const uint16_t MAXIMUM = 16000;             // Make it higher, A response within 200ms suffices in most applications
volatile uint8_t switchWasPressed = 0;
//volatile uint8_t raisedEdge = 0;


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
void displayNoRisingDebounced(void){
	
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
		output = 1; // This 'switch1pressed = output = 1;' is unusual code, but it is allowed.
		/// integrator=MAXIMUM; // This seems to be a useless line, because you enter the if when 'integrator == MAXIMUM'
		displayNumber(no, 0);
		no++;
		if (no==10){
			no=0;
		}
		switch1pressed = 0;
		raisedEdge = 1;
	}
}

// This function needs the raisedEdge flag form displayNoRaisingDebounced function. Thus cannot be called stand alone
void displayNoFallingDebounced(void){
	
	if ((PINB & (1<<PB4))==0 && raisedEdge == 1){	// Button is NOT pressed and a rising edge has occurred.
		if (integratorfall < MAXIMUM){				// Button WAS pressed.
			//blinkRed();
			integratorfall++;
		}
	}
	else if (integratorfall > 0 && raisedEdge == 1) { // Button is pressed again
		integratorfall--;
	}
	else if ( integratorfall == 0 ) {   // if the button is not pressed anymore after a falling edge 
										// and 'integrator' has become zero only then the displayNumber() may be called
		switchWasPressed = 1;
	}
	
	// ------ ACTIONS if debouncing is certain ------
	if (integratorfall == 0){
		output = 0;
	}
	else if (integratorfall == MAXIMUM && switchWasPressed == 1){
		output = 1;
		displayNumber(no, 0);
		no++;
		if (no==10){
			no=0;
		}
		switchWasPressed = 0;
	}	
}

int main(void)
{

    // Registers voor leds en switches
    DDRB = 0b00001111;
    PORTB = 0b00000000;

    while(1)
    {
		displayNoRisingDebounced();
		displayNoFallingDebounced();
    }
}

