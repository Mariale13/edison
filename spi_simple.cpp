#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 1000000  

int running = 0;	
int i=0;
int j, error= 0;


void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\n OK= %d \n error = %d \n Total Lost %d\n", j, error, i);
        printf("closing spi nicely\n");
        running = -1;
    }
}

int main(){   
   
    mraa::Spi* spi;
    spi = new mraa::Spi(0);
    spi->frequency(1000000);
    uint8_t data[] = { 0x00, 100 };
    uint8_t rxBuf[2];
    uint8_t* recv;
    while (running == 0) {
        int i;
        for (i = 90; i < 130; i++) {
            data[1] = i;
            recv = spi->write(data, 2);
            printf("Writing -%i", i);
            if (recv) {
                printf("RECIVED-%i-%i\n", recv[0], recv[1]);
                free(recv);
            }
            //usleep(100000);
        }
        for (i = 130; i > 90; i--) {
            data[1] = i;
            if (spi->transfer(data, rxBuf, 2) == mraa::SUCCESS) {
                printf("Writing -%i", i);
                printf("RECIVED-%i-%i\n", rxBuf[0], rxBuf[1]);
            }
            //usleep(100000);
        }
    }
    delete spi;
 }

