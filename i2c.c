
#include "ti_drivers_config.h"
#include "spii2c.h"

static I2C_Handle i2c_handles[CONFIG_I2C_COUNT];

void
i2c_init(void)
{
    unsigned int i;
    I2C_Params i2cParams;

    I2C_init();

    for (i = 0; i < CONFIG_I2C_COUNT; i++) {
	I2C_Params_init(&i2cParams);
	i2cParams.bitRate = I2C_100kHz;
	i2c_handles[i] = I2C_open(i, &i2cParams);
	if (!i2c_handles[i]) {
	    while (1) {
	    }
	}
    }
}

bool
i2c_transaction(unsigned int i2cnum, unsigned int addr,
		void *tx, unsigned int tx_size,
		void *rx, unsigned int rx_size)
{
    I2C_Transaction t;
    int rv;

    t.targetAddress = addr;
    t.writeBuf = tx;
    t.writeCount = tx_size;
    t.readBuf = rx;
    t.readCount = rx_size;
    rv = I2C_transferTimeout(i2c_handles[i2cnum], &t, 200);
    if (rv != I2C_STATUS_SUCCESS) {
	printf("Error from I2C: %d\n", rv);
	return false;
    }
    return true;
}
