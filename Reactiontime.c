/*
 * Reaction Time Tester.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 16000000
#define maxDel 3000
#define minDel 1

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int teller, uitlees = 0;

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
			PORTB |= 0b00000001;
			PORTD = 0b11101100;
			break;
		case 8:
			PORTB |= 0b00000001;
			PORTD = 0b11111100;
			break;
		case 7:
			PORTB |= 0b00000001;
			PORTD = 0b11000000;
			break;
		case 6:
			PORTB |= 0b00000001;
			PORTD = 0b01111100;
			break;
		case 5:
			PORTB |= 0b00000001;
			PORTD = 0b01101100;
			break;
		case 4:
			PORTB &= ~0b00000001;
			PORTD = 0b11101000;
			break;
		case 3:
			PORTB |= 0b00000001;
			PORTD = 0b11100100;
			break;
		case 2:
			PORTB |= 0b00000001;
			PORTD = 0b10110100;
			break;
		case 1:
			PORTB &= ~0b00000001;
			PORTD = 0b11000000;
			break;
		case 0:
			PORTB |= 0b00000001;
			PORTD = 0b11011100;
			break;
		default:
			PORTB = 0b00000000;
			PORTD = 0b00000000;
			break;
	}

}


ISR(PCINT0_vect){
	
	uitlees = (teller+1)/2;
	
}

ISR(TIMER0_OVF_vect){
	//redOn();
}

void init(){
	//Enable global interrupts
	sei();
	//SREG |= (1<<7);

	// Registers voor leds en switches
	DDRB = 0b00001111;
	PORTB = 0b00000000;
		
	// Enable pin change interrupt register
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT4);
	PCMSK0 |= (1<<PCINT5);
}


int main(void)
{
	init();

	while(1)
	{		
		cli();
		//_delay_ms(random(minDel, maxDel));
		_delay_ms(3000);
		redOn();
		sei();
		while (uitlees ==0){
			teller++;
			_delay_ms(50);
		}
		
		if (uitlees > 0 && uitlees < 10){
			greenOn();
			displayNumber(uitlees,3);
			_delay_ms(3000);
			uitlees = 0;
			teller = 0;
			redOff();
			greenOff();
		} else {
			displayNumber(1,2);
			redOff();
			teller = 0;
		}
	}
}

