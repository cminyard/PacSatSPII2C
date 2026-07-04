## PacSat SPI to I2C Converter

This code runs on a TI MSPML1228RGE processor and translates from a
SPI bus to three I2C busses.  Though it is designed for PacSat, it's
really a general purpose converter.  I looked around and couldn't find
a chip that could do this as well as doing it this way, I could find
SPI to single I2C chips, but they were larger than this chip and you
would have to have three of them.

This is a fairly standard TI Code Composer Studio (CCS) project using
FreeRTOS.

The pinout is described in the PacSatSPII2CSysconfig repository.  That
is another CCS project with sysconfig setup.  Since you really can't
use sysconfig with FreeRTOS, it can't be here, but it's extremely
helpful for configuring pinouts on these chips.

## Peripherals & Pin Assignments

| Peripheral	| Pin	| Name		| Function |
| ----------	| ---	| ----		| -------- |
| UART0			|  1	| PA1		| UART RX (input) |
| Core			|  2	| NRST		| Reset line |
| Power			|  3	| VDD/VBAT  | Power |
| Power			|  4	| VSS		| Ground |
| SPI0			|  5	| PA2		| Peripheral SPI Chip Select |
| GPIO			|  6	| PA3		| Interrupt out line to the host. |
| SPI0			|  7	| PA4		| Peripheral SPI POCI |
| SPI0			|  8	| PA9		| Peripheral SPI PICO |
| I2C0			|  9	| PA10		| SDA (data) line |
| SPI0			| 10	| PA11		| Peripheral SPI clock |
| I2C2			| 11	| PA15		| SCL (clock) line |
| GPIO			| 12	| PA16		| General GPIO. |
| I2C1			| 13	| PA17		| SCL (clock) line |
| I2C1			| 14	| PA18		| SDA (data) line |
| Debug			| 15	| SWDIO		| Debugger line |
| Debug			| 16	| SWCLK		| Debugger line |
| GPIO			| 17	| PA21		| General GPIO. |
| I2C0			| 18	| PA22		| SCL (clock) line |
| GPIO			| 19	| PA23		| General GPIO. |
| I2C2			| 20	| PA24		| SDA (data) line |
| GPIO			| 21	| PA25		| General GPIO. |
| GPIO			| 22	| PA26		| General GPIO. |
| Power			| 23	| VCore		| |
| UART0			| 24	| PA0		| UART TX (output) (needs external pullup) |

## Console

UART0 provides a basic console at 9600n81 that you can use to aid
debugging.  You can perform basic SPI and I2C operations.  See the
commands.c file for details, or type "help" on the console.

The TX line from the UART must have an external pullup on it.  There's
no internal pullup in the chip for this line.

## I2C[0-2]

The I2C lines are normal I2C.  They default to 100Kbps, but this can
be changed easily in the code.

## SPI

The device acts as a SPI peripheral.  It doesn't work like a normal
SPI device because it can't.  Instead it works as a message transfer
bus.  Messages can go both ways.  The main processor acts as the SPI
controller, so it runs the clock and starts all SPI transactions.

If the host wants to send an I2C message, it asserts the chip select
(CS) line and waits for this device to assert the interrupt out line.
Then it runs the SPI transaction and raises CS when done.  It must
then wait for the interrupt out line to be de-asserted before another
transaction can be run.

If this device wants to send a message to the host, it asserts the
interrupt out line.  That causes the host to run a SPI transaction in
the same way it normally does, except the interrupt out line is
already asserted so it just goes on after that.

Note that a SPI transaction always sends data both ways.  So messages
can go both directions at the same time if they are available on both
sides.

## GPIOS

A number of GPIOs are available.  These are used on the PacSat board
for various purposes (power control on the ADCs, general lines).

## Messages

Note that the I2C devices are numbered in the messages somewhat
strangely:

| I2C index | Device I2C Bus |
| --------- | -------------- |
| 0         | I2C0           |
| 1         | I2C2           |
| 2			| I2C1			 |

GPIOs have the following numbering scheme:

| GPIO Index | GPIO pin |
| ---------- | -------- |
| 0          | 12 (PA16) |
| 1          | 17 (PA21) |
| 2          | 19 (PA23) |
| 3          | 21 (PA25) |
| 4          | 22 (PA26) |

### DO\_I2C (host->peripheral)

Transmits 0 or more I2C bytes on the given bus and requests a number
of bytes read.  An I2C\_RESULT is always returned, even if the
requested number of bytes is 0.  Only one I2C transaction per bus is
allowed at a time.  If one is pending and you send another, the second
will be silently ignored.

|byte	|Name			|Description |
|----	|------------	|----------- |
|0		|DO\_I2C (1)	|  |
|1		|I2C bus		| The I2C bus to target (0-2) |
|2		|TX Count		| The number of bytes to send (0-27) |
|3		|RX Count		| The number of bytes to receive (0-27) |
|4		|I2C Address	| The address of the device to target |
|5-31	|Tx Data		| Data to transmit |

This will return an I2C\_RESULT message.

### I2C\_RESULT (peripheral->host)

Return the results of an I2C transaction.

|byte	|Name			|Description |
|----	|------------	|----------- |
|0		|I2C\_Result (2)|  |
|1		|I2C bus		| The I2C bus to target (0-2) |
|2		|Result			| 0 for success, non-zero for failure |
|3		|RX Count		| The number of bytes received (0-27) |
|4		|I2C Address	| Target address |
|5-31	|Rx Data		| Received data |

### SET\_GPIO (host->peripheral)

Set a GPIO pin value.

|byte	|Name			|Description |
|----	|------------	|----------- |
|0		|SET\_GPIO (3)	|  |
|1		|GPIO #			| Which GPIO to set |
|2		|Value			| The value to set the GPIO to |
|3-31	|unused			|  |

### GET\_GPIO (host->peripheral)

Return a GPIO pin value.

|byte	|Name			|Description |
|----	|------------	|----------- |
|0		|SET\_GPIO (4)	|  |
|1		|GPIO #			| Which GPIO to get |
|2-31	|unused			|  |

### GPIO\_VALUE (peripheral->host)

Return a GPIO pin value.

|byte	|Name			|Description |
|----	|------------	|----------- |
|0		|GPIO\_VALUE (5)|  |
|1		|GPIO #			| Which GPIO |
|2		|Value			| The value of the GPIO |
|3-31	|unused			|  |

