<?php
require_once "./conf.php";
require_once "./bb2html.php";
$pass = $_POST['pass'];
$key = $_POST['key'];
$num = $_POST['num'];
$dat = $_POST['dat'];

function check_login($forcelogin=1) {
	global $ad_pass,$ad_id;
	if (!isset($_SESSION['username']) || !isset($_SESSION['password'])) {
		$logged_in=0;
		if($forcelogin) SysError("ユーザーの認証が必要です。<br /><form action=\"".$_SERVER['PHP_SELF']."?action=login\" method=\"POST\">ユーザー：<INPUT type=\"text\" name=\"username\"><br />パスワード：<INPUT type=\"password\" name=\"password\"><INPUT type=\"submit\" value=\"認証\"></form>");
		else return $logged_in;
	} else {
		if($_SESSION['username']!=$ad_id || $_SESSION['password']!=$ad_pass) {
			$logged_in=0;
			unset($_SESSION['username']);
			unset($_SESSION['password']);
			// kill incorrect session variables.
			if($forcelogin) SysError("ユーザーの認証が必要です。<br /><form action=\"".$_SERVER['PHP_SELF']."?action=login\" method=\"POST\">ユーザー：<INPUT type=\"text\" name=\"username\"><br />パスワード：<INPUT type=\"password\" name=\"password\"><INPUT type=\"submit\" value=\"認証\"></form>");
			else return $logged_in;
		}else { 
			// valid password for username
			$logged_in = 1;		// they have correct info
			return $logged_in;	// in session variables.
		}
	}
}
function SysError($text) {
	echo '<HTML>
<HEAD>
<META http-equiv="Content-Type" content="text/html; charset=UTF-8">
<META http-equiv="Content-Style-Type" content="text/css">
<TITLE>ERROR！</TITLE>
</HEAD>
<body>
<FONT size="+1" color="#FF0000"><B>ERROR：'.$text.'</B></FONT>
</body></HRML>
';
	exit();
}

$mode = $_GET['mode'];

if ($mode == "remake") {
	include("index.inc");
	echo '<META http-equiv="refresh" content="0;URL=./?">';
	exit();
	}

session_start();

switch($_GET['action']){
case 'logout':
	unset($_SESSION['username']);
	unset($_SESSION['password']);
#	header("Location: ".$_SERVER['PHP_SELF']);
	echo "<META http-equiv=\"refresh\" content=\"1;URL=$_SERVER[PHP_SELF]\">";
	break;
case 'login':
	if(isset($_POST['username'])&&isset($_POST['password'])) {
		$_SESSION['username']=$_POST['username'];
		$_SESSION['password']=$_POST['password'];
	}
#	header("Location: ".$_SERVER['PHP_SELF']);
	echo "<META http-equiv=\"refresh\" content=\"1;URL=$_SERVER[PHP_SELF]\">";
	break;
}

if (count($_POST) > 0) {
	// if(isset($pass) && $pass != "$delpass")
	// $err = "パスワードがちがいます";
	// if(!$pass) $err = "パスワードをいれてください"; 
	// 削除
	if (isset($_POST['act_del'])) {
		check_login();
		if ($err == "" && is_array($num) && isset($dat)) { // スレッドの削除
			$res = file($ddir . $dat . $ext);
			for($n = 0; $n < count($res); $n++) {
				if ($num[$n]) {
					list(,,$now)=explode(",",$res[$n]);
					$new[] = "$aborn,,$aborn,$aborn\n";
				}
				else $new[] = $res[$n];
			} 
			$fp = fopen($ddir . $dat . $ext, "w");
			fputs($fp, implode('', $new));
			fclose($fp);
			$del = $dat;
		} 
		if ($err == "" && is_array($key)) { // スレッドの削除
			check_login();
			$sub = file($sub_back);
			for($j = 0; $j < count($sub); $j++) {
				$old = explode(",", $sub[$j]);
				foreach($key as $val) {
					if ($old[0] == $val.$ext) {
						$sub[$j] = "";
						$flag = true;
					} 
				} 
			} 
			if ($flag) {
				$sp = fopen($sub_back, "w");
				flock($sp, LOCK_EX);
				fputs($sp, implode('', $sub));
				fclose($sp);

				$sf = fopen($subj_file, "w");
				flock($sf, LOCK_EX);
				for($i = 0; $i < $thre_def; $i++) {
					fputs($sf, $sub[$i]);
				} 
				fclose($sf);
			} 
			foreach($key as $val) {
				if (file_exists($ddir . $val . $ext)) {
					unlink($ddir . $val . $ext);
					echo "$ddir$val$ext を削除しました<br>";
				} 
			} 
		} 
		include("./index.inc");
	} 
	// スレスト
	if (isset($_POST['act_stop'])) {
		check_login();
		if ($err == "" && isset($dat)) {
			$fp = fopen($ddir . $dat . $ext, "a");
			flock($fp, LOCK_EX);
			$data = "$st_name,,$st_date,$st_com\n";
			fputs($fp, $data);
			fclose($fp);

			@chmod($ddir . $dat . $ext, 0444);
			echo "$ddir$dat$ext を書き込み禁止にしました<br>";
		} 
		include("./index.inc");
	} 
	// HTML化
	if (isset($_POST['act_html'])) {
		check_login();
		if ($err == "" && isset($dat)) {
			$log = file($ddir . $dat . $ext);
			list($fname, $femail, $fdate, $fcom, $fsub) = explode(",", $log[0]); //親
			$fcom = bb2html($fcom);
			$kako = "<title>$fsub</title><body><dl><b><font size=+1 color=red>$fsub</font></b>";
			$kako .= "<dt>1 <font color=\"forestgreen\"><b>$fname</b></font> [ $fdate ]<dt><dd>$fcom<br><br><br>";
			for($i = 2; $i < count($log) + 1; $i++) {
				list($name, $email, $date, $com) = explode(",", $log[$i-1]);
				$com = bb2html($com);
				$kako .= "<dt>$i <font color=forestgreen><b>$name</b></font> [ $date ]<dd>$com<br><br>";
			} 
			$kako .= "</dl><hr size=1></body>";

			$fp = fopen($kdir . $dat . $kext, "w");
			fputs($fp, $kako);
			fclose($fp);

			echo "<a href=\"$kdir$dat$kext\"> $fsub </a>HTML化完了<br>";
		} 
	} 
} 
echo '
<html><head><title>'.$tit.'</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
</head><body>
';

