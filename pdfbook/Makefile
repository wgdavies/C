# Simple makefile to build pdfbook executable and zipfile.

CFLAGS=-Wall -g -O2

TARGET=pdfbook

default: build

build: $(TARGET)

dist: $(TARGET).zip

$(TARGET): %: %.c

$(TARGET).zip: %.zip: %.c Makefile README
	TEMPDIR=`mktemp -d` CURDIR=`pwd` && ln -s $$CURDIR $$TEMPDIR/$(TARGET) && \
	( cd $$TEMPDIR && zip $$CURDIR/$@ $(addprefix $(TARGET)/,$^) ) && \
	rm -rf $$TEMPDIR

clean:
	rm -f $(TARGET) $(TARGET).zip

.PHONY: default build dist clean
