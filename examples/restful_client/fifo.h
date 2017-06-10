#ifndef _CMD_FIFO
#define _CMD_FIFO

#include <stdint.h>

void CFifo();
int Fifo_fileExists(const char* file);
int Fifo_Open(int index , const char * myfifo, int mode)  ;
void Fifo_Close(int index);
int Fifo_Write(int i , const uint8_t *buffer, int index, int size);
int Fifo_Read (int i , uint8_t *buffer, int index, int size);



#endif 
