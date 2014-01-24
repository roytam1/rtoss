<?php
$unmap = 0xFFFF0000;
$mapdata=array();

$hex=file(@$_SERVER['argv'][1]) or die('Cannot open hex file');
$hexcnt=count($hex);

$mode=@$_SERVER['argv'][3]; // mode, 1=use line number to map column 2 in map file, 2=use map file column 1, 3=remap by original line number
if($mode<1 || $mode>3) $mode=1; // use line number

if($mode<3) {
	$map=file(@$_SERVER['argv'][2]) or die('Cannot open map file');
	$mapcnt=count($map);
}

if($mode==1){
	if($mapcnt < $hexcnt) die('Not enough lines in map file');
	for($i=0;$i<$hexcnt;$i++){
		list($hexcp,$hexdata) = explode(':',$hex[$i]);
		list($mapcp,$mapto) = explode("\t",$map[$i]);

		$mapto=rtrim($mapto);
		if($mapto) $mapto=hexdec($mapto);
		else $mapto=$unmap++;

		printf("%04X:%s",$mapto,$hexdata);
	}
} else if($mode==2) {
	for($i=0;$i<$mapcnt;$i++){
		list($mapcp,$mapto) = explode("\t",$map[$i]);
		$mapto=rtrim($mapto);
		if($mapto) $mapdata[hexdec($mapcp)]=hexdec($mapto);
	}
	for($i=0;$i<$hexcnt;$i++){
		list($hexcp,$hexdata) = explode(':',$hex[$i]);
		$hexcp=hexdec('0x'.$hexcp);
		if(isset($mapdata[$hexcp])) $mapto=$mapdata[$hexcp];
		else $mapto=$unmap+$hexcp;

		printf("%04X:%s",$mapto,$hexdata);
	}
} else if($mode==3) {
	for($i=0;$i<$hexcnt;$i++){
		list($hexcp,$hexdata) = explode(':',$hex[$i]);
		printf("%04X:%s",$i,$hexdata);
	}
}