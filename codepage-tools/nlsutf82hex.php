<?php

function GetMaskIndex($n) {
	if( $n < 0xc0 ) return 1; // 00~10111111
	if( $n < 0xe0 ) return 2; // 110xxxxx
	if( $n < 0xf0 ) return 3; // 1110xxxx
	if( $n < 0xf8 ) return 4; // 11110xxx
	if( $n < 0xfc ) return 5; // 111110xx
	                return 6; // 1111110x
}
function utf8tohex($s) {
	$mask = array( 0, 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01 );
	$out=0;
	$length = strlen($s);
	$sint = ord($s{0});
	$utflen = GetMaskIndex($sint);

	$out = $sint & $mask[$utflen];
	for($x=$utflen;$x>1;$x--) {
		$out = $out << 6;
		$out |= ord($s{$length-$x+1}) & 0x3f;
	}
	return $out;
}

	$filename = @$_SERVER['argv'][1];
	$tmp = file($filename) or die('file cannot be opened.');
	$codetable = array();
	while (list($key, $value) = each($tmp)) {
		list($ansikey,$ukey)=explode("\t",$value);
		$u = rtrim($ukey);
		if($u) $u=sprintf('0x%04x',utf8tohex($u));
		echo "$ansikey\t$u\n";
	}
