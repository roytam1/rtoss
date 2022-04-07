<?
/*
Directory Listing Script - Version 2 (lighttpd edition)
====================================
Script Author: Ash Young <ash@evoluted.net>. www.evoluted.net
Layout: lighttpd team. www.lighttpd.net

REQUIREMENTS
============
This script requires PHP.

INSTRUCTIONS
============
1) Unzip all files
2) Edit this file, making sure everything is setup as required.
3) Upload to server
4) ??????
5) Profit!

CONFIGURATION
=============
Edit the variables in this section to make the script work as
you require.

Start Directory - To list the files contained within the current
directory enter '.', otherwise enter the path to the directory
you wish to list. The path must be relative to the current
directory.
*/
$startdir = '.';

/*
Show Thumbnails? - Set to true if you wish to use the
scripts auto-thumbnail generation capabilities.
This requires that GD2 is installed.
*/
$showthumbnails = false;

/*
Show Directories - Do you want to make subdirectories available?
If not set this to false
*/
$showdirs = true;

/*
Force downloads - Do you want to force people to download the files
rather than viewing them in their browser?
*/
$forcedownloads = false;

/*
Hide Files - If you wish to hide certain files or directories
then enter their details here. The values entered are matched
against the file/directory names. If any part of the name
matches what is entered below then it is now shown.
*/
$hide = array(
				'index.php',
				'Thumbs',
				'.htaccess',
				'.htpasswd'
			);

/*
Show index files - if an index file is found in a directory
to you want to display that rather than the listing output
from this script?
*/
$displayindex = true;

/*
Show MIME types? (not available in win32)
It may slow when there is lots of files.
*/
$showmime = true;

/*
Index files - The follow array contains all the index files
that will be used if $displayindex (above) is set to true.
Feel free to add, delete or alter these
*/

$indexfiles = array (
				'index.html',
				'index.htm',
				'index.php',
				'default.htm',
				'default.html'
			);

/*
That's it! You are now ready to upload this script to the server.

Only edit what is below this line if you are sure that you know what you
are doing!
*/
error_reporting(0);

// cgi.fix_pathinfo Workaround
if(isset($_SERVER['ORIG_PATH_INFO'])) $_SERVER['PATH_INFO'] = $_SERVER['ORIG_PATH_INFO'];

// Abyss Web Server Workaround
if($_SERVER['SCRIPT_URL'] != $_SERVER['SCRIPT_NAME']) $_SERVER['PATH_INFO'] = substr($_SERVER['SCRIPT_URL'],strlen($_SERVER['SCRIPT_NAME']));

// IIS Workaround
if(strpos($_SERVER['PATH_INFO'],$_SERVER['SCRIPT_NAME'])!==false) $_SERVER['PATH_INFO'] = substr($_SERVER['PATH_INFO'],strlen($_SERVER['SCRIPT_NAME']));

// No MIME Type in win32
$iswin32=(substr (PHP_OS, 0, 3)== 'WIN');
if($showmime && $iswin32) $showmime = false;

function properSize($size,$maxcnt=0) {
	$suffix=''; $suxAry=array("K","M","G","T");
	$ccnt=$maxcnt?$maxcnt:count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return $suffix?sprintf("%.1f",$size).$suffix:$size.'';
}

function my_pathinfo($path, $type='dirname') {
	global $iswin32;
	if(substr($path, -1, 1)=='/') $path.='x';
	$path_parts = pathinfo($path);
	if($iswin32 && $type=='dirname') $path_parts['dirname'] = str_replace('\\','/',$path_parts['dirname']);
	return $path_parts[$type];
}
function remove_qs($uri) {
	if(($pos = strpos($uri, '?'))!==false) return substr($uri,0,$pos);
	else return $uri;
}

