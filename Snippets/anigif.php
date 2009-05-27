<?php
/*
 *** Enhanced is_ani function ***
 *
 * orginal is_ani function by ZeBadger
 * http://php.net/manual/en/function.imagecreatefromgif.php#59787
 *
 * version history:
 *
 * RTv1: 28-May-2009 - Properly handling comments/Application block
 * orginal: 15-Dec-2005 05:15
 */

function is_ani($filename) {
	$filecontents=file_get_contents($filename);

	$str_loc=0;
	$count=0;
	while ($count<2) { # There is no point in continuing after we find a 2nd frame

		$where1=strpos($filecontents,"\x21\xF9\x04",$str_loc); # Graphic Control Extension block
		if ($where1===FALSE) {
			break;
		} else {
			$where1+=3;
			$wherefollow=strpos($filecontents,"\x03\x01",$where1); # Extra data before Image data?
			$where2=strpos($filecontents,"\x00\x2C",$where1); # Image Data block
			if ($where2===FALSE) {
				break;
			} else {
				if (!$wherefollow===FALSE && $where2>$wherefollow) {
					$where1=$wherefollow;
				}
				if ($where1+4 == $where2) {
					$count++;
				}
				$str_loc=$where2+1;
			}
		}
	}

	if ($count > 1) {
		return(true);
	} else {
		return(false);
	}
}

if(isset($_SERVER['argv'][1])) {
	if ($_SERVER['argv'][1] == '-l') {
		$allfiles=glob('*.gif');
		foreach ($allfiles as $thisfile) {
		if (is_ani($thisfile))
		echo "$thisfile is animated\n";
		else
		echo "$thisfile is NOT animated\n";
	}
    } else if (file_exists($_SERVER['argv'][1])) {
    	exit((int)is_ani($_SERVER['argv'][1]));
    }
}
?>
