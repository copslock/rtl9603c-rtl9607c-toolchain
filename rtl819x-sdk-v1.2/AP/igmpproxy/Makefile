CC=mips-uclibc-gcc
#CFLAGS=-I../../linux-2.4.18/include -O2
CFLAGS=-I../../linux-2.4.18/include -Os
EXEC = igmpproxy
OBJS = mroute-api.o lib.o ifvc.o syslog.o timeout.o main.o

ifeq ($(RTL8186_EC),1)
CFLAGS+=-DCONFIG_RTL8186_EC
endif
ifeq ($(RTL8186_TR),1)
CFLAGS+=-DCONFIG_RTL8186_TR
endif
ifeq ($(RTL8196C_EC),1)
CFLAGS+=-DCONFIG_RTL8196C_EC
endif
ifeq ($(RTL865X_AC),1)
CFLAGS+=-DCONFIG_RTL865X_AC
endif
ifeq ($(RTL865X_KLD),1)
CFLAGS+=-DCONFIG_RTL865X_KLD
endif

all: BUILT_TIME $(EXEC)


$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(INCLUDE) -o $@ $(OBJS) $(LDLIBS$(LDLIBS-$(@)))
	mips-uclibc-strip igmpproxy

romfs:
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.elf *.o *.bak built_time

BUILT_TIME:
	@echo  \#define BUILT_TIME \"`TZ=UTC date -u "+%Y.%m.%d-%H:%M%z" `\" > ./built_time	