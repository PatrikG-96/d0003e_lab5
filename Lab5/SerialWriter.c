/*
 * SerialWriter.c
 *
 * Created: 2021-03-09 14:46:18
 *  Author: shirt
 */ 

void USART_write(SerialWriter *self, int arg0) {
	UDR0 = arg0;
}