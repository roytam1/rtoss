<?php
require_once "./conf.php";
// extract($_POST);
// extract($_GET);
function CleanStr($u_str)
{
	if (get_magic_quotes_gpc()) {
		$u_str = stripslashes($u_str);
	}
	$u_str = htmlspecialchars($u_str);
	return str_replace(array(",",'$'), array("&#44;","&#36;"), $u_str);
}

function matchCIDR($addr, $cidr) {
	list($ip, $mask) = explode('/', $cidr);
	return (ip2long($addr) >> (32 - $mask) == ip2long($ip.str_repeat('.0', 3 - substr_count($ip, '.'))) >> (32 - $mask));
}
/* 取得 (Transparent) Proxy 提供之 IP 參數 */
function getREMOTE_ADDR(){
	if(isset($_SERVER['HTTP_X_FORWARDED_FOR'])){
		$tmp = preg_split('/[ ,]+/', $_SERVER['HTTP_X_FORWARDED_FOR']);
		return $tmp[0];
	}
	return $_SERVER['REMOTE_ADDR'];
}

function reCAPTCHA($title,$desc) {
	global $key,$FROM1,$mail,$subject,$MESSAGE,$c_pass;
	require_once './recaptchalib.php';
	$publickey = ""; // you got this from the signup page
	$privatekey = ""; // (same as above)
	$error = '';

	if($_POST["recaptcha_response_field"]) {
        $resp = recaptcha_check_answer ($privatekey,
                                        $_SERVER["REMOTE_ADDR"],
                                        $_POST["recaptcha_challenge_field"],
                                        $_POST["recaptcha_response_field"]);

        if ($resp->is_valid)
        	return true;
        else
            $error = $resp->error;
	}
	echo '<html><head><title>'.$title.'</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<body>'.$desc.'
<form method="post" action="'.$_SERVER['PHP_SELF'].'">
<input type="hidden" name="key" value="'.$key.'"><input type="hidden" name="nick" value="'.$FROM1.'"><input type="hidden" name="mail" value="'.$mail.'"><br>
<input type="hidden" name="subject" value="'.$subject.'">
<input type="hidden" name="content" value="'.$MESSAGE.'"><input type="hidden" name="url" value="">
<input type="hidden" name="delk" value="'.$c_pass.'">';
	echo recaptcha_get_html($publickey, $error);
	echo '<input type="submit" value="確認">
</form>
</body></html>';
	return false;
}

$subject = CleanStr($_REQUEST['subject']);
$FROM1 = $_REQUEST['nick'];
$FROM = CleanStr($_REQUEST['nick']);
$FROM = preg_replace("/[\r\n]/", "", $FROM);
$MESSAGE = CleanStr($_REQUEST['content']);
$mail = Cleanstr($_REQUEST['mail']);
$c_pass = $_REQUEST['delk'];
$delk = substr(md5($_REQUEST['delk']), 2, 8);
$key = $_REQUEST['key'];
$HOST = gethostbyaddr($IP=getREMOTE_ADDR());
$idtag=false;

if (preg_match("/^( |　|\t)*$/", $MESSAGE)) {
	error("本文がありません！", $FROM, $mail, $HOST, $MESSAGE);
}
if ($key == "" && (preg_match("/^( |　|\t)*$/", $subject))) {
	error("サブジェクトが存在しません！", $FROM, $mail, $HOST, $MESSAGE);
}
if (!isset($_REQUEST['url']) || (isset($_REQUEST['url']) && $_REQUEST['url']!="")) {
	error("投稿が禁止されています", $FROM, $mail, $HOST, $MESSAGE);
}


// reCAPTCHA
if(!$key) {
	if(reCAPTCHA('スレッド作成','スレッド作成のは CAPTCHA 認証が必要です。') === false) exit;
}

