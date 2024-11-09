CC=gcc
SOURCES=thread_mandelbrot single_mandelbrot
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(FILES)

% :: %.c read_ppm.c write_ppm.c
	$(CC) $(FLAGS) $< read_ppm.c write_ppm.c -o $@ -lpthread

clean:
	rm -rf $(FILES)

