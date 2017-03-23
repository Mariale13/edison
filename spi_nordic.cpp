#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define WHO_AM_I_REG 0xF5 		// including the bit for reading
#define fSCLK 8000000  

int running = 0;	
int i,j=0;
int maxDif, error,restartCount = 0;


void
sig_handler(int signo)
{
    if (signo == SIGINT) {
        printf("\n MaxDifference = %d \n OK= %d \n error = %d \n Total Lost %d\n Nr DataLost %d\n", maxDif, j, error, i, restartCount);
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

    uint8_t rxBuf[14];
    uint8_t* recv;
        
    while (running == 0) {  
    	prevTime = time;  
		gpio->write(0);
		if (spi->transfer(NULL, rxBuf,14) == mraa::SUCCESS) {
	    	gpio->write(1);
  		    time0 = (rxBuf[3]<<24) | (rxBuf[2]<<16) | (rxBuf[1]<<8) |rxBuf[0] ;
  		    time = (rxBuf[13]<<24) | (rxBuf[12]<<16) | (rxBuf[11]<<8) |rxBuf[10] ;
        	currentDiff = time-prevTime;
		    if(time !=0){  
			     j++;
   		  	     fprintf(fileWrite,"\nRaw0 0x%.2x%.2x%.2x%.2x ",rxBuf[3],rxBuf[2],rxBuf[1],rxBuf[0]);
   		       	 fprintf(fileWrite," Raw1 0x%.2x%.2x%.2x%.2x ",rxBuf[13],rxBuf[12],rxBuf[11],rxBuf[10]);
		       	 fprintf(fileWrite," Time: %d; Time0: %d; DifTime: %d	", time, time0, currentDiff);
		    }else{
		       	i++;
		    }           
		    if (currentDiff> maxDif && firstFlag<0 ){
		    	maxDif = currentDiff;
	    	}else if(currentDiff < 0 && firstFlag <0){
		    	restartCount++;
	    		fprintf(fileWrite,"Data Lost!!"); 
			firstFlag = 1;
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
    fprintf(fileWrite,"\n MaxDifference = %d \n OK= %d \n error = %d \n Total Lost %d\n Nr DataLost %d\n", maxDif, j, error, i,restartCount);
    fprintf(fileWrite,"closing spi nicely\n");    
   	fclose(fileWrite);
    //! [Interesting]
    return mraa::SUCCESS;
}
