/*
 * Clock.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 1600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


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
    
	DDRB = 0b00001111;
	
    // Configureer alle display selectie pinnen als een uitgang
    DDRC = 0b00011111;
    
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



void timer2_normalmode_init(void){
	
	//Timer control A & B
	TCCR2A = 0x00;
	//Prescaler op /1024
	TCCR2B = 0b00000111;
	//Counter reg
	//TCNT0
	//Output compare A&B
	//OCR0A
	//OCR0B
	//TC interrupt enable reg
	TIMSK2 = 0b00000001;

}

void timer2_CTCmode_init(void){
	
	//Timer control A & B
	TCCR2A = 0x02;
	//Prescaler op /1024
	TCCR2B = 0b00000111;
	//TC interrupt enable reg
	TIMSK2 = 0b00000010;
	OCR2A = 50;

}

ISR(TIMER2_OVF_vect){
	
	PORTB ^= (1 <<3); // flip LED to test delay
	
}

ISR(TIMER2_COMPA_vect){
	
	// count with static, will be init only first call then remain cross isr calls.
	
	static int seconden = 0;
	static int tiendensecs = 0;
	static int minuten = 0;
	static int tiendenminuten = 0;
	static int i = 0;
	i++;
	if(i==250){
		seconden++;
		if (seconden==10)
		{
			seconden=0;
			tiendensecs++;
			if (tiendensecs==6)
			{
				tiendensecs = 0;
				minuten++;
				if (minuten==10)
				{
					tiendenminuten++;
					minuten=0;
					if (tiendenminuten==6)
					{
						tiendenminuten=0;
					}
				}
			}
		}
		i=0;
	}
	//int tijd[4]={seconden,tiendensecs,minuten,tiendenminuten};
	int tijd[4]={tiendenminuten, minuten, tiendensecs, seconden};
	
	//multiplexing
	static int display = 4;
	display--;
	displayNumber(tijd[display],display);
	if (display==1)PORTC |= 0x10;
	if (display==0)display = 4;
	
}

// Exact 10 ms wachten met FCLK = 2.4576 MHz .
// 1/2457600 * 256 = 10ms (Doel van dit vb?)
// 2457600/256 = 9600 Hz -> 1/9600 * 256 = 26ms
// 10 / 26 * 256 = 96.nogwat
void wacht10ms ()
{
	TCCR2A = 0b00000000 ; // gebruik Timer / Counter0 in normal mode
	TCCR2B = 0b00000110 ; // prescaler op 256
	TCNT2 = 0;
	while ( TCNT2 < 96)
	{
		// wachten ...
	}
}

// Exact 1 s wachten met FCLK = 2.4576 MHz. 
void wacht1s()
{
    TCCR2A = 0b00000000;
    TCCR2B = 0b00000111; // prescaler op 1024
    TCNT2 = 160;
    for (int i = 0; i < 10; i++)
    {
        while ((TIFR2 & 1<<TOV2) == 0)  
        {
            // wachten tot TOV0 geset wordt.
        }
        TIFR2 = (1<<TOV2); // reset TOV0
    }
}


int main(void)
{
	sei();	
	//timer2_normalmode_init();
	timer2_CTCmode_init();
	
	DDRB = 0b00001110 ;
	while (1)
	{

		//wacht10ms ();
		//PORTB ^= (1 <<3); // flip LED to test delay
	}
		
	/*
	if ((PINB & (1<<5))){
		blinkRed();
		timer0_init();
	}
	*/
    
}

