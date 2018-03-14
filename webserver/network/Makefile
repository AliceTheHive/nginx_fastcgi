compile := g++
remove := /bin/rm -rf
copy := install
cflags += -I../public/include -I../public/include/log -I./include
cxxflags += -g -W -Wall

ifeq ($(debug), )
cxxflags += -O2
endif

ifeq ($(wall), 1)
cxxflags += -Wshadow -Wpointer-arith -Wcast-qual -Waggregate-return -Winline -Wundef -Wredundant-decls -Wcast-align
endif

libpath := -L../public/lib
dynamic_libs +=
static_libs += -llog
target := libnetwork.a
target_dir := ./lib
source_suffix := cc
source_dirs := ./src
source_list := $(foreach dir, $(source_dirs), $(wildcard $(dir)/*.$(source_suffix)))
object_list := $(patsubst %.$(source_suffix), %.o, $(source_list))
deps_list := $(patsubst %.$(source_suffix), %.d, $(source_list))

$(target) : $(object_list)
	-@$(remove) $(target); \
	ar crs $@ $(object_list)

#copy二进制文件到指定目录
	$(copy) $(target) $(target_dir)

$(object_list) : %.o : %.$(source_suffix)
	$(compile) -c $(cflags) $(cxxflags) $< -o $@ $(libpath) $(dynamic_libs) -Wl,-Bstatic $(static_libs) -Wl,-Bdynamic

$(deps_list) : %.d : %.$(source_suffix)
	$(remove) $@; \
	$(compile) -MM $(cflags) $(cxxflags) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(remove) $@.$$$$

include $(deps_list)

.PHONY : clean

clean:
	-@$(remove) $(object_list) $(target) $(target_dir)/$(target) $(deps_list)
