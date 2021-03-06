//credits to http://www.appelsiini.net/2011/simple-usart-with-avr-libc
//for tutorial on uart communication

#include <avr/io.h>

//this is for the uart code
#include <stdio.h>

//Note: I hope this is compatible with the hardware interrupt scheme
#include <avr/interrupt.h>

//using this for pause hacks
#include <util/delay.h>

//this library simplifies the return data
#include <string.h>

//set the baud rate
#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

//define TX RX buffer sizes
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

//these are some macros which help set hte bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))



//Uart setup
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

//this allows us to use printf and stuff
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
//end uart setup

int main(void) {

//SERIAL SETUP
  //baud rate has a strange setup, split between two registers
  UBRR0H = (BRC >> 8); // this is for setting the baud rate
  UBRR0L = BRC;        // like a barabara it is cut in half

  //enables rx and tx, page 183 example from documentation
  UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0 ) | (1 << TXCIE0);

  //198 of documentation, these two bits make it 8-bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  //since we don't use 2x
  UCSR0A &= ~(_BV(U2X0));


  //redirecting standard out and in to UART -- very clever tuupola
  stdout = &uart_output;
  stdin  = &uart_input;

//END SERIAL SETUP

//TIMER SETUP BEGIN

  //CTC Mode
  TCCR0A = (1 << WGM01); //WGM01 sets this for CTC mode


  //TIMSK0 register controls the interrupts-- when this happens I want this interrupt to occur
  //OCIE0A this bit want interrupt setup so that when compare 'A' is equal it will set up an interrupt
  TIMSK0 = (1 << OCIE0A);

//conclude setup

  //sets the direction of the DDRB to be an OUTPUT
  DDRD =   (1 << PORTD6);

  char input;

puts("hello world");
_delay_ms(2200);
puts("hello world again");
  return 0;
}


void uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
}


char uart_getchar(FILE *stream) {
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}

//    PORTB ^=  (1 << PORTB0); //toggles an led -- this is really useful!!

