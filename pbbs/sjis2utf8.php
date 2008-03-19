<?php
include_once("./u2utf8.php");
function sjis2utf8($sjis,$RefCode=1) {
	if (!trim($sjis))
		return $sjis;

	$codepagefile = "CP932.TXT";
	$codepagesld = "CP932.SLD";
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
	$utf8 = "";
	while ($sjis) {
		if (ord(substr($sjis, 0, 1)) > 127) {
			if (ord(substr($sjis, 0, 1)) >= 161 && ord(substr($sjis, 0, 1)) <= 223) {
				$curr = substr($sjis, 0, 1);
				$sjis = substr($sjis, 1, strlen($sjis));
			}else{
				$curr = substr($sjis, 0, 2);
				$sjis = substr($sjis, 2, strlen($sjis));
			}
			$char=hexdec(bin2hex($curr));
			$char=isset($codetable[$char])?$codetable[$char]:'20';
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($char));
			}else{
				$ret .= "&#".hexdec($char).";";
			}
		} else {
			$ret .= substr($sjis, 0, 1);
			$sjis = substr($sjis, 1, strlen($sjis));
		}
	}
	return $ret;
}
?>