$self=isset($_SERVER['REQUEST_URI'])&&!empty($_SERVER['REQUEST_URI'])?remove_qs($_SERVER['REQUEST_URI']):$_SERVER['PHP_SELF'];
$myname=preg_replace('/.*\/+/', '', $self);
$mypath=preg_replace("/$myname$/", '', $self);
$myfilespath='http://'.$_SERVER['HTTP_HOST'].$mypath.'dlf';

if(!function_exists('imagecreatetruecolor')) $showthumbnails = false;
$leadon = $startdir;
if($leadon=='.') $leadon = '';
if((substr($leadon, -1, 1)!='/') && $leadon!='') $leadon = $leadon . '/';
$startdir = $leadon;

$cdir='.'.my_pathinfo($self).(isset($_GET['dir'])?$_GET['dir']:substr($_SERVER['PATH_INFO'],1));
if($cdir=='./') $cdir='';

if($cdir) {
	//check this is okay.
	if(substr($cdir, -1, 1)!='/') $cdir = $cdir.'/';

	$dirok = true;
	$dirnames = explode('/', $cdir);
	$dotdotdir='/';
	for($di=1; $di<sizeof($dirnames); $di++) {
		if($di<(sizeof($dirnames)-2)) $dotdotdir .= $dirnames[$di].'/';
		if($dirnames[$di] == '..') $dirok = false;
	}

	if(substr($cdir, 0, 1)=='/') $dirok = false;
	if($dirok) $leadon = $leadon.$cdir;
}

if($_GET['download'] && $forcedownloads) {
	$file = str_replace('/', '', $_GET['download']);
	$file = str_replace('..', '', $file);

	if(file_exists($leadon . $file)) {
		header("Content-type: application/x-download");
		header("Content-Length: ".filesize($leadon.$file));
		header('Content-Disposition: attachment; filename="'.$file.'"');
		readfile($leadon.$file);
		die();
	}
}

$opendir = $leadon;
if(!$leadon) $opendir = '.';
if(!file_exists($opendir)) {
	$opendir = '.';
	$leadon = $startdir;
}

clearstatcache();
if ($handle = opendir($opendir)) {
	while (false !== ($file = readdir($handle))) {
		//first see if this file is required in the listing
		if ($file == "." || $file == "..")  continue;
		$discard = false;
		for($hi=0;$hi<sizeof($hide);$hi++) {
			if(strpos($file, $hide[$hi])!==false) $discard = true;
		}

		if($discard) continue;
		if (@filetype($leadon.$file) == "dir") {
			if(!$showdirs) continue;

			$n++;
			if($_GET['sort']=="date") $key = @filemtime($leadon.$file) . ".$n";
			else $key = $n;
			$dirs[$key] = $file;// . "/";
		} else {
			$n++;
			if($_GET['sort']=="date") $key = @filemtime($leadon.$file) . ".$n";
			elseif($_GET['sort']=="size") $key = @filesize($leadon.$file) . ".$n";
			else $key = $n;
			$files[$key] = $file;

			if($displayindex) {
				if(in_array(strtolower($file), $indexfiles)) {
					header('Location: http://'.$_SERVER['HTTP_HOST']."$mypath$leadon$file");
					die();
				}
			}
		}
	}
	closedir($handle);
}

//sort our files
if($_GET['sort']=="date") {
	@ksort($dirs, SORT_NUMERIC);
	@ksort($files, SORT_NUMERIC);
}
elseif($_GET['sort']=="size") {
	@natcasesort($dirs);
	@ksort($files, SORT_NUMERIC);
}
else {
	@natcasesort($dirs);
	@natcasesort($files);
}

//order correctly
if($_GET['order']=="desc" && $_GET['sort']!="size") {$dirs = @array_reverse($dirs);}
if($_GET['order']=="desc") {$files = @array_reverse($files);}
$dirs = @array_values($dirs); $files = @array_values($files);

