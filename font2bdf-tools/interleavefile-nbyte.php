<?php

$handle1 = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file 1.');
$fsize1 = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle1, $fsize1); 
$byteArray1 = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);

$handle2 = fopen(@$_SERVER['argv'][2],'rb') or die('Cannot open file 2.');
$fsize2 = filesize(@$_SERVER['argv'][2]);
$contents = fread($handle2, $fsize2);
$byteArray2 = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//print_r($byteArray);

$nbytes = intval(@$_SERVER['argv'][3]);
if (!$nbytes) $nbytes=1;


if($fsize1 != $fsize2) die('Cannot interleave files in different size.');
$fp=fopen(@$_SERVER['argv'][1].'.interleave','wb') or die('Cannot write file');

for ($i=1; $i<=$fsize1; $i++)
{
	for($j=0;$j<$nbytes;$j++) {
		fwrite($fp,chr($byteArray1[$i+$j]));
	}
	for($j=0;$j<$nbytes;$j++) {
		fwrite($fp,chr($byteArray2[$i+$j]));
	}
	$i+=$nbytes-1;
}

fclose($fp);
