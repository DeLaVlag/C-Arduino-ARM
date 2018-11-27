/*
 * Thermostat.c
 *
 * Created: 12/07/2016 16:30:34
 * Author : Michiel
 */

#define F_CPU 1600000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//globals
int temp[4];
int timeSP[4];
int spFlag=0;
int buttonO=0, buttonT =0;

//intergrator
uint16_t integrator = 0; /* Will range from 0 to MAXIMUM */
const uint16_t MAXIMUM = 16000;
volatile uint8_t switch1pressed = 0;
volatile uint8_t raisedEdge = 0;

//enums
typedef enum{IDLE,HEAT,COOL,DISPSP,INCR,DECR,TEST}state;
state currentState = IDLE;

//protos
void rearrangeTemp(int temperature);
int convert10bits(int tenbits);
void displayTemp(void);

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

void timer2_CTCmode_init(void){
	
	//Timer control A & B
	TCCR2A = 0x02;
	//Prescaler op /1024
	TCCR2B = 0b00000111;
	//TC interrupt enable reg
	TIMSK2 = 0b00000010;
	OCR2A = 50;

}

void timer0_CTCmode_init(void){
	
	//Timer control A & B
	TCCR0A = 0x02;
	//Prescaler op /1024
	TCCR0B = 0b00000101;
	//TC interrupt enable reg
	TIMSK0 = 0b00000010;
	OCR0A = 50;

}


ISR(TIMER2_COMPA_vect){
	
	// count with static, will be init only first call then remain cross isr calls.
	static int i = 0;
	i++;
	if(i==250){
		// 6 ADSC = start conversion (every hAlf second)
		ADCSRA |= (1<<ADSC);
		//PORTB ^= (1 <<3);
		i=0;
	}

	//displayTemp();
}

ISR(TIMER0_COMPA_vect){
	

	// count with static, will be init only first call then remain cross isr calls.
	static uint16_t j = 0;
	j++;
	if(j==250){
		spFlag=1;
		j=0;
		//redOn();
	}
}

void displayTemp(void){
	
		//not necessary cuz of ADC reg is already 10 bit concat
		//int lowreg = ADCL;
		//int highreg = ADCH<<8;
		//int bothreg = highreg + lowreg;
		rearrangeTemp(convert10bits(ADC));
		
		//multiplexing display
		static int display = 4;
		display--;
		//displayNumber(ADCL,display);
		displayNumber(temp[display],display);
		if (display==1)PORTC |= 0x10;
		if (display==0)display = 4;
		
		//clear adc flag not necessary
		//ADCSRA |= (1<<ADIF);
}


void adcInit(){
	
	//Vref selection, left adj, mux: tmpsens is connected to ADC5, VRef = 1.1V is ok sinds tempsens is 10mv/C is range upto 100C.
	ADMUX = 0b11000101;
	// 7 ADEN = ADC enable, 6 ADSC = start conversion, 5 ADATE = autotrigger enable, 4 ADIF = iflag, 3 ADIE = interrupt enable, 2-0 prescaler
	ADCSRA = 0b10001111;
	// Lower 3 bits define auto trigger source. (bit 6(acme) undefined?!). 000 freerunning mode if ADATE = 1
	ADCSRB = 0b00000000;
	//Disable digital input buffer on pin which is connected to ADC(5)
	DIDR0 = 0b0010000;
	// resultreg; ADC = (Vin * 1024)/VRef
	//ADCH + ADCL	
	PRR = (0<<PRADC);
}


// ADC = (Vin * 1024)/VRef
// Example for 30 degreesC
// ADC = (0.2 * 1024) / 1.1 = 187 = 10111011
int convert10bits(int tenbits){
	
	//int temperature = ((tenbits * 1.1)/1024)/0.01;
	int temperature = tenbits * 10.7;
	return temperature;
}

void rearrangeTemp(int temperature){
	
	int parttemp=temperature;
	
	for (int i=3;i>=0;i--){
		temp[i]=parttemp % 10;
		parttemp = parttemp /10;
	}
}

void rearrangeSP(int sp){

	for (int i=1;i>=0;i--){
		timeSP[i]=sp % 10;
		sp = sp /10;
	}		
}

ISR(PCINT0_vect){
	
	//for ass 5.2
	//PORTB ^= (1<<PB3);
	if (PINB & (1<<PCINT4)) buttonO=1;
	if (PINB & (1<<PCINT5)) buttonT=1;
}

/*
void pinChangeInit(){
		// Enable pin change interrupt register
		PCICR |= (1<<PCIE0);
		PCMSK0 |= (1<<PCINT4);
		PCMSK0 |= (1<<PCINT5);
}
*/

void debounceButton1(void){
	
	if ((PINB & (1<<PB4))==0){  // Button is NOT pressed.
		if (integrator > 0) {
			integrator--;
		}
		else{   
			switch1pressed = 1;
		}
	}
	else if (integrator < MAXIMUM){   
		integrator++;
	}
	
	// if integrator becomes 0 and a raising edge has been detected a falling edge has occured.
	if (integrator == 0 && raisedEdge == 1){
		raisedEdge = 0;
	}
	else if (integrator == MAXIMUM && switch1pressed == 1){
		
		switch1pressed = 0;
		raisedEdge = 1;
		buttonO=1;
	}
	
}

int main(void)
{
	DDRB = 0b00001111;
	DDRC = 0b00011111;
	DDRD = 0b11111100;
		
	sei();	
	adcInit();
	//timer2_normalmode_init();
	timer2_CTCmode_init();
	timer0_CTCmode_init();
	
	//pinChangeInit();
	
	//thermos variables
	int sp = 4000;
	//static int timetoSwitch=0;
	
	while (1)
	{
		debounceButton1();
		
		switch(currentState){
			case IDLE:
				displayTemp();
				greenOn();
				redOff();
				blueOff();
				/*if( (convert10bits(ADC))>2500 ) 
				{
					currentState = HEAT;
				}
				else
				{
					currentState = COOL;
				}*/
				
				if(buttonO){
					greenOff();
					currentState = COOL;
				}
				
				break;
			case HEAT:
				displayTemp();
				blueOff();
				greenOff();
				redOn();
				//if( convert10bits(ADC)>= sp ) currentState = IDLE;
				break;
			case COOL:
				displayTemp();
				redOff();
				greenOff();
				blueOn();
				//if( convert10bits(ADC)<= sp ) currentState = IDLE;
				break;
			case DISPSP:
				blueOff();
				//blueOn();
				//redOn();
								
				/*
				rearrangeSP(sp);
				
				static int displaySP = 2;
				displaySP--;
				displayNumber(timeSP[displaySP],displaySP);
				if (displaySP==0)displaySP = 2;
				*/
				if(buttonO==1){
					currentState = COOL;
				}
				//if(button1 ==1)currentState = INCR;
				//if(spFlag==1 && button2 ==1)currentState = DECR;
				break;
			case INCR:
				sp++;
				redOff();
				greenOff();
				blueOff();
				//currentState = DISPSP;
				break;
			case DECR:
				sp--;
				redOff();
				greenOff();
				blueOff();
				//currentState = DISPSP;
				break;
			
			case TEST:
				debounceButton1();
				greenOff();
				blueOff();
				break;
			
			default:
				break;
		}
	}
}

