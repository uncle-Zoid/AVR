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

int main(void)
{
	AvrPort owiPort{&DDRC, &PINC, &PORTC};
	
	//byte_t config[3];
		
	DS18B20 sensor(owiPort, 0);
	//sensor.writeConfigRegister(config, false);
	Owi owi(owiPort, 0);
	
	DDRA = 0xFF;
	PORTA = 0x00;
	Uart::instance().init(UART_BAUD_SELECT(9600, F_CPU));
    /* Replace with your application code */
	
	sei();
	
    while (1) 
    {
		sensor.startConversion();
		_delay_ms(1000);
		int t = sensor.temperature();
		
		//Uart::instance().send(sensor.power());
		//Uart::instance().send(sensor.rom(), DS18B20::ROM_SIZE);
		//Uart::instance().send(sensor.scratchpad(), DS18B20::SCRATCHPAD_SIZE);
		Uart::instance().send((t >> 8) & 0xFF);
		Uart::instance().send(t & 0xFF);
		
		PORTA ^= 0xFF;
		
		//Uart::instance.read();
		
    }
}
