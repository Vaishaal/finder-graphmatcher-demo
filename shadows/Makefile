CC = g++
CFLAGS = -Wall

all: shadow_main shadow_calib

shadow_main:
	$(CC) -o shadow_main shadow_main.cpp shadow_util.cpp shadow.cpp

shadow_calib:
	$(CC) -o shadow_calib shadow_calib.cpp

clean:
	rm -rf *.o
	rm -rf shadow_calib
	rm -rf shadow_main
