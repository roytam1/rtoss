<?php
function idx2gb2312($idx) {
	$base = 0x2121;
	$oidx=$idx;

	$area = intval($idx/94);
	$pos = $idx % 94;
	return $base + $area * 0x100 + $pos;
}

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle, $fsize);
//$byteArray = unpack("C*",$contents); $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//$byteArray=array();
//for($i=0;$i<$fsize;$i++) {$byteArray[]=ord(substr($contents,$i,1));}
//print_r($byteArray);

$w=@$_SERVER['argv'][2] ? @$_SERVER['argv'][2] : 8;
$h=@$_SERVER['argv'][3] ? @$_SERVER['argv'][3] : 16;
$offset=@$_SERVER['argv'][4] ? @$_SERVER['argv'][4] : 0;
$skip=@$_SERVER['argv'][5] ? @$_SERVER['argv'][5] : 0;
$chrs=@$_SERVER['argv'][6] ? @$_SERVER['argv'][6] : 256;

$fp=fopen(str_replace('.','-',@$_SERVER['argv'][1]).'.bdf','wb') or die('Cannot write file');
$fa=ceil($h*0.75);
$fd=$h-$fa;
$hdr= <<<HEADER
STARTFONT 2.1
FONT ${w}x$h
SIZE $h 75 75
FONTBOUNDINGBOX $w $h 0 -$fd
STARTPROPERTIES 6
FONT_ASCENT $fa
FONT_DESCENT $fd
CHARSET_REGISTRY "GB2312"
CHARSET_ENCODING "0"
SPACING "C"
DEFAULT_CHAR 32
ENDPROPERTIES
CHARS $chrs

HEADER;
fwrite($fp,$hdr);

$swidth = $w*90;
$bwidth=ceil($w/8);
for ($char=0; $char<$chrs; $char++)
{
	$chrcode=idx2gb2312($char);
	fwrite($fp,"STARTCHAR ".dechex($chrcode)."\n");
	fwrite($fp,"ENCODING ".($chrcode)."\n");
	fwrite($fp,"COMMENT CHRIDX ".($char)."\n");
	fwrite($fp,"SWIDTH $swidth 0\n");
	fwrite($fp,"DWIDTH $w 0\n");
	fwrite($fp,"BBX $w $h 0 -$fd\n");
	fwrite($fp,"BITMAP\n");

	for ($y=0; $y<$h*$bwidth; $y++)
	{
		$v='';
		//printf("offset=%d, chr=%d\n",$char*($bwidth*$h)+$y,$byteArray[$char*($bwidth*$h)+$y]);
		for($j=1;$j<$bwidth;$j++)
			$v.=sprintf("%02X",ord($contents{$offset+$char*$skip+$char*($bwidth*$h)+$y++}));
		$v.=sprintf("%02X\n",ord($contents{$offset+$char*$skip+$char*($bwidth*$h)+$y}));
		fwrite($fp, $v);
	}

	fwrite($fp,"ENDCHAR\n");
}

fwrite($fp,"ENDFONT\n");
fclose($fp);
