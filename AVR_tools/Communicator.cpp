/*
 * Communicator.cpp
 *
 * Created: 12.08.2020 8:43:53
 *  Author: pappd
 */ 

#include "Communicator.h"

Communicator::Communicator(uint32_t baud) : uart_ (Uart::instance())
{
	Uart::instance().init(UART_BAUD_SELECT(baud, F_CPU));
}

void Communicator::send(Commands command, byte_t *data, int size)
{
	uart_.send(HEAD);
	uart_.send(size + 3);
	uart_.send(byte_t(command));
	
	for (int i = 0; i < size; ++i)
	{
		uart_.send(data[i]);
	}
}

void Communicator::send(const Packet &pck)
{
	for (int i = 0; i < pck.size; ++i)
	{
		uart_.send(pck.data[i]);
	}
}

void Communicator::receive(Packet &p)
{
	// TODO pridat overovani casu
	p.size = -1;
	p.error = 0;
	byte_t availableBytes = uart_.available();
 	if (availableBytes >= 3)
 	{
 		if (uart_.bufferRx_[(uart_.tailRx_ + 1) & Uart::BUFFER_MASK] == HEAD)
 		{
 			byte_t needBytes = uart_.bufferRx_[(uart_.tailRx_ + 2) & Uart::BUFFER_MASK];
 			if (needBytes > p.maxSize())
 			{
 				uart_.headRx_ = 0;
 				uart_.tailRx_ = 0;
 				p.error = Uart::UART_PROTOCOL_ERROR;
 				return;
 			}
 			if (availableBytes >= needBytes)
 			{
 				p.size = needBytes;
 				for (byte_t i = 0; i < needBytes; ++i)
 				{
 					uart_.tailRx_ = (uart_.tailRx_ + 1) & Uart::BUFFER_MASK;
 					p.data[i] = uart_.bufferRx_ [uart_.tailRx_];
 				}
 			}
 		}
 		else
 		{
 			uart_.headRx_ = 0;
 			uart_.tailRx_ = 0;
 			p.error = Uart::UART_PROTOCOL_ERROR;
 		}
 	}
}