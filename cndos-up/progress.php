<?php
if(isset($_GET['progress_key'])) {
	$status = apc_fetch('upload_'.$_GET['progress_key']);
//	print_r($status);
	printf('{"total":%s,"current":%s}',$status['total'],$status['current']);
}
?>