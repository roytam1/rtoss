<?php
include_once("./u2utf8.php");
function big52utf8($big,$RefCode=1) {
	if (!trim($big))
		return $big;

	$codepagefile = "CP950.TXT";
	$codepagesld = "CP950.SLD";
	if(file_exists($codepagesld)) {
		$codetable=unserialize(implode('',file($codepagesld)));
	} else {
		$tmp = file($codepagefile);
		$codetable = array();
		while (list($key, $value) = each($tmp)) {
			list($ansikey,$ukey)=explode("\t",trim($value));
			$codetable[hexdec($ansikey)] = $ukey;
		}
		unset($tmp);

		$rp = fopen($codepagesld, "w");
		@fputs($rp,serialize($codetable));
		fclose($rp);
		chmod($codepagesld,0666);
	}

	$ret = "";
	while ($big) {
		if (ord(substr($big, 0, 1)) > 127) {
			$curr = substr($big, 0, 2);
			$big = substr($big, 2, strlen($big));
			$char=hexdec(bin2hex($curr));
			$char=isset($codetable[$char])?$codetable[$char]:'20';
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($char));
			}else{
				$ret .= "&#".hexdec($char).";";
			}
		} else {
			$ret .= substr($big, 0, 1);
			$big = substr($big, 1, strlen($big));
		}
	}
	return $ret;
}
?>
