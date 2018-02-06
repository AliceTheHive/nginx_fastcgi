target := demo.cgi

src_list := demo
src_objects := $(patsubst %,%.o, ${src_list})

${target} : ${src_objects}
	g++ -I../include/ -L../lib/ ${src_objects} -Wl,-Bstatic -lfcgi -Wl,-Bdynamic -o ${target}

${src_objects} : %.o : %.cpp
	g++ -g -c -Wall -O2 $< -o $@

.PHONY : clean
	-@/bin/rm -rf ${src_objects} ${target}
