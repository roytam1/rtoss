<html><head>
<META HTTP-EQUIV="Content-Type" content="text/html; charset=utf-8">
<META HTTP-EQUIV="pragma" CONTENT="no-cache"><META HTTP-EQUIV="Cache-Control" CONTENT="no-cache"></head>
<style>a{ display:block;float:left;width:33%;line-height:1.5em; }</style>
<body><font size=2>
<?php
require_once "./conf.php";
$subject = @file($sub_back);

for($i = 0; $i < count($subject); $i++) {
	list($kdate, $title) = explode(",", $subject[$i]);
	$no = $i + 1;
	$res = chop($res);
	list($key, $ext) = explode(".", $kdate);
	$title=str_replace("\n","",$title);
	echo "<a href=\"./read.php/$key/l50\">$no: $title</a> \n";
} 

?>
</font></body></html>
