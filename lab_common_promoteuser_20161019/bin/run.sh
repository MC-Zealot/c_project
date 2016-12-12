#export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
#export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=../lib:/home/w/lib64/:/usr/local/lib/:$LD_LIBRARY_PATH
dir=`pwd`
./saferun $dir/lab_common_svr ../conf/lab_common_svr.conf >> ../log/work_interface.log 2>&1 &
#$dir/lab_common_svr ../conf/lab_common_svr.conf >> ../log/work_interface.log 2>&1 &
#./lab_common_svr ../conf/lab_common_svr.conf
#ps aux|grep lab_common | grep wangli
