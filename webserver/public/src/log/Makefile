include ../../scripts/Makefile

module_list += . log
cflags := $(patsubst %, -I../../include/%/, $(module_list))
libpath :=
dynamic_libs +=
static_libs +=
target := liblog.a
target_dir := ../../lib

$(target) : $(object_list)
	-@$(remove) $(target); \
	ar crs $(target) $(object_list)
#copy 目标到指定文件
	-@$(copy) $(target) $(target_dir)

$(object_list) : %.o : %.$(source_suffix)
	$(compile) $(cxxflags) $(cflags) -c $< -o $@ $(libpath) $(dynamic_libs) -Wl,-Bstatic $(static_libs) -Wl,-Bdynamic

$(deps_list) : %.d : %.$(source_suffix)
	$(remove) $@; \
	$(compile) -MM $(cxxflags) $(cflags) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(remove) $@.$$$$

include $(deps_list)

clean:
	-@$(remove) $(target) $(object_list) $(target_dir)/$(target) $(deps_list)
