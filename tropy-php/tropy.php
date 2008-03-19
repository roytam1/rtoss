<?php
/*
 * tropy.php - PHP version of tropy.cgi
 *
 * # tropy.cgi - Version 20051105.
 * #
 * # Copyright (C) 2005 by Hiroshi Yuki.
 * # <hyuki@hyuki.com>
 * # http://www.hyuki.com/tropy/
 * #
 * # This program is free software; you can redistribute it and/or
 * # modify it under the same terms as Perl itself.
 */

/* Config section */

$thisurl_absolute = "http://".$_SERVER["HTTP_HOST"].preg_replace("/".preg_replace('/.*\/+/',"",$_SERVER['PHP_SELF'])."$/","",$_SERVER['PHP_SELF']); //Auto Setting
$site_name = 'Tropy';
$dir_html = 'html'; # Directory for Cached HTML files.
$dir_files = 'files'; # Directory for user-written text files and the index file.
$file_index = "$dir_files/index.txt";
$tropy_self = 'tropy.php';
/******************/
$sendmail = 0;
$adminmail = "";
# $adminmail = 'somebody@example.com';
$maxidlen = 8;
$id_pattern = "[0-9]{".$maxidlen."}";
$maxlength = 1000;
$maxlines = 20;
$maxcols = 80;
$maxrows = $maxlines;
$maxtitle = $maxcols - 3;
$maxpages = 10000;
$session_digest = md5(microtime().uniqid('Tropy',true));
$random_seed = hexdec(substr($session_digest, 0, 8));
$create_id = substr(sprintf("%0${maxidlen}u", $random_seed), 0, $maxidlen);
$time_visible_msec = "3000";
$time_jumpto_sec = "3";
$content_type = "Content-type: text/html; charset=utf-8\n\n";
$allow_rebuild = 1;
$allow_toc = 1;
$allow_html = 0;
$show_plink = 1;
$auto_link = 1;
$allow_edit = 1;
$return_url="../";
$form=array_merge($_GET,$_POST);
$allid=array();

main();
exit(0);

function main() {
	global $main_command,$form,$id_pattern,$random_seed,$allow_toc,$allow_rebuild,$show_plink,$allid;
	
	# Random init.
	srand($random_seed);
	rand(); rand(); rand();
	TIEHASH();

	foreach ($form as $key => $val) {
		if (preg_match("/^c$/",$key)) {
			$main_command = 'create';
			do_create();
			break;
		} elseif (preg_match("/^e(".$id_pattern.")$/",$key,$matches)) {
			$main_command = 'edit';
			do_edit($matches[1]);
			break;
		} elseif (preg_match("/^w(".$id_pattern.")$/",$key,$matches)) {
			$main_command = 'write';
			do_write($matches[1]);
			break;
		} elseif (preg_match("/^toc$/",$key)) {
			if ($allow_toc) {
				$main_command = 'toc';
				$show_plink=0;
				show_toc();
			}
			break;
		} elseif (preg_match("/^rebuild$/",$key)) {
			if ($allow_rebuild) {
				foreach ($allid as $id => $val) {
					if (preg_match("/^".$id_pattern."$/",$id)) {
						unlink_html($id);
						build_html($id);
					}
				}
			}
		}
	}
	if (! $main_command) {
		$main_command = 'random';
		do_random();
	}
}

function do_edit($id) {
	global $main_command,$allid,$show_plink;
	if ($main_command == 'create' || isset($allid[$id])) {
		$show_plink=0;
		$title = isset($allid[$id]) ? get_subject($id) : "New Page";
		print_header($id, $title);
		print_editform($id, @implode('',FETCH($id)));
		echo get_footer();
	} else {
		print_error("edit non id");
	}
}

function print_editform($id,$msg) {
	global $maxrows,$maxcols,$tropy_self;
	$mymsg = htmlspecialchars($msg);
	echo <<<EOD
  <form action="$tropy_self" method="post">
	<input type="hidden" name="w$id" value="1">
	<textarea cols="$maxcols" rows="$maxrows" name="mymsg">$mymsg</textarea><br><input type="submit" value="Write">
  </form>
EOD;
}

function do_write($id) {
	global $form,$maxlength,$maxlines,$maxpages,$allid;
	$msg = $form['mymsg'];
	$msg = preg_replace("/\n+$/","\n",$msg);
	$msglen = strlen($msg);
	$cr = $msglen -  strlen(str_replace("\n","",$msg));

	if ($maxpages < count($allid)) {
		DELETE_random_page();
	}

	if ($msglen > $maxlength) {
		print_header($id, "length > $maxlength");
		print_editform($id, $msg);
		echo get_footer();
	} elseif ($cr >= $maxlines) {
		print_header($id, "lines > $maxlines");
		print_editform($id, $msg);
		echo get_footer();
	} else {
		if ($form['mymsg']) {
			STORE($id,$msg);
			send_mail($id, get_subject($id), 'edit', $msg);
			unlink_html($id);
			build_html($id);
			print_jumpto($id);
		} else {
			send_mail($id, get_subject($id), 'delete', FETCH($id));
			DELETE($id);
			unlink_html($id);
			do_random();
		}
	}
}

