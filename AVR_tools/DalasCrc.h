/* 
* DalasCrc.h
*
* Created: 26.06.2020 14:49:08
* Author: pappd
*/


#ifndef __DALASCRC_H__
#define __DALASCRC_H__

#include <inttypes.h>

using byte_t = uint8_t;

class DalasCrc
{
//variables
public:
protected:
private:

//functions
public:
	DalasCrc();
	~DalasCrc() = default;
	
	
	byte_t compute8(const byte_t *message, int size);
	

protected:
private:
	DalasCrc( const DalasCrc &c );
	DalasCrc& operator=( const DalasCrc &c );
	
	byte_t crcTable[256];
	
	void init();

}; //DalasCrc

#endif //__DALASCRC_H__
