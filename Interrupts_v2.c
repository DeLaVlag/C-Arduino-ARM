/*
 * Interrupts.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/* These are the variables used: */
uint8_t input = 0; /* 0 or 1 depending on the input signal */
uint16_t integrator = 0; /* Will range from 0 to MAXIMUM */
uint8_t output; /* Cleaned-up version of input signal */
/* Choose value so integrator reaches MAXIMUM after 20 ms */
/* Clock is 8 MHz. Every step is 0.00125 ms. This x 16000 = 20 ms */
const uint16_t MAXIMUM = 3;
// for countdown buttonpress show numbers
volatile uint8_t switch1pressed = 1;

void delay(int ms){

	for(int i=0;i<ms;i++)
		_delay_ms(1);
}

void greenOn(void){
	//Set PORTB output high, | zodat hij de rest met rust laat
	PORTB |= (1<<PB2);   //of PORTB = (1<<3);

}

void greenOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
	//Set PORTB output low, | zodat hij de rest met rust laat
	PORTB &= ~(1<<PB2);
}

void blueOn(void){
	//Set PORTB output high, | zodat hij de rest met rust laat
	PORTB |= (1<<PB1);   //of PORTB = (1<<3);

}

void blueOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
	//Set PORTB output low, | zodat hij de rest met rust laat
	PORTB &= ~(1<<PB1);
}

void redOn(void){
	//Set PORTB output high
	PORTB |= (1<<PB3);   //of PORTB = (1<<3);

}

void redOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
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
	// Registers voor 9 op 1e display
	// PORTC0 - 3 is voor selecteren display en stip eventueel
	//PORTB = 0b00000001;
	
	//display selectie
	//PORTC = 0b00000001;
	// Allen een uitgang
	//DDRC = 0b00001111;

	//PORTD = 0b11101100;
	//DDRD = 0b11101100;
	
}

int no = 0;

ISR(PCINT0_vect){

	
	if (integrator < MAXIMUM){   // Button IS pressed.
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
	}
}

int main(void)
{
	//Set direction input of output for PB2
	//DDRB = (1<<DDB3)|(1<<DDB2)|(1<<DDB1);
	//DDRB = 0b00001110;
	//DDRB = (1<<DDB2);
	//DDRB = (1<<DDB1);
	
	//Enable global interrupts
	sei();
	SREG |= (1<<7);

	// Registers voor leds en switches
	DDRB = 0b00001111;
	PORTB = 0b00000000;
	
	// Enable pin change interrupt register
	PCICR |= (1<<PCIE0);
	//Pin change mask reg 0;
	PCMSK0 |= (1<<PCINT4);
	//EICRA |= 0b00001111;

	// Registers voor display
	// PORTC0 - 3 is voor selecteren display
	//PORTB = 0b00000001;
	//PORTC = 0b00001111;
	// Allen een uitgang
	// DDRC = 0b00001111;

	//PORTD = 0b11111100;
	//DDRD = 0b11111100;
	

	while(1)
	{
		if ((PCIFR |= 0x01)==0){  // Interupt flag is not set is NOT pressed.
			if (integrator > 0) {
				integrator--;
			}
			else{   // if the button is not pressed anymore and 'integrator' has become zero only then
				// the displayNumber() may be called
				//blinkBlue();
				switch1pressed = 1;
			}
		}
		/*if (integrator == 0){
			output = 0;
		}
		else if (integrator == MAXIMUM){
			switch1pressed = output = 1;
			integrator=MAXIMUM;
		}
		
				
		if (switch1pressed==1){
			
			displayNumber(no, 0);
			no++;
			if (no==10){
				no=0;
			}
			switch1pressed = 0;
		}*/

	}
}

