## Process this file with automake to produce Makefile.in

# compile for debugging ?
if DEBUG
    OPT_CFLAGS = -O0 -g
else
    OPT_CFLAGS = -O2
endif


bin_PROGRAMS = galadecode

galadecode_SOURCES = \
	galadecode.c

galadecode_CFLAGS = \
	-Wall -Wextra -Werror -Wno-unused-parameter \
	$(OPT_CFLAGS)

galadecode_LDADD = 

