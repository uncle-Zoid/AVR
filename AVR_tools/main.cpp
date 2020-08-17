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
#include "SevenSeg.h"

volatile int timer = 0;

static SevenSeg *psevenseg = nullptr;

ISR (TIMER0_OVF_vect)
{
	++ timer;
	
	psevenseg->display();
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
	
	


	Communicator com(9600);

		
	AvrPort owiPort{&DDRC, &PINC, &PORTC};
	DS18B20 sensor(owiPort, 0);
	
	
	AvrPort sevenDataPort   {&DDRA, &PINA, &PORTA};
	AvrPort sevenComamndPort{&DDRB, &PINB, &PORTB};
	SevenSeg sevenseg(sevenDataPort, sevenComamndPort, 4, 2);
	psevenseg = &sevenseg;
		
	Packet pck;
	int automaticMeassureDelay = 10000; //1000 * (256 * 64) / F_CPU;  // (256 * 64) / F_CPU ~~ 1ms
	byte_t automaticMeassure = AUTOMATIC_MEASSURE;
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
				auto t = sensor.temperature();
				com.send(t, Commands::SEND_TEMPERATURE);
				sevenseg.showValue(t);
				sensor.startConversion();
			}	
			break;
			
		case SINGLE_SHOT:
			if (timer > conversionWait)
			{
				auto t = sensor.temperature();
				sevenseg.showValue(t);				
				com.send(t, Commands::SEND_TEMPERATURE);
				automaticMeassure = NO_MEASSURE;
			}
			break;
		}
		
				
		com.receive(pck);
		if (pck.isValid())
		{
			switch (Commands(pck.data[Communicator::OFFSET_COMMAND]))
			{
				case Commands::SEND_POWER_MODE:
					sensor.readPowerSupply();
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
					sensor.startConversion();
					automaticMeassure = SINGLE_SHOT;
					conversionWait = 1000; //dam pevnou dobu cekani //sensor.waitTime();
					restartTimer0();
					break;
					
				case Commands::SET_AUTOMATIC_MEASURE:
					automaticMeassure = AUTOMATIC_MEASSURE;
					sensor.startConversion();
					restartTimer0();					
					break;
					
				case Commands::SET_MEASURE_PERIOD:
					automaticMeassureDelay = (pck.data[Communicator::OFFSET_DATA] << 8) | pck.data[Communicator::OFFSET_DATA + 1];
					if (automaticMeassureDelay < 1000)
					{
						automaticMeassureDelay = 1000;						
					}
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
