
#/bin/sh

echo "install ad rpm"
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/ad_log4cpp-4.0.9-1.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/libevent-2.0.21-2.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/protobuf-1.0.0-4.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/boost-1_45-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/uuid-1.5.1-3.el5.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/uuid-devel-1.5.1-3.el5.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/ad_json-1.0.0-5.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/monitor-client-1.0.1-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/zeromq-2.1.9-2.x86_64.rpm 
rpm -ivh http://10.210.208.36/rpms-ad/zeromq-dev-2.1.9-1.x86_64.rpm --force
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/bidfeed-sharelib-2.0.2-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/spio_api_cpp-1.1.0-4.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/nspio-1.1.1-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/spio_proxy-1.0.0-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/vadecommon-4.4.1-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/cpm-test/aes-1.0.0-2.x86_64.rpm
echo  "/home/w/lib64" >> /etc/ld.so.conf
echo "/usr/local/lib"  >> /etc/ld.so.conf
ldconfig


cd depend/tools/

echo "setup curl"
tar -zxvf curl-7.19.0.tar.gz
cd curl-7.19.0
./configure && make && make install
cd -
rm curl-7.19.0 -rf
echo "finish setup curl"

echo "setup hiredis"
unzip hiredis-master.zip
cd hiredis-master
make && make install
cd -
rm hiredis-master -rf
echo "finish setup hiredis"

echo "setup json-c"
rm /usr/local/include/json -rf
rm /usr/local/include/json-c -rf
unzip json-c-master.zip
cd json-c-master
./autogen.sh && ./configure && make && make install
cd -
rm json-c-master -rf
echo "finish setup json-c"

echo "setup libevent"
tar -zxvf libevent-2.0.10-stable.tar.gz
cd libevent-2.0.10-stable
./configure && make && make install
cd -
rm libevent-2.0.10-stable -rf
echo "finish setup libevent"

echo "setup memcached"
tar -zxvf memcached-1.4.5.tar.gz
cd memcached-1.4.5
./configure && make && make install
cd -
rm memcached-1.4.5 -rf
echo "finish setup memcached"

echo "setup libmemcached"
tar -zxvf libmemcached-0.49.tar.gz
cd libmemcached-0.49
./configure --with-memcached && make && make install
cd -
rm libmemcached-0.49 -rf
echo "finish setup libmemcached"
