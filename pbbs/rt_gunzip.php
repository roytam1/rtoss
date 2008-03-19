<?php
function gzfile_get_contents($filename, $use_include_path = 0) {
   $file = @gzopen($filename, 'rb', $use_include_path);
   if ($file) {
       $data = '';
       while (!gzeof($file)) {
           $data .= gzread($file, 1024);
       }
       gzclose($file);
   }
   return $data;
}

if(is_file($_REQUEST["QUERY_STRING"])) {
	$fil=str_replace(".gz","",$_REQUEST["QUERY_STRING"]);
	$fp = fopen($fil, "w");
	set_file_buffer($fp, 0);
	rewind($fp);
	fputs($fp,gzfile_get_contents($_REQUEST["QUERY_STRING"]));
	fclose($fp);
	chmod($fil,0666);
	echo "$fil was decompressed successfully."
}else{
	echo "!!!ERROR!!!<br>$fil was not found."
}
?>