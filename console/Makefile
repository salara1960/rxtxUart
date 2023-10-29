
NAME=rxtxUtil
proc=main
func_lib=func
uart_lib=uart



CC=${CROSS_COMPILE}gcc
RM=rm
INC=${SYSROOT}/usr/include
EXTRA_CFLAGS += -Wall -D_GNU_SOURCE

ifeq ($(ARCH), arm)
    ##EXTRA_CFLAGS += -DSET_PRN_MUTEX
    LIB_DIR =
    UART_DEV_NAME = "/dev/ttySTM5"
    EXTRA_CFLAGS += -Os -static -mcpu=cortex-m7 -mthumb
    ## -mfloat-abi=hard -mfpu=fpv5-d16
else
    LIB_DIR = -L/usr/lib -lpthread -lrt -ldl
    UART_DEV_NAME = "/dev/ttyUSB0"
    EXTRA_CFLAGS += -O2
endif
export ${UART_DEV_NAME}
export ${EXTRA_CFLAGS}


$(NAME):  $(func_lib).o $(uart_lib).o $(proc).o
	$(CC) -o $(NAME) $(func_lib).o $(uart_lib).o $(proc).o $(LIB_DIR)
$(proc).o: $(proc).c
	$(CC) -c $(proc).c ${EXTRA_CFLAGS}
$(func_lib).o: $(func_lib).c
	$(CC) -c $(func_lib).c ${EXTRA_CFLAGS}
$(uart_lib).o: $(uart_lib).c
	$(CC) -c $(uart_lib).c ${EXTRA_CFLAGS}

clean:
	$(RM) -f *.o $(NAME) $(NAME).*
