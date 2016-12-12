l=`grep ";7;" lab_common.log |grep "output" |wc -l |awk '{print $1}'`
tm=`date +"%s"`
wget "http://trend.recom.i.weibo.com/v4/datachart/interface/add.php?key=adalgo_nf:exp_128&rank=$l&time=$tm" -o /tmp/a.out -O /tmp/b.out
