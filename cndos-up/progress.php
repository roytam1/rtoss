<?php
if(isset($_GET['progress_key']) && function_exists('apc_fetch')) {
	$status = apc_fetch('upload_'.$_GET['progress_key']);
//	print_r($status);
	printf('{"total":%u,"current":%u}',$status['total'],$status['current']);
}
?>