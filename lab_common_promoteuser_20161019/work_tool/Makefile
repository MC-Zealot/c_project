include Makefile.in

INC_PATH=-I../alg_tool/src/include/ -I../alg_tool/src/include/utils/ -I/home/w/include/ -I../depend/xbson/include -I../depend/feature_pool_bin/include/ -I../depend/uthash/ -I../depend/mapdb/include/ -I../depend/woo/include -I/usr/local/include/json -I/usr/local/include/hiredis/ -I../include/ -I../include/db/ -I../include/utility -I../include/work -I../include/algorithm  -I/home/w/include/aes/
LIB_LINK=-L../depend/xbson/lib -L../depend/feature_pool_bin/lib/ -L../depend/mapdb/lib/ -L../depend/woo/lib -L/usr/local/lib -L/home/w/lib64/ -lxbson -lmemcached -lapifp -lmapdb -lcurl -lssl -ljson-c -lwoo -lstdc++ -lz -lhiredis -lpthread  -lcrypto
#ALL_BIN =  ../lib/GoodsAdWorkInterface.so ../lib/WaxWorkInterface.so
ALL_BIN = ../lib/PromoteFansWorkInterface.so #../lib/FansTopRefreshWorkInterface.so #../lib/FansTopRefreshWorkInterface.so ../lib/ScenceWorkInterface.so ../lib/WaxWorkInterface.so ../lib/NonFansWorkInterface.so ../lib/FansTargetWorkInterface.so ../lib/FansTargetIdxWorkInterface.so

all:$(ALL_BIN)

../lib/GoodsAdWorkInterface.so : ./src/goods_ad_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/WaxWorkInterface.so : ./src/wax_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/ScenceWorkInterface.so : ./src/scence_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/NonFansWorkInterface.so : ./src/nonfans_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/FansTargetWorkInterface.so : ./src/fanstarget_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/FansTargetIdxWorkInterface.so : ./src/fanstargetidx_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib                                                          
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/FansTopRefreshWorkInterface.so : ./src/fanstop_refresh_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/PromoteFansWorkInterface.so : ./src/promotefans_work_interface.o ../src/ini_file.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg

clean:
	rm $(ALL_BIN) ./src/*.o
