#ifndef CRC_H
#define CRC_H

//#include  <cinttypes>
#include  <inttypes.h>


template <class T, uint32_t polynom>
class Crc
{

public:
    Crc()
    {
        init();
    }

    T compute(const uint8_t *message, int size, T initRemainder = 0x00)
    {
        T remainder = initRemainder;

        for (int byte = 0; byte < size; ++byte)
        {
            uint8_t data = message[byte] ^ (remainder >> (WIDTH - 8));

            remainder = crcTable[data] ^ (remainder << 8);
        }

        return remainder;
    }

private:

    void init()
    {
        T  remainder;
        for (int i = 0; i < 256; ++i)
        {
            remainder = i << (WIDTH - 8);
            for (uint8_t bit = 8; bit > 0; --bit)
            {
                if (remainder & TOPBIT)
                {
                    remainder = (remainder << 1) ^ polynom;
                }
                else
                {
                    remainder = (remainder << 1);
                }
            }
            crcTable[i] = remainder;
        }
    }


    const int WIDTH  = (8 * sizeof(T));
    const int TOPBIT = (1 << (WIDTH - 1));


    T crcTable[256];
};

#endif // CRC_H
