#include <Wire.h>
#include "LC512.h"

void setup() {
  Wire.begin();
  Wire.setClock(400000);  // 2K pull-up resistors recommended
  Serial.begin(115200);

  randomSeed(analogRead(0));  // to randomize write data

  while (!Serial);

  byte write[1024];  // make sure long enough
  byte read[1024];   // for the longest test

  // write/read a short one
  test(0, write, read, 16);

  // write/read a short one at odd location and length
  test(49, write, read, 35);

  // long write/read spanning multiple pages
  test(100, write, read, 1024);

  // long write/read at odd location and length
  test(1111, write, read, 999);

  // zero all memory
  writeZeros();
}

void loop() {}


const uint8_t i2c = 0x50;


void test(uint16_t start, byte* write, byte* read, size_t len) {
  memset(read, 0, len);

  randomize(write, len);

  writeReadCheck(start, write, read, len);
}

void randomize(byte* bs, size_t len) {
  for (int i=0; i < len; i++) {
    bs[i] = random(0, 256);
  }
}

void writeReadCheck(uint16_t start, byte* write, byte* read, size_t len) {
  uint16_t addr = start;

  printBytes("To write", write, len);

  // Write
  if ( ee_write(i2c, &addr, write, len, 0) != Success ) {
    Serial.println("Write FAILED");
    return;
  }

  // addr got incremented ?
  printInt("addr == ", addr);

  // Read back the bytes just written
  addr = start;

  if ( ee_addr(i2c, addr) != Success
    || ee_read(i2c, &addr, read, len) != Success
  ) {
    Serial.println("Read FAILED");
    return;
  }

  // addr got incremented ?
  printInt("addr == ", addr);

  // bytes read == bytes written ?
  if ( memcmp(write, read, len) ) {
    printBytes("DIFFERENT", read, len);
  }
  else {
    Serial.println("Same");
  }
  Serial.println("-------------------------------------");
}

void printBytes(const char* head, byte* bs, size_t len) {
  Serial.print(
    String(head) + String(" [") + String(len) + String("]:")
  );

  for (int i=0; i < len; i++) {
    Serial.print(' ');
    Serial.print(bs[i], HEX);
  }

  Serial.println();
}

void printInt(const char* head, int x) {
  Serial.println(String(head) + String(x));
}

void writeZeros() {
  const int len = 1024;
  byte zeros[len];

  memset(zeros, 0, len);

  uint16_t addr = 0;

  while (1) {
    printInt("Writing zeros at: ", addr);

    uint16_t prev = addr;
    if ( ee_write(i2c, &addr, zeros, len, 0) != Success ) {
      Serial.println("Write FAILED");
      break;
    }
    else if ( addr < prev ) {  // wrapped around
      Serial.println("Done");
      break;
    }
  }
}
