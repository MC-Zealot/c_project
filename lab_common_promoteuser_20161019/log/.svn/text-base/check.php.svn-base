<?php
$fp = fopen("x1", "r");
while(($buff = fgets($fp, 4096))!=false){
	//echo $buff;
        $arr = explode("output:", $buff);
        $a = strchr($arr[1], "{");
        $js = json_decode($a);

	//print_r($js->result);
	$b = $js->result;
	if (count($b) >= 1){
		//echo $b[0]->alg_log;
		$c = $b[0]->alg_log;
		$arr_1 = explode(";", $c);
		echo $arr_1[1];
		echo "\n";
	}
	/*	
        foreach($js->body->ad_req as $r){
                print $brr[2]."\t";
                print "\t".$js->body->access_info->uid;
                print "\t".$r->type."\n";
        }*/
	//break;

}
fclose($fp);
?>
