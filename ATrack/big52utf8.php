<?
include_once("u2utf8.php");
function big52utf8($big,$RefCode=1) {
	if (!trim($big))
		return $big;
	$filename = "CP950.TXT";
	$tmp = file($filename);
	$codetable = array();
	while (list($key, $value) = each($tmp)) {
		list($ansikey,$ukey)=explode("\t",$value);
		$codetable[hexdec($ansikey)] = $ukey;
	}

	$ret = "";
	$utf8 = "";
	while ($big) {
		if (ord(substr($big, 0, 1)) > 127) {
			$this = substr($big, 0, 2);
			$big = substr($big, 2, strlen($big));
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($codetable[hexdec(bin2hex($this))]));
			}else{
				$ret .= "&#".hexdec($codetable[hexdec(bin2hex($this))]).";";
			}
		} else {
			$ret .= substr($big, 0, 1);
			$big = substr($big, 1, strlen($big));
		}
	}
	return $ret;
}
?>
