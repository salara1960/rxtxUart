#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <wchar.h>
#include <locale.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/resource.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <termios.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>
//#include <zlib.h>


//#define SET_ARM
//#define SET_PRN_MUTEX


#pragma once

enum {
    devOk = 0,
    devMem = 1,
    devTimeout = 2,
    devUart = 4
#ifdef SET_PRN_MUTEX
    ,devMutex = 8
#endif
};

#ifdef SET_PRN_MUTEX
    #define MAX_ERROR 5
#else
    #define MAX_ERROR 4
#endif


#ifdef SET_ARM
    #define UART_DEV_NAME "/dev/ttySTM5"
#else
    #define UART_DEV_NAME "/dev/ttyUSB0"
#endif

#define HTONS(x) \
    ((uint16_t)((x >> 8) | ((x << 8) & 0xff00)))
#define HTONL(x) \
    ((uint32_t)((x >> 24) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000)))



//-----------------------------------------------------------------

