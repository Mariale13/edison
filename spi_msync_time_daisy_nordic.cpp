/* This code is used to test  */ 

#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <thread>

#include "mraa.hpp"
#define fSCLK 8000000  


int running = 0;	
int i,j=0;
int maxDif, error,restartCount = 0;
int timeNode1, timeNode2, prevTime1, prevTime2, timeNodesDrift,  currentDiff1, currentDiff2, maxNodeDrift = 0; 
static volatile bool timerFlag = false;

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

void setInterval(){  
	while(running == 0){
	   timerFlag = true; 
       usleep(1000);
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
    mraa::Gpio* gpio_cs = new mraa::Gpio(19);
    mraa::Gpio* gpio_sync = new mraa::Gpio(20);
    if (gpio_cs == NULL || gpio_sync==NULL) {
        return mraa::ERROR_UNSPECIFIED;
    }
    mraa::Result response = gpio_cs->dir(mraa::DIR_OUT);
    response = gpio_sync->dir(mraa::DIR_OUT);
    if (gpio_sync->dir(mraa::DIR_OUT) != mraa::SUCCESS || gpio_cs->dir(mraa::DIR_OUT) !=mraa::SUCCESS) {
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
    
    // temporal 
    std::thread t1(setInterval);  

    uint8_t rxBuf[54];
    uint8_t txBuf[4] = {1,2,3,4};
    uint8_t* recv;
    sleep(1);
        
    while (running == 0) {  
 		if (timerFlag){
 			timerFlag = false;
    		prevTime1 = timeNode1;  
			prevTime2 = timeNode2; 
	 		gpio_sync->write(1);	// trigger getData signal
			usleep(500);
	 		gpio_sync->write(0);	// trigger getData signal    	
			gpio_cs->write(0);
			if (spi->transfer(NULL, rxBuf,50) == mraa::SUCCESS) {
			  gpio_cs->write(1);
			  if (rxBuf[0] == 0xFF && rxBuf[25] == 0xFF ){		//Temporal Added to not include the frames not received
			  	fprintf(fileWrite,"\nFrame Not Received");
			  }else{
	  		    timeNode1 = (rxBuf[4]<<24) | (rxBuf[3]<<16) | (rxBuf[2]<<8) |rxBuf[1] ;
	   		    timeNode2 = (rxBuf[29]<<24) | (rxBuf[28]<<16) | (rxBuf[27]<<8) |rxBuf[26] ;
		    	currentDiff1 = timeNode1-prevTime1;
		    	currentDiff2 = timeNode2-prevTime2;
		    	timeNodesDrift = abs(timeNode2 - timeNode1);
				if(timeNode1 !=0){  
					 j++;
    	   		  	 fprintf(fileWrite,"\n\nRaw Node1 0x%.2x %.2x %.2x %.2x %.2x",rxBuf[4],rxBuf[3],rxBuf[2],rxBuf[1],rxBuf[0]);
					 for (int m= 5; m<25 ; m++){
						fprintf(fileWrite," %.2x", rxBuf[m]);
					 }
	   		  	     // fprintf(fileWrite,"\nRaw Node2 0x%.2x%.2x%.2x%.2x%.2x ",rxBuf[9],rxBuf[8],rxBuf[7],rxBuf[6],rxBuf[5]);
				   	 fprintf(fileWrite,"\nDifBetTX_Node1: %d ; DifBetTX_Node2: %d	\n",  currentDiff1, currentDiff2);
				}else{
				   	i++;
				} 
			 }
			}else {		// else transfer not completed
				error++;
			}
		  memset(rxBuf,1,14);	
		  firstFlag--;
		}  	//end of IF timerFlag 
    }
    
    delete spi;
    delete gpio_cs;
   	// std::terminate();
	// t1.~thread();
    fseek (fileWrite, 0, SEEK_SET);     
    fprintf(fileWrite,"\n MaxDifference Between Transmissions = %d \n Received_OK= %d \n error = %d \n Total Lost %d\n Nr DataLost %d\n", maxDif, j, error, i,restartCount);
    fprintf(fileWrite,"\nMax Drifting between Noded = %d", maxNodeDrift);
    fprintf(fileWrite,"\nClosing spi nicely\n");    
    fclose(fileWrite);

    return mraa::SUCCESS;
}
