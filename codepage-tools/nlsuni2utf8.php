<?php
function u2utf8($c) {
	$ret = $str = '';
	if ($c < 0x80) {
		$str .= $c;
	} else if ($c < 0x800) {
		$str .= (0xC0 | $c >> 6);
		$str .= (0x80 | $c & 0x3F);
	} else if ($c < 0x10000) {
		$str .= (0xE0 | $c >> 12);
		$str .= (0x80 | $c >> 6 & 0x3F);
		$str .= (0x80 | $c & 0x3F);
	} else if ($c < 0x200000) {
		$str .= (0xF0 | $c >> 18);
		$str .= (0x80 | $c >> 12 & 0x3F);
		$str .= (0x80 | $c >> 6 & 0x3F);
		$str .= (0x80 | $c & 0x3F);
	}
	for ($i = 0;$i < strlen($str);$i += 3) $ret .= chr(substr($str, $i, 3));
	return $ret;
}

	$filename = @$_SERVER['argv'][1];
	$tmp = file($filename) or die('file cannot be opened.');
	$codetable = array();
	while (list($key, $value) = each($tmp)) {
		list($ansikey,$ukey)=explode("\t",$value);
		$u = rtrim($ukey);
		if($u) {
			$u = u2utf8(hexdec($u));
			echo "$ansikey\t$u\n";
		}
	}