// ホスト、禁止ホスト
$IsBanned=false;
$killip = file("killip.cgi");
$checkTwice = ($IP != $HOST); // 是否需檢查第二次
$killip = explode(',', $killip[0]);
foreach ($killip as $kill) {
	$kill = rtrim($kill);
	if ($kill) {
		$slash = substr_count($kill, '/');
		if($slash==2){ // RegExp
			$kill .= 'i';
		}elseif($slash==1){ // CIDR Notation
			if(matchCIDR($IP, $kill)){ $IsBanned = true; break; }
			continue;
		}elseif(strpos($kill, '*')!==false || strpos($kill, '?')!==false){ // Wildcard
			$kill = '/^'.str_replace(array('.', '*', '?'), array('\.', '.*', '.?'), $kill).'$/i';
		}else{ // Full-text
			if($IP==$kill || ($checkTwice && $HOST==strtolower($kill))){ $IsBanned = true; break; }
			$kill = '/'.str_replace('.','\.',$kill).'/i'; // Go for regmatch
//			continue;
		}
		if(preg_match($kill, $HOST) || ($checkTwice && preg_match($kill, $IP))){ $IsBanned = true; break; }
	}
}
if($IsBanned) error("投稿が禁止されています (x1)", $FROM, $mail, $HOST, $MESSAGE);

if(count($ngfiles)) {
	foreach($ngfiles as $ngfile) {
		if(is_file($ngfile)) {
			$ngwords=explode(',',rtrim(implode('',file($ngfile))));
			foreach($ngwords as $value){
				if($value!="" && (strpos($MESSAGE, $value)!==false || strpos($subject ,$value)!==false || strpos($FROM, $value)!==false || strpos($mail,$value)!==false))
					error("投稿が禁止されています (x2)", $FROM, $mail, $HOST, $MESSAGE);
			}
		}
	}
}
if(is_file($rengfile)) {
	$ngwords=file($rengfile);
	foreach($ngwords as $value){
		$value = trim($value);
		if($value){
			$value="/$value/";
			if((preg_match($value,$MESSAGE) || preg_match($value,$subject) || preg_match($value,$FROM) || preg_match($value,$mail)))
				error("投稿が禁止されています (x3)", $FROM, $mail, $HOST, $MESSAGE);
		}
	}
}

// ID 処理
if(strpos($mail,'!id')!==false) {
	$mail=str_replace('!id','',$mail);
	$idtag=true;
}
/*
if (!empty($mail)) {
	$id = " ID:???";
} else*/ if($idtag) {
	$idnum = substr(strtr($_SERVER['REMOTE_ADDR'], '.', ''), 8);
	$bbscrypt = ord($_SERVER['PHP_SELF'][3]) + ord($_SERVER['PHP_SELF'][4]);
	$idcrypt = substr(crypt(($bbscrypt + $idnum), gmdate('Ymd', time() + $TZ * 3600)), -8);
	$id = ' ID:' . $idcrypt;
} else {
	// IP
	$id = ' IP:'.preg_replace('/\d+$/','*',$IP);
}

$qcnt=$exflg=0;
if($extipq && $IP != "127.0.0.1" && strpos($FROM,"fusianasan")===false && strpos($FROM,"mokorikomo")===false) {
	$rev = implode('.', array_reverse(explode('.', $IP)));
	$queries = array( 'list.dsbl.org','bbx.2ch.net','dnsbl.ahbl.org','niku.2ch.net','virus.rbl.jp','ircbl.ahbl.org','tor.ahbl.org' );
	foreach ( $queries as $query ) {
		$qres=gethostbyname($rev.'.'.$query);
		if($rev.'.'.$query!=$qres){ $exflg=1; break; }
		$qcnt++;
		if($qcnt>=$extipq) break;
	}
}
if($exflg) error("投稿が禁止されています (#".$qcnt.')', $FROM, $mail, $HOST, $MESSAGE);


$FROM = str_replace("fusianasan", "</b>" . $HOST . "<b>", $FROM); //fusianasan？
$FROM = str_replace("mokorikomo", "</b>" . $IP . "<b>", $FROM); //mokorikomo？

