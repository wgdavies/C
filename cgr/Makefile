#
# IMPORTANT: This makefile is not generic !!! it compiles only on 64 bith arch
#
TARGET := pmond

OBJS := pmond.o flog.o event_proc.o

CC := $(CROSS_COMPILE)gcc

DEPENDENCIES := libcgroup

SYSROOT := $(shell $(CC) --print-sysroot)
PKGCONFIG := env PKG_CONFIG_SYSROOT_DIR=$(SYSROOT) PKG_CONFIG_LIBDIR=$(SYSROOT)/usr/lib64/pkgconfig pkg-config

CFLAGS := $(shell $(PKGCONFIG) --cflags $(DEPENDENCIES))

LDFLAGS := $(shell $(PKGCONFIG) --libs $(DEPENDENCIES))
LDFLAGS += -lproc -lcgroup

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(OBJS) *.log