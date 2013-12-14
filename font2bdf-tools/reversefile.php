<?php

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.'); 
$fsize = filesize(@$_SERVER['argv'][1]); 
$contents = fread($handle, $fsize); 
$byteArray = unpack("C*",$contents);// $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//print_r($byteArray);

$fp=fopen(@$_SERVER['argv'][1].'.rev','wb') or die('Cannot write file');
for ($char=$fsize; $char>0; $char--)
{
	fwrite($fp,chr($byteArray[$char]));
}

fclose($fp);
