#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 8000000  

int running = 0;	
int i,j=0;
int error = 0;


void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\n OK= %d \n error = %d \n Total Lost %d\n ", j, error, i);
        printf("closing spi nicely\n");
        running = -1;
    }
}

int main(){
	FILE * fileWrite;
	int firstFlag = 20;
	fileWrite=fopen("DataIntCol.txt","w");	
	if(!fileWrite) {
		printf("File not Opened");
		return 0;
	}
	
	/* GPIO  */
    mraa::Gpio* gpio = new mraa::Gpio(19);
    if (gpio == NULL) {
        return mraa::ERROR_UNSPECIFIED;
    }
    mraa::Result response = gpio->dir(mraa::DIR_OUT);
    if (response != mraa::SUCCESS) {
        mraa::printError(response);
        return 1;
    }

	
	int time, time0, prevTime, currentDiff = 0; 
    signal(SIGINT, sig_handler);
    mraa::Spi* spi;

    spi = new mraa::Spi(5);
    spi->frequency(fSCLK );
    spi->mode(mraa::SPI_MODE3);
    spi->lsbmode(0);
    spi->bitPerWord(8);

    uint8_t rxBuf[10];
    uint8_t* recv;
        
    while (running == 0) {  
    	prevTime = time;  
		gpio->write(0);
		if (spi->transfer(NULL, rxBuf,10) == mraa::SUCCESS) {
	    	gpio->write(1);
			fprintf(fileWrite,"\nRaw: ");
			for (int m=0; m<10;m++){
   		  	     fprintf(fileWrite,"0x%.2x ",rxBuf[m]);
   		  	}
		}else {
			error++;
		}
	  memset(rxBuf,1,14);	
    	// 	sleep(1);
      firstFlag--;
    }
    delete spi;
    delete gpio;
    fseek (fileWrite, 0, SEEK_SET);     
    fprintf(fileWrite,"\n OK= %d \n error = %d \n Total Lost %d\n", j, error, i);
    fprintf(fileWrite,"closing spi nicely\n");    
   	fclose(fileWrite);
    //! [Interesting]
    return mraa::SUCCESS;
}
