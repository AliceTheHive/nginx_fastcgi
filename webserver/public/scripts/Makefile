compile := g++
remove := /bin/rm -rf
copy := install
cxxflags += -g -Wall

ifeq ($(debug), )
cxxflags += -O2
endif

ifeq ($(wall), 1)
cxxflags += -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Waggregate-return -Winline -Wunreachable-code -Wcast-align -Wundef -Wredundant-decls
endif

source_suffix := cc
source_list := $(wildcard ./*.$(source_suffix))
object_list := $(patsubst %.$(source_suffix), %.o, $(source_list))
deps_list := $(patsubst %.$(source_suffix), %.d, $(source_list))

.PHONY : clean
