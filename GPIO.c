/*
 * GPIO.c
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
	//Set PORTB output high, | zodat hij de rest met rust laat
	PORTB |= (1<<PB2);   //of PORTB = (1<<3);

}

void greenOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
	//Set PORTB output low, | zodat hij de rest met rust laat
	PORTB |= (0<<PB2);
}

void blueOn(void){
	//Set PORTB output high, | zodat hij de rest met rust laat
	PORTB |= (1<<PB1);   //of PORTB = (1<<3);

}

void blueOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
	//Set PORTB output low, | zodat hij de rest met rust laat
	PORTB |= (0<<PB1);
}

void redOn(void){
	//Set PORTB output high
	PORTB |= (1<<PB3);   //of PORTB = (1<<3);

}

void redOff(void){
	//PORTB = ~(1<<PB2); Beter niet dit gebruiken pin wordt niet helemaal low, blauw led brandt half
	//Set PORTB output low
	PORTB |= (0<<PB3);
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


int main(void)
{
	//Set direction input of output for PB2
	//DDRB = (1<<DDB3)|(1<<DDB2)|(1<<DDB1);
	//DDRB = 0b00001110;
	//DDRB = (1<<DDB2);
	//DDRB = (1<<DDB1);

	// Registers voor leds en switches
	DDRB = 0b00001111;
	PORTB = 0b00000000;

	// Registers voor display
	// PORTC0 - 3 is voor selecteren display
	PORTB = 0b00000001;
	PORTC = 0b00001111;
	// Allen een uitgang
	DDRC = 0b00001111;

	PORTD = 0b11111100;
	DDRD = 0b11111100;

	while(1)
	{

		blinkRed();
		blinkGreen();
		blinkBlue();
		
		displayNumber(9, 0);
		blueOn();
		delay(1000);
		blueOff();
		displayNumber(8, 1);
		delay(1000);
		displayNumber(7, 2);
		delay(1000);
		displayNumber(6, 3);
		delay(1000);
		displayNumber(5, 0);
		delay(1000);
		displayNumber(4, 1);
		delay(1000);
		displayNumber(3, 2);
		delay(1000);
		displayNumber(2, 3);
		delay(1000);
		displayNumber(1, 0);
		delay(1000);
		displayNumber(0, 1);
		delay(1000);


		//if (PINB == 0b00110000 ) {

			//blinkRed();

		//}

		//PORTC = 0b00000001;
		//DDRC = 0b00000001;

		//PINB = 0b00000000;
		//DDRB = 0b00000000;
		//DDRB = (0<<DDB4);

		//per toeval werkt dit voor sw3
		//if (PINB == 0b00110000 ) {

		//PINB uitlezen als schakelaar gezet wordt
		// Is inverted dus & met 0?
		// if (PINB & ~(1<<5) ) {

		//
		//if (PINB & 0b00010000) {

		if ((PINB & 0b00110000) == 0b00110000) {

			blinkRed();
			blinkGreen();
			blinkBlue();

		}

		//DDRB = 0b00000000;

		/*if (PINB & ~(0b00001000)) {

			blinkRed();
			blinkGreen();
			blinkBlue();

		}*/

	}
}