$MESSAGE = str_replace("\r\n", "\r", $MESSAGE); //改行文字の統一。
$MESSAGE = str_replace("\r", "\n", $MESSAGE);
/* 投稿制限 */
if (substr_count($MESSAGE, "\n") > $postline) error("改行が多すぎます！", $FROM, $mail, $HOST, $MESSAGE);
// $temp = str_replace("\n", "\n"."a",$MESSAGE);
// if(strlen($temp)-strlen($MESSAGE) > $postline){ error("投稿行数が長すぎます!"); }
if (strlen($MESSAGE) > $postbyte) {
	error("本文が長すぎます！", $FROM, $mail, $HOST, $MESSAGE);
}

$MESSAGE = str_replace("\n", "<br>", $MESSAGE); //改行文字の前に<br>を代入する。
$now = gmdate("Y/m/d(D) H:i", time() + $TZ * 3600);
$now .= $id;

if ($subject) $key = time();

/* レスポンスアンカー */
$MESSAGE = preg_replace("/(^|r>)(&gt;){1,2}(\d+)-(\d+)/si", "\\1<a href=\"{$dir_path}read.php/$key/\\3-\\4\" target=_blank>&gt;&gt;\\3-\\4</a>", $MESSAGE);
$MESSAGE = preg_replace("/(^|r>)(&gt;){1,2}(\d+)/si", "\\1<a href=\"{$dir_path}read.php/$key/\\3n\" target=_blank>&gt;&gt;\\3</a>", $MESSAGE);

/* 多重カキコチェック */
$last = fopen($last_file, "r+");
$lsize = fread($last, filesize($last_file));
list($lname, $lcom) = explode("\t", $lsize);
if ($FROM == $lname && $MESSAGE == $lcom) {
	error("二重かきこですか？？", $FROM, $mail, $HOST, $MESSAGE);
}
rewind($last);
fputs($last, "$FROM\t$MESSAGE\t");
fclose($last);

