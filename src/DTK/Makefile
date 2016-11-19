DSO_TARGET = libdtk.so

SRCS := $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cc) 

LDFLAGS = -shared -fPIC -D_REENTRANT -Wl,-Bsymbolic -Wl,-rpath=./

TOP_DIR := ../../make

include $(TOP_DIR)/MK.config

