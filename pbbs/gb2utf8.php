<?php
include_once("./u2utf8.php");
function gb2utf8($gb,$RefCode=1) {
	if (!trim($gb))
		return $gb;

	$codepagefile = "CP936.TXT";
	$codepagesld = "CP936.SLD";
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
	while ($gb) {
		if (ord(substr($gb, 0, 1)) > 127) {
			if (ord(substr($gb, 0, 1)) == 128) {
				$curr = substr($gb, 0, 1);
				$gb = substr($gb, 1, strlen($gb));
			}else{
				$curr = substr($gb, 0, 2);
				$gb = substr($gb, 2, strlen($gb));
			}
			$char=hexdec(bin2hex($curr));
			$char=isset($codetable[$char])?$codetable[$char]:'20';
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($char));
			}else{
				$ret .= "&#".hexdec($char).";";
			}
		} else {
			$ret .= substr($gb, 0, 1);
			$gb = substr($gb, 1, strlen($gb));
		}
	}
	return $ret;
}
?>
