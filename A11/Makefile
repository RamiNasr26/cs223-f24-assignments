CC=gcc
SOURCES=grep tree_tests dependency 
FILES := $(subst .c,,$(SOURCES))
FLAGS=-g -Wall -Wvla -Werror -Wno-unused-variable -Wno-unused-but-set-variable

# By default, make runs the first target in the file
all: $(FILES)

% :: %.c tree.c
	$(CC) $(FLAGS) $< tree.c -o $@ -lpthread -lreadline

clean:
	rm -rf $(FILES)

