DSO_TARGET = libdtk.so

SRCS := $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cc) 

LDFLAGS = -shared -fPIC -D_REENTRANT -Wl,-Bsymbolic -Wl,-rpath=./

CFLAGS = -lpthread -liconv

TOP_DIR := ../../make

include $(TOP_DIR)/MK.config

