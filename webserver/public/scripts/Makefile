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

define BUILD_LIBRARY
$(if $(wildcard $@),@$(remove) $@)
$(if $(wildcard ar.mac),@$(remove) ar.mac)
$(if $(filter %.a, $^),
@echo CREATE $@ > ar.mac
@echo SAVE >> ar.mac
@echo END >> ar.mac
@ar -M < ar.mac
)
$(if $(filter %.o,$^),@ar -q $@ $(filter %.o, $^))
$(if $(filter %.a, $^),
@echo OPEN $@ > ar.mac
$(foreach LIB, $(filter %.a, $^),
@echo ADDLIB $(LIB) >> ar.mac
)
@echo SAVE >> ar.mac
@echo END >> ar.mac
@ar -M < ar.mac
@$(remove) ar.mac
)
endef
