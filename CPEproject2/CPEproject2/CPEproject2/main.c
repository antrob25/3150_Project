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
#define COUNTER_MAX		9
#define BAUDRATE		9600

void tenth_delay();
//void USART_Init();
//char USART_RxChar();
//void USART_TxChar(char data);

ISR(INT0_vect);

int main(void)
{
	BUTTON_DDR = 0x00;
	BUTTON_PORT = 0xFF;
	//USART_Init();
	sei();
	
	unsigned int stopwatch_enable = 0;
	unsigned int stopwatch_counter = 0;
	
    while (1) 
    {
		if ( bit_is_clear( BUTTON_PIN, STOPWATCH_START ) )
		{
			while ( bit_is_clear( BUTTON_PIN, STOPWATCH_START ) );
			
			stopwatch_enable = 1;
			
			while ( stopwatch_enable == 1 )
			{
				tenth_delay();
				
				if ( stopwatch_counter == COUNTER_MAX )
					stopwatch_counter = 0;
				
				else
					stopwatch_counter++;
					
				if ( bit_is_clear( BUTTON_PIN, STOPWATCH_STOP ) )	
				{
					while ( bit_is_clear( BUTTON_PIN, STOPWATCH_STOP ) );
					
					stopwatch_enable = 0;
					
				}
			}
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