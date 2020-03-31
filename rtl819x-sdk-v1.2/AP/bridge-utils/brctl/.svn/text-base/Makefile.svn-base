CFLAGS = -Wall -c -g -Os $(HARD_FLOAT)
LDFLAGS = -s -Wall -g 

ifeq ($(SLINK),1)
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += --static -Wl,--gc-sections
endif

ifeq ($(GUEST_ZONE),1)
CFLAGS  += -DGUEST_ZONE
endif

all:			brctl brctld

brctl:			brctl.o brctl_cmd.o brctl_disp.o ../libbridge/libbridge.a
			mips-uclibc-gcc $(LDFLAGS) -o brctl brctl.o brctl_cmd.o brctl_disp.o ../libbridge/libbridge.a

brctld:			brctld.o brctl_cmd.o brctl_disp.o ../libbridge/libbridge.a
			mips-uclibc-gcc $(LDFLAGS) -o brctld brctld.o brctl_cmd.o brctl_disp.o ../libbridge/libbridge.a

clean:
			rm -f *.o
			rm -f brctl
			rm -f brctld
			rm -f core

brctl.o:		brctl.c brctl.h
			mips-uclibc-gcc -s -I../libbridge $(CFLAGS) -o brctl.o brctl.c

brctld.o:		brctld.c brctl.h
			mips-uclibc-gcc -s -I../libbridge $(CFLAGS) -o brctld.o brctld.c

brctl_cmd.o:		brctl_cmd.c brctl.h
			mips-uclibc-gcc -s -I../libbridge $(CFLAGS) -o brctl_cmd.o brctl_cmd.c

brctl_disp.o:		brctl_disp.c brctl.h
			mips-uclibc-gcc -s -I../libbridge $(CFLAGS) -o brctl_disp.o brctl_disp.c
