<?php
/*
mod_gzip : 即時壓縮靜態網頁內容 ver.050816
由 scribe 隨手亂寫
*/

/* 檢查瀏覽器是否支援gzip壓縮方式 */
function CheckCanGZip(){
	$HTTP_ACCEPT_ENCODING = isset($_SERVER['HTTP_ACCEPT_ENCODING']) ? $_SERVER['HTTP_ACCEPT_ENCODING'] : "";
	if (headers_sent() || connection_aborted()){
		return 0;
	}
	if (strpos($HTTP_ACCEPT_ENCODING, 'x-gzip') !== false) return "x-gzip";
	if (strpos($HTTP_ACCEPT_ENCODING,'gzip') !== false) return "gzip";
	return 0;
}

/* $level = 壓縮等級 0-9, 0=不壓縮, 9=最大壓縮 */
function GZipOutput($level=3,$debug=0){
	$gzdata = "";
	$ENCODING = CheckCanGZip();
	if ($ENCODING){
		$Contents = ob_get_contents(); // 取得緩衝區內容
		ob_end_clean(); // 清空緩衝區
		if ($debug){
			$s = "\n<!-- Use compress $ENCODING -->\n";
			$s .= "<p>Not compress length: ".strlen($Contents); 
			$s .= "<br>Compressed length: ".strlen(gzcompress($Contents,$level));
			$Contents .= $s;
        }

		$gzdata = "\x1f\x8b\x08\x00\x00\x00\x00\x00";  // 輸出GZip標準檔頭
		$Size = strlen($Contents);
		$Crc = crc32($Contents);
		$Contents = gzcompress($Contents, $level);
		$Contents = substr($Contents, 0, strlen($Contents) - 4);
		$gzdata .= $Contents;
		$gzdata .= pack('V', $Crc); // 輸出檢查碼
		$gzdata .= pack('V', $Size); // 輸出原檔大小

		header("Content-Encoding: $ENCODING");
		header('Content-Encoding-By: mod_gzip.php [by: scribe]');
		header('Content-Encoding-Level: '.$level);
		header('Vary: Accept-Encoding');
		header('Content-Length: '.strlen($gzdata));

		print $gzdata;

		exit;
	}else{
		ob_end_flush(); // 直接印出緩衝區內容，同時清空緩衝
		exit;
	}
}

ob_start(); // 開啟緩衝區
ob_implicit_flush(0);

$file_path = isset($_SERVER["QUERY_STRING"]) ? $_SERVER["QUERY_STRING"] : ""; // 進行即時壓縮之檔案名稱
$file_path=str_replace("?","",$file_path);
$file_path=str_replace("=","",$file_path);
#$file_path=substr($_SERVER['PATH_INFO'],1); // 進行即時壓縮之檔案名稱
if($file_path && eregi(".htm", $file_path) && @filesize($file_path)){
	readfile($file_path);
	GZipOutput(2); // 使用等級二壓縮輸出
}else{
	print <<< _EOF01_
<html>
<head>
<title>mod_gzip : 參數錯誤</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head>
<body>
_EOF01_;

if($file_path==""){
	print "這是mod_gzip : 即時壓縮靜態網頁內容程式";
}else{
	print "指定參數：{$file_path}錯誤！程式停止動作。";
}

print <<< _EOF02_
</body>
</html>
_EOF02_;
	exit();
}
?>