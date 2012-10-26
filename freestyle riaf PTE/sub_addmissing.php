<?php
require_once "./conf.php";

$idx = file($sub_back);
$sub = array();
foreach($idx as $line) {
	list($fil,$tit) = explode(',',$line);
	$sub[$fil] = $tit;
}

$d = dir($ddir);
while ($ent = $d->read()) {
        if (preg_match('/^[0-9]+\\'.$ext.'$/', $ent)) {
		if(!isset($sub[$ent])) {
			$f = file($ddir.$ent);
			list(,,,,$title) = explode(',',$f[0]);
			$title=trim($title);
			$idx[] = "$ent,$title(".count($f).")\n";
		}
	}
}
$d->close();

$sp = fopen($sub_back, "w");
flock($sp, LOCK_EX);
fputs($sp, implode('', $idx));
fclose($sp);

echo "done!";
?>
