<?php
require_once "./conf.php";

$idx = file($sub_back);
for($i = 0; $i < count($idx); $i++) {
	list($key,$topic,$res) = explode(",", $idx[$i]);
	$res=(int)$res;
	if(!$res) continue;
	$idx[$i]="$key$ext,$topic($res)\n";
}


$sp = fopen($sub_back, "w");
flock($sp, LOCK_EX);
fputs($sp, implode('', $idx));
fclose($sp);

$sf = fopen($subj_file, "w");
flock($sf, LOCK_EX);
for($i = 0; $i < $thre_def; $i++) {
    fputs($sf, $idx[$i]);
} 
fclose($sf);

echo "done!";
?>