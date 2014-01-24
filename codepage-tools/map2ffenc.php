<?php

$cpl=file_get_contents($_SERVER['argv'][1]) or die('Unable to open file');
$cpl=explode("\n",$cpl);

$gl=explode("\n",file_get_contents('glyphlist.txt'));
$g=array();$c=array();$n='';

foreach($gl as $l){
	$l=trim($l);
	if($l != '' && $l{0} != '#') {
		$n=explode(';',$l);
		if(isset($n[1]))
			$g[$n[1]]=$n[0];
	}
}

foreach($cpl as $l){
	$l=ltrim($l);
	if($l != '' && $l{0} != '#') {
		$n=explode("\t",$l);
		if(isset($n[1])) {
			$t=trim(strtoupper(substr($n[1],2)));
			if(isset($g[$t]))
				$c[$n[0]]=$g[$t];
			else {
				if($t && $t != '0000') $c[$n[0]]='uni'.$t;
				else $c[$n[0]]='.notdef';
			}
		}
	}
}

$name = str_ireplace('.txt','',$_SERVER['argv'][1]);
echo "/$name {\n";
foreach($c as $k => $v){
	echo " /$v\n";
}
echo "}";
