<?php
// 
// 簡単RSS配信(手抜きだよ
// 
require_once "./conf.php";
require_once "./bb2html.php";
define('_RSS_MAX_' , 15); // 何件まで表示する？
define('FULL_PATH' , "http://2-cat.twbbs.org/~2cat/0d/"); // httpから始まるフルパス

//mb_http_output("UTF-8");

$arg = array();

if (!empty($_SERVER['PATH_INFO'])) {
	$path = explode("/", $_SERVER['PATH_INFO']);
	$id = $path[1];
} else {
	$id = $_GET['thread_id'];
} 
if (!is_numeric($id) && isset($id)) {
	error("不正なIDですよ。うん、きっと。", "", "", gethostbyaddr($_SERVER["REMOTE_ADDR"]), "");
} 

$arg['about'] = FULL_PATH . "backend.php";

function main()
{
	global $arg, $ddir, $ext, $subj_file, $tit, $tmes;
	$lines = file($subj_file);

	$ch_link = FULL_PATH;
	$rss .= <<<RSS_HEAD
<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet href="rss2.xsl" type="text/xsl" media="screen"?>
<rss version="2.0">
 <channel>
   <title>$tit</title>
   <link>$ch_link</link>
   <description>$tmes</description>
   <language>ja</language>\n
RSS_HEAD;

	for($i = 0;$i < _RSS_MAX_ ; $i++) {
		if (empty($lines[$i])) {
			break;
		} 
		list($dat, $title) = explode(",", $lines[$i]);
		$id = substr($dat, 0, strrpos($dat, "."));
		$link = FULL_PATH . "read.php/" . $id . "/l50";
		if (!file_exists($ddir . $id . $ext)) {
			break;
		} 
		$line_dat = file($ddir . $id . $ext);
		list(, , , $desc,) = explode(",", $line_dat[0]);
		$pubdate = date("r", filemtime($ddir . $id . $ext));

		$title = trim($title);
		$desc = strip_tags(bb2html($desc));

		$rss .= <<<ITEM
   <item>
	<title>$title</title>
	<link>$link</link>
	<description>$desc</description>
	<pubDate>$pubdate</pubDate>
   </item>\n
ITEM;
	} 
	$rss .= <<<RSS_FOOT
  </channel>
</rss>
RSS_FOOT;

	header ('Content-Type: text/xml; charset=utf-8');
//	print(mb_convert_encoding($rss, "UTF-8", "SJIS"));
	echo $rss;
} 

function thread($id)
{
	global $arg, $id, $ddir, $ext;
	
	if (!file_exists($ddir . $id . $ext)) {
		error("Thread Not Found !!");
	} 

	$lines = file($ddir . $id . $ext);
	$size = count($lines);
	list(, , , $des, $title) = explode(",", $lines[0]);
	$lines = array_reverse($lines);

	$ch_title = trim($title);
	$ch_link = FULL_PATH . "read.php/" . $id . "/l50";
	$ch_desc = strip_tags(bb2html($des));

	$rss .= <<<RSS_HEAD
<?xml version="1.0" encoding="UTF-8" ?>
<?xml-stylesheet href="rss2.xsl" type="text/xsl" media="screen"?>
<rss version="2.0">
 <channel>
   <title>$ch_title</title>
   <link>$ch_link</link>
   <description>$ch_desc</description>
   <language>ja</language>\n
RSS_HEAD;

	for($i = 0 ;$i < _RSS_MAX_ ; $i++) {
		if ($lines[$i] == "") {
			break;
		} 
		list($name, $email, $date, $com) = explode(",", $lines[$i]);
		$com = bb2html($com);
		$com = strip_tags($com);
		$name = strip_tags($name);

		$no = $size - $i ;

		if ($date != $abone && $date != $st_date) {
			$pos = strpos($date, "ID:");
			if($pos !== false) $date = substr($date, 0, $pos);
			if(version_compare("5.0.0", PHP_VERSION)<1) $date = preg_replace('/\(.*\)/', '', $date);
			
			$date = strtotime(trim($date));
			$pubdate = date("r", $date);
		} else {
			$pubdate = date("r");
		} 

		$title = $name;
		$link = FULL_PATH . "read.php/" . $id . "/" . $no . "n";
		$desc = $com;

		$rss .= <<<ITEM
   <item>
	<title>$title</title>
	<link>$link</link>
	<description>$desc</description>
	<pubDate>$pubdate</pubDate>
   </item>\n
ITEM;
	} 

	$rss .= <<<RSS_FOOT
  </channel>
</rss>
RSS_FOOT;

	header ('Content-Type: text/xml; charset=utf-8');
	echo $rss;
//	print(mb_convert_encoding($rss, "UTF-8", "SJIS"));
} 

if ($id > 0) {
	thread($id);
} else {
	main();
} 

?>