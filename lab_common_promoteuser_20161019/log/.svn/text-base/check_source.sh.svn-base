#!/bin/sh

#获取广告主和各个来源
grep position lab_common.log |grep "UID"|grep ":582"|awk '{print $9"\t"$NF}'> x

#分类广告主的尾号
python trans.py

#统计广告主的source值
echo "==4":
awk '{print $2}' x_4 | sort | uniq -c
echo "!=4":
awk '{print $2}' x__4 | sort | uniq -c
