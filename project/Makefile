CC	= gcc
CFLAGS	= -fcilkplus -O2 -ftree-vectorize -fopt-info-vec-optimized -lrt
RM	= rm -f

default: all

all: prefix r_prefix

prefix: prefix.c
	$(CC) $(CFLAGS) -o prefix prefix.c

r_prefix: r_prefix.c
	$(CC) $(CFLAGS) -o r_prefix r_prefix.c

clean veryclean:
	$(RM) prefix r_prefix

test:
	bash test.sh
