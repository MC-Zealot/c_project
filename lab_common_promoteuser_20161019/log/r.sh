grep "input" lab_common.log|grep "fanstargetidx" >x
/data1/php/bin/php check.php|awk '{print $3}'|sort |uniq -c
