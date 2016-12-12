include Makefile.in

INC_PATH=-I./alg_tool/src/include/ -I./alg_tool/src/include/utils/  -I/home/w/include/ -I./depend/xbson/include -I./depend/feature_pool_bin/include/ -I./depend/uthash/ -I./depend/mapdb/include/ -I./depend/woo/include -I/usr/local/include/json -I/usr/local/include/hiredis/ -I./include/ -I./include/db/ -I./include/utility -I./include/work -I./include/algorithm  -I/home/w/include/aes/
LIB_LINK=-L./lib/ -L./depend/xbson/lib -L./depend/feature_pool_bin/lib/ -L./depend/mapdb/lib/ -L./depend/woo/lib -L/usr/local/lib -L/home/w/lib64 -lxbson -lmemcached -lapifp -lmapdb -lcurl -lssl -ljson-c -lwoo -lstdc++ -lz -lhiredis -lpthread  -laes  
ALL_BIN = ./bin/lab_common_svr ./bin/lab_common_main
all:$(ALL_BIN)

./bin/lab_common_svr : ./src/lab_common_svr.o ./src/ini_file.o ./src/encode_convert.o
	test -d ./bin || mkdir ./bin
	$(CC) $(CFLAGS) -g -o  $@ $^ $(LIB_LINK)

./bin/lab_common_main : ./src/lab_common_main.o ./src/ini_file.o ./src/encode_convert.o
	test -d ./bin || mkdir ./bin
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg

clean:
	rm $(ALL_BIN) ./src/*.o
