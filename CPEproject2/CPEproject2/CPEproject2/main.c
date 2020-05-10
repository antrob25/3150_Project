/*
 * CPEproject2.c
 *
 * Created: 5/9/2020 3:46:04 PM
 * Author : Anthony Robles
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sfr_defs.h>

#define F_CPU 16000000UL
#define BUTTON_DDR		DDRA
#define BUTTON_PORT		PORTA
#define BUTTON_PIN		PINA
#define STOPWATCH_START	PINA7
#define STOPWATCH_STOP	PINA6
#define TIMER_INPUT		PINA5
#define TIMER_START		PINA4
#define COUNTER_MAX		9
#define BAUDRATE		9600
#define BEEP_DDR		DDRE
#define BEEP_PORT		PORTE

void tenth_delay();
void hundredth_delay();
void timer_mode();
void display(char);
void display_init();
void stopwatch_init();
unsigned int stopwatch();
void beep_init();
void start_beep();
void stop_beep();
//void USART_Init();
//char USART_RxChar();
//void USART_TxChar(char data);

int main(void)
{
	//USART_Init();
	display_init();
	stopwatch_init();
	beep_init();
	sei();
	
	//unsigned int stopwatch_time = 0;
	
    while (1) 
    {
		stopwatch();
		timer_mode();
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
	BEEP_DDR = 0x10;
	BEEP_PORT = 0x00;
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
		hundredth_delay();
	}
	BEEP_PORT = 0x00;
}

void stopwatch_init()
{
	BUTTON_DDR = 0x00;
	BUTTON_PORT = 0xFF;
}

unsigned int stopwatch()
{
	unsigned int total_time = 0;
	unsigned int stopwatch_enable = 0;
	unsigned char stopwatch_counter = '0';
	
	if ( bit_is_clear( BUTTON_PIN, STOPWATCH_START ) )
	{
		while ( bit_is_clear( BUTTON_PIN, STOPWATCH_START ) );
		
		start_beep();
		stopwatch_enable = 1;
		
		while ( stopwatch_enable == 1 )
		{
			tenth_delay();
			
			if ( stopwatch_counter == COUNTER_MAX )
			{
				stopwatch_counter = '0';
				display(stopwatch_counter);
			}
			
			else
			{
				stopwatch_counter++;
				display(stopwatch_counter);
			}
			
			if ( bit_is_clear( BUTTON_PIN, STOPWATCH_STOP ) )
			{
				while ( bit_is_clear( BUTTON_PIN, STOPWATCH_STOP ) );
				
				stop_beep();
				stopwatch_enable = 0;
			}
		}
	}
	return total_time;
}
/*
void USART_Init()	
{
	unsigned int ubbr_value = (F_CPU/(16*BAUDRATE))-1; //Define prescale value
    //Set UBRR register for desired baud rate
	UBRR0L = ubbr_value;
	UBRR0H = ( ubbr_value>>8 );
	
	UCSR0B |= ( 1<<RXEN ) | ( 1<<TXEN ); //Enable USART transmitter and receiver
	//Write USCRC for 8 bit data and 1 stop bit
	UCSR0B |= ( 0<<UCSZ2 );
	UCSR0C |= ( 1<<UCSZ1 ) | ( 1<<UCSZ0 );
	UCSR0C &= ~( 1<<USBS );
}

char USART_RxChar()
{
	char rx_value;
	//checks to see if there is a character to receive
	if (bit_is_set(UCSR0A, RXC))
	{
		rx_value = UDR0;
	}
	else
	{
		rx_value = '\0';
	}
	return rx_value;
}

void USART_TxChar(char data)
{
	UDR0 = data; //Write data to be transmitting in UDR
	while (bit_is_clear(UCSR0A, TXC)); //Wait until data transmit and buffer get empty
	UCSR0A |= ( 1<<TXC );
}*/

void timer_mode()
{
	unsigned char input = 0;
	char inputmode = 1;
	while(inputmode)
	{
		if(bit_is_clear(BUTTON_PIN, TIMER_INPUT))
		{
			tenth_delay();
			input++;
		}
		if(bit_is_clear(BUTTON_PIN, TIMER_START))
		{
			inputmode = 0;
			start_beep();
		}
	}
	
	while(input > 0)
	{
		display(input-- % 10);
		tenth_delay();		
	}
	
	while(bit_is_set(BUTTON_PIN, TIMER_START))
	{
		start_beep();
		tenth_delay();
	}
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