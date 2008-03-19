<?php 

// 削除用

// スキン未完成･･･

require_once "conf.php";
require_once "bb2html.php";
list($cname, $cmail, $cpass) = explode(",", stripslashes($_COOKIE['fstyle'])); //クッキーちょうだい

$key = $_POST['key'];
if ($_SERVER['PATH_INFO']) {
	list(, $key,) = explode("/", $_SERVER['PATH_INFO']);
} 

if (!$key) d_error("スレッドを選択してください！");

$del_no = isset($_POST['no'])?intval($_POST['no']):0;
$delpwd = $_POST['pass'];

$mode = "";
if ($del_no && $delpwd) $mode = "del";

function form()
{
	global $key, $cpass, $del_no, $delpwd, $dir_path;
	if ($delpwd)$cpass = $delpwd;

	$line = file("subject.txt");
	for($i = 0;$i < count($line);$i++) {
		list($key_dat, $sub) = explode(",", $line[$i]);
		list($thread,) = explode(".", $key_dat);
		$arr[$thread] = $sub;
	} 
	head();

	?>
<p><font color="red"><big><b><?=$arr[$key];
	?></b></big></font>
<form action="<?=$dir_path;?>usr_del.php" method=POST>
<input type="hidden" name="mode" value="del">
<input type="hidden" name="key" value="<?=$key;?>">
No<input type=text size=4 name=no value="<?=$del_no;?>">
 Pass<input type=password name=pass size=8 style="font-size:12px;" value="<?=$cpass;?>">
 <input type="submit" value=" 削除 ">
</form>
<?php
	foot();
} 

function del()
{
	extract($GLOBALS);

	$log_file = $ddir . $key . $ext;
	$cgi_file = $ddir . $key . $ext_cgi;
	$log = @file($log_file); //ログを配列にぶちこむ
	$log_cgi = @file($cgi_file);
	if (!$log) d_error("スレッドが無いよう");
	$line = sizeof($log); //ログの行数
	$del_no -= 1;
	if ($log[$del_no] == "") d_error("そんな記事ないよ(´･ω･`)");

	if($line!=count($log_cgi)) d_error("ＤＥＢＵＧ：ログの行数は不一致です。");
	list($dname, $demail, $dnow, $dcom,) = explode(",", $log[$del_no]);
	list(,,,,,,$pass) = explode(",", $log_cgi[$del_no]);
	if($delpwd != $ad_pass) {
		if (trim($pass) == "") d_error("パスワードがありません。");
		if ($pass != substr(md5($delpwd), 2, 8)) d_error("パスワードが違うみたい。。。");
	}
	$log[$del_no] = "$aborn,,$aborn,[i]この記事は投稿者によって削除されました[/i],\n";

	if (!is_writable($log_file)) d_error("削除できない。。。");

	$re = @fopen($log_file, "w") or d_error("削除できない。。。");
	flock($re, 2);
	reset($log);
	fputs($re, implode('', $log));
	fclose($re);

	$com = bb2html($com);

	include_once("index.inc");

	head();

	?>
<p>以下の記事を削除しました。たぶん。</p>
<table>
<tr><td>名前</td><td><?=$dname;?></td></tr>
<tr><td>日付</td><td><?=$ddate;?></td></tr>
<tr><td>記事</td><td><?=$dcom;?></td></tr>
</table>
<p><b><a href="<?=$dir_path;?>index.html">掲示板に戻る</a></b></p>
<?php
	foot();
} 

function head()
{
	global $dir_path;

	?>
<html>
<head><title>削除β</title></head>
<BODY TEXT=#000000 link=#0000FF alink=#FF0000 vlink=#660099 background="<?=$dir_path;?>ba.gif">
<table border=1 cellspacing=7 cellpadding=3 width=95% bgcolor=#CCFFCC align=center>
<tr><td align=center>
<TABLE BORDER=0 CELLPADDING=1 WIDTH=100%>
<TR><TD nowrap COLSPAN=2><font size=+1><b>削除モード</b></font><br>	</TD>
<TD nowrap width=5% ALIGN=right VALIGN=top><a name=top></a><a href="<?=$dir_path;?>?">掲示板に戻る</a></TD></TR>
<TR><TD COLSPAN=3>
テスト中です。スレッドごと無くなっちゃうかもしれないので使用する際はご注意
<?php
} 

function foot()
{

	?>
</TD></TR>
</td></tr></table></table>
</body>
</html>
<?php
} 

function d_error($msg)
{
	head();
	echo "<p>Error : " . $msg;
	foot();
	exit();
} 

switch ($mode) {
	case 'del':
		del();
		break;

	default:
		form();
} 

?>