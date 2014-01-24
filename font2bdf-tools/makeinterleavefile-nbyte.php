<?php

$handle1 = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file 1.');
$fsize1 = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle1, $fsize1); 
$byteArray1 = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);


$n = (@$_SERVER['argv'][2] ? intval(@$_SERVER['argv'][2]) : 2);
//print_r($byteArray);

$fp1=fopen(@$_SERVER['argv'][1].'.low'.$n,'wb') or die('Cannot write file 1');
$fp2=fopen(@$_SERVER['argv'][1].'.high'.$n,'wb') or die('Cannot write file 2');

$ncnt=0;
for ($i=1; $i<=$fsize1; $i++)
{
	$ncnt=intval(($i-1)/$n);
	fwrite($ncnt%$n?$fp1:$fp2,chr($byteArray1[$i]));
}

fclose($fp1);
fclose($fp2);
