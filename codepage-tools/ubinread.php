<?php

$fp=fopen('unicode.bin','rb');
$b = fread($fp,4);
list($entries) = array_values(unpack('V',$b));
$cpinfo=array();

printf("entries = %d\n",$entries);
for($i=0; $i < $entries; $i++) {
$b=fread($fp,0x1C);
$chdr=unpack('Vcpid/Vcp2ucOff/Vcp2ucSiz/Vxx1/Vuc2cpOff/Vuc2cpSiz/Vxx2',$b);
$cpinfo[]=$chdr;
print_r($chdr);
}

for($x=0; $x < $entries; $x++) {
	$txtfp=fopen('cp'.$cpinfo[$x]['cpid'].'.txt','wb');
	fseek($fp,$cpinfo[$x]['cp2ucOff']);
	$b=fread($fp,$cpinfo[$x]['cp2ucSiz']);
	$cdat=unpack('v*',$b);
	//print_r($cdat);
	$icnt=count($cdat);
	for($i=23;$i <= $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",$i-23,$cdat[$i]);
	}
	fclose($txtfp);
}
