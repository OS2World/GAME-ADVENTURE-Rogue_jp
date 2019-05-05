#
# Makefile for JROGUE, with OS2/EMX
#
#	MSDOS or UNIX   as targte system...
#	CURSES		use local Curses
#	JAPAN		for Japaneze Message
#	COLOR		for Color Ext. support
#	TOPSCO		for Top Score  support
#

CC = gcc -c -g
CFLAGS= -Zmtd -O2 -DOS2EMX -DCURSES -DJAPAN -DUSE_COLOR -DTOPSCO
LD = gcc
LDFLAGS= -s -Zmtd

.c.o :
	$(CC) $(CFLAGS) $*.c

ROBJS = main.o hit.o init.o invent.o level.o \
	message.o monster.o move.o object.o pack.o play.o \
	random.o ring.o room.o save.o score.o spechit.o throw.o \
	trap.o use.o zap.o os2cur.o os2sys.o os2kbd.o

COBJS = 

TARGET = rogue.exe

rogue.exe : $(ROBJS) $(COBJS) rogue.def
	$(LD) $(LDFLAGS) -o rogue.exe rogue.def $(ROBJS) $(COBJS)

hit.o : hit.c rogue.h

init.o : init.c rogue.h

invent.o : invent.c rogue.h

level.o : level.c rogue.h

machdep.o : machdep.c rogue.h

main.o : main.c rogue.h

message.o : message.c rogue.h

monster.o : monster.c rogue.h

move.o : move.c rogue.h

object.o : object.c rogue.h

pack.o : pack.c rogue.h

play.o : play.c rogue.h

random.o : random.c rogue.h

ring.o : ring.c rogue.h

room.o : room.c rogue.h

save.o : save.c rogue.h

score.o : score.c rogue.h

spechit.o : spechit.c rogue.h

throw.o : throw.c rogue.h

trap.o : trap.c rogue.h

use.o : use.c rogue.h

zap.o : zap.c rogue.h

os2cur.o : os2cur.c rogue.h

os2sys.o : os2sys.c rogue.h

os2kbd.o : os2kbd.c rogue.h
