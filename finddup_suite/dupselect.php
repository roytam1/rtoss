<?php
// Settings
$excludes_order = array('detail');
$includes_order = array('/\d{5,8}(_big)?(_p\d+)?\.','/waren','/kaberen','/moeren','/kabeura','/moeura');
$deselects = array('this-one-needs-duplicate');
$normal_depth = isset($_GET['depth']) ? intval($_GET['depth']) : 2;

if(!isset($_GET['file'])) select_file();
elseif(isset($_POST['generate'])) generate_new_list();
elseif(isset($_GET['calctotal'])) calc_total();
else dupselect_ui();

function proper_size($size,$maxcnt=0) {
	$suffix=''; $suxAry=array("KB","MB","GB","TB");
	$ccnt=$maxcnt?$maxcnt:count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return $suffix?sprintf("%.1f",$size).$suffix:$size.'B';
}

function countchar($str,$chr) {
	return (strlen($str) - strlen(str_replace($chr,'',$str))) / strlen($chr);
}

function mysort($a, $b) { // row(md5,size,path) $a,$b
	global $excludes_order,$includes_order,$normal_depth;
	// basic sort first
	if($a['size'] != $b['size']) return $a['size'] > $b['size'] ? -1 : 1;
	if($a['md5'] != $b['md5']) return strnatcmp($a['md5'], $b['md5']);
	
//	$a_pathdepth = countchar($a['path'],'/');
//	$b_pathdepth = countchar($b['path'],'/');
	
	// deeper path first
	if(($a['depth'] + $b['depth']) > $normal_depth*2) {
		if($a['depth'] == $b['depth']) return strnatcmp($a['path'], $b['path']);
		return $a['depth'] > $b['depth'] ? -1 : 1;
	}
	
	// exclude
	foreach($excludes_order as $excl) {
		$a_exclude = !!strstr($a['path'],$excl);
		$b_exclude = !!strstr($b['path'],$excl);
		if($a_exclude + $b_exclude) {
			if($a_exclude == $b_exclude) return strnatcmp(basename($a['path']), basename($b['path']));
			else return $a_exclude ? 1 : -1;
		}
	}
	
	// include
	foreach($includes_order as $incl) {
		$a_include = preg_match('|'.$incl.'|i',$a['path']);
		$b_include = preg_match('|'.$incl.'|i',$b['path']);
		if($a_include + $b_include) {
			if($a_include == $b_include) return strnatcmp(basename($a['path']), basename($b['path']));
			else return $a_include ? -1 : 1;
		}
	}
	
	// last: strnatcmp
	return strnatcmp($a['path'], $b['path']);
}

function read_and_sort($file, &$ary,$names_only=false) {
	$ary = array('files'=>array(),'names'=>array());
	
	$f = file(basename($file));
	foreach($f as $line) {
		$file = array();
		list($file['md5'],$file['size'],$file['path']) = explode("\t",rtrim($line));
		if(!$names_only) {
			$file['depth'] = countchar($file['path'],'/');
			$ary['files'][] = $file;
		}
		$ary['names'][$file['path']] = array('md5'=>$file['md5'],'size'=>$file['size']);
	}
	if(!$names_only) usort($ary['files'],'mysort');
}

function hdr() {
//	header('Content-Type: text/html; charset=big5');
	echo '<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=big5"/>
<title>Dup Selector</title>
<style>a:active, a:hover {color:#f00}</style>
</head>
<body>';
}

function select_file() {
	$fs = glob('*.txt');

	hdr();
	foreach ($fs as $f)
		echo $f.' <a href="'.$_SERVER['PHP_SELF'].'?file='.$f.'">DupSelect</a><a href="'.$_SERVER['PHP_SELF'].'?file='.$f.'&depth=3">(3)</a> <a href="'.$_SERVER['PHP_SELF'].'?file='.$f.'&calctotal=1">CalcTotal</a><br/>';
	echo '</body></html>';
}

function generate_new_list() {
	$my_ary = array();

	$fp = fopen(substr(basename($_GET['file']),0,-4).'-delete.lst','wb+');
	fwrite($fp,implode("\n",$_POST['files']));
	fclose($fp);

	read_and_sort($_GET['file'],$my_ary,true);
	$fp = fopen(substr(basename($_GET['file']),0,-4).'-delete.txt','wb+');
	foreach($_POST['files'] as $f)
		fwrite($fp,$my_ary['names'][$f]['md5']."\t".$my_ary['names'][$f]['size']."\t$f\n");
	fclose($fp);

	hdr();
	echo '<pre>';
	print_r($_POST['files']);
	echo '</pre></body></html>';
}

function calc_total() {
	$my_ary = array();
	read_and_sort($_GET['file'],$my_ary,true);
	$fsize = 0;

	hdr();
	echo '<pre>';
	echo "           Size  MD5       Filename\n";
	echo "--------------------------------------------------------------\n";
	foreach($my_ary['names'] as $f=>$g) {
		$fsize += $g['size'];
		echo str_pad(number_format($g['size'],0,'.',','),15,' ',STR_PAD_LEFT).'  '.$g['md5']."  $f\n";
	}
	echo 'Total: '.proper_size($fsize).' ('.number_format($fsize,0,'.',',').')<br/>';
	echo '</pre></body></html>';
}
function dupselect_ui() {
	global $deselects,$normal_depth;
	$my_ary = array();
	read_and_sort($_GET['file'],$my_ary);
	$perv_md5='';$perv_depth=0; $twostar=false; $deselect=false;

	hdr();
	echo '<form action="'.$_SERVER['PHP_SELF'].'?file='.$_GET['file'].'" method="post">'."\n";
	echo '<input type="submit" name="generate" value="generate"/>'."\n";
	foreach($my_ary['files'] as $f) {
		if($perv_md5 != $f['md5']) {
			$twostar=false;
			$deselect=true;
			$perv_depth=0;
			echo "<hr/>\n";
		} else {
			$deselect=false;
		}
		foreach($deselects as $d) {
			if(strstr($f['path'],$d)) {
				$deselect=true;
				break;
			}
		}
		
		echo '<label><input type="checkbox" name="files[]" value="'.$f['path'].'"'.(!$deselect ? ' checked=checked':'').'/>'.$f['path'].' ('.$f['md5'].($perv_md5 != $f['md5'] && $f['depth']>$normal_depth?') <b>(*)</b>':($perv_md5 == $f['md5'] && $twostar==false && $perv_depth>$normal_depth && $f['depth']>$normal_depth?') <b>(**)</b>':')'))."</label><br/>\n";
		if($perv_depth>$normal_depth && $f['depth']>$normal_depth) $twostar=true;
		$perv_md5 = $f['md5']; $perv_depth = $f['depth'];
	}
	echo '</form></body></html>';
}