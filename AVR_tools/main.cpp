/*
 * GccApplication2.cpp
 *
 * Created: 18.06.2020 10:05:02
 * Author : pappd
 */ 

#include "defines.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "Owi.h"
#include "DS18B20.h"
#include "Uart.h"
#include "DalasCrc.h"
#include "Communicator.h"

volatile int delayCounter;
volatile int timer = 0;

ISR (TIMER0_OVF_vect)
{
	--delayCounter;
	timer ++;
}

enum {
	NO_MEASSURE
	, SINGLE_SHOT
	, AUTOMATIC_MEASSURE
};

void restartTimer0();


int main(void)
{	
	DDRA = 0xFF;
	PORTA = 0x00;
	
	TCCR0 = (1 << CS00) | (1 << CS01);
	TIMSK |= (1 << TOIE0);
	
	delayCounter = 1000; //1000 * (256 * 64) / F_CPU;  // (256 * 64) / F_CPU ~~ 1ms


	Communicator com(9600);

		
	AvrPort owiPort{&DDRC, &PINC, &PORTC};		
	DS18B20 sensor(owiPort, 0);
	
	Packet pck;
	int automaticMeassureDelay = 1000;
	byte_t automaticMeassure = NO_MEASSURE;
	uint16_t conversionWait = 0;
	sei();
	
    while (1) 
    {
		switch (automaticMeassure)
		{
		case AUTOMATIC_MEASSURE:
			if (timer > automaticMeassureDelay)
			{
				timer = 0;
				PORTA ^= 0xFF;
				
				com.send(sensor.temperature(), Commands::SEND_TEMPERATURE);
				sensor.startConversion();
			}	
			break;
			
		case SINGLE_SHOT:
			if (timer > conversionWait)
			{
				PORTA ^= 0xFF;
				automaticMeassure = NO_MEASSURE;
				com.send(sensor.temperature(), Commands::SEND_TEMPERATURE);
			}
			break;
		}
		
				
		com.receive(pck);
		if (pck.isValid())
		{
			switch (Commands(pck.data[Communicator::OFFSET_COMMAND]))
			{
				case Commands::SEND_POWER_MODE:
					sensor.readRom();
					com.send(sensor.power(), Commands::SEND_POWER_MODE);
					break;
					
				case Commands::SEND_ROM:
					sensor.readRom();
					com.send(sensor.rom(), DS18B20::ROM_SIZE, Commands::SEND_ROM);
					break;
				
				case Commands::SEND_SCRATCHPAD:
					sensor.readScratchpad();
					com.send(sensor.scratchpad(), DS18B20::SCRATCHPAD_SIZE, Commands::SEND_SCRATCHPAD);
					break;
					
				case Commands::SEND_TEMPERATURE:
					sensor.readScratchpad();
					automaticMeassure = SINGLE_SHOT;
					conversionWait = sensor.waitTime();
					restartTimer0();
					break;
					
				case Commands::SET_AUTOMATIC_MEASURE:
					automaticMeassure = AUTOMATIC_MEASSURE;
					restartTimer0();					
					break;
					
				case Commands::SET_MEASURE_PERIOD:
					automaticMeassureDelay = (pck.data[Communicator::OFFSET_DATA] << 8) | pck.data[Communicator::OFFSET_DATA + 1];
					com.send(automaticMeassureDelay, Commands::SET_MEASURE_PERIOD);
					restartTimer0();
					break;
					
				case Commands::SET_SENSOR_PARAMS:
					sensor.writeConfigRegister(pck.data + Communicator::OFFSET_DATA);					
					break;
			}
		}	
    }
}

void restartTimer0()
{
	cli();
	TCNT0 = 0;
	timer = 0;	
	sei();
}
