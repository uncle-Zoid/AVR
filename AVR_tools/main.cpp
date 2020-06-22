/*
 * GccApplication2.cpp
 *
 * Created: 18.06.2020 10:05:02
 * Author : pappd
 */ 

#include <avr/io.h>
#include "Owi.h"
#include "DS18B20.h"


int main(void)
{
	AvrPort owiPort{&DDRA, &PINA, &PORTA};
	
	byte_t config[3];
		
	DS18B20 sensor(owiPort, 0);
	sensor.writeConfigRegister(config, false);

	
    /* Replace with your application code */
    while (1) 
    {
    }
}