function delete_random_page() {
	$id = get_random_id();
	DELETE($id);
	unlink_html($id);
}

function print_jumpto($id) {
	global $content_type,$time_jumpto_sec,$thisurl_absolute,$dir_html,$site_name;
	$meta = "<meta http-equiv=\"refresh\" content=\"$time_jumpto_sec; url=${thisurl_absolute}${dir_html}/${id}.html\">\n";
	$navi = "<div id=\"navi\"></div>";
	header($content_type);
	header("Cache-Control: no-cache, must-revalidate"); // HTTP/1.1
	echo get_header("$site_name...", "", 'black', 'white', $navi, $meta);
	echo get_footer();
}

function print_error($msg) {
	$msg = htmlspecialchars($msg);
	print_header(null, '?');
	echo "<h1>$msg</h1>";
	echo get_footer();
	exit(0);
}

function print_header($id,$title) {
	global $content_type,$form,$site_name,$allow_edit;
	list($color, $bgcolor) = get_style($id);
	$navi = get_navi($id, $allow_edit);
	header($content_type);
	echo get_header("$title - $site_name", $title, $color, $bgcolor, $navi, "");
}

function get_navi($id, $editable) {
	global $tropy_self,$allow_toc,$show_plink,$return_url,$dir_html;
	$mod = 17;
	$random_url = "$tropy_self";
	if ($id % $mod > 0) {
		for ($x=1;$x<$mod * 3;$x++) {
			$nextid = get_random_id();
			if (($id + 1) % $mod == $nextid % $mod) {
				# Found.
				$random_url = "$dir_html/$nextid.html";
				break;
			}
		}
	}
	$return = $return_url ? "<a href='$return_url' title='Return to Home'>Return</a>" : '';
	$toc = $allow_toc ? "<a href='$tropy_self?toc' title='Table of Contents'>TOC</a>" : '';
	$edit = $editable ? "<a href='$tropy_self?e$id' title='Edit this tropy'>Edit</a>" : '';
	$create = "<a href='$tropy_self?c' title='Create new tropy'>Create</a>";
	$random = "<a href='$random_url' title='Jump to random tropy'>Random</a>";
	$plink = $show_plink ? "<a href='$dir_html/$id.html' title='Permalink of this tropy'>Permalink</a>" : '';
	return "<p id='navi'>$return $toc $edit $create $random $plink</p>";
}

function do_random() {
	global $id_pattern,$main_command,$id_pattern;
	$random_id = get_random_id();
	if (preg_match("/^".$id_pattern."$/",$random_id)) {
		build_html($random_id);
		print_jumpto($random_id);
	} else {
		$main_command = 'create';
		do_create();
	}
}

function get_random_id() {
	global $allid;
	$id = array_keys($allid);
	$rid = array_rand($id);
	return isset($id[$rid])?$id[$rid]:null;
}

function unlink_html($id) {
	global $dir_html;
	@unlink("$dir_html/$id.html");
}

function build_html($id) {
	global $dir_html,$allow_html,$site_name,$allow_edit,$auto_link;
	list($color, $bgcolor) = get_style($id);
	$navi = get_navi($id, $allow_edit);

	# Check.
	if (file_exists("$dir_html/$id.html")) {
		return;
	}

	# Read.
	$fil=FETCH($id);
	$caption=array_shift($fil);
	$caption=rtrim(htmlspecialchars($caption));
	$content = implode('',$fil);
	if (!$allow_html) {
		$content = htmlspecialchars($content);
		$content = preg_replace("/(\n\n+)/","\n\n",$content);
		$content = preg_replace("/(\n\n)/","</p><p>",$content);
		$content = preg_replace("/(\n)/","<br />",$content);
	}
	if ($auto_link) $content = preg_replace("/(https?|ftp|news)(\:\/\/[[:alnum:]\+\$\;\?\.%,!#~*\/\:@&=_-]+)/i","<a href=\"$1$2\" target=\"_blank\">$1$2</a>",$content);

	# Write.
	if (!$wfil=fopen("$dir_html/$id.html","wb")) {
		print_error("build to $id");
	}
	fwrite($wfil,get_header("$caption - $site_name", $caption, $color, $bgcolor, $navi, ""));
	fwrite($wfil,"<p>$content</p>");
	fwrite($wfil,get_footer());
	fclose($wfil);
}

function show_toc() {
	global $dir_html,$site_name,$allow_edit,$auto_link,$allid;
	$content='';

	# Read.
	foreach ($allid as $id => $val) {
		$fil=FETCH($id);
		$caption=array_shift($fil);
		$caption=rtrim(htmlspecialchars($caption));
		$content.="<a href='$dir_html/$id.html'>$id - $caption</a><br />";
	}

	echo get_header("TOC - $site_name", "Table Of Contents", 'black', 'white', get_navi(null,0), '');
	echo "<p>$content</p>";
	echo get_footer();
}

