<?php
/**
 * freeStyle bbs	   by ToR
 * 
 * 2ch風なスレッドフローと式掲示板（仮
 * 
 * 2001/06/19 v1.00
 * 2004/02/28 v2.00
 * 
 * ご意見はこちらまで
 * http://php.s3.to/bbs/fstyle/
 */
//error_reporting(E_ALL);
error_reporting(0);

$subj_file = './subject.txt'; //ヘッドラインファイル
$ddir = './dat/'; //ログディレクトリ
$ext = ".txt"; //データファイルの拡張子
$sub_back = './subback.txt'; //過去用ヘッドラインファイル
$last_file = "./lastsize.cgi"; //二重カキコチェック用

$dir_path = 'http://'.$_SERVER['HTTP_HOST'].preg_replace('|[^/]*?\.php.*$|U', '', $_SERVER['PHP_SELF']); //設置ディレクトリの絶対ﾊﾟｽ (ｵｰﾄ設置)

$page_def = 10; //１ページに何スレッド？
$res_def = 7; //１スレッドに何個レス？
$thre_def = 45; //ヘッドライン表示最大数
$read_def = 50; //最新何個レス
$extag = 1; //独自タグを使う？
$imgtag = 1; //上記１の場合、imgタグ使う？
$nanasi = '名無しさん'; //無記名時の名前
$thre_cut = 26; //スレ表示文字数(追加

$viewline = 25; //表示時の最大行数（これ以上はリンク）
$viewbyte = 3000; //表示時の最大バイト数（これ以上はリンク）
$postline = 45; //投稿時の最大行数
$postbyte = 4000; //投稿時の最大バイト数
$numlimit = 1000; //レス最大投稿数（これ以上は書き込めない）
$taborn = 2; //透明あぼーん

// admin.phpで使用
$ad_id = "admin"; //管理用ID
$ad_pass = "0123"; //管理用パスワード
$aborn = "あぼーん"; //レス削除時のメッセージ

$ext_cgi = ".cgi";
// スレスト時の名前、時間、コメント
$st_name = "停止しました";
$st_date = "停止";
$st_com = "スレッドストッパー。。。(￣ー￣)ﾆﾔﾘｯ";
// HTML化保存ディレクトリ(777)と拡張子
$kdir = "./kako/";
$kext = ".html";

$pc_index = "fstyle.html";
$NIch_index = "2ch.html";
$mb_index = "./i/index.html"; 
// PC用ｽｷﾝ
$bbs_skin_pc = "fstyle.tpl";
$NIch_skin_pc = "2ch.tpl";
$read_skin_pc = "read.tpl"; 
// ｹｲﾀｲ用スキン
$bbs_skin_mb = "mb.tpl";
$read_skin_mb = "read_mb.tpl";

$tit = 'freeStyle';	//<title>tag的文字(html title)
$rmes= '關於這板的bug、問題、要求等，歡迎在此提出。';
$tmes= '';	//top的title與message
$kanban = ''; //看板のタグ
$TZ='+8'; //Timezone

$ngfiles=array('./spamdata.cgi','./ngwords.cgi');
$rengfile='./rengwords.cgi';
$extipq=7;
$faillog='./nglog.log';

if(isset($_SERVER['ORIG_PATH_INFO'])) $_SERVER['PATH_INFO']=$_SERVER['ORIG_PATH_INFO']; //PHP5 workaround

$read_def_mb = 10; //携帯1ページレス表示数
if ($_GET['ua'] == "i" || preg_match("/\.(ido|ezweb|docomo|pdx|pipopa|jp-[cdhknqrst])\.ne\.jp$/", gethostbyaddr(getenv('REMOTE_ADDR')))) {
	$bbs_skin = $bbs_skin_mb; //TOPスキン
	$read_skin = $read_skin_mb; //レス表示スキン
	$read_def = $read_def_mb; //1ページレス表示数
} else {
	$bbs_skin = $bbs_skin_pc;
	$read_skin = $read_skin_pc;
	$read_def = 50;
} 

if (phpversion() < "4.1.0") {
	$_GET = $HTTP_GET_VARS;
	$_POST = $HTTP_POST_VARS;
	$_COOKIE = $HTTP_COOKIE_VERS;
	$_SERVER = $HTTP_SERVER_VERS;
} 
$PHPSELF = $_SERVER['PHP_SELF'];

// $host = @gethostbyaddr (getenv("REMOTE_ADDR"));
function error($emsg, $name = "", $mail = "", $host = "", $msg = "")
{
	global $dir_path;
	print "<head><title>ＥＲＲＯＲ！</title>";
	print "<meta http-equiv=\"Content-Type\" content=\"text/html; ";
	print "charset=utf-8\"></head><body bgcolor=\"#FFFFFF\">";
	print "<!-- 2ch_X:error -->\n";
	print "<font size=+1 color=#FF0000><b>ＥＲＲＯＲ：$emsg</b>";
	print "</font><ul><br>ホスト<b>$host</B><br>";
	print "<b> </b><br>名前： $name<br>E-mail： $mail<br>内容：<br>";
	print "$msg<br><br></ul><small>こちらでリロードしてください。";
	print "<a href=\"$dir_path\"> GO! </a><hr>\n";
	print "</small></body></html>";
	exit();
} 

function nglog_append($ip,$tim,$msg){
	global $faillog;
	$fp = fopen($faillog,'ab') or fopen($faillog,'wb');
	flock($fp,2);
	fputs($fp,"$ip\t$tim\t$msg\n");
	fclose($fp);
}

?>
