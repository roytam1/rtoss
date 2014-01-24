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
$thisleadpair=0;
$thisleadminus=0;
$leadpairs=array();
function getLead($idx) {
	global $thisleadpair,$thisleadminus,$leadpairs,$nlshdr;
	if(!count($leadpairs)) {
		$leadpairs = array(array($nlshdr['LeadByte1'],$nlshdr['LeadByte2']),
			array($nlshdr['LeadByte3'],$nlshdr['LeadByte4']),
			array($nlshdr['LeadByte5'],$nlshdr['LeadByte6']),
			array($nlshdr['LeadByte7'],$nlshdr['LeadByte8']),
			array($nlshdr['LeadByte9'],$nlshdr['LeadByte10']),
			array($nlshdr['LeadByte11'],$nlshdr['LeadByte12']),
			);
	}
	$thislead = ((($leadpairs[$thisleadpair][0] << 8) + $idx-$thisleadminus) >> 8) & 0xff;
	if($thislead > $leadpairs[$thisleadpair][1]) {
//		if($leadpairs[$thisleadpair+1][0]) {
			$thisleadminus += ($leadpairs[$thisleadpair][1]+1 - $leadpairs[$thisleadpair][0]) << 8;
			$thisleadpair++;
//		}
	}
	return (($leadpairs[$thisleadpair][0] << 8) + $idx-$thisleadminus) & 0xffff;
}

$fp=fopen(@$_SERVER['argv'][1],'rb') or die('Cannot open NLS '.@$_SERVER['argv'][1]);
$b = fread($fp,26);
$nlshdr = unpack('vSize/vCodePage/vMaxCharSize/C2DefaultChar/vUnicodeDefaultChar/vunknown1/vunknown2/C12LeadByte',$b);

print_r($nlshdr);
getLead(0);
print_r($leadpairs);



$b = fread($fp,2);
list($offset) = array_values(unpack('v',$b));

printf("offset = %x\n",$offset);

	$txtfp=fopen('c_'.$nlshdr['CodePage'].'.txt','wb');
	$b=fread($fp,0x200);
	$cdat=unpack('v*',$b);
	$icnt=count($cdat);
	for($i=1;$i <= $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",$i-1,$cdat[$i]);
	}

if($offset==0x203){
	$txtfp=fopen('c_'.$nlshdr['CodePage'].'.dos','wb');
	$b=fread($fp,2); // table size
	$b=fread($fp,0x200);
	$cdat=unpack('v*',$b);
	$icnt=count($cdat);
	for($i=1;$i <= $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",$i-1,$cdat[$i]);
	}
}
else if($offset!=0x103) { // DBCS
	fseek($fp,0x420);
	$b=fread($fp,($offset-515)*2);
	$cdat=unpack('v*',$b);
	$icnt=count($cdat);
	for($i=1;$i <= $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",getLead($i-1),$cdat[$i]);
	}
}
/*printf("entries = %d\n",$entries);
for($i=0; $i < $entries; $i++) {
$b=fread($fp,0x1C);
$chdr=unpack('Vcpid/Vcp2ucOff/Vcp2ucSiz/Vxx1/Vuc2cpOff/Vuc2cpSiz/Vxx2',$b);
$cpinfo[]=$chdr;
print_r($chdr);
}

for($x=0; $x < $entries; $x++) {
	$txtfp=fopen('cp'.$cpinfo[$x]['cpid'].'.txt','wb');
	fseek($fp,$cpinfo[$x]['cp2ucOff']);
	$b=fread($fp,$cpinfo[$x]['cp2ucSiz']);
	$cdat=unpack('v*',$b);
	//print_r($cdat);
	$icnt=count($cdat);
	for($i=23;$i < $icnt; $i++){
		fprintf($txtfp,"0x%02x\t0x%04x\n",$i-23,$cdat[$i]);
	}
	fclose($txtfp);
}*/
