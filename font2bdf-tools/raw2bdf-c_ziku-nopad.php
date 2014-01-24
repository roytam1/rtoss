<?php
function idx2big5($idx) {
	$oidx=$idx;
	if($idx>=408 && $idx<684) $idx-=0x6000;
	if($oidx>=684) $idx=471+($idx-684);
	if($oidx>=2096) $idx+=1;
	if($oidx==2520) $idx=471+(2096-684);
	if($oidx>2520) $idx-=1;
	if($oidx>4397) $idx-=1;
	if($oidx>=4785) $idx+=1;
	if($oidx==4397) $idx=471+(4784-684);
	if($oidx>5310) $idx-=1;
	if($oidx>=5500) $idx+=1;
	if($oidx==5310) $idx=471+(5500-684-1);
	if($oidx>5584) $idx-=1;
	if($oidx>=5731) $idx+=1;
	if($oidx==5584) $idx=471+(5731-684-1);
	if($oidx==6085) return 0xf9d6;
	if($oidx>=6086) $idx=6493+($idx-6086);
	if($oidx>=6096) $idx+=1;
	if($oidx==6129) return 0xcabe;
	if($oidx>6129) $idx-=1;
	if($oidx>=6178) $idx+=1;
	if($oidx==6191) return 0xc9be;
	//if($oidx>6191) $idx-=1;
	if($oidx==6224) return 0xcaf7;
	if($oidx>6224) $idx-=1;
	if($oidx>=6392) $idx+=1;
	if($oidx>=8341) $idx+=1;
	if($oidx==8979) $idx=6493+(8341-6086-157-32-16-8)+1;
	if($oidx>=8980) $idx-=1;
	if($oidx>=9379) $idx+=1;
	if($oidx>11016) $idx-=1;
	if($oidx>11163) $idx-=1;
	if($oidx>11451) $idx-=1;
	if($oidx>=11568) $idx+=1;
	if($oidx==11016) $idx=6493+(11568-6086-157-32-16-8-2)+1;
	if($oidx>=11705) $idx+=1;
	if($oidx==11163) $idx=6493+(11705-6086-157-32-16-8-1)+1;
	if($oidx==11809) return 0xf056;
	if($oidx>11809) $idx-=1;
	if($oidx>=12032) $idx+=1;

	if($oidx>=12312) $idx+=1;
	if($oidx==11451) $idx=6493+(12312-6086-157-32-16-8-1)+1+2;
	if($oidx>=12230) $idx+=1;
	if($oidx==12398) return 0xeeeb;
	if($oidx>12398) $idx-=1;
	if($oidx>=12407) $idx+=1;
	if($oidx>=12560) $idx+=1;
	if($oidx>12616) $idx-=1;
	if($oidx==12730) return 0xf16b;
	if($oidx>12730) $idx-=1;
	if($oidx==12873) $idx=6493+(12560-6086-157-32-16-8-1)+1+3;
	if($oidx>12873) $idx-=1;
	if($oidx>=13185) $idx+=1;
	if($oidx==12616) return 0xf4b5;
	if($oidx>=12919) $idx+=1;
	if($oidx==12990) return 0xf663;
	if($oidx>12990) $idx-=1;
	if($oidx==13675) $idx=6493+(13719-6086-157-32-16-8-1)+2;
	if($oidx>13675) $idx-=1;
	if($oidx>=13719) $idx+=1;
	$grid=floor($idx/157);
	$gidx=$idx%157;
	return 0xa100+$grid*0x100+($gidx > 62 ? $gidx + 34: $gidx) + 0x40;
}

$handle = fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open file.');
$fsize = filesize(@$_SERVER['argv'][1]);
$contents = fread($handle, $fsize);
//$byteArray = unpack("C*",$contents); $tmp=array_shift($byteArray); array_unshift($byteArray,$tmp);
$byteArray=array();
for($i=0;$i<$fsize;$i++) {$byteArray[]=ord(substr($contents,$i,1));}
//print_r($byteArray);

$w=@$_SERVER['argv'][2] ? @$_SERVER['argv'][2] : 8;
$realh=$h=@$_SERVER['argv'][3] ? @$_SERVER['argv'][3] : 16;
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
SPACING "C"
DEFAULT_CHAR 41280
CHARSET_REGISTRY "BIG5"
CHARSET_ENCODING "0"
ENDPROPERTIES
CHARS $chrs

HEADER;
fwrite($fp,$hdr);

$swidth = $w*90;
$bwidth=ceil($w/8);
for ($char=0; $char<$chrs; $char++)
{
	$chrcode=idx2big5($char);
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
			$v.=sprintf("%02X",$byteArray[$offset+$char*$skip+$char*($bwidth*$h)+$y++]);
		$v.=sprintf("%02X\n",$byteArray[$offset+$char*$skip+$char*($bwidth*$h)+$y]);
		fwrite($fp, $v);
	}

	fwrite($fp,"ENDCHAR\n");
}

fwrite($fp,"ENDFONT\n");
fclose($fp);
