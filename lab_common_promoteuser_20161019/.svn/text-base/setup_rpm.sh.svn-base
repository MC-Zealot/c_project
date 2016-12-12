
#/bin/sh

echo "install ad rpm"
rpm -ivh http://10.210.208.36/rpms-ad/cpm-test/aes-1.0.0-2.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/thrift-fb303-1.0.1-0.x86_64.rpm
rpm -ivh http://10.210.208.36/rpms-ad/feed-test/libhbase-1.0.0-1.x86_64.rpm
echo  "/home/w/lib64" >> /etc/ld.so.conf
echo "/usr/local/lib"  >> /etc/ld.so.conf
ldconfig
