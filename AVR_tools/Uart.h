/* 
* Uart.h
*
* Created: 23.06.2020 14:21:08
* Author: pappd
*/


#ifndef __UART_H__
#define __UART_H__

/** @brief  UART Baudrate Expression
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz          
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu) ((xtalCpu)/((baudRate)*16l)-1)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalcpu  system clock in Mhz, e.g. 4000000L for 4Mhz           
 *  @param  baudrate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) (((xtalCpu)/((baudRate)*8l)-1)|0x8000)


#include <inttypes.h>

using byte_t = uint8_t;


class Uart
{
//variables
public:
	enum UartError{
			UART_NO_ERROR		 = 0x00,
			UART_NO_DATA		 = 0x01,
			UART_BUFFER_OVERFLOW = 0x02,
			UART_DATA_OVERRUN	 = 0x04,			
			UART_FRAME_ERROR	 = 0x08,
			UART_PROTOCOL_ERROR  = 0x10
		};
		

protected:
private:
	// buffer musi byt mocnina dvou
	static constexpr byte_t BUFFER_SIZE = 32;
	static constexpr byte_t BUFFER_MASK = BUFFER_SIZE - 1;
	volatile byte_t bufferRx_[BUFFER_SIZE];
	volatile byte_t bufferTx_[BUFFER_SIZE];
	volatile byte_t headRx_;
	volatile byte_t tailRx_;
	volatile byte_t headTx_;
	volatile byte_t tailTx_;
	volatile byte_t error_;
	
//functions
public:	
	~Uart() = default;
	
	// c++ compliler -fno-threadsafe-statics
	//Do not emit the extra code to use the routines specified in the C++ ABI for 
	//thread-safe initialization of local statics. You can use this option to reduce 
	//code size slightly in code that doesn't need to be thread-safe

	static Uart &instance()
	{
		static Uart ref;
		return ref;
	}
	
	/**
	Inicializace seriove linky
	Pro baudrate pouzij makro UART_BAUD_SELECT, nebo UART_BAUD_SELECT_DOUBLE_SPEED
	*/
	void init(uint16_t uartBaudrate);
	
	
	inline void receiveInteruptRoutine();
	inline void transmitInteruptRoutine();
	
	void send (byte_t data);
	void send (const byte_t *data, int size);
	
	byte_t read (UartError &err);
	
	byte_t available () const;
	
	void clear();
		
protected:
private:
	Uart() = default;

	Uart( const Uart &c ) = delete;
	Uart& operator=( const Uart &c ) = delete;		
	
	friend class Communicator;	
}; //Uart

#endif //__UART_H__
