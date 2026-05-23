TARGET= $(wildcard *.c)
FLAGS=-Wall -pedantic 
LIBS=-lX11
OUTPUT=nwm

build: $(MAIN)
	$(CC) -o $(OUTPUT) $(TARGET) $(LIBS) $(FLAGS)


run:
	xinit ./xinitrc -- /usr/bin/Xephyr :100 -ac -screen 1100x700 -host-cursor

clean:
	rm -rf $(OUTPUT)
