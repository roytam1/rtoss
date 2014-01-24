<?php

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle, $fsize);
//$byteArray = unpack("C*",$contents); $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
//print_r($byteArray);

$w=@$_SERVER['argv'][2] ? @$_SERVER['argv'][2] : 16;
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
STARTPROPERTIES 4
FONT_ASCENT $fa
FONT_DESCENT $fd
SPACING "C"
DEFAULT_CHAR 32
ENDPROPERTIES
CHARS $chrs

HEADER;
fwrite($fp,$hdr);

$swidth = $w*90;
$bwidth=ceil($w/8);
$bheight=ceil($h/8);
$cellsize=$bwidth*$h;

$pow = array(1);
for ($x=1; $x<$bwidth*8; $x++) {
	$pow[$x] = $pow[$x-1]*2;
}

for ($char=0; $char<$chrs; $char++)
{
	fwrite($fp,"STARTCHAR ".$char."\n");
	fwrite($fp,"ENCODING ".$char."\n");
	fwrite($fp,"SWIDTH $swidth 0\n");
	fwrite($fp,"DWIDTH $w 0\n");
	fwrite($fp,"BBX $w $h 0 -$fd\n");
	fwrite($fp,"BITMAP\n");

	for ($s=0; $s<$bheight; $s++) {
		$rot=array_fill(0,8,0);
		for ($x=0; $x<$w; $x++)
		{
			for($y=0;$y<8;$y++) {
				$rot[$y] |= (ord($contents{$offset+$char*$skip+$char*$cellsize+$s*$w+$x}) & $pow[$y]) ? $pow[$bwidth*8-1-$x] : 0;
			}
		}
		$v='';
		for ($x=0; $x<8; $x++) {
			$v.=sprintf('%0'.($bwidth*2)."X\n",$rot[$x]);
		}
		fwrite($fp, $v);
	}

	fwrite($fp,"ENDCHAR\n");
}

fwrite($fp,"ENDFONT\n");
fclose($fp);
