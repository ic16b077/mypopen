##
## @file Makefile
## Betriebsysteme mypopen Makefile
## Beispiel 2
##
## @author Aleksandar Pavlovic, ic16b056
## @author Michael Käfer ic16b077
## @author Johanna Hoffmann ic16b083
## @date 2017/04/31
##
## @version $Revision: 1.0 $
##
## Last Modified: $Author: ic16b056 $
##

##
## ------------------------------------------------------------- variables --
##

CC=gcc52
CFLAGS=-DDEBUG -Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -O3 -g -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS=mypopen.o

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o : %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

all: popentest testpipe

popentest: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -lpopentest
		
testpipe: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ -ltest-pipe

.PHONY: clean

clean:
	$(RM) *.o popentest testpipe

doc: html pdf

html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf; \
	$(MV) refman.tex refman_save.tex; \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex; \
	$(RM) refman_save.tex; \
	make; \
	$(MV) refman.pdf refman.save; \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile; \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

##
## =================================================================== eof ==
##
