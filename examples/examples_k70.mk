SOURCES = $(PROG).c ../../mongoose.c  fifo.c  jWrite.c   AcuteHttpClientApi.c   HttpClientApi.c  nxjson.c

INCLUDE=${INSTALL_ROOT}/A2F/root/usr/include
LIBS=${INSTALL_ROOT}/A2F/root/usr/lib  -pthread

CFLAGS = -g -W -Wall -Werror -I../.. -Wno-unused-function $(CFLAGS_EXTRA) $(MODULE_CFLAGS)  -pthread   -Wno-missing-field-initializers
CFLAGS+=-mcpu=cortex-m4 -mthumb  -I$(INCLUDE)  -L$(LIBS)

all: $(PROG)

#CC = gcc
CC=arm-uclinuxeabi-gcc

ifeq ($(SSL_LIB),openssl)
CFLAGS += -DMG_ENABLE_SSL  -lssl  -lcrypto  
endif
ifeq ($(SSL_LIB), krypton)
CFLAGS += -DMG_ENABLE_SSL ../../../krypton/krypton.c -I../../../krypton
endif
ifeq ($(SSL_LIB),mbedtls)
CFLAGS += -DMG_ENABLE_SSL -DMG_SSL_IF=MG_SSL_IF_MBEDTLS -DMG_SSL_MBED_DUMMY_RANDOM -lmbedcrypto -lmbedtls -lmbedx509
endif

ifeq ($(JS), yes)
	V7_PATH = ../../deps/v7
	CFLAGS_EXTRA += -DMG_ENABLE_JAVASCRIPT -I $(V7_PATH) $(V7_PATH)/v7.c
endif

$(PROG): $(SOURCES)
	$(CC) $(SOURCES) -o $@ $(CFLAGS)

$(PROG).exe: $(SOURCES)
	cl $(SOURCES) /I../.. /MD /Fe$@

clean:
	rm -rf *.gc* *.dSYM *.exe *.obj *.o a.out $(PROG)
