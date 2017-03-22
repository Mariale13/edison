#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 2000000  

int running = 0;	
int i,j=0;
int maxDif, error= 0;


void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\n MaxDifference = %d \n OK= %d \n error = %d \n Total Lost %d\n", maxDif, j, error, i);
        printf("closing spi nicely\n");
        running = -1;
    }
}

int main(){
	FILE * fileWrite;
	fileWrite=fopen("DataIntCol.txt","w");	
	if(!fileWrite) {
		printf("File not Opened");
		return 0;
	}	
	
	int time, prevTime, currentDiff = 0; 
    signal(SIGINT, sig_handler);
    mraa::Spi* spi;

    spi = new mraa::Spi(5);
    spi->frequency(fSCLK );
    spi->mode(mraa::SPI_MODE3);
    spi->lsbmode(0);
    spi->bitPerWord(8);

    uint8_t txBuf[4] = {0,0,0,0};
    uint8_t rxBuf[4] = {0,0,0,0};
    uint8_t* recv;
        
    while (running == 0) {  
    	prevTime = time;      
		if (spi->transfer(NULL, rxBuf,1) == mraa::SUCCESS) {
		    spi->transfer(NULL, rxBuf+1, 1);
		    spi->transfer(NULL, rxBuf+2, 1);
		    spi->transfer(NULL, rxBuf+3, 1);
  		    time = (rxBuf[3]<<24) | (rxBuf[2]<<16) | (rxBuf[1]<<8) |rxBuf[0] ;
        	currentDiff = time-prevTime;
		    if(time !=0){  
			     j++;
   		       	 fprintf(fileWrite,"Raw 0x%.2x%.2x%.2x%.2x ",rxBuf[3],rxBuf[2],rxBuf[1],rxBuf[0]);
		       	 fprintf(fileWrite," Time: %10d;  DifTime: %d\n", time, currentDiff);

		    }else{
		       	i++;
		    }           
		    if (currentDiff> maxDif)
		    	maxDif = currentDiff;

		}else {
			error++;
		}
	//	sleep(1);
    }
    delete spi;
    fprintf(fileWrite,"\n MaxDifference = %d \n OK= %d \n error = %d \n Total Lost %d\n", maxDif, j, error, i);
    fprintf(fileWrite,"closing spi nicely\n");    
   	fclose(fileWrite);
    //! [Interesting]
    return mraa::SUCCESS;
}
