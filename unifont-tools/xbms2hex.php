<?php
function BitReverse($bData) {
	$lookup = array( 0, 8,  4, 12,
                     2, 10, 6, 14,
                     1, 9,  5, 13,
                     3, 11, 7, 15 );
	return (($lookup[($bData & 0x0F)]) << 4) + $lookup[(($bData & 0xF0) >> 4)];
}

function byterev(&$val){
	$val = sprintf("%02x",BitReverse(hexdec($val)));
} 

$g=glob('uni*.xbm');

foreach($g as $fn) {
	$fb=file_get_contents($fn);
	preg_match('/uni([0-9A-Fa-f]{4})/',$fn,$m);
	echo $m[1].":";
	preg_match_all('/0x([0-9A-Fa-f]{2})/',$fb,$m);
	array_walk($m[1],'byterev');
	echo implode('',$m[1])."\n";
}
