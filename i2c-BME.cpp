#include <unistd.h>
#include <signal.h>

#include "mraa.hpp"
#include "math.h"

#define MAX_BUFFER_LENGTH 6
// #define HMC5883L_I2C_ADDR 0x1E
#define BME280_I2C_ADDR 0x76
#define BME_WHOAMI_REG  0xD0

// mode register
#define HMC5883L_MODE_REG 0x02

// data register
#define HMC5883L_X_MSB_REG 0
#define HMC5883L_X_LSB_REG 1
#define HMC5883L_Z_MSB_REG 2
#define HMC5883L_Z_LSB_REG 3
#define HMC5883L_Y_MSB_REG 4
#define HMC5883L_Y_LSB_REG 5
#define DATA_REG_SIZE 6

// status register
#define HMC5883L_STATUS_REG 0x09

// ID registers
#define HMC5883L_ID_A_REG 0x0A
#define HMC5883L_ID_B_REG 0x0B
#define HMC5883L_ID_C_REG 0x0C



int running = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT){
        printf("closing nicely\n");
        running = -1;
    }
}

int main()
{
    float direction = 0;
    uint8_t rx_tx_buf[MAX_BUFFER_LENGTH];

    //! [Interesting]
    mraa::I2c* i2c;
    i2c = new mraa::I2c(1);

    signal(SIGINT, sig_handler);

    while (running == 0) {
        i2c->address(BME280_I2C_ADDR);
        i2c->writeByte(BME_WHOAMI_REG);

        i2c->address(BME280_I2C_ADDR);
        i2c->read(rx_tx_buf, 1);
        
        printf("Who Am I Reg = %d", rx_tx_buf[0]);
        //sleep(1);
    }
    delete i2c;

    return MRAA_SUCCESS;
}
