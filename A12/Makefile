CC=gcc
SOURCES=memstats unit_tests
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(FILES)

% :: %.c 
	$(CC) $(FLAGS) $< -o $@

memstats: memstats.c mylloc_list.c sbrk.c rand.c
	$(CC) -g -Wall -Wvla -Werror memstats.c mylloc_list.c sbrk.c rand.c -o $@ -lm

unit_tests: unit_tests.c mylloc_list.c sbrk.c rand.c
	$(CC) -g -Wall -Wvla -Werror unit_tests.c mylloc_list.c sbrk.c rand.c -o $@ -lm

clean:
	rm -rf $(FILES)

