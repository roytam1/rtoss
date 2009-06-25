<?php
if(isset($_GET['progress_key'])) {
	$status = apc_fetch('upload_'.$_GET['progress_key']);
//	print_r($status);
	echo $status['total']?($status['current']/$status['total']*100):'0';
}
?>