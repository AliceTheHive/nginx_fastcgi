compile := g++
remove := /bin/rm -rf
copy := install
cflags += -I../public/include -I../public/include/log -I../public/include/config -I./include -I../network/include -I../thread/include
cxxflags += -g -W -Wall

ifeq ($(debug), )
cxxflags += -O2
endif

ifeq ($(wall), 1)
cxxflags += -Wshadow -Wpointer-arith -Wcast-qual -Waggregate-return -Winline -Wundef -Wredundant-decls -Wcast-align
endif

libpath := -L../public/lib
dynamic_libs += -lpthread -lrt -ldl
static_libs += -lpublic
source_suffix := cc
source_dirs += ./src ../network/src ../thread/src
ifeq ($(target), tcpservice)
source_dirs += ./tcpserver
target := tcpservice
else
source_dirs += ./fastcgi
target := gateway.cgi
endif
target_dir := ./bin
source_list := $(foreach dir, $(source_dirs), $(wildcard $(dir)/*.$(source_suffix)))
object_list := $(patsubst %.$(source_suffix), %.o, $(source_list))
deps_list := $(patsubst %.$(source_suffix), %.d, $(source_list))

$(target) : $(object_list)
	$(compile) $(cflags) $(cxxflags) $(object_list) -o $(target) $(libpath) $(dynamic_libs) -Wl,-Bstatic $(static_libs) -Wl,-Bdynamic

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