$mail1 = $mail;
$FROM = str_replace("★", "☆", $FROM);
$FROM = str_replace("◆", "◇", $FROM);
/* トリップ */
if (strstr($FROM, "#")) {
	$pass = substr($FROM, strpos($FROM, "#") + 1);
	$pass = str_replace('&#44;', ',', $pass);
	$salt = substr($pass . "H.", 1, 2);
	$salt = preg_replace("/[^\.-z]/", ".", $salt);
	$salt = strtr($salt, ":;<=>?@[\\]^_`", "ABCDEFGabcdef");
	$FROM2 = substr($FROM, 0, strpos($FROM, "#"));
	$FROM = $FROM2 . " ◆" . substr(crypt($pass, $salt), -10);
}
/* キャップ */
if (file_exists("cap.php") && strstr($mail, "#")) {
	$caparr = file("cap.php");
	for($c = 0; $c < count($caparr); $c++) {
		list($he, $cname, $cpass, $fo) = explode("<>", $caparr[$c]);
		$cap = substr($mail, strpos($mail, "#"));
		if ($cap == "#$cpass") {
			if ($FROM) $FROM .= " ＠";
			$FROM .= "$cname ★";
			break;
		}
	}
	$mail = substr($mail, 0, strpos($mail, "#"));
}
if (preg_match("/^( |　|\t)*$/", $FROM)) {
	$FROM = $nanasi;
}
// 記事フォーマット
$newlog = "$FROM,$mail,$now,$MESSAGE,$subject\n";
$newcgi = "$FROM,$mail,$now,$MESSAGE,$subject,$HOST,$delk,\n";
// スレ一覧読み込む
$subj_arr = file($sub_back);
// 親スレ投稿の場合、ファイル作成
if ($subject) {
	// 既にある場合（time()）は1増やす（意味無いかも
	if (file_exists("$ddir$key$ext")) $key++;
	$fp = fopen("$ddir$key$ext", "w");
	fputs($fp, $newlog);
	fclose($fp);
	// 読み込み不可ファイル生成
	$fc = fopen("$ddir$key$ext_cgi", "w");
	fputs($fc, $newcgi);
	fclose($fc);
	// ﾊﾟｰﾐｯｼｮﾝ666にする
	chmod("$ddir$key$ext", 0666);
	chmod("$ddir$key$ext_cgi", 0666);
	// スレ一覧の先頭に加える
	$new_subj = "$key$ext,$subject(1)\n";
	array_unshift($subj_arr, $new_subj);
	// レス投稿の場合
} else {
	// レスファイル名
	$resfile = $ddir . $key . $ext;
	$cgifile = $ddir . $key . $ext_cgi;
	if (!file_exists($resfile)) error("書き込もうとしているスレッドは存在しないか、削除されています。。。", $FROM, $mail, $HOST, $MESSAGE);
	// 該当レス読み込む
	$res_arr = file($resfile);
	// レス数取得
	$resline = count($res_arr);
	// ｶｳﾝﾄアップ
	$resline += 1;
	// レス数制限オーバー
	if ($resline > $numlimit) {
		chmod($resfile, 0444);
		// chmod($cgifile, 0444);
	}

	if ($resline == $numlimit) {
		$nextnum = $numlimit + 1;
		$MESSAGE = "[color=red]このスレッドは $numlimit を超えました<br />もう書けないので新しいスレッドを立てて下さい[/color]";
		$newlog .= "$nextnum,,Over $numlimit Thread,$MESSAGE,\n";
		$resline = $nextnum;
	}
	// レス書き込み
	if (!is_writable($resfile)) error("現在この掲示板は読取専用です。ここは待つしかない。。。", $FROM, $mail, $HOST, $MESSAGE);

	$re = fopen($resfile, "a") or error("このスレッドは停止されてます。もう書けない。。。", $FROM, $mail, $HOST, $MESSAGE);
	fputs($re, $newlog);
	fclose($re);

	$recgi = fopen($cgifile, "a") or error("このスレッドは停止されてます。もう書けない。。。", $FROM, $mail, $HOST, $MESSAGE);
	fputs($recgi, $newcgi);
	fclose($recgi);
	// レスｶｳﾝﾄアップ
	for ($r = 0; $r < count($subj_arr); $r++) {
		list($kdate, $t_r) = explode(",", $subj_arr[$r]);

		//$t_r = str_replace(")", "", $t_r);
		//list($title, $rescnt) = explode("(", $t_r);
		$tlen = strlen($t_r);
		$lastcur = strrpos($t_r, '(');
		$title = substr($t_r,0,$lastcur);
		$rescnt = substr($t_r, $lastcur+1, $tlen-$lastcur-2);

		list($dkey,) = explode(".", $kdate);
		if ($dkey == $key) {
			if (strstr($mail, "sage")) {
				$subj_arr[$r] = "$kdate,$title($resline)\n";
			} else {
				array_unshift($subj_arr, "$kdate,$title($resline)\n");
				array_splice($subj_arr, $r + 1, 1);
			}
		}
	}
}
// subback.html更新、全部
$bf = fopen($sub_back, "w");
flock($bf, 2);
reset($subj_arr);
fputs($bf, implode('', $subj_arr));
fclose($bf);
// subject.txt更新、一定数に収める
$sf = fopen($subj_file, "w");
flock($sf, 2);
for($i = 0; $i < $thre_def; $i++) {
	fputs($sf, $subj_arr[$i]);
}
fclose($sf);
// $cookval = implode(",", array($FROM1,$mail1));
/*if (function_exists("mb_convert_encoding")) {
	$FROM1 = mb_convert_encoding($FROM1, "UTF-8", "SJIS");
	$mail1 = mb_convert_encoding($mail1, "UTF-8", "SJIS");
} else {
	require_once('jcode.php');
	include_once('code_table.jis2ucs');
	$FROM1 = JcodeConvert($FROM1, 0, 4);
	$mail1 = JcodeConvert($mail1, 0, 4);
}*/

setcookie("NAME", $FROM1, time() + 30 * 24 * 3600);
setcookie("MAIL", $mail1, time() + 30 * 24 * 3600);
setcookie("PASS", $c_pass, time() + 30 * 24 * 3600);

include("index.inc");
// header("Location: bbs.php?mode=remake");
echo '<html><head><title>書きこみました。</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<META http-equiv="refresh" content="1;URL=./?"></head>
<body>書きこみが終わりました。<br><br>画面を切り替えるまでしばらくお待ち下さい。<br><br><br><br><br><hr></body></html>';

?>
