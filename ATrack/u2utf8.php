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

function utf8Encode($source) {
	$utf8Str = $utf8val='';
	$entityArray = explode('&#', $source);
	$size = count($entityArray);
	for($i = 0; $i < $size; $i++) {
		$utf8Substring='';
		$subStr = $entityArray[$i];
		$nonEntity = strstr($subStr, ';');
		if($nonEntity !== false) {
			$unicode = intval(substr($subStr, 0, strpos($subStr, ';') + 1));
			// determine how many chars are needed to reprsent this unicode char
			$utf8Substring .=u2utf8($unicode);
#			for ($y = 0;$y < strlen($utf8val);$y += 3)
#				$utf8Substring .= chr(substr($utf8val, $y, 3));

			if(strlen ($nonEntity) > 1)
				$nonEntity = substr ($nonEntity, 1); // chop the first char (';')
			else
				$nonEntity = '';

			$utf8Str .= $utf8Substring . $nonEntity;
		} else {
			$utf8Str .= $subStr;
		}
	}

	return $utf8Str;
}

?>