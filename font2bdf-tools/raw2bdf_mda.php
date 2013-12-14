<?php

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle, $fsize);
$byteArray = unpack("C*",$contents); $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//print_r($byteArray);

$w=@$_SERVER['argv'][2] ? @$_SERVER['argv'][2] : 8;
$h=@$_SERVER['argv'][3] ? @$_SERVER['argv'][3] : 14;

$fp=fopen(str_replace('.','-',@$_SERVER['argv'][1]).'.bdf','wb') or die('Cannot write file');
$fa=ceil($h*0.75);
$fd=$h-$fa;
$hdr= <<<HEADER
STARTFONT 2.1
FONT ${w}x$h
SIZE $h 75 75
FONTBOUNDINGBOX $w $h 0 -$fd
STARTPROPERTIES 4
FONT_ASCENT $fa
FONT_DESCENT $fd
SPACING "C"
DEFAULT_CHAR 32
ENDPROPERTIES
CHARS 256

HEADER;
fwrite($fp,$hdr);

$swidth = $w*90;
$bwidth=ceil($w/8);
for ($char=0; $char<256; $char++)
{
$h=14;
	fwrite($fp,"STARTCHAR $char\n");
	fwrite($fp,"ENCODING $char\n");
	fwrite($fp,"SWIDTH $swidth 0\n");
	fwrite($fp,"DWIDTH $w 0\n");
	fwrite($fp,"BBX $w $h 0 -$fd\n");
	fwrite($fp,"BITMAP\n");

$h=8;
	for ($y=0; $y<$h*$bwidth; $y++)
	{
		$v='';
		//printf("offset=%d, chr=%d\n",$char*($bwidth*$h)+$y,$byteArray[$char*($bwidth*$h)+$y]);
		$v.=sprintf("%02X\n",$byteArray[$char*($bwidth*$h)+$y]);
		fwrite($fp, $v);
	}

	for ($y=0; $y<$h*$bwidth-2; $y++)
	{
		$v='';
		//printf("offset=%d, chr=%d\n",$char*($bwidth*$h)+$y,$byteArray[$char*($bwidth*$h)+$y]);
		$v.=sprintf("%02X\n",$byteArray[128*$h*2+$char*($bwidth*$h)+$y]);
		fwrite($fp, $v);
	}

	fwrite($fp,"ENDCHAR\n");
}

fwrite($fp,"ENDFONT\n");
fclose($fp);
