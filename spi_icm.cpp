#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 4000000  

int running = 0;
int i=0;
int j = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\nOK= %d ; Total Lost %d\n", j, i);
        printf("closing spi nicely\n");
        running = -1;
    }
}

int
main()
{
    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::Spi* spi;

    spi = new mraa::Spi(5);
    spi->frequency(fSCLK );

    uint8_t reg = WHO_AM_I_REG;
    uint8_t rxBuf[2] = {0 , 0};
    uint8_t* recv;

    
    while (running == 0) {
        
		if (spi->transfer(&reg, rxBuf, 2) == mraa::SUCCESS) {
                //printf("Writing - ");
                if(rxBuf[1] !=0){
	                j++;
                	//printf("RECIVED-%i-0x%x\n", rxBuf[0], rxBuf[1]);
                }else {
                	printf("Lost\n");
                	i++;
                }
        }           
        rxBuf[1] = 0; 
    }
    delete spi;
    //! [Interesting]

    return mraa::SUCCESS;
}
