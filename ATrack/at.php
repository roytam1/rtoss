<?php
define(TTF_LOCATION,"./SUNG14.TTF");
define(SECRET_WORD,"plm");
define(FONT_SIZE,9);
define(LOG_MAX,15);
include("big52utf8.php");
include("gb2utf8.php");
include("sjis2utf8.php");
include("tblless2utf8.php");
$doConv2UTF8=true;
$RefCode=false;

	extract($_GET);
	$atfile="at.log";
	$cntfile="at.count";
	$songcount=(int)(implode('',file($cntfile)));

	if (isset($length)&&$secret==SECRET_WORD) {
		$title=trim($title);
		if (get_magic_quotes_gpc()) {$title=stripslashes($title);$album=stripslashes($album);}

		$old_log = file($atfile);
		$new_log[0] = $album.",,".$title.",,".$length."\n";
		for($i=1; $i<LOG_MAX; $i++) {
			$new_log[$i] = $old_log[$i-1];
		}
		updatelog($new_log);
		updatecount();
		echo "update success!";

	}else{
		$outputtext=titleOut(0,0);
		
		if(!strlen($_SERVER['PATH_INFO'])) {
			if(isset($log)&&isset($js)) { outHeader("application/x-javascript"); echo jsOut(nl2br(showLog())); exit; }
			if(isset($log)&&isset($pic)) { imgOut(showLog()); exit; }
			if(isset($js)) { outHeader("application/x-javascript"); echo jsOut($outputtext[0]); exit; }
			if(isset($pic)) { imgOut($outputtext[0]); exit; }
			if(isset($log)) { outHeader("text/plain"); echo showLog(); exit; }
			if(!isset($log)&&!isset($js)&&!isset($pic)) { outHeader("text/plain"); echo $outputtext[0]; exit; }
		}else{
			if (strpos($_SERVER['PATH_INFO'],"/",1)) list(,$CP,)=explode("/",$_SERVER['PATH_INFO']);
			$outputtext=titleOut(0,0);

			if(strpos(strtolower($_SERVER['PATH_INFO']),"log.js")) { outHeader("application/x-javascript"); echo jsOut(nl2br(showLog())); exit; }
			if(strpos(strtolower($_SERVER['PATH_INFO']),"log.png")) { imgOut(showLog()); exit; }
			if(strpos(strtolower($_SERVER['PATH_INFO']),".js")) { outHeader("application/x-javascript"); echo jsOut($outputtext[0]); exit; }
			if(strpos(strtolower($_SERVER['PATH_INFO']),".png")) { imgOut($outputtext[0]); exit; }
			if(strpos(strtolower($_SERVER['PATH_INFO']),"log.txt")) { outHeader("text/plain"); echo showLog(); exit; }
			if(strpos(strtolower($_SERVER['PATH_INFO']),".txt")) { outHeader("text/plain"); echo $outputtext[0]; exit; }
			
		}
	}

function outHeader($mime) {
	if($mime) Header("Content-type: $mime");
	Header("Pragma: no-cache");
	Header('Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0');
	Header("Expires: Thu, 01 Dec 1994 16:00:00 GMT");
}

function jsOut($str) {
	return "document.write(\"".str_replace("\n",'',$str)."\");";
}

function showLog() {
	global $atfile;
	$arr = stat($atfile);
	return implode("\n",titleOut(0,LOG_MAX,1))."\n\nLast updated: ".strftime("%d %b, %Y %H:%M %Z", $arr[9]);;
}

function imgOut($str) {
#			Header("HTTP/1.0 404 Not Found");
			outHeader("image/png");

			$dim= imagettfbbox(FONT_SIZE, 0, TTF_LOCATION, $str);
			$min_x= min($dim[0], $dim[2], $dim[4], $dim[6]);
			$max_x= max($dim[0], $dim[2], $dim[4], $dim[6]);
			$width= $max_x-$min_x+1;
			$min_y= min($dim[1], $dim[3], $dim[5], $dim[7]);
			$max_y= max($dim[1], $dim[3], $dim[5], $dim[7]);
			$height= $max_y-$min_y+1;

			$img= imagecreate($width+1, $height+1);
			$white = ImageColorAllocate($img, 255,255,255);
			$twhite=imagecolortransparent($img,$white);
			$black = ImageColorAllocate($img, 0,0,0);
			ImageTTFText($img, FONT_SIZE, 0, -$min_x+$dim[0],-$min_y, $black, TTF_LOCATION, $str);

			ImagePng($img);
			ImageDestroy($img);
}

function titleOut($startat,$endat,$addnum=0) {
	global $atfile,$RefCode,$CP,$doConv2UTF8,$songcount;
		$line = file($atfile);
		
		for ($i=$startat;$i<=$endat;$i++) {
			list($album,$title,$length) = explode(",,",rtrim($line[$i]));
			if (!$length) continue;
			if ($doConv2UTF8) {
				$album=Conv2UTF8($album,$RefCode,$CP);
				$title=Conv2UTF8($title,$RefCode,$CP);
			}
			$outputtext[$i]=($addnum?($songcount-$i).".":"").((!empty($album))?$album." - ":"").$title." (".$length.")";
		}

	return $outputtext;
}

function updatelog($arrline){
	global $atfile;

	$rp = fopen($atfile, "w");
	while(list(,$val)=each($arrline)){
		fputs($rp,$val);
	}
	fclose($rp);
	chmod($atfile,0666);
}

function updatecount(){
	global $cntfile,$songcount;

	$rp = fopen($cntfile, "w");
		fputs($rp,++$songcount);
	fclose($rp);
	chmod($cntfile,0666);
}

function Conv2UTF8($text,$RefCode=1,$Codepage="big5") {
	switch (strtolower($Codepage)) {
		case "gb":
			return gb2utf8($text,$RefCode);
		case "sjis":
			return sjis2utf8($text,$RefCode);
		case "ws":
			return ws2utf8($text,$RefCode);
		case "ru":
			return ru2utf8($text,$RefCode);
		case "heb":
			return heb2utf8($text,$RefCode);
		case "utf8":
			return utf82u($text,$RefCode);
		case "big5":
		default:
			return big52utf8($text,$RefCode);
	}
}
?>