compile := g++
remove := /bin/rm -rf
copy := install
cflags += -I../include/
cxxflags += -g -W -Wall -Wshadow -Wpointer-arith -Wcast-qual -Waggregate-return -Winline -Wunreachable-code -Wunreachable-code -Wundef -Wredundant-decls -Wcast-align

ifeq ($(debug), )
override cxxflags += -O2
endif

target := libnetwork.a
target_dir := ./

source_suffix := cc
source_dirs := ../src/
source_list := $(foreach dir, $(source_dirs), $(wildcard $(dir)/*.$(source_suffix)))
object_list := $(patsubst %.$(source_suffix), %.o, $(source_list))
deps_list := $(patsubst %.$(source_suffix), %.d, $(source_list))

$(object_list) : %.o : %.$(source_suffix)
	$(compile) -c $(cflags) $(cxxflags) $< -o $@

$(target) : $(object_list)
	$(compile) $(object_list) -o $(target)
#copy二进制文件到指定目录
	-@$(copy) $(target) $(target_dir)

$(deps_list) : %.d : %.$(source_suffix)
	$(remove) $@; \
	$(compile) -MM $(cxxflags) $< > $@.$$$$; \
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; \
	$(remove) $@.$$$$

include $(deps_list)

.PHONY : clean

clean:
	-@$(remove) $(object_list) $(target) $(target_dir)/$(target) $(deps_list)