function get_header($title, $caption, $color, $bgcolor, $navi, $meta) {
	global $time_visible_msec,$thisurl_absolute;
	return <<<EOD
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html lang="ja">
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<meta http-equiv="content-style-type" content="text/css">
$meta<base href="$thisurl_absolute">
<style type="text/css"><!--
body{font-family:Verdana,sans-serif;margin:2% 20% 10% 20%;color:$color;background-color:$bgcolor;}
input{font-family:Verdana,sans-serif;}
#navi{text-align:right;}
p{line-height:150%;}
a{color:gray;background-color:$bgcolor;text-decoration:none;}
a:hover{text-decoration:underline;color:white;background-color:gray;}
--></style>
<script type="text/javascript">
<!--
function onLoad(){
  document.getElementById("navi").style.visibility="hidden";
  setTimeout("onTimeout()", $time_visible_msec);
}
function onTimeout(){
  document.getElementById("navi").style.visibility="visible";
}
// -->
</script>
<title>$title</title>
</head>
<body onLoad="onLoad()">
$navi<h1>$caption</h1>
EOD;
}

function get_footer() {
	return "</body></html>\n";
}

function send_mail($pagename, $subject, $mode, $content) {
	global $sendmail,$adminmail,$allid;
	if(!$sendmail) return;
	$pages = count($allid);
	$header = <<<EOD
To: $adminmail
From: $adminmail
Subject: [Tropy/$mode] $subject
MIME-Version: 1.0
Content-type: text/plain; charset=UTF-8
Content-Transfer-Encoding: 8bit
EOD;

	$message = <<<EOD
--------
MODE = $mode
REMOTE_ADDR = $_SERVER[REMOTE_ADDR]
REMOTE_HOST = $_SERVER[REMOTE_HOST]
--------
${thisurl_absolute}$pagename.html
--------
$content
--------
pages = $pages
--------
EOD;
	mail($adminmail,"[Tropy/$mode] $subject",$message,$header);
}

function get_style($id) {
	global $main_command;
	list($color, $bgcolor) = array('black', 'white');
	if ($main_command != 'create') {
		$digest = md5($id);
		$range = 32;
		$r = 255 - (hexdec(substr($digest, 0, 2)) % $range);
		$g = 255 - (hexdec(substr($digest, 2, 2)) % $range);
		$b = 255 - (hexdec(substr($digest, 4, 2)) % $range);
		$color = 'black';
		$bgcolor = sprintf("#%02X%02X%02X", $r, $g, $b);
	}
	return array($color, $bgcolor);
}

function do_create() {
	global $create_id;
	do_edit($create_id);
}

function get_subject($id) {
	global $maxtitle;
	$item=FETCH($id);
	if (preg_match("/^(.*)/",$item[0],$matches)) {
		$caption = $matches[1];
		if (strlen($caption) > $maxtitle) {
			$caption = preg_replace("/^(.{".$maxtitle."}).*/","$1...",$caption);
		}
		return htmlspecialchars($caption);
	} else {
		return "";
	}
}

function TIEHASH() {
	global $file_index,$allid;
	_make_index();
	$keys = array_map('rtrim',file($file_index));
	foreach($keys as $v) if($v) $allid[$v]=1;
}

function STORE($id, $value) {
	global $file_index,$dir_files,$id_pattern,$allid;
	if (!preg_match("/^(".$id_pattern.")$/",$id)) {
		die("store nonid");
	}

	# Replace file.
	$filename = "$dir_files/$id.txt";
	if (!$fil= fopen("$filename","wb")) {
		die("store $id file");
	}
	fwrite($fil,str_replace("\r\n","\n",$value));
	fclose($fil);

	# Append $id.
	if (!isset($allid[$id])) {
		if (!$idx= fopen("$file_index","a")) {
			die("store index");
		}
		fwrite($idx,"$id\n");
		fclose($idx);
		$allid[$id] = 1;
	}

	return $value;
}

function FETCH($key) {
	global $file_index,$dir_files;
	$filename = "$dir_files/$key.txt";
	return @file($filename);
}

function DELETE($key) {
	global $file_index,$dir_files,$allid;
	$filename = "$dir_files/$key.txt";
	unlink($filename);
	unset($allid[$key]);

	# Write index.
	$id = array_keys($allid);
		if (!$idx= fopen("$file_index","wb")) {
		die("delete index");
	}
	fwrite($idx,implode("\n", $id)."\n");
	fclose($idx);
}

function _make_index() {
	global $file_index,$dir_files,$id_pattern;
	if (file_exists($file_index)) {
		return;
	}
	if (!$di=opendir($dir_files)) {
		print_error("build index dir");
	}
	$index=array();
	$entry='';
	while ($entry = readdir($di)) {
		if (preg_match("/^(".$id_pattern.")\.txt$/",$entry,$matches)) {
			$id = $matches[1];
			if (-s("$dir_files/$id.txt") > 0) {
				$index[]=$id;
			}
		}
	}
	closedir($di);
	if (!$idx=fopen("$file_index","wb")) {
		print_error("build index open");
	}
	fwrite($idx,implode("\n", $index)."\n");
	fclose($idx);
}

?>