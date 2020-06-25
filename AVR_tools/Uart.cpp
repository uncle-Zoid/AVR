/* 
* Uart.cpp
*
* Created: 23.06.2020 14:21:08
* Author: pappd
*/

#include <avr/interrupt.h>

#include "Uart.h"


ISR (USART_RXC_vect)
{
	Uart::instance().receiveInteruptRoutine();
}

ISR (USART_UDRE_vect)
{
	Uart::instance().transmitInteruptRoutine();
}

void Uart::receiveInteruptRoutine()
{
	cli();
	
	byte_t error = (UCSRA & ((1 << FE)| (1 <<DOR)));
	byte_t head = (headRx_ + 1) & BUFFER_MASK;
	if (head == tailRx_)
	{
		error = UART_BUFFER_OVERFLOW;
	}
	else
	{
		bufferRx_[head] = UDR;
		headRx_ = head;
	}
	error_ = error;
	sei();
}

void Uart::transmitInteruptRoutine()
{
	cli();
	// v bufferu jsou data, posilej bajt po bajtu
	if ( headTx_ != tailTx_)
	{
		byte_t tail = (tailTx_ + 1) & BUFFER_MASK;
		UDR = bufferTx_[tail];	
		tailTx_ = tail;
	}
	else
	{
		// buffer je prazdny, vypni preruseni
		UCSRB &= ~(1 << UDRIE);
	}
	sei();
}

void Uart::send(byte_t data)
{
	byte_t head  = (headTx_ + 1) & BUFFER_MASK;
	
	while (head == tailTx_); // cekej na vyprazdneni bufferu
	
	bufferTx_[head] = data;
	headTx_ = head;

	// povol preruseni pro odeslani dat
	UCSRB |= (1 << UDRIE);
}

void Uart::send(byte_t *data, int size)
{
	for (int i = 0; i < size; ++i)
	{
		send(data[i]);
	}
}

byte_t Uart::read(UartError &err)
{
	if (headRx_ == tailRx_)
	{
		err = UART_NO_DATA;
		return 0x00;
	}
	tailRx_ = (tailRx_ + 1) & BUFFER_MASK;
	err = UartError(error_);
	return bufferRx_ [tailRx_];
}

byte_t Uart::available() const
{
	return (BUFFER_MASK + headRx_ - tailRx_) % BUFFER_MASK;
}

void Uart::init(uint16_t uartBaudrate)
{
	headRx_ = 0;
	tailRx_ = 0;
	headTx_ = 0;
	tailTx_ = 0;
	
	if (uartBaudrate & 0x8000)
	{
		uartBaudrate &= ~0x8000;
		UCSRA |= (1 << U2X);
	}
	
	UBRRL = uartBaudrate & 0xFF;
	UBRRH = (uartBaudrate >> 8) & 0xFF;
	// povoleni preruseni pri prijmu, povoleni prijimace a vysilace
	UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
	// asynchronni prenos, bez parity, 1 stopbit, 8 datovych bitu
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
}