$pathtitle=str_replace('//','/',dirname($_SERVER['PHP_SELF']).'/'.$leadon);
$pathtitle=str_replace('\\','',$pathtitle);
	$baseurl = $self.'?';
	$fileurl = 'sort=name&amp;order=asc';
	$sizeurl = 'sort=size&amp;order=asc';
	$dateurl = 'sort=date&amp;order=asc';

	switch ($_GET['sort']) {
		case 'name':
			if($_GET['order']=='asc') $fileurl = 'sort=name&amp;order=desc';
			break;
		case 'size':
			if($_GET['order']=='asc') $sizeurl = 'sort=size&amp;order=desc';
			break;
		case 'date':
			if($_GET['order']=='asc') $dateurl = 'sort=date&amp;order=desc';
			break;
		default:
			$fileurl = 'sort=name&amp;order=desc';
			break;
	}
	echo '<?xml version="1.0" encoding="iso-8859-1"?>
';?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN" "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">

<head>
<title>Index of <?=$pathtitle;?></title>
<style type="text/css">
a, a:active {text-decoration: none; color: blue;}
a:visited {color: #48468F;}
a:hover, a:focus {text-decoration: underline; color: red;}
body {background-color: #F5F5F5;}
h2 {margin-bottom: 12px;}
table {margin-left: 12px;}
th, td { font: 90% monospace; text-align: left;}
th { font-weight: bold; padding-right: 14px; padding-bottom: 3px;}
td {padding-right: 14px;}
td.s, th.s {text-align: right;}
div.list { background-color: white; border-top: 1px solid #646464; border-bottom: 1px solid #646464; padding-top: 10px; padding-bottom: 14px;}
div.foot { font: 90% monospace; color: #787878; padding-top: 4px;}
</style>
</head>
<body>
<h2>Index of <?=$pathtitle;?></h2>
<div class="list">
<table summary="Directory Listing" cellpadding="0" cellspacing="0">
<thead><tr><th class="n"><a href="<?=$baseurl.$fileurl;?>">Name</a></th><th class="m"><a href="<?=$baseurl.$dateurl;?>">Last Modified</a></th><th class="s"><a href="<?=$baseurl.$sizeurl;?>">Size</a></th><?php echo $showmime?'<th class="t">Type</th>':'';?></tr></thead>
<tbody>
	<?php
	if ($dirok) {
	?>
<tr><td class="n"><a href="../">Parent Directory</a>/</td><td class="m">&nbsp;</td><td class="s">- &nbsp;</td><?php echo $showmime?'<td class="t">Directory</td>':'';?></tr>
	<?php
	}
	$arsize = sizeof($dirs);
	for($i=0;$i<$arsize;$i++) {
	?>
<tr><td class="n"><a href="<?=$dirs[$i];?>/"><?=$dirs[$i];?></a>/</td><td class="m"><?=date ("Y-M-d H:i:s", filemtime($leadon.$dirs[$i]));?></td><td class="s">- &nbsp;</td><?php echo $showmime?'<td class="t">Directory</td>':'';?></tr>
	<?php
	}

	$arsize = sizeof($files);
	for($i=0;$i<$arsize;$i++) {

		$fileurl = $mypath.$leadon.$files[$i];
		if($forcedownloads) {
			$fileurl = $_SESSION['PHP_SELF'] . '?dir=' . urlencode($leadon) . '&amp;download=' . urlencode($files[$i]);
		}
	?>
<tr><td class="n"><a href="<?=$fileurl;?>"><?=$files[$i];?></a></td><td class="m"><?=date ("Y-M-d H:i:s", filemtime($leadon.$files[$i]));?></td><td class="s"><?=properSize(filesize($leadon.$files[$i]),2);?></td><?php echo $showmime?'<td class="t">'.(preg_match('/\.log$/',$files[$i]) ? 'text/plain' : mime_content_type($leadon.$files[$i])).'</td>':'';?></tr>
	<?
	}
	?></tbody>
</table>
</div>
<div class="foot"><?=$_SERVER["SERVER_SOFTWARE"] ?></div>
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-428449-13']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
</body>
</html>
