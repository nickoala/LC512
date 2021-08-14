#include "LC512.h"
#include <Wire.h>

const uint16_t HighestAddr = 0xffff;
const uint16_t WriteSize = 64;
const uint16_t ReadSize = 128;

enum ee_result ee_write(
    uint8_t i2c,
    uint16_t* addr, byte* bs, size_t len,
    unsigned int ms
) {
  if ( *addr + len > HighestAddr + 1 ) return ExceedRomCapacity;

  while ( len > 0 ) {
    // Actual number of bytes to write in this round:
    // length to page boundary, or length of data left,
    // whichever is shorter.
    //
    int n = min( WriteSize - *addr % WriteSize, (int) len );

    Wire.beginTransmission( i2c );
    Wire.write( *addr >> 8 );
    Wire.write( *addr & 0xff );
    Wire.write( bs, n );

    if ( byte x = Wire.endTransmission() )
      return (enum ee_result) x;

    bs += n;
    *addr += n;
    len -= n;

    // Wait for it to finish before next operation.
    if ( ms > 0 ) {
      delay( ms );  // delay a fixed number of milliseconds
    }
    else {
      unsigned long t = millis();

      while (1) {    // poll for acknowledge
        delay( 1 );  // every 1 ms

        Wire.beginTransmission( i2c );
        if ( Wire.endTransmission() == 0 )
          break;
        else if ( millis() - t > 20 )
          return Timeout;
      }
    }
  }

  return Success;
}

enum ee_result ee_addr( uint8_t i2c, uint16_t addr ) {
  Wire.beginTransmission( i2c );
  Wire.write( addr >> 8 );
  Wire.write( addr & 0xff );

  return (enum ee_result) Wire.endTransmission();
}

enum ee_result ee_read(
    uint8_t i2c,
    uint16_t* addr, byte* bs, size_t len
) {
  if ( *addr + len > HighestAddr + 1 ) return ExceedRomCapacity;

  // Skip if no buffer
  if ( !bs ) {
    *addr += len;
    return ee_addr( i2c, *addr );
  }

  while ( len > 0 ) {
    int n = min( (int) ReadSize, (int) len );

    if ( n != Wire.requestFrom( i2c, (uint8_t) n ))
      return UnexpectedDataLength;

    unsigned long t = millis();

    for ( int i=0; i < n; i++ ) {
      if ( Wire.available() ) {
        *bs++ = Wire.read();
        (*addr)++;
        len--;
        t = millis();  // update read time
      }
      else if ( millis() - t > 20 ) {
        return Timeout;
      }
    }
  }

  return Success;
}

enum ee_result ee_uint8(
    uint8_t i2c,
    uint16_t* addr, uint8_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, len );
}

enum ee_result ee_int8(
    uint8_t i2c,
    uint16_t* addr, int8_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, len );
}

enum ee_result ee_uint16(
    uint8_t i2c,
    uint16_t* addr, uint16_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, 2*len );
}

enum ee_result ee_int16(
    uint8_t i2c,
    uint16_t* addr, int16_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, 2*len );
}

enum ee_result ee_uint32(
    uint8_t i2c,
    uint16_t* addr, uint32_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, 4*len );
}

enum ee_result ee_int32(
    uint8_t i2c,
    uint16_t* addr, int32_t* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, 4*len );
}

enum ee_result ee_float(
    uint8_t i2c,
    uint16_t* addr, float* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, sizeof(float)*len );
}

enum ee_result ee_double(
    uint8_t i2c,
    uint16_t* addr, double* xs, size_t len
) {
  return ee_read( i2c, addr, (byte*) xs, sizeof(double)*len );
}

enum ee_result ee_uint8_(
    uint8_t i2c,
    uint16_t* addr, uint8_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, len, ms );
}

enum ee_result ee_int8_(
    uint8_t i2c,
    uint16_t* addr, int8_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, len, ms );
}

enum ee_result ee_uint16_(
    uint8_t i2c,
    uint16_t* addr, uint16_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, 2*len, ms );
}

enum ee_result ee_int16_(
    uint8_t i2c,
    uint16_t* addr, int16_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, 2*len, ms );
}

enum ee_result ee_uint32_(
    uint8_t i2c,
    uint16_t* addr, uint32_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, 4*len, ms );
}

enum ee_result ee_int32_(
    uint8_t i2c,
    uint16_t* addr, int32_t* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, 4*len, ms );
}

enum ee_result ee_float_(
    uint8_t i2c,
    uint16_t* addr, float* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, sizeof(float)*len, ms );
}

enum ee_result ee_double_(
    uint8_t i2c,
    uint16_t* addr, double* xs, size_t len,
    unsigned int ms
) {
  return ee_write( i2c, addr, (byte*) xs, sizeof(double)*len, ms );
}
