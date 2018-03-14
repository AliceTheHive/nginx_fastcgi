include ./scripts/Makefile

module_name += log config
module_dir := src
module_list := $(patsubst %, $(module_dir)/%, $(module_name))
source_list := $(foreach module, $(module_list), $(wildcard $(module)/*.$(source_suffix)))
object_list := $(patsubst %.$(source_suffix), %.o, $(source_list))
libpath :=
dynamic_libs +=
static_libs +=
target := libpublic.a
target_dir := ./lib

#定义进入一个子模块进行make的函数
#-C 进入目录命令
#$(1) 调用函数传进来的第一个参数
modules_make = $(MAKE) -C $(1);
modules_clean = $(MAKE) clean -C $(1);

$(target) : mm
	-@$(remove) $(target);
	ar crs $(target) $(object_list)
#copy 目标到指定文件
	-@$(copy) $(target) $(target_dir)
	-@echo $(target) make done!

.PHONY : mm mc clean

mm :
	@$(foreach module, $(module_list), $(call modules_make, $(module)))

mc :
	@$(foreach module, $(module_list), $(call modules_clean, $(module)))

clean : mc
	-@$(remove) $(target) $(target_dir)/$(target)
	-@echo clean done!
