#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 8000000  

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
	int time = 0; 
    signal(SIGINT, sig_handler);
    mraa::Spi* spi;

    spi = new mraa::Spi(5);
    spi->frequency(fSCLK );
    spi->mode(mraa::SPI_MODE3);
	//spi->lsbmode(0);

    uint8_t txBuf[2] = {WHO_AM_I_REG, 0xFF };
    uint8_t rxBuf[4] = {0 , 0 , 0 , 0};
    uint8_t* recv;

    
    while (running == 0) {        
		if (spi->transfer(txBuf, rxBuf, 4) == mraa::SUCCESS) {
		    time = (rxBuf[3]<<24) | (rxBuf[2]<<16) | (rxBuf[1]<<8) |rxBuf[0] ;
		    if(time !=0){  
			     j++;
		       	 printf("Time-%d\n", time);
		    }else{
		       	printf("Lost\n");
		       	i++;
		    }           

		}else {
			error++;
		}
    }
    delete spi;
    //! [Interesting]
    return mraa::SUCCESS;
}