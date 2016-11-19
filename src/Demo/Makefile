APP_TARGET = AppDemo

SRCS := $(wildcard *.cpp) $(wildcard *.c) $(wildcard *.cc) 


#LDFLAGS = -shared -fPIC -D_REENTRANT -Wl,-Bsymbolic -Wl,-rpath=./

CFLAGS += -I../DTK

LDFLAGS += -L../DTK -ldtk 

TOP_DIR := ../../make

include $(TOP_DIR)/MK.config

