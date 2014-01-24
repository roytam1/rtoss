<?php

$handle1 = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file 1.');
$fsize1 = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle1, $fsize1); 
$byteArray1 = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);

//print_r($byteArray);

$fp1=fopen(@$_SERVER['argv'][1].'.low','wb') or die('Cannot write file 1');
$fp2=fopen(@$_SERVER['argv'][1].'.high','wb') or die('Cannot write file 2');

for ($i=1; $i<=$fsize1; $i++)
{
	fwrite($i%2?$fp1:$fp2,chr($byteArray1[$i]));
}

fclose($fp1);
fclose($fp2);
