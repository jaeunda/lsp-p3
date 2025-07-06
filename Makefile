CC = gcc
CFLAGS = -Wall -g
TARGET = ssu_ext2

SRCS = src/ext2_list.c src/ext2_print.c src/ext2_tree.c src/ext2_util.c src/ssu_ext2.c
OBJS = $(SRCS:.c=.o)
DEPS = ext2_command.h ext2_fs.h ext2_list.h ext2_print.h ext2_tree.h ext2_util.h ssu_ext2.h

all: $(TARGET) clean_obj

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean_obj:
	rm -f src/*.o

clean:
	rm -f $(TARGET)

