<?php
function properSize($size,$maxcnt=0) {
	$suffix=''; $suxAry=array("KB","MB","GB","TB");
	$ccnt=$maxcnt?$maxcnt:count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return sprintf("%.1f",$size).$suffix;
}

function fullURL() {
	$filename=preg_replace('/.*\/+/',"",$_SERVER['PHP_SELF']);
	$path=preg_replace("/$filename$/","",$_SERVER['PHP_SELF']);
	return "http://".$_SERVER["HTTP_HOST"].$path;
}

function _c_sort($a,$b) {
	global $files;
	if($files[$a]<$files[$b]) return 1;
	elseif($files[$a]>$files[$b]) return -1;
	else return 0;
}

$url=fullURL();

require_once("./name.inc.php");
$files=array();

clearstatcache();
if ($dh = opendir('./')) {
	while (($file = readdir($dh)) !== false) {
		$files[$file]=@filemtime($file);
	}
	closedir($dh);
}
uksort($files,"_c_sort");
header("Content-type: application/xml");
echo '<?xml version="1.0" encoding="utf-8"?>
<?xml-stylesheet href="rss2.xsl" type="text/xsl" media="screen"?>

<rss version="2.0">

<channel>
  <title>Firefox Musume Nightly Builds</title>
  <link>'.$url.'</link>
  <description>Firefox musume is a customized and optimized version of Mozilla Firefox.</description>
  <ttl>720</ttl>
';
foreach($files as $file => $props) {
	foreach($ary as $name => $regexp) {
		if(preg_match($regexp,$file,$opts)) {
			echo "<item><title>".htmlspecialchars(str_replace("%s",isset($opts[1])?$opts[1]:'',$name))."</title><link>".htmlspecialchars($url.$file)."</link><description>".htmlspecialchars("$name".(isset($opts[1])?"($opts[1])":''))."</description><pubDate>".date("r",$props)."</pubDate></item>";
			break;
		}
	}
}

echo "</channel>
</rss>";

?>