if (check_login(0)) {
	echo "<form action=\"$PHP_SELF\" method=POST>
<a href=\"./\">【掲示板に戻る】</a> <a href=\"$PHP_SELF?\">【一覧に戻る】</a> <a href=\"$PHP_SELF?mode=remake\">【INDEXを書き直す】</a> <br>
<h3><font color=red>$err</font></h3><ul>
削除したいスレをチェックしてください。<br>
スレをクリックするとレスの削除画面に行きます。  ";
	if ($mode == "all") echo "<a href=$PHP_SELF?><b>最新スレ表示</b></a><br><br>";
	else echo "<a href=$PHP_SELF?mode=all><b>全スレ表示</b></a><br><br>";

	$del = (int)$_GET['del'];
	$mode = $_GET['mode'];

	if ($del) {
		echo "<dl>";
		$delfile = $ddir . $del . $ext;
		$delcgi = $ddir . $del . $ext_cgi;
		$line = file($delfile);
		$cgiline = @file($delcgi);
		list($name, $email, $now, $com, $sub) = explode(",", isset($_POST['viewcgi'])?$cgiline[0]:$line[0]);
		echo "<font size=+1 color=red>$sub</font>";
		for($i = 0; $i < count($line); $i++) {
			list($name, $email, $now, $com, $sub) = explode(",", isset($_POST['viewcgi'])?$cgiline[$i]:$line[$i]);
			list(,,,,, $host) = @explode(",", $cgiline[$i]);
			if ($email != "") {
				$name = "<a href=\"mailto:$email\">$name</a>";
			} 
			$com = bb2html($com);
			$n = $i + 1;
			if($host) $now = preg_replace('/I[PD].*$/',preg_match('/[a-zA-Z\-]/',$host)?"Host: $host":"IP: $host", $now);
			echo "<dt><input type=checkbox name=\"num[$i]\"> ";
			echo "$n <font color=\"forestgreen\"><b>$name</b></font> [ $now ]<dd>$com<br><br>\n";
		} 
		echo "</dl><input type=hidden name=dat value=$del>";
		$stop = " <input type=submit name=act_stop value='スレッド停止'><input type=submit name=act_html value='HTML化'>";
		$backup = isset($_POST['viewcgi'])?"<input type=submit name=viewdat value=\" ログ本体を見る \">":"<input type=submit name=viewcgi value=\" バックアップを見る \">";
	} else {
		$filename = ($mode == "all") ? $sub_back : $subj_file;
		$idx = file($filename);
		for($i = 0; $i < count($idx); $i++) {
			$data = explode(",", $idx[$i]);
			list($key,) = explode(".", $data[0]);
			echo "<input type=checkbox name=key[] value=$key> ";
			echo " <a href=admin.php?del=$key>$data[1]</a><br>\n";
		} 
	} 
	echo "<br>
<input type=submit name=act_del value=\" 削除する \">$backup$stop<br>各ボタンを押してね</form>
<FORM method=\"POST\" action=\"$_SERVER[PHP_SELF]?action=logout\" ENCTYPE=\"multipart/form-data\">
<INPUT type=\"submit\" value=\"ログアウト\"></form>";
	echo "</body></html>";
} else echo "<a href=\"./\">【掲示板に戻る】</a> <a href=\"$PHP_SELF?mode=remake\">【INDEXを書き直す】</a> <br>ユーザーの認証が必要です。<br /><form action=\"".$_SERVER['PHP_SELF']."?action=login\" method=\"POST\">ユーザー：<INPUT type=\"text\" name=\"username\"><br />パスワード：<INPUT type=\"password\" name=\"password\"><INPUT type=\"submit\" value=\"認証\"></form></body></html>";
?>
