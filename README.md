# Arduino Library to read and write 24LC512 Serial EEPROM

I use the 512K I<sup>2</sup>C Serial EEPROM in an ESP32 project. [There
are](https://github.com/sparkfun/SparkFun_External_EEPROM_Arduino_Library) [a
few libraries](https://github.com/Naguissa/uEEPROMLib) [out
there](https://github.com/zacharyvincze/esp32-i2c-eeprom), but I find them too
heavy in the amount of code, or only works with ESP-IDF. I want one working with
Arduino.

This library uses the `Wire` library for I<sup>2</sup>C communication. **I have
only tested it on ESP32 with 24LC512**, although I expect it to work on other
Arduino-compatible MCUs, and with a little [adaptation](#adapt), on other
EEPROMs of the same family as well, e.g. 24LC256.


## Usage

Just copy the files `LC512.h` and `LC512.cpp` into your project.

In `setup()`, initialize the `Wire` library:

```C++
void setup() {
  Wire.begin();
  Wire.setClock(400000);  // pick your speed
}
```

`test.ino` illustrates basic usage, which I also discuss below.


## Write

The function is `ee_write()`. Consider an example:

```C++
const uint8_t i2c = 0x50;

uint16_t addr = 0;

const int len = 1024;
byte data[len];

// Pretend to fill in `data`

ee_write( i2c, &addr, data, len, 0 );
```

- `i2c` is the I<sup>2</sup>C address of the EEPROM. It is configurable by
  3 chip select pins on the IC. When using one EEPROM, the most usual
  configuration is to hard-wire them to ground, resulting in an I<sup>2</sup>C
  address of `0x50`.

- `addr` is the address into EEPROM where writing will start. It is passed to
  `ee_write()` as a pointer, to be incremented as writing occurs. This is meant
  to make `addr` mirror the EEPROM's internal address counter. You can call
  `ee_write()` successively without explicitly incrementing `addr` yourself.


- `data` and `len` are self-explanatory. The idea is to supply data of
  _arbitrary lengths_ without worrying about the EEPROM's internal page limit.
  `ee_write()` breaks long data into chunks to fit within page boundaries.

- The trailing argument tells `ee_write()` how many _milliseconds_ to wait
  between successive write operations. A write takes about 5 ms, so waiting
  more than 5 ms is safe. **A better way is just say `0` as above**, which
  tells it to poll the EEPROM for readiness. Polling is done every 1 ms, and
  times out after 20 ms.  

A good habit is to always check the return value of `ee_write()` for failure. No
check is done here for simplicity. Read [this section](#check-return-value) to
learn how to interpret the return value.


## Read

Use `ee_addr()` to set the EEPROM's internal address, then use `ee_read()` to
start reading. For example:

```C++
const uint8_t i2c = 0x50;

uint16_t addr = 0;

const int len = 1024;
byte data[len];

ee_addr( i2c, addr );

ee_read( i2c, &addr, data, len );

// Pretend to process `data`

ee_read( i2c, &addr, data, len );  // keep reading
```

- `i2c` is the I<sup>2</sup>C address of the EEPROM, as before.

- `ee_addr()` sets the EEPROM's internal address.

- `ee_read()` increments `addr` as it reads. This makes `addr` mirror the
  EEPROM's internal address counter. You can read contiguous blocks of memory by
  calling `ee_read()` successively, _with no `ee_addr()` in between_.
  `ee_addr()` is required only to jump around.

- `data` and `len` are self-explanatory. The idea is to read data of
  _arbitrary lengths_ without worrying about the EEPROM's internal read limit.

- If `data` is `NULL`, no read occurs. Instead,
  - `addr` is increased by `len`
  - `ee_addr( i2c, addr )` is called to keep `addr` mirroring the EEPROM's
    internal address

  In effect, `len` bytes of memory have been skipped. Use `ee_read()` to keep
  reading.

A good habit is to always check the return value of `ee_addr()` and `ee_read()`
for failure. No check is done here for simplicity. Read [this
section](#check-return-value) to learn how to interpret the return value.


## More than bytes

`ee_read()` and `ee_write()` accept `byte`s, which is all you need. But other
types are so common there is no reason not to make convenience functions for
them.

Function signatures are exactly the same, except the data types. Read the source
files for absolute clarity.

Read into arrays of various types:

```C++
ee_uint8 ( ..., uint8_t*, ... );
ee_int8  ( ..., int8_t*, ... );
ee_uint16( ..., uint16_t*, ... );
ee_int16 ( ..., int16_t*, ... );
ee_uint32( ..., uint32_t*, ... );
ee_int32 ( ..., int32_t*, ... );
ee_float ( ..., float*, ... );
ee_double( ..., double*, ... );
```

Write arrays of various types:

```C++
ee_uint8_ ( ..., uint8_t*, ... );
ee_int8_  ( ..., int8_t*, ... );
ee_uint16_( ..., uint16_t*, ... );
ee_int16_ ( ..., int16_t*, ... );
ee_uint32_( ..., uint32_t*, ... );
ee_int32_ ( ..., int32_t*, ... );
ee_float_ ( ..., float*, ... );
ee_double_( ..., double*, ... );
```


<a name="check-return-value"></a>
## Check `enum ee_result`

All functions return this enum:

```C++
enum ee_result {
  Success = 0,
  DataTooLongToBuffer, NackOnAddr, NackOnData, OtherTransmissionError,
  ExceedRomCapacity, UnexpectedDataLength, Timeout
};
```

Non-zero indicates error.

`DataTooLongToBuffer`<br>
`NackOnAddr`<br>
`NackOnData`<br>
`OtherTransmissionError`<br>

- These are error codes returned by
  [`Wire.endTransmission()`](https://www.arduino.cc/en/Reference/WireEndTransmission),
  unfiltered.

Others are self-explanatory, or look for them in `LC512.cpp` to see how they
arise.

**You should always check the return value to see if the read/write is
successful.** For example, if I want to store an UTF8-encoded string, this is
how I would do it:

```C++

struct utf8 {
  uint8_t length;
  byte* bytes;
};

struct utf8 x = {
  // Pretend to initialize it
};

if ( ee_uint8_( i2c, &addr, &x.length, 1, 0 ) != Success
  || ee_write( i2c, &addr, x.bytes, x.length, 0 ) != Success
) {
  // Handle error
}

// Keep doing stuff

```


<a name="adapt"></a>
## Adapt for other EEPROMs

`LC512.cpp` defines these constants:

```C++
const uint16_t HighestAddr = 0xffff;
const uint16_t WriteSize = 64;
const uint16_t ReadSize = 128;
```

- `HighestAddr` obviously depends on the EEPROM's capacity

- `WriteSize` is how many bytes can be written in one write operation

- `ReadSize` is how many bytes can be read in one read operation

To make this library work on other EEPROMs of the same family, I suspect you
have to modify `HighestAddr` to match the capacity. Whether `WriteSize` or
`ReadSize` requires modification depends on the specs obviously. I guess you can
find out by reading the datasheets, and/or by simple experimentation.
