<?
include_once("u2utf8.php");
function gb2utf8($gb,$RefCode=1) {
	if (!trim($gb))
		return $gb;
	$filename = "CP936.TXT";
	$tmp = file($filename);
	$codetable = array();
	while (list($key, $value) = each($tmp)) {
		list($ansikey,$ukey)=explode("\t",$value);
		$codetable[hexdec($ansikey)] = $ukey;
	}

	$ret = "";
	$utf8 = "";
	while ($gb) {
		if (ord(substr($gb, 0, 1)) > 127) {
			if (ord(substr($gb, 0, 1)) == 128) {
				$this = substr($gb, 0, 1);
				$gb = substr($gb, 1, strlen($gb));
			}else{
				$this = substr($gb, 0, 2);
				$gb = substr($gb, 2, strlen($gb));
			}
			if (!$RefCode) {
				$ret .= u2utf8(hexdec($codetable[hexdec(bin2hex($this))]));
			}else{
				$ret .= "&#".hexdec($codetable[hexdec(bin2hex($this))]).";";
			}
		} else {
			$ret .= substr($gb, 0, 1);
			$gb = substr($gb, 1, strlen($gb));
		}
	}
	return $ret;
}
?>
