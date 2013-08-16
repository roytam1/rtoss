<?php

$f=glob('[0-9]*.html');

sort($f);

if(isset($_GET['direct']))

$buf='';
foreach($f as $i){
	$l=file_get_contents($i);
	preg_match('#<title>(.*)</title>#sm',$l,$m);
	$title=rtrim($m[1]);
	preg_match('#(\d{4}/\d{2}/\d{2}\(.*?\) \d{2}\:\d{2})#',$l,$m);
	$ctime=$m[1];
	$lastdt=strrpos($l,'<dt>');
	$lastnospace=strpos($l,' ',$lastdt);
	$lastno=max(1,intval(substr($l,$lastdt+4,$lastnospace-$lastdt-4)));
	$buf.="<a href='$i'>$title ($lastno) [$ctime]</a><br>";
}

if(isset($_GET['direct'])){
	echo $buf;
}else{
	$fp=fopen('index.html','w');
	flock($fp,LOCK_EX);
	fwrite($fp,$buf);
	flock($fp,LOCK_UN);
	fclose($fp);
	echo 'OK';
}

