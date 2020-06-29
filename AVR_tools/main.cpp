/*
 * GccApplication2.cpp
 *
 * Created: 18.06.2020 10:05:02
 * Author : pappd
 */ 

#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Owi.h"
#include "DS18B20.h"
#include "Uart.h"
#include "DalasCrc.h"

volatile int delayCounter;

ISR (TIMER0_OVF_vect)
{
	--delayCounter;
}


int main(void)
{
	DDRA = 0xFF;
	PORTA = 0x00;
	
	TCCR0 = (1 << CS00) | (1 << CS01);
	TIMSK |= (1 << TOIE0);
	
	delayCounter = 1000; //1000 * (256 * 64) / F_CPU;  // (256 * 64) / F_CPU ~~ 1ms
	
	DalasCrc crc;

	AvrPort owiPort{&DDRC, &PINC, &PORTC};		
	DS18B20 sensor(owiPort, 0);
	Uart::instance().init(UART_BAUD_SELECT(9600, F_CPU));
	Packet pck;
	int delay = 1000;
	byte_t firstRun = 1;
	sei();
	
    while (1) 
    {

		
		//Uart::instance().send(sensor.power());
		//Uart::instance().send(sensor.rom(), DS18B20::ROM_SIZE);
		//Uart::instance().send(sensor.scratchpad(), DS18B20::SCRATCHPAD_SIZE);
		//Uart::instance().send((t >> 8) & 0xFF);
		//Uart::instance().send(t & 0xFF);
		
		
		//Uart::instance().send(crc.compute8(sensor.scratchpad(), DS18B20::SCRATCHPAD_SIZE));
		if (delayCounter == 0)
		{
			delayCounter = delay;
			PORTA ^= 0xFF;
			
			if (!firstRun)
			{
				int t = sensor.temperature();
				Uart::instance().send((t >> 8) & 0xFF);
				Uart::instance().send(t & 0xFF);
			}
			
			sensor.startConversion();
			firstRun = false;
			
		}
		
		
		Uart::instance().read(pck);
		if (pck.isValid() && pck.size >= 4)
		{
			cli();
			TCNT0 = 0;
			delayCounter = 0;
			delay = (pck.data[2] << 8) | pck.data[3];			
			sei();
			
			Uart::instance().send(pck);
		}
		//Uart::instance().send(Uart::instance().available());
		//while (Uart::instance().available())
		//{			
		//	byte_t res = Uart::instance().read(err);
		//	if (err != Uart::UART_NO_ERROR)
		//	{
		//		static const uint8_t ERTEXT[] = {'H', 'U', 'U'};
		//		Uart::instance().send(ERTEXT, 3);
		//		_delay_ms(10);
		//		Uart::instance().clear();
		//		break;
		//	}
		//	
		//	{
		//		Uart::instance().send(++counter);
		//		//Uart::instance().send(res);
		//	}			
		//}		
    }
}
