/* This code is used to test the simple daisy Chain Communication, testing what is being received on both nodes */ 

#include <unistd.h>
#include <signal.h>
#include <stdint.h>

#include "mraa.hpp"
#define fSCLK 8000000  


int running = 0;	
int i,j=0;
int maxDif, error,restartCount = 0;
int timeNode1, timeNode2, prevTime1, prevTime2, timeNodesDrift,  currentDiff1, currentDiff2, maxNodeDrift = 0; 


void
sig_handler(int signo)
{
    if (signo == SIGINT) {
		printf("\n MaxDifference Between Transmissions = %d \n Received_OK= %d \n error = %d \n Total Lost %d\n Nr DataLost %d\n", maxDif, j, error, i,restartCount);
		printf("\n Max Drifting between Noded = %d", maxNodeDrift); 
	    printf("\nClosing spi nicely\n");    		     
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

    signal(SIGINT, sig_handler);
    mraa::Spi* spi;

    spi = new mraa::Spi(5);
    spi->frequency(fSCLK );
    spi->mode(mraa::SPI_MODE3);
    spi->lsbmode(0);
    spi->bitPerWord(8);

    uint8_t rxBuf[54];
    uint8_t txBuf[4] = {1,2,3,4};
    uint8_t* recv;
        
    while (running == 0) {  
    	prevTime1 = timeNode1;  
    	prevTime2 = timeNode2;  
		gpio->write(0);
		if (spi->transfer(NULL, rxBuf,10) == mraa::SUCCESS) {
	    	gpio->write(1);
  		    timeNode1 = (rxBuf[4]<<24) | (rxBuf[3]<<16) | (rxBuf[2]<<8) |rxBuf[1] ;
   		    timeNode2 = (rxBuf[9]<<24) | (rxBuf[8]<<16) | (rxBuf[7]<<8) |rxBuf[6] ;
        	currentDiff1 = timeNode1-prevTime1;
        	currentDiff2 = timeNode2-prevTime2;
        	timeNodesDrift = abs(timeNode2 - timeNode1);
		    if(time !=0){  
			     j++;
   		  	     fprintf(fileWrite,"\n\nRaw Node1 0x%.2x%.2x%.2x%.2x%.2x ",rxBuf[4],rxBuf[3],rxBuf[2],rxBuf[1],rxBuf[0]);
   		  	     fprintf(fileWrite,"\nRaw Node2 0x%.2x%.2x%.2x%.2x%.2x ",rxBuf[9],rxBuf[8],rxBuf[7],rxBuf[6],rxBuf[5]);
		       	 fprintf(fileWrite,"\nTimeNode1: %d; TimeNode2: %d; Node Time Diff: %d;\n DifBetTX_Node1: %d ; DifBetTX_Node2: %d	",timeNode1,timeNode2, timeNodesDrift,  currentDiff1, currentDiff2);
		    }else{
		       	i++;
		    } 
		    /* Difference between Transmissions */           
		    if (currentDiff1> maxDif && firstFlag<0 ){
		    	maxDif = currentDiff1;
	    	}else if(currentDiff1 < 0 && firstFlag <0){
		    	restartCount++;
	    		fprintf(fileWrite,"Data Lost!!"); 
			    firstFlag = 1;
	    	}
	    	if(timeNodesDrift > maxNodeDrift && firstFlag<0 ){
	    		maxNodeDrift = timeNodesDrift; 
	    	}

		}else {
			error++;
		}
	  memset(rxBuf,1,14);	
////     	usleep(100);
      firstFlag--;
    }
    delete spi;
    delete gpio;
    fseek (fileWrite, 0, SEEK_SET);     
    fprintf(fileWrite,"\n MaxDifference Between Transmissions = %d \n Received_OK= %d \n error = %d \n Total Lost %d\n Nr DataLost %d\n", maxDif, j, error, i,restartCount);
    fprintf(fileWrite,"\nMax Drifting between Noded = %d", maxNodeDrift);
    fprintf(fileWrite,"\nClosing spi nicely\n");    
    fclose(fileWrite);
    //! [Interesting]
    return mraa::SUCCESS;
}
