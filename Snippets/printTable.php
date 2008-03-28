<?php
function printTable($ary,$colmax=3) {
	$out='<table><tr>';
	$col=0;
	foreach($ary as $k => $v) {
		$out.='<td><b>'.$k.':</b></td><td>'.$v.'</td>';
		if((++$col%$colmax)==0) $out.='</tr><tr>';
	}
	return $out.'</tr></table>';
}

function printTableWithHeader1($act,$ary=null) {
	switch($act) {
		case 'new':
			$out='<table border="1" cellpadding="0" cellspacing="0"><tr>';$out2='';
			foreach($ary as $k => $v) {
				$out.='<th>'.$k.'</th>';
				$out2.='<td>'.$v.'</td>';
			}
			return $out.'</tr><tr>'.$out2.'</tr>';
			break;
		case 'cont':
			$out='<tr>';
			foreach($ary as $k => $v) {
				$out.='<td>'.$v.'</td>';
			}
			return $out.'</tr>';
			break;
		case 'end':
			return '</table>';
			break;
	}
}

function printTableWithHeader2($ary='end') {
	static $firstrun = true;
	if($firstrun) {
			$firstrun = false;
			$out='<table border="1" cellpadding="0" cellspacing="0"><tr>';$out2='';
			foreach($ary as $k => $v) {
				$out.='<th>'.$k.'</th>';
				$out2.='<td>'.$v.'</td>';
			}
			return $out.'</tr><tr>'.$out2.'</tr>';
	} elseif($ary=='end') {
			$firstrun = true;
			return '</table>';
	} else {
			$out='<tr>';
			foreach($ary as $k => $v) {
				$out.='<td>'.$v.'</td>';
			}
			return $out.'</tr>';
	}
}
