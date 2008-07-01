<?php

$dir='files/';
$datecache='./files.date';
$feedcache='./rss.xml';

function properSize($size,$maxcnt=0) {
	$suffix=''; $suxAry=array('KB','MB','GB','TB');
	$ccnt=$maxcnt?$maxcnt:count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return sprintf('%.1f',$size).$suffix;
}

function fullURL() {
	$filename=preg_replace('/.*\/+/','',$_SERVER['PHP_SELF']);
	$path=preg_replace("/$filename$/",'',$_SERVER['PHP_SELF']);
	return 'http://'.$_SERVER['HTTP_HOST'].$path;
}

function _c_sort($a,$b) {
	global $files;
	if($files[$a]<$files[$b]) return 1;
	elseif($files[$a]>$files[$b]) return -1;
	else return 0;
}

function writeFile($file,$data) {
	$rp = fopen($file, 'w');
	flock($rp, LOCK_EX); // 鎖定檔案
	@fputs($rp,$data);
	flock($rp, LOCK_UN); // 解鎖
	fclose($rp);
	chmod($file,0666);
}

$url=fullURL();

$filesdate=date('YmdHis', filemtime($dir));
$cachedate=@file_get_contents($datecache);
if($filesdate<=$cachedate && file_exists($feedcache)) {
	header('Content-type: application/xml');
	readfile($feedcache);
	exit;
}

require_once('./name.inc.php');
$files=array();

clearstatcache();
if ($dh = opendir('./'.$dir)) {
	while (($file = readdir($dh)) !== false) {
		$files[$file]=@filemtime($dir.$file);
	}
	closedir($dh);
}
uksort($files,'_c_sort');
$dat='<?xml version="1.0" encoding="utf-8"?>
<?xml-stylesheet href="rss2.xsl" type="text/xsl" media="screen"?>

<rss version="2.0">

<channel>
  <title>yourname File Releases</title>
  <link>'.$url.'</link>
  <description>Latest files of yourname</description>
  <ttl>720</ttl>
';
foreach($files as $file => $props) {
	foreach($ary as $name => $regexp) {
		if(preg_match($regexp,$file,$opts)) {
			$dat.="  <item>\n    <title>".htmlspecialchars(str_replace('%s',isset($opts[1])?$opts[1]:'',$name))."</title>\n    <link>".htmlspecialchars($url.$dir.$file)."</link>\n    <description>".htmlspecialchars(str_replace('%s',isset($opts[1])?$opts[1]:'',$name))."</description>\n    <pubDate>".date('r',$props)."</pubDate>\n  </item>\n";
			break;
		}
	}
}

$dat.='</channel>
</rss>';

writeFile($feedcache,$dat);
writeFile($datecache,$filesdate);

header('Content-type: application/xml');
echo $dat;
?>
