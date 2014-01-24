<?php

/*
typedef struct {
    WORD wSize; // in words 0x000D
    WORD CodePage;
    WORD MaxCharSize; // 1 or 2
    BYTE DefaultChar[MAX_DEFAULTCHAR]; //MAX_DEFAULTCHAR = 2
    WCHAR UnicodeDefaultChar;
    WCHAR unknown1;
    WCHAR unknown2;
    BYTE LeadByte[MAX_LEADBYTES]; // MAX_LEADBYTES = 12
} __attribute__((packed)) NLS_FILE_HEADER;

*/

$fp=fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open charset_dat '.@$_SERVER['argv'][1]);
$b = fread($fp,12);
$csdathdr = unpack('vFirstChar/vLastChar/vxx1/vxx2/vStartOffset/vxx3',$b);

print_r($csdathdr);


	$txtfp=fopen('cs_'.@$_SERVER['argv'][1].'.txt','wb');
	$b=fread($fp,0x200);
	$cdat=unpack('v*',$b);
	$icnt=count($cdat);
	for($i=1;$i < $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",$i-1,$cdat[$i]);
	}
