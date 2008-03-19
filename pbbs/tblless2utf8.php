<?php
include_once("./u2utf8.php");

# Russian
    function ru2utf8 ($ru,$RefCode=1){
        for ($i=0; $i < strlen($ru); $i++){
            $thischar=substr($ru,$i,1);
            $charcode=ord($thischar);
            if ($charcode==168){
            	$utfchar=(26*32+3+12+10+($charcode));
            }elseif ($charcode==184){
            	$utfchar=(26*32+3+12+10+60+4+($charcode));
            }elseif(($charcode>=192) and ($charcode<=255)){
            	$utfchar=(26*32+3+12+1+($charcode));
            }else{
                $uniline.=$thischar;
            }
            if ($utfchar) {
            	if (!$RefCode) $uniline.=u2utf8($utfchar);
            	else $uniline.="&#".$utfchar.";";
            }
            $utfchar='';
        }
        return $uniline;
    }

# Hebrew
	function heb2utf8($s,$RefCode=1) {
		$l=strlen($s);
		$o="";
		for($i=0;$i<$l;$i++) {
			$c=ord($s[$i]);
			$o.=$c>223&&$c<251?($RefCode?'&#'.(1264+$c).';':u2utf8((1264+$c))):chr($c);
		}
		return $o;
	}

#Windows Symbol
	function ws2utf8 ($ws,$RefCode=1){
		for ($i=0; $i < strlen($ws); $i++){
			$thischar=substr($ws,$i,1);
			$charcode=ord($thischar);
			$uniline.=$RefCode?"&#".(61440+($charcode)).";":u2utf8((61440+($charcode)));
		}
		return $uniline;
	}
	
#UTF-8 to Unicode
	function utf82u($str,$RefCode=1) {
        
        $unicode = array();
        $values = array();
        $lookingFor = 1;
        
        for ($i = 0; $i < strlen( $str ); $i++ ) {
            $thisValue = ord( $str[ $i ] );
            if ( $thisValue < 128 ) $unicode[] = $thisValue;
            else {
                if ( count( $values ) == 0 ) $lookingFor = ( $thisValue < 224 ) ? 2 : 3;
                $values[] = $thisValue;
                if ( count( $values ) == $lookingFor ) {
                    $number = ( $lookingFor == 3 ) ?
                        ( ( $values[0] % 16 ) * 4096 ) + ( ( $values[1] % 64 ) * 64 ) + ( $values[2] % 64 ):
                    	( ( $values[0] % 32 ) * 64 ) + ( $values[1] % 64 );
                    $unicode[] = $number;
                    $values = array();
                    $lookingFor = 1;
                } // if
            } // if
        } // for

		if ($RefCode) {
			$entities = '';
        	foreach( $unicode as $value ) $entities .= ( $value > 127 ) ? '&#' . $value . ';' : chr( $value );
        	return $entities;
		} else {
			$wchars = '';
        	foreach( $unicode as $value ) $wchars .= chr($value);
        	return $wchars;
		}
	}

?>