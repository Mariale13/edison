#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading

int running = 0;

void
sig_handler(int signo)
{
    if (signo == SIGINT) {
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

    uint8_t reg = WHO_AM_I_REG;
    uint8_t rxBuf[2];
    uint8_t* recv;
    
    while (running == 0) {
        
		if (spi->transfer(&reg, rxBuf, 1) == mraa::SUCCESS) {
                printf("Writing - ");
                printf("RECIVED-%i-0x%x\n", rxBuf[0], rxBuf[1]);
        }            
    }
    delete spi;
    //! [Interesting]

    return mraa::SUCCESS;
}
