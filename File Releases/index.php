<?php
function properSize($size,$maxcnt=0) {
	$suffix=''; $suxAry=array("KB","MB","GB","TB");
	$ccnt=$maxcnt?$maxcnt:count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return $suffix?sprintf("%.1f",$size).$suffix:$size.'B';
}

if(file_exists('./header.msg')) include('./header.msg');
require_once("./name.inc.php");

function _c_sort($a,$b) {
	global $ary;
	$a_re=false;$b_re=false;$i=0;
	foreach($ary as $nanme => $regexp){
		if($a_re!==false && $b_re!==false) break;
		if($a_re===false && preg_match($regexp,$a)) $a_re=$i;
		if($b_re===false && preg_match($regexp,$b)) $b_re=$i;
		$i++;
	}
	if($a_re<$b_re) return -1;
	elseif($a_re>$b_re) return 1;
	else return strnatcmp($a,$b);
}

$files=array();
$totalsize=0;

clearstatcache();
if ($dh = opendir('./')) {
	while (($file = readdir($dh)) !== false) {
		$files[$file]=array(@filesize($leadon.$file),@filemtime($leadon.$file));
	}
	closedir($dh);
}

//uksort($files,"_c_sort");
foreach($files as $file => $props) {
	foreach($ary as $name => $regexp) {
		if(preg_match($regexp,$file,$opts)) {
			echo "<div onmouseover='hoverThis(this)' onmouseout='unhoverThis(this)'><a href='$file'>".str_replace("%s",isset($opts[1])?$opts[1]:'',$name).(isset($opts[3])?" ($opts[3])":'')."</a> <samp>".properSize($props[0],1)."</samp> <kbd>".date("r",$props[1])."</kbd></div>";
			$totalsize+=$props[0];
			break;
		}
	}
}
if(!$totalsize) echo "<div style='text-align:center'>Currently no file is available.</div>";

exec("df|grep data",$df);
$df=preg_split("/\s+/",$df[0]);

echo "<div style='border-top:1px solid'> <samp>Total Size: ".properSize($totalsize,2)."</samp></div>";

if(file_exists('./footer.msg')) include('./footer.msg');

?>
