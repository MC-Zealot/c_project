include Makefile.in

INC_PATH=-I./src/include/ -I./src/include/utils/ -I/home/w/include/ -I../depend/xbson/include -I../depend/feature_pool_bin/include/ -I../depend/uthash/ -I../depend/mapdb/include/ -I../depend/woo/include -I/usr/local/include/json -I/usr/local/include/hiredis/ -I../include/ -I../include/db/ -I../include/utility -I../include/work -I../include/algorithm  -I/home/w/include/aes/
LIB_LINK=-L../lib/ -L../depend/xbson/lib -L../depend/feature_pool_bin/lib/ -L../depend/mapdb/lib/ -L../depend/woo/lib -L/usr/local/lib  -L/usr/local/lib -L/home/w/lib64 -lxbson -lmemcached -lapifp -lmapdb -lcurl -lssl -ljson-c -lwoo -lstdc++ -lz -lhiredis -lpthread  -laes
#ALL_BIN=../lib/FansTargetAlgorithmInterface.so  #../lib/FansTopRefreshAlgorithmInterface.so ../lib/ScenceAlgorithmInterface.so ../lib/WaxAlgorithmInterface.so  ../lib/FansTargetAlgorithmInterface.so #../lib/GoodsAdAlgorithmInterface.so #../lib/InterAlgorithmInterface.so ../lib/UserRpmAlgorithmInterface.so ../lib/GoodsAdAlgorithmInterface.so
ALL_BIN= ../lib/PromoteFansAlgorithmInterface.so #../lib/FansTopRefreshAlgorithmInterface.so ../lib/FansTargetAlgorithmInterface.so  #../lib/FansTopRefreshAlgorithmInterface.so ../lib/ScenceAlgorithmInterface.so ../lib/WaxAlgorithmInterface.so  ../lib/FansTargetAlgorithmInterface.so #../lib/GoodsAdAlgorithmInterface.so #../lib/InterAlgorithmInterface.so ../lib/UserRpmAlgorithmInterface.so ../lib/GoodsAdAlgorithmInterface.so

all:$(ALL_BIN)

../lib/InterAlgorithmInterface.so : ./src/inter_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/UserRpmAlgorithmInterface.so : ./src/user_rpm_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/GoodsAdAlgorithmInterface.so : ./src/goods_ad_algorithm_interface.o ./src/ContAdvSim.o ./src/sim_rangking_plugin.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/WaxAlgorithmInterface.so : ./src/wax_ad_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/ScenceAlgorithmInterface.so : ./src/scence_ad_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/FansTargetAlgorithmInterface.so : ./src/fanstarget_ad_algorithm_interface.o src/fanstarget_ad_algorithm_high_quality.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/FansTopRefreshAlgorithmInterface.so : ./src/fanstop_refresh_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

../lib/PromoteFansAlgorithmInterface.so : ./src/promotefans_algorithm_interface.o
	test -d ../lib || mkdir ../lib
	$(CC) $(CFLAGS) -g -o $@ $^ $(LIB_LINK)

%.o:%.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg
%.o:%.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_PATH) -pg

clean:
	rm $(ALL_BIN) ./src/*.o
