<?
$TTF_LOCATION="./Mona-UMingTW-MSPG.ttf";
$FONT_SIZE=12;
include ("./big52utf8.php");
include ("./gb2utf8.php");
include ("./sjis2utf8.php");
include ("./tblless2utf8.php");

//extract($_GET);

function text2etc($text,$CP,$mode="png",$trans=1) {
	global $TTF_LOCATION,$FONT_SIZE;
	//$outputtext = implode('',file($f));
    $outputtext=$text;
	$outputtext=Conv2UTF8($outputtext,1,$CP);
	$outputtext = str_replace("\r\n","\n",$outputtext); 
	$outputtext = str_replace("\r","\n",$outputtext);
	$outputtext = str_replace("\n","\r\n",$outputtext); 
	$outputtext = str_replace("<br />","\r\n",$outputtext); 
	$outputtext = str_replace("&nbsp;"," ",$outputtext); 
	$outputtext = unhtmlentities($outputtext); 
		
	if ($mode=="png") {
		$dim= imageftbbox($FONT_SIZE, 0, $TTF_LOCATION, $outputtext, Array("linespacing" => 1.0));
#		$dim= imagettfbbox($FONT_SIZE, 0, $TTF_LOCATION, $outputtext);
		$min_x= min($dim[0], $dim[2], $dim[4], $dim[6]);
		$max_x= max($dim[0], $dim[2], $dim[4], $dim[6]);
		$width= $max_x-$min_x+1;
		$min_y= min($dim[1], $dim[3], $dim[5], $dim[7]);
		$max_y= max($dim[1], $dim[3], $dim[5], $dim[7]);
		$height= $max_y-$min_y+1;

		$img= imagecreate($width+1, $height+1);
		$white = ImageColorAllocate($img, 255,255,255);
		if($trans) $twhite=imagecolortransparent($img,$white);
		$black = ImageColorAllocate($img, 0,0,0);
#		ImageTTFText($img, $FONT_SIZE, 0, -$min_x+$dim[0],-$min_y, $black, $TTF_LOCATION, $outputtext);
		ImageFTText($img, $FONT_SIZE, 0, -$min_x+$dim[0],-$min_y, $black, $TTF_LOCATION, $outputtext, Array("linespacing" => 1.0));

		Header("Content-type: image/png");
		ImagePng($img);
		ImageDestroy($img);
	}  else if ($mode=="pre") {
		echo "<pre>\n$outputtext\n</pre>";
	} else if ($mode=="text") {
		Header("Content-type: text/plain");
		echo utf8Encode($outputtext);
	}
}
function unhtmlentities($string) {
   $trans_tbl=get_html_translation_table(HTML_ENTITIES);
   $trans_tbl=array_flip($trans_tbl);
   return strtr($string,$trans_tbl);
}
function Conv2UTF8($text,$RefCode=1,$Codepage="big5") {
	switch ($Codepage) {
		case "gb":
		case "936":
			return gb2utf8($text,$RefCode);
		case "sjis":
		case "932":
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
		case "950":
		default:
			return big52utf8($text,$RefCode);
	}
}
?>