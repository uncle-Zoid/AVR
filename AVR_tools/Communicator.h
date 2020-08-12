/* 
* Communicator.h
*
* Created: 12.08.2020 8:42:16
* Author: pappd
*/


#ifndef __COMMUNICATOR_H__
#define __COMMUNICATOR_H__

#include "defines.h"
#include "Uart.h"

#ifndef F_CPU
#error "F_CPU have to be defined"
#endif

// commands
enum class Commands{
	  SEND_POWER_MODE = 1
	, SEND_ROM
	, SEND_SCRATCHPAD
	, SEND_TEMPERATURE
	, SET_AUTOMATIC_MEASURE // automaticke mereni teploty start/stop
	, SET_SENSOR_PARAMS
	, SET_MEASURE_PERIOD
};


// | HEAD | LEN | COMMAND | data
struct Packet
{
	static constexpr byte_t MaxSize = 32;
	Packet ()
		: size  (-1)
		, error (0)
	{};
	
	bool isValid () const
	{
		return (size > 0) && (error == 0);
	}
	
	byte_t maxSize()
	{
		return MaxSize;
	}
	
	byte_t data[MaxSize];
	int size;
	int error;
};


class Communicator
{
public:
	static constexpr byte_t HEAD = 0x1A;
	static constexpr byte_t OFFSET_COMMAND = 2;
	static constexpr byte_t OFFSET_DATA    = 3;

		

public:
	Communicator(uint32_t baud);
	~Communicator() = default;

	Communicator( const Communicator &c ) = delete;
	Communicator& operator=( const Communicator &c ) = delete;
	
		
	template <class T>
	void send(T data, Commands command)
	{
		byte_t *pdataBegin = reinterpret_cast<byte_t*>(&data);
		send(pdataBegin, sizeof (T), command);
	}
	
	void send(byte_t *data, int size, Commands command);
	void send(const Packet &pck);
	
	void receive(Packet &p);
	
private:
	Uart &uart_;
}; //Communicator

#endif //__COMMUNICATOR_H__
