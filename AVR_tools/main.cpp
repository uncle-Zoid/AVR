/*
 * GccApplication2.cpp
 *
 * Created: 18.06.2020 10:05:02
 * Author : pappd
 */ 

#include <avr/io.h>
#include "Owi.h"

int main(void)
{
	AvrPort owiPort{&DDRA, &PINA, &PORTA};
		
	Owi owi(owiPort, 0);

	owi.reset();
    /* Replace with your application code */
    while (1) 
    {
    }
}

