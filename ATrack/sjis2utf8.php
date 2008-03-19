<?
include_once("u2utf8.php");
function sjis2utf8($sjis,$RefCode=1) {
	if (!trim($sjis))
		return $sjis;
	$filename = "CP932.TXT";
	$tmp = file($filename);
	$codetable = array();
	while (list($key, $value) = each($tmp)) {
		list($ansikey,$ukey)=explode("\t",$value);
		$codetable[hexdec($ansikey)] = $ukey;
	}

	$ret = "";
	$utf8 = "";
	while ($sjis) {
		if (ord(substr($sjis, 0, 1)) > 127) {
			if (ord(substr($sjis, 0, 1)) >= 161 && ord(substr($sjis, 0, 1)) <= 223) {
				$this = substr($sjis, 0, 1);
				$sjis = substr($sjis, 1, strlen($sjis));
			}else{
				$this = substr($sjis, 0, 2);
				$sjis = substr($sjis, 2, strlen($sjis));
			}
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($codetable[hexdec(bin2hex($this))]));
			}else{
				$ret .= "&#".hexdec($codetable[hexdec(bin2hex($this))]).";";
			}
		} else {
			$ret .= substr($sjis, 0, 1);
			$sjis = substr($sjis, 1, strlen($sjis));
		}
	}
	return $ret;
}
?>
