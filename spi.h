/*
 *  SPII2C - A program for a SPI to I2C converter device
 *  Copyright (C) 2026  Corey Minyard <corey@minyard.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef SPI_H
#define SPI_H

#include "list.h"

#define SPI_MSG_LEN 32

#define ACP_MSG_ID_INVALID 0xff

struct spi_tx_msg {
    struct dlist_link link;

    unsigned char buf[SPI_MSG_LEN];
    void (*done)(struct spi_tx_msg *msg);
};

void spi_init(void);

void *spiThread(void *arg0);

int spi_send(struct spi_tx_msg *msg);

extern void (*spi_recv_msg_handler)(unsigned char *msg);

extern bool spi_trace;

#endif /* SPI_H */
