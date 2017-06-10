#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "fifo.h"
 

static int m_fd[2] = {-1,-1};



void CFifo()
{
    
  m_fd[0] = -1;
  m_fd[1] = -1;
               
}

int Fifo_fileExists(const char* file)
{
    struct stat buf;
    return (stat(file, &buf) == 0);
}

int Fifo_Open(int index , const char * myfifo, int mode)
{
    
    if (Fifo_fileExists(myfifo) == 0)
    {    
        int result = mkfifo (myfifo, S_IRUSR| S_IWUSR);
        //printf ("result %d\n" , result);
        if (result < 0) {
           perror ("mkfifo");
           return 0;
        } 
        
        char mode[] = "0777";        
        int i;
        i = strtol(mode, 0, 8);
        if (chmod (myfifo,i) < 0)
        {
            perror ("chmod");
            return 0;
        }                
    }
    if (mode == 1)
    	m_fd[index] = open(myfifo, O_RDONLY /*| O_NONBLOCK*/);
    if (mode == 2)
    	m_fd[index] = open(myfifo, O_WRONLY /*| O_NONBLOCK*/);
    if (mode == 3)
    	m_fd[index] = open(myfifo, O_RDWR /*| O_NONBLOCK*/);
    if (m_fd[index] == -1)
    {
        perror("error  opening fifo");
        return 0;
    }

    return m_fd[index];
}
    
void Fifo_Close(int index )
{
    if (m_fd[index] != -1)
    {
        close(m_fd[index]);
		m_fd[index] = -1;
    }
}
        
int Fifo_Write(int index, const uint8_t *buffer, int i, int size)
{


    if (m_fd[index] == -1)
		 return 0;

    //printf("write to fifo %d sizeof %d\n" , index, size);
    if (write(m_fd[index], buffer + i, size) == size)
    {
		return 1;
    }

	return 0;
	 
}
int Fifo_Read (int index, uint8_t *buffer, int i, int size)
{


	//printf("fifo read  %d %d\n" ,  index, m_fd[index]);
    if (m_fd[index] == -1)
    {
		 return 0;
    }
    
    //printf("wait for read from fifo %d sizeof %d\n", index, size);
    int res = read(m_fd[index], buffer + i, size);
	if (res == size)
	{
		 return 1;
	}

	return 0;
}


