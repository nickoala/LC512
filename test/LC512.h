#ifndef __LC512_h
#define __LC512_h

#include <Arduino.h>

enum ee_result {
  Success = 0,
  DataTooLongToBuffer, NackOnAddr, NackOnData, OtherTransmissionError,
  ExceedRomCapacity, UnexpectedDataLength, Timeout
};

enum ee_result ee_write(
    uint8_t i2c,
    uint16_t* addr, byte* bs, size_t len,
    unsigned int ms
);

enum ee_result ee_addr( uint8_t i2c, uint16_t addr );

enum ee_result ee_read(
    uint8_t i2c,
    uint16_t* addr, byte* bs, size_t len
);

enum ee_result ee_uint8 ( uint8_t, uint16_t*, uint8_t*, size_t );
enum ee_result ee_int8  ( uint8_t, uint16_t*, int8_t*, size_t );
enum ee_result ee_uint16( uint8_t, uint16_t*, uint16_t*, size_t );
enum ee_result ee_int16 ( uint8_t, uint16_t*, int16_t*, size_t );
enum ee_result ee_uint32( uint8_t, uint16_t*, uint32_t*, size_t );
enum ee_result ee_int32 ( uint8_t, uint16_t*, int32_t*, size_t );
enum ee_result ee_float ( uint8_t, uint16_t*, float*, size_t );
enum ee_result ee_double( uint8_t, uint16_t*, double*, size_t );

enum ee_result ee_uint8_ ( uint8_t, uint16_t*, uint8_t*, size_t, unsigned int );
enum ee_result ee_int8_  ( uint8_t, uint16_t*, int8_t*, size_t, unsigned int );
enum ee_result ee_uint16_( uint8_t, uint16_t*, uint16_t*, size_t, unsigned int );
enum ee_result ee_int16_ ( uint8_t, uint16_t*, int16_t*, size_t, unsigned int );
enum ee_result ee_uint32_( uint8_t, uint16_t*, uint32_t*, size_t, unsigned int );
enum ee_result ee_int32_ ( uint8_t, uint16_t*, int32_t*, size_t, unsigned int );
enum ee_result ee_float_ ( uint8_t, uint16_t*, float*, size_t, unsigned int );
enum ee_result ee_double_( uint8_t, uint16_t*, double*, size_t, unsigned int );

#endif
