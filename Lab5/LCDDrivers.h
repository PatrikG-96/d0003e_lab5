
#include <stdint-gcc.h>

void initLCD();

void writeChar(char ch, int pos);

void writeLong(long i);

void printAt(long num, int pos);

void setbits(volatile uint8_t *ptr, int width, int start, uint8_t value);
