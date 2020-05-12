/*
 * CPEproject2.c
 *
 * Created: 5/9/2020 3:46:04 PM
 * Author : Anthony Robles, Sithija Gunasinghe, Cade Johnstone, Daniel Napierkowsi
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>
#include <stdlib.h>

#define F_CPU			16000000UL
#define BUTTON_DDR		DDRA
#define BUTTON_PORT		PORTA
#define BUTTON_PIN		PINA
#define STOPWATCH_START	PINA7
#define STOPWATCH_STOP	PINA6
#define TIMER_INPUT		PINA5
#define TIMER_START		PINA4
#define STOPWATCH_SPLIT	PINA3
#define COUNTER_MAX		9
#define BEEP_DDR		DDRE
#define BEEP_PORT		PORTE
#define OUTPUT_DDR		DDRC
#define OUTPUT_PORT		PORTC

void tenth_delay();
void hundredth_delay();
void timer_mode();
void display(char);
void display_init();
void stopwatch_init();
void stopwatch();
void beep_init();
void start_beep();
void stop_beep();

//A constant character array that is the menu for selecting the timer modes

int main(void)
{
	display_init();
	stopwatch_init();
	beep_init();
	sei();
	OUTPUT_DDR = 0xFF;
	OUTPUT_PORT = 0x00;
	
    while (1) 
    {
	    //while loop polls for received data keeps polling until a '1' or a '2' is received
	    //while((bit_is_set( BUTTON_PIN, PINA0 )) || (bit_is_set( BUTTON_PIN, PINA1 )) );
	    
	    if(bit_is_set( BUTTON_PIN, PINA0 ))
	    {
			while ( bit_is_set( BUTTON_PIN, PINA0 ) );
		    stopwatch();  //Runs stopwatch mode if 1 was received
	    }
	    if(bit_is_set( BUTTON_PIN, PINA1 ))
	    {
			while ( bit_is_set( BUTTON_PIN, PINA1 ) );
		    timer_mode(); //Runs in timer mode if 2 was received
	    }
    }
}

void tenth_delay()
{
	TCNT1H = 0xF3;
	TCNT1L = 0xCB;
	TCCR1B = 0x04;
	
	while ( ( TIFR1& ( 0x1<<TOV1 ) ) == 0 );
	
	TCCR1B = 0;
	TIFR1 = 0x1<<TOV1;
}

void hundredth_delay()
{
	TCNT1H = 0xFE;
	TCNT1L = 0xC7;
	TCCR1B = 0x04;
	
	while ( ( TIFR1& ( 0x1<<TOV1 ) ) == 0 );
	
	TCCR1B = 0;
	TIFR1 = 0x1<<TOV1;
}

void beep_init()
{
	BEEP_DDR |= ( 1<<PINE4 );
}

void start_beep()
{
	BEEP_PORT = 0x10;
	tenth_delay();
	BEEP_PORT = 0x00;
}

void stop_beep()
{
	BEEP_PORT = 0x10;
	for ( int i = 0; i < 5; i++ )
	{
		//hundredth_delay();
	}
	BEEP_PORT = 0x00;
}

void stopwatch_init()
{
	BUTTON_DDR = 0x00;
	BUTTON_PORT = 0xFF;
}

void stopwatch()
{
	unsigned int total_time = 0;
	unsigned int split_time_checkpoint = 0;
	unsigned int split_time;
	unsigned int stopwatch_enable = 0;
	unsigned char stopwatch_counter = '0';
	OUTPUT_DDR = 0xFF;
//	if ( bit_is_set( BUTTON_PIN, STOPWATCH_START ) )
//	{
		while ( bit_is_clear( BUTTON_PIN, STOPWATCH_START ) );
		
		//start_beep();
		stopwatch_enable = 1;
		
		while ( stopwatch_enable == 1 )
		{
			//tenth_delay();
			
			if ( stopwatch_counter == COUNTER_MAX )
			{
				stopwatch_counter = '0';
				total_time++;
				display(stopwatch_counter);
			}
			
			else
			{
				stopwatch_counter++;
				total_time++;
				display(stopwatch_counter);
			}
			
			//when pressed, finds the time since last split and outputs it to serial
			if(bit_is_set(BUTTON_PIN, STOPWATCH_SPLIT))
			{
				while ( bit_is_set( BUTTON_PIN, STOPWATCH_SPLIT ) );
				
				//Subtract total time from last checkpoint to get the delta
				split_time = total_time - split_time_checkpoint;
				
				//Get the new checkpoint value to compare for next split
				split_time_checkpoint = total_time;
				
				//Output the split to serial
				OUTPUT_PORT = split_time;
				
			}
			
			if ( bit_is_set( BUTTON_PIN, STOPWATCH_STOP ) )
			{
				while ( bit_is_set( BUTTON_PIN, STOPWATCH_STOP ) );
				
				//stop_beep();
				stopwatch_enable = 0;
			}
		}
		OUTPUT_PORT = total_time;
//	}
	
}

void timer_mode()
{
	unsigned char input = 0;
	char inputmode = 1;
	while(inputmode)
	{
		if(bit_is_set(BUTTON_PIN, TIMER_INPUT))
		{
			//tenth_delay();
			input++;
		}
		if(bit_is_set(BUTTON_PIN, TIMER_START))
		{
			inputmode = 0;
			//start_beep();
		}
	}
	
	while(input > 0)
	{
		display(input-- % 10);
		//tenth_delay();		
	}
	
	while(bit_is_clear(BUTTON_PIN, TIMER_START))
	{
		display(0);
		//start_beep();
		//tenth_delay();
	}
	display(10);
}

void display_init()
{
	DDRD = 0xFF;
	DDRE = 0x20;
	PORTD = 0x00;
	PORTE = 0x00;
}
void display(char num)
{
	switch(num)
	{
		case 0:
			PORTD = 0xFF;
			PORTE &= ~(1<<PORTE5);
			break;
		case 1:
			PORTD = 0x01;
			PORTE &= ~(1<<PORTE5);
			break;
		case 2:
			PORTD = 0x02;
			PORTE &= ~(1<<PORTE5);
			break;
		case 3:
			PORTD = 0x04;
			PORTE &= ~(1<<PORTE5);
			break;
		case 4:
			PORTD = 0x08;
			PORTE &= ~(1<<PORTE5);
			break;
		case 5:
			PORTD = 0x00;
			PORTE |= (1<<PORTE5);
			break;
		case 6:
			PORTD = 0x10;
			PORTE &= ~(1<<PORTE5);
			break;
		case 7:
			PORTD = 0x20;
			PORTE &= ~(1<<PORTE5);
			break;
		case 8:
			PORTD = 0x40;
			PORTE &= ~(1<<PORTE5);
			break;
		case 9:
			PORTD = 0x80;
			PORTE &= ~(1<<PORTE5);
			break;
		case 10: //for display off
			PORTD = 0x00;
			PORTE &= ~(1<<PORTE5);
			break;
	}
	
}
