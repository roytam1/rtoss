<?php
require_once "./conf.php";
require_once "./bb2html.php";
require_once "./lib_pte.php";
$arg = array();
if ($_GET['mode'] == "post") {
	$arg['post'] = true;
} else {
	$arg['main'] = true;
} 
/* たぶん合ってる変数解説
st=* *レス目から表示。
to=* *レス目まで表示。
ls=* 最新*件表示
nofirst=true trueの時は1を表示しない。*/
$st = $_GET['st'];
$to = $_GET['to'];
$ls = $_GET['ls'];
$nofirst = $_GET['nofirst'];
$key = $_GET['key'];

if (!empty($_SERVER['PATH_INFO'])) {
	$buff = $_SERVER['PATH_INFO'];
	$arr = explode('/', $buff);
	$key = $arr[1];
	if ($arr[2]) {
		if (preg_match("/n/", $arr[2])) {
			$nofirst = 'true';
			$arr[2] = str_replace("n", "", $arr[2]);
		} 
		if (substr($arr[2], 0, 1) == 'l') {
			$ls = substr($arr[2], 1);
		} else if (preg_match("/\-/", $arr[2])) {
			list($st, $to) = explode('-', $arr[2]);
			if (!$st) {
				$st = 1;
			} 
		} else {
			$st = $arr[2];
			$to = $arr[2];
		} 
	} 
} 
$arg['dir_path'] = $dir_path;
$arg['home'] = $dir_path;
$arg['PHP_SELF'] = $dir_path . "read.php";
$arg['key'] = $key;

if (!empty($_COOKIE)) {
	$cname = stripslashes($_COOKIE['NAME']);
	$cmail = stripslashes($_COOKIE['MAIL']);
	$cpass = stripslashes($_COOKIE['PASS']);

/*	if (function_exists("mb_convert_encoding")) {
		$cname = mb_convert_encoding($cname, "SJIS", "UTF-8");
		$cmail = mb_convert_encoding($cmail, "SJIS", "UTF-8");
	} else {
		require_once('jcode.php');
		include_once('code_table.jis2ucs');
		$cname = JcodeConvert($cname, 4, 2);
		$cmail = JcodeConvert($cmail, 4, 2);
	} 
*/
	$arg['cname'] = $cname;
	$arg['cmail'] = $cmail;
	$arg['cpass'] = $cpass;
} else
	$arg['cname'] = $arg['cmail'] = $arg['cpass'] = "";

$log_file = $ddir . $key . $ext;
$log = file($log_file); //ログを配列にぶちこむ
if (!$log) die("ファイルが見つかりません<br><br><a href=$home>＜＜戻る</a>");
$stopped = !is_writable($log_file);
$line = sizeof($log); //ログの行数
$arg['size'] = round(filesize($log_file) / 1024);
$arg['new'] = $line;
// レス制限
$arg['numlimit'] = $numlimit;
if ($line >= $numlimit - $read_def) {
	$arg['notice'] = $numlimit - $read_def;
	$arg['notice_err'] = true;
} 
if ($line >= $numlimit) {
	$arg['over_err'] = true;
	if (!$to && !$ls) $to = $read_def;
} 
list($fname, $femail, $fdate, $fcom, $fsub) = explode(",", $log[0]); //親

if (isset($_GET['ua']) && $_GET['ua'] == 'i' && function_exists("mb_convert_encoding")) {
	$fname = mb_convert_encoding($fname, "SJIS", "UTF-8");
	$fcom = mb_convert_encoding($fcom, "SJIS", "UTF-8");
	$femail = mb_convert_encoding($femail, "SJIS", "UTF-8");
	$fsub = mb_convert_encoding($fsub, "SJIS", "UTF-8");
}

$fcom = bb2html($fcom);
// ページング
for($i = 1; $i <= $line; $i += $read_def * 2) {
	$end = $i + $read_def * 2 - 1;
	$arg['page'][] = array('st' => $i, 'to' => $end);
} 
$f = compact('fname', 'femail', 'fdate', 'fcom', 'fsub', 'stopped');
$arg = array_merge($arg, $f);
// 親記事表示
if ($nofirst != "true" || $st == 1) {
	$arg['first'] = true;
} 
// lsがログ数より大きいなら引いた所から表示
if ($ls != "" && $line > $ls) $st = $line - $ls + 2;
if ($st == "" || $st == 1) $st = 2; //st空なら2番目から
if ($to == "") $to = $line; //to空なら最後まで
// レス表示
for($i = $st; $i <= $to; $i++) {
	$show = true;
	if ($log[$i-1] == "") break;
	list($name, $email, $date, $com) = explode(",", $log[$i-1]);
    if($taborn > 1 && $date == $aborn) $show = false;
    if($taborn > 0 && $date == $aborn && trim($com) == $aborn) $show = false;
	if (isset($_GET['ua']) && $_GET['ua'] == 'i' && function_exists("mb_convert_encoding")) {
		$name = mb_convert_encoding($name, "SJIS", "UTF-8");
		$com = mb_convert_encoding($com, "SJIS", "UTF-8");
		$email = mb_convert_encoding($email, "SJIS", "UTF-8");
		$date = mb_convert_encoding($date, "SJIS", "UTF-8");
		$sub = mb_convert_encoding($sub, "SJIS", "UTF-8");
	}
	$com = bb2html($com);
	$r = compact('name', 'email', 'date', 'com', 'show');
	$r['num'] = $i;
	$arg['res'][] = $r;
} 
$arg['num'] = $i;
$arg['read_def'] = $read_def;
// 改ページ処理
if ($line > 1 && $i < $line) {
	$arg['next'] = $i + $read_def;
} 
if ($st > 2) {
	$arg['prev_st'] = $i - $read_def - $read_def;
	if ($arg['prev_st'] < 0) $arg['prev_st'] = "";
	$arg['prev'] = $i - $read_def;
} 
//print_r($arg);
//$tm1=microtime();
$PTE = new PTELibrary($read_skin);
//$tm2=microtime();
$pteargs = toPTEargs($arg);
//$tm3=microtime();
echo $PTE->ParseBlock('READ',$pteargs);
//$tm4=microtime();

//echo "Init: ".(($tm2-$tm1)*1000)." ms, Parse: ".(($tm4-$tm3)*1000)." ms";
?>