<?php
$f=file_get_contents(@$_SERVER['argv'][1]) or die("Cannot open file.");
$f=explode("\n",$f);

$c=count($f);
$a=array();
for($i=0;$i<$c;$i++){
	if(trim($f[$i])) {
		$k=substr($f[$i],0,5);
		if(!isset($a[$k])){
			echo $f[$i]."\n";
			$a[$k]=1;
		}
	}
}