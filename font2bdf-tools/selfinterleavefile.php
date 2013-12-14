<?php
$order = @$_SERVER['argv'][2] == 'rev' ? 1 : 0;

$handle1 = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize1 = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle1, $fsize1);
$byteArray1 = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);

//print_r($byteArray);

if(($fsize1 % 2) !== 0) die('Cannot interleave files not in even size.');
$fp=fopen(@$_SERVER['argv'][1].'.selfinterleave','wb') or die('Cannot write file');

$middle=$fsize1/2;
if(!$order) { // normal, lower part is odd
	for ($i=1; $i<=$middle; $i++) {
		fwrite($fp,chr($byteArray1[$i]));
		fwrite($fp,chr($byteArray1[$middle+$i]));
	}
} else { // upper part is odd
	for ($i=1; $i<=$middle; $i++) {
		fwrite($fp,chr($byteArray1[$middle+$i]));
		fwrite($fp,chr($byteArray1[$i]));
	}
}

fclose($fp);
