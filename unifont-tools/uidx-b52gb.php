<?php
$idx=file_get_contents('b5gb.idx');
$idx=explode("\n",$idx);

$f=file_get_contents(@$_SERVER['argv'][1]) or die("Cannot open file.");
$f=explode("\n",$f);

$c=count($f);
for($i=0;$i<$c;$i++){
		printf("%04X%s\n",hexdec($idx[$i])-0x8080,substr($f[$i],4));
}