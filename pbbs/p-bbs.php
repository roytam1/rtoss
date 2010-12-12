<?php
/*
 * P-BBS by ToR
 * http://php.s3.to
 *
 * 2000/12/02 pre	完成
 * 2001/03/06 v1.0 完成ー
 * 2001/03/11 v1.1 HTML書き出すOnOff、書き込み後Locationで飛ばす、管理ﾓｰﾄﾞpass→apass
 * 2001/04/16 v1.2 過去ログ対応、＞がつくと色変わる。デザイン変更
 * 2001/04/24 v1.23 書き込み後表示関数化、ページング変更、管理ﾓｰﾄﾞ実行後修正、ホスト表示、Re:[2]
 * 2001/05/04 v1.231 クッキーをHTMLに書き出してしまうバグ修正,過去ログモードの非表示<br>
 * 2001/05/17 v1.232 文字数制限、行数制限追加
 * 2001/05/27 v1.24 autolink修正、書き込み後refreshで飛ばす
 * 2001/06/02 v1.25 GET投稿禁止、外部投稿禁止
 * 2001/11/15 v1.26 >の後のスペース無くす。PHP3の時レスで<br>となるバグ修正
 * 2002/05/25 v1.27 i18n削除、空白ﾁｪｯｸ修正
 * 2002/02/11 v1.28 クッキーの文字化け対策
 * 2003/05/25 v1.29 禁止ホスト、禁止ワード追加
 * 2003/06/07 v1.3	複数削除出来るように
 *
 * シンプルな掲示板です。管理モード付
 * 空のログファイルを用意して、パーミッションを606にしてください
 * HTMLを書き出す場合は、そのディレクトリが707か777じゃないとダメです
 */

/****
 P-BBS AA / Arranged by roytam1

 Changelog:
 2005~2007   AA Hack, text-to-png, Trip, gzipped log, quick delete js, pastlog loading optimization
 2007/03/07  Simple Field Trap
 2009/09/23  Add non-transparent txt2png link, dnsbl
 2009/10/27  Add exclusive fopen file locking, remove list.dsbl.org from externalIPQuery list, simple double post dectector
****/
$mode='';
if (!isset($_SERVER)) {
	 $_GET = &$HTTP_GET_VARS;
	 $_POST = &$HTTP_POST_VARS;
	 $_ENV = &$HTTP_ENV_VARS;
	 $_SERVER = &$HTTP_SERVER_VARS;
	 $_COOKIE = &$HTTP_COOKIE_VARS;
	 $_REQUEST = array_merge($_GET, $_POST, $_COOKIE);
}
extract($_REQUEST);
extract($_SERVER);
if(@!isset($trans)) $trans = 1;

// Javascript Simple Field Trap
if($mode=="regist") {
if(strpos($url,'!')!==0) die("Trapped.");
else $url=substr($url,1);
}
//-------------設定ここから-------------
/* <title>に入れるタイトル */
$title1 = 'P-BBS';
/* 掲示板のTOPタイトル（HTML可）*/
$title2 = "<font size=5 face=Verdana color=gray><b>$title1</b></font>";
/* <body>タグ */
$body = '<body bgcolor="#ddf2ed" text="#444444" link="#0000AA">';

/* 管理者用パスワード。必ず変更して下さい。*/
$admin_pass = '0123';

/* ログ保存ファイル */
$logfile = 'bbs.log';

/* ログ保存ファイルを圧縮する （yes=1 no=0）*/
$gzlog=0;

/* TOPページをHTMLに書き出すか （yes=1 no=0）*/
$htmlw = 1;
/* 静的HTMLを書き出す場合のHTMLファイル */
$html_file = 'pbbs.html';

/* 戻り先（HOME）*/
$home = 'http://cuc.moe.hm/';
/* 一ページあたりの表示記事数 */
$page_def = 20;
/* 最大記録件数 これを越えると古い物から過去ログへ移ります。*/
$max = 800;
/* 文字数制限（名前、題名、本文）全角だとこの半分です */
$maxn	= 50;
$maxs	= 100;
$maxv	= 30000;
/* 本文の改行数制限 */
$maxline = 175;
/* 同一ホストからの連続投稿を制限
	--> 秒数を記述するとその時間以上を経過しないと連続投稿できない*/
$w_regist = 3;
/* 文中で自動リンクするかどうか（yes=1 no=0）*/
$autolink = 1;
/* HTMLタグを有効にするか（yes=1 no=0）*/
$tag = 0;
/* タイトル無しで投稿された場合 */
$mudai = '(無題)';
/* ＞がついた時の色 */
$re_color = "#225588";
/* ホストを表示するか（表示しない=0 <!-->内で表示=1 表示=2）*/
$hostview = 1;
/* 外部書き込み禁止にする?(する=1,しない=0) */
define("GAIBU", 0);
/* 名無し書き込み禁止にする?（no=1 yes=0）*/
define("ANONY_POST", 1);

/* 使用するファイルロックのタイプ（mkdir=1 flock=2 fopen-x=3 使わない=0）*/
define("LOCKEY", 3); 		//通常は2でOK
/* mkdirロックを使う時はlockという名でディレクトリを作成して777にしてください */
define("LOCK" , "lock/plock");	//lockの中に作るロックファイル名

/* 過去ログ作成する? */
$past_key = 1;
/* 過去ログ番号ファイル */
$past_no	= "pastno.log";
/* 過去ログ作成ディレクトリ(書き込み権限必要) */
$past_dir = "./";
/* 過去ログ一つに書き込む行数 */
$past_line= "50";
$past_prefix = "past";

// 閲覧禁止?
$no_host_read = 0;

// 禁止ホスト（正規表現可
$no_host[] = 'anonymizer.com';

// 使用禁止ワード
//$no_word[] = '死ね';
//$no_word[] = '馬鹿';

//---------設定ここまで--------------
if($gzlog) {
include_once("./lib_gzip.php");
}

function externalIPQuery($addr) {
	global $EXTIPQ;
	if(!isset($GLOBALS['EXTIPQ'])) $EXTIPQ=7;
	$flg=0;$cnt=0;
	if($EXTIPQ && $addr != "127.0.0.1") {
		$rev = implode('.', array_reverse(explode('.', $addr)));
		$queries = array( 'bbx.2ch.net','dnsbl.ahbl.org','niku.2ch.net','cbl.abuseat.org','sbl-xbl.spamhaus.org','bl.blbl.org','bl.spamcop.net','virus.rbl.jp','ircbl.ahbl.org','tor.ahbl.org' );
		foreach ( $queries as $query ) {
			$qres=gethostbyname($rev.'.'.$query);
			if($rev.'.'.$query!=$qres){ $flg=1; break; }
			$cnt++;
			if($cnt>=$EXTIPQ) break;
		}
	}
	return $flg;
}

function matchCIDR($addr, $cidr) {
	list($ip, $mask) = explode('/', $cidr);
	return (ip2long($addr) >> (32 - $mask) == ip2long($ip.str_repeat('.0', 3 - substr_count($ip, '.'))) >> (32 - $mask));
}

// 禁止ホスト
function hostblock() {
	global $no_host;
	if (is_array($no_host)) {
		$HOST = strtolower(gethostbyaddr($IP=getenv("REMOTE_ADDR")));
		$checkTwice = ($IP != $HOST);
		$IsBanned = false;
		foreach($no_host as $pattern){
			$slash = substr_count($pattern, '/');
			if($slash==2){ // RegExp
				$pattern .= 'i';
			}elseif($slash==1){ // CIDR Notation
				if(matchCIDR($IP, $pattern)){ $IsBanned = true; break; }
				continue;
			}elseif(strpos($pattern, '*')!==false || strpos($pattern, '?')!==false){ // Wildcard
				$pattern = '/^'.str_replace(array('.', '*', '?'), array('\.', '.*', '.?'), $pattern).'$/i';
			}else{ // Full-text
				if($IP==$pattern || ($checkTwice && $HOST==strtolower($pattern))){ $IsBanned = true; break; }
				continue;
			}
			if(preg_match($pattern, $HOST) || ($checkTwice && preg_match($pattern, $IP))){ $IsBanned = true; break; }
		}
		if($IsBanned || externalIPQuery($IP)) die("403");
	}
}

if($no_host_read) hostblock();

function head(&$dat){ 		//ヘッダー表示部
	global $mode,$no,$PHP_SELF,$logfile,$gzlog,$title1,$title2,$body,$p_bbs,$htmlw,$max,$page_def;
	$r_sub='';$r_com='';

	if($mode == "resmsg"){	//レスの場合
		$res = $gzlog?ungzlog($logfile):file($logfile);
		$flag = 0;
		while (list($key, $value) = each ($res)) {
			list($rno,$date,$name,$email,$sub,$com,$url) = explode("<>", $value);
			if ($no == "$rno"){ $flag=1; break; }
		}
		if ($flag == 0) error("該当記事が見つかりません");

		if(preg_match("/Re\[([0-9]+)\]:/", $sub, $reg)){
			$reg[1]++;
			$r_sub=preg_replace("/Re\[([0-9]+)\]:/", "Re[$reg[1]]:", $sub);
		}elseif(preg_match("/^Re:/", $sub)){
			$r_sub=preg_replace("/^Re:/", "Re[2]:", $sub);
		}else{ $r_sub = "Re:$sub"; }
		$r_com = "&gt;$com";
		$r_com = preg_replace("#<br( /)?>#i","\r&gt;",$r_com);
	}
	$head='<html lang="ja"><head>
<META HTTP-EQUIV="Content-type" CONTENT="text/html; charset=UTF-8">
<META HTTP-EQUIV="pragma" CONTENT="no-cache">
<title>'.$title1.'</title>
<style type="text/css">
<!--
	body,tr,td,tbody,blockquote,tt,pre,textarea { font-size:12pt; font-family: "MS PGothic Ex+","MS-PGothic","MS PGothic","ＭＳ Ｐゴシック","Osaka-AA","Mona","mona-gothic-jisx0208.1990-0",sans-serif; line-height:1em; }
	a {text-decoration: none;}
	a:hover {text-decoration: underline;}
	code {border-bottom: 1px dotted; font-weight:normal;}
-->
</style>
<script>
<!--
function l(e){var P=getCookie("p_bbs"),i;var pb=P.split(",");if(pb.length!=4) return false;with(document){for(i=0;i<forms.length;i++){if(forms[i].pwd)with(forms[i]){pwd.value=pb[2];}if(forms[i].password)with(forms[i]){password.value=pb[2];}if(forms[i].name)with(forms[i]){name.value=pb[0];}if(forms[i].url)with(forms[i]){url.value=pb[3];}if(forms[i].email)with(forms[i]){email.value=pb[1].replace("+"," ");}}}};onload=l;
function getCookie(key, tmp1, tmp2, xx1, xx2, xx3) {tmp1 = " " + document.cookie + ";";xx1 = xx2 = 0;len = tmp1.length;	while (xx1 < len) {xx2 = tmp1.indexOf(";", xx1);tmp2 = tmp1.substring(xx1 + 1, xx2);xx3 = tmp2.indexOf("=");if (tmp2.substring(0, xx3) == key) {return(unescape(tmp2.substring(xx3 + 1, xx2 - xx1 - 1)));}xx1 = xx2 + 1;}return("");}
function s(o){if(o.url.value.match(/^http/)) o.url.value="!"+o.url.value;}
-->
</script>
</head>';
$dat=$head.$body.'
<form method="POST" action="'.$PHP_SELF.'" enctype="multipart/form-data" onsubmit="s(this);return true;">
<input type="hidden" name="mode" value="regist">
<BASEFONT SIZE="3">'.$title2.'<hr size=1><br>
<TT>
お名前 <input type=text name="name" size=20 value="" maxlength=24><br>
メール <input type=text name="email" size=30 value=""><br>
題名　 <input type=text name="sub" size=30 value="'.$r_sub.'">
<input type=submit value="    投稿    "><input type=reset value="消す"><br>
<textarea name="com" rows=7 cols=82 wrap="off">'.$r_com.'</textarea><br><br>
ＵＲＬ　 <input type=text name="url" size=70 value="http://"><noscript>＊「!」から始まってください。 例えば、「!http://...」</noscript><br>
削除キー <input type=password name="password" size=8 value="">(記事の削除用。英数字で8文字以内)
</form></TT>
<hr size=1><font size=-2>新しい記事から表示します。最高'.$max.'件の記事が記録され、それを超えると古い記事から過去ログへ移ります。<br>
 １回の表示で'.$page_def.'件を越える場合は、下のボタンを押すことで次の画面の記事を表示します。</font>
';
}
function foot(&$dat){ //フッター表示部
	global $PHP_SELF,$home,$past_key;

$dat.='<div align="right"><a name="form"></a><form method="POST" action="'.$PHP_SELF.'" name=userdel>
<input type=hidden name=mode value="usrdel">No <input type=text name=no size=2>
pass <input type=password name=pwd size=4 maxlength=8>
<input type=submit value="Del"></form>
[ <a href="'.$home.'">ホーム</a> ] [ <a href="'.$PHP_SELF.'?mode=admin">管理</a> ] ';
if($past_key) $dat.='[ <a href="'.$PHP_SELF.'?mode=past">過去ログ</a> ]';
$dat.='<br><br><small><!-- P-BBS v1.232 --><!-- RTHack 20060116 -->
- <a href="http://php.s3.to" target="_top">P-BBS</a> + arranged by <a href="http://scrappedblog.blogspot.com" target="_top">roytam1</a> -</small></div>
</body></html>';
}
function Main(&$dat){	//記事表示部
	global $logfile,$gzlog,$page_def,$page,$PHP_SELF,$autolink,$re_color,$hostview;
	$p=0;

	$view = $gzlog?ungzlog($logfile):file($logfile);
	$total = sizeof($view);
	$total2 = $total;

	$start = (isset($page)) ? $page : 0;
	$end = $start + $page_def;
	$st	= $start + 1;

	for($s = $start;$s < $end;$s++){
		if(@!$view[$s]) break;
		list($no,$now,$name,$email,$sub,$com,$url,$host,$pw) = explode("<>", $view[$s]);

		if($url){ $url = "<a href=\"http://$url\" target=\"_blank\">http://$url</a>";}
		if($email){ $name = "<a href=\"mailto:$email\">$name</a>";}
		// ＞がある時は色変更
		$com = preg_replace("/(^|>)(&gt;[^<]*)/i", "\\1<font color=$re_color>\\2</font>", $com);
		// URL自動リンク
		if ($autolink) { $com=auto_link($com); }
		// Host表示形式
		if($hostview==1){ $host="<!--$host-->"; }
		elseif($hostview==2){ $host="[ $host ]"; }
		else{ $host=""; }

		$dat.='<hr size=1>[<a href="'.$PHP_SELF.'?mode=resmsg&no='.$no.'" name="'.$no.'">'.$no.'</a>] ';
		$dat.='<font size="+1" color="#D01166"><b>'.$sub.'</b></font>';
		$dat.='　Name：<font color="#007000"><b>'.$name.'</b></font><font size="-1">　Date： '.$now.'</font> <font size="-1">[<a href="'.$PHP_SELF.'?mode=txt2png&no='.$no.'">画像化</a> <a href="'.$PHP_SELF.'?mode=txt2png&no='.$no.'&trans=0">(非透過)</a>]	[<a href="#form" onclick="document.userdel.no.value='.$no.';">処理</a>]</font>';
		$dat.='<p><blockquote><tt style="white-space: nowrap;">'.$com.'<br></tt>';
		$dat.='</p>'.$url.'<br>'.$host.'</blockquote><br>';

		$p++;
	} //end for
	$prev = $page - $page_def;
	$next = $page + $page_def;
	$dat.= sprintf("<hr size=1> %d 番目から %d 番目の記事を表示<br><center>Page:[<b> ",$st,$st+$p-1);
	$dat.= ($page > 0) ? "<a href=\"$PHP_SELF?page=$prev\">&lt;&lt;</a> " : " ";
	$p_no=1;$p_li=0;
	while ($total > 0) {
		$dat.= ($page == $p_li) ? "$p_no " : "<a href=\"$PHP_SELF?page=$p_li\">$p_no</a> ";
		$p_no++;
		$p_li	= $p_li	+ $page_def;
		$total = $total - $page_def;
	}
	$dat.= ($total2 > $next) ? " <a href=\"$PHP_SELF?page=$next\">&gt;&gt;</a>" : " ";
	$dat.="</b> ]\n";
}
function regist(){	//ログ書き込み
	global $name,$email,$sub,$com,$url,$tag,$past_key,$maxn,$maxs,$maxv,$maxline;
	global $password,$html_url,$logfile,$gzlog,$jisa,$max,$w_regist,$autolink,$mudai,$PHP_SELF,$REQUEST_METHOD,$no_word;

	// 禁止ホスト
	hostblock();

	//クッキー保存
	$cookvalue = implode(",", array($name,$email,$password,$url));
	setcookie ("p_bbs", $cookvalue,time()+14*24*3600);	/* 2週間で期限切れ */

	if($REQUEST_METHOD != "POST") error("不正な投稿をしないで下さい");
	if(GAIBU && !preg_match('/'.$PHP_SELF.'/i',getenv("HTTP_REFERER"))) error("外部から書き込みできません");
	// フォーム内容をチェック
	if(!$name||preg_match("/^( |　)*$/",$name)){ if(ANONY_POST) $name="名無し"; else error("名前が書き込まれていません"); }
	if(!$com||preg_match("/^( |　|\t|\r|\n)*$/",$com)){ error("本文が書き込まれていません"); }
	if(!$sub||preg_match("/^( |　)*$/",$sub)){ $sub=$mudai; }

	if(strlen($name) > $maxn){ error("名前が長すぎますっ！"); }
	if(strlen($sub) > $maxs){ error("タイトルが長すぎますっ！"); }
	if(strlen($com) > $maxv){ error("本文が長すぎますっ！"); }

	// 禁止ワード
	if (is_array($no_word)) {
	foreach ($no_word as $fuck) {
		if (preg_match("/$fuck/", $com)) error("使用できない言葉が含まれています！");
		if (preg_match("/$fuck/", $sub)) error("使用できない言葉が含まれています！");
		if (preg_match("/$fuck/", $name)) error("使用できない言葉が含まれています！");
	}
	}
	$times = time();

	$check = $gzlog?ungzlog($logfile):file($logfile);
	$tail = sizeof($check);

	list($tno,$tdate,$tname,$tmail,$tsub,$tcom,,,$tpw,$ttime) = explode("<>", $check[0]);
	if($name == $tname && $com == $tcom) error("二重投稿は禁止です");

	if ($w_regist && $times - $ttime < $w_regist)
	error("連続投稿はもうしばらく時間を置いてからお願い致します");

	// 記事Noを採番
	$no = $tno + 1;

	// ホスト名を取得
	$host = getenv("REMOTE_HOST");
	$addr = getenv("REMOTE_ADDR");
	if($host == "" || $host == $addr){//gethostbyddrが使えるか
		$host=@gethostbyaddr($addr);
	}

	// 削除キーを暗号化
	$PW = ($password) ? crypt(($password),"aa") : '';

	$now = gmdate("Y/m/d(D) H:i",time()+9*60*60);
	$url = preg_replace("#^http://#","",$url);

	CleanStr($com);
	CleanStr($sub);
	CleanStr($name);
	CleanStr($email);
	CleanStr($url);

	$name = str_replace("◆","◇",$name);
	if(preg_match("/(#|＃)(.*)/",$name,$regs)){ // 使用トリップ(Trip)機能 (ex：無名#abcd)
		$cap = $regs[2];
		$cap = strtr($cap,array("&amp;"=>"&","&#44;"=>","));
		$name = preg_replace("/(#|＃)(.*)/","",$name);
		$salt = substr($cap."H.",1,2);
		$salt = preg_replace("/[^\.-z]/",".",$salt);
		$salt = strtr($salt,":;<=>?@[\\]^_`","ABCDEFGabcdef");
		$name .= "<code>◆".substr(crypt($cap,$salt),-10)."</code>";
	}

	$com = str_replace( " ","&nbsp;", $com);
	$com = str_replace( "\r\n",	"\r", $com);	//改行文字の統一。
	$com = str_replace( "\r","\n", $com);
	/* \n数える（substr_countの代わり）*/
	$temp = str_replace("\n", "\n"."a",$com);
	$str_cnt=strlen($temp)-strlen($com);
	if($str_cnt > $maxline) error("行数が長すぎますっ！");
	$com = preg_replace("/\n((　| |\t)*\n){3,}/","\n",$com);//連続する空行を一行
	$com = nl2br($com);	//改行文字の前に<br>を代入する。
	$com = str_replace( "\n","", $com);	//\nを文字列から消す。

	$new_msg="$no<>$now<>$name<>$email<>$sub<>$com<>$url<>$host<>$PW<>$times\n";

	$old_log = $gzlog?ungzlog($logfile):file($logfile);

	list($ono,$odat,$oname,$oemail,$osub,$ocom,$ourl,$ohost,$opas) = @explode("<>",@$old_log[0]);
	if($com == $ocom) error("連続投稿はお止めください。");

	$line = sizeof($old_log);
	$new_log[0] = $new_msg;//先頭に新記事
	if($past_key && $line >= $max){//はみ出した記事を過去ログへ
		for($s=$max; $s<=$line; $s++){//念の為複数行対応
			past_log($old_log[$s-1]);
		}
	}
	for($i=1; $i<$max; $i++) {//最大記事数処理
		if($i>count($old_log)) break;
		$new_log[$i] = $old_log[$i-1];
	}
	renewlog($new_log);//ログ更新

}
function usrdel(){	//ユーザー削除
	global $admin_pass,$pwd,$no,$logfile,$gzlog;
	if ($no == "" || $pwd == "")
	error("削除Noまたは削除キーが入力モレです");

	$logall = $gzlog?ungzlog($logfile):file($logfile);
	$flag=0;

	while(list(,$lines)=each($logall)){
	list($ono,$dat,$name,$email,$sub,$com,$url,$host,$opas) = explode("<>",$lines);
	if ($no == "$ono") { $flag=1; $pass=$opas; }
	else { $pushlog[]=$lines; }
	}
	if ($flag == 0) { error("該当記事が見当たりません"); }
	if ($pwd != $admin_pass) {
		if ($pass == "") { error("該当記事には削除キーが設定されていません"); }

		// 削除キーを照合
		$match = crypt(($pwd),"aa");
		if (($match != $pass)) { error("削除キーが違います"); }
	}

	// ログを更新
	renewlog($pushlog);
}
function admin(){	//管理機能
	global $admin_pass,$PHP_SELF,$logfile,$htmlw,$gzlog;
	global $del,$apass,$head,$body;
	if ($apass && $apass != "$admin_pass")
	{ error("パスワードが違います"); }
	echo "$head";
	echo "$body";
	echo "[<a href=\"$PHP_SELF?\">掲示板に戻る</a>]";
	if($htmlw) echo "[<a href=\"$PHP_SELF?mode=remake\">TOPページを更新する</a>]";
	echo "<table width='100%'><tr><th bgcolor=\"#508000\">\n";
	echo "<font color=\"#FFFFFF\">管理モード</font>\n";
	echo "</th></tr></table>\n";

	if (!$apass) {
	echo "<P><center><h4>パスワードを入力して下さい</h4>\n";
	echo "<form action=\"$PHP_SELF\" method=\"POST\">\n";
	echo "<input type=hidden name=mode value=\"admin\">\n";
	echo "<input type=password name=apass size=8>";
	echo "<input type=submit value=\" 認証 \"></form>\n";
	}else {
	// 削除処理
	if (is_array($del)) {
		// 削除情報をマッチングし更新
		$delall = $gzlog?ungzlog($logfile):file($logfile);

		for($i=0; $i<count($delall); $i++) {
			list($no,) = explode("<>",$delall[$i]);
			if (in_array($no, $del)) $delall[$i] = "";
		}
		// ログを更新
		renewlog($delall);
	}

	// 削除画面を表示
	echo "<form action=\"$PHP_SELF\" method=\"POST\">\n";
	echo "<input type=hidden name=mode value=\"admin\">\n";
	echo "<input type=hidden name=apass value=\"$apass\">\n";
	echo "<center><P>削除したい記事のチェックボックスにチェックを入れ、削除ボタンを押して下さい。\n";
	echo "<P><table border=0 cellspacing=0>\n";
	echo "<tr bgcolor=bbbbbb><th>削除</th><th>記事No</th><th>投稿日</th><th>題名</th>";
	echo "<th>投稿者</th><th>コメント</th><th>ホスト名</th>";
	echo "</tr>\n";

	$delmode = $gzlog?ungzlog($logfile):file($logfile);

	if (is_array($delmode)) {
		while (list($l,$val)=each($delmode)){
		list($no,$date,$name,$email,$sub,$com,$url,$host,$pw,$time) = explode("<>",$val);

		list($date,$dmy) = split("\(", $date);
		if ($email) { $name="<a href=\"mailto:$email\">$name</a>"; }
		$com = str_replace("<br />","",$com);
		//$com = htmlspecialchars($com);
		if(strlen($com) > 40){ $com = substr($com,0,38) . " ..."; }

		echo ($l % 2) ? "<tr bgcolor=#F8F8F8>" : "<tr bgcolor=#DDDDDD>";
		echo "<th><input type=checkbox name=del[] value=\"$no\"></th>";
		echo "<th><a href=\"$PHP_SELF?mode=resmsg&no=$no\">$no</a></th><td><small>$date</small></td><td>$sub</td>";
		echo "<td><b>$name</b></td><td><small>$com</small></td>";
		echo "<td>$host</td></tr>\n";
		}
	}

	echo "</table>\n";
	echo "<P><input type=submit value=\"削除する\">";
	echo "<input type=reset value=\"リセット\"></form>\n";

	}
	echo '</center></body></html>';
}
function lock_dir($name=""){//ディレクトリロック
	if($name=="") $name="lock";

	// 3分以上前のディレクトリなら解除失敗とみなして削除
	if ((file_exists($name))&&filemtime($name) < time() - 180) {
	@RmDir($name);
	}

	do{
	if (@MkDir($name,0777)){
		return 1;
	}
	sleep(1);// 一秒待って再トライ
	$i++;
	}while($i < 5);

	return 0;
}
function unlock_dir($name=""){//ロック解除
	if($name=="") $name="lock";
	@rmdir($name);
}
function m_lock_file( $format = null ) {// get/set lock file name
    static $file_format = './%s.lock';
   
    if ($format !== null) {
        $file_format = $format;
    }
   
    return $file_format;
}
function m_lock( $lockId, $acquire = null ) {// acquire/check/release lock
    static $handlers = array();
   
    if (is_bool($acquire)) {
        $file = sprintf(m_lock_file(), md5($lockId), $lockId);
    }
   
    if ($acquire === false) {
        if (isset($handlers[$lockId])) {
            @fclose($handlers[$lockId]);
            @unlink($file);
            unset($handlers[$lockId]);
        } else {
//            trigger_error("Lock '$lockId' is already unlocked", E_USER_WARNING);
        }
    }
   
    if ($acquire === true) {
        if (!isset($handlers[$lockId])) {
            $handler = false;
            $count = 100;
            do {
                if (!file_exists($file) || @unlink($file)) {
                    $handler = @fopen($file, "x");
                }
                if (false === $handler) {
                    usleep(10000);
                } else {
                    $handlers[$lockId] = $handler;
                }
            } while (false === $handler && $count-- > 0);
        } else {
//            trigger_error("Lock '$lockId' is already locked", E_USER_WARNING);
        }
    }
   
    return isset($handlers[$lockId]);
}
function lock_error() {
	error("ロックエラー<br>しばらく待ってからにして下さい");
}

function renewlog($arrline){//ログ更新	入力:配列
	global $logfile,$gzlog;

	if(LOCKEY==1){ lock_dir(LOCK) or lock_error(); }
	if(LOCKEY==3){ m_lock($logfile, true); m_lock($logfile) or lock_error(); }
	$rp = $gzlog?gzopen($logfile, "w"):fopen($logfile, "w");
	if(LOCKEY>=2){ @flock($rp, 2); }
	set_file_buffer($rp, 0);
	fputs($rp, implode("",$arrline));
	@ftruncate($rp, ftell($rp));
	if(!$gzlog) fclose($rp);
	else gzclose($rp);
	if(LOCKEY==1){ unlock_dir(LOCK); }
	if(LOCKEY==3){ m_lock($logfile, false); }
}
function MakeHtml(){	//HTML生成
	global $html_file;

	head($buf);
	Main($buf);
	foot($buf);

	$hp = @fopen($html_file,"w");
	flock($hp,2);
	fputs($hp, $buf);
	fclose($hp);
	@chmod($html_file,0666);
}
function ShowHtml(){
	global $html_file,$htmlw,$page,$mode;

	if(!$htmlw || $page || $mode=='resmsg') {
		head($buf);
		Main($buf);
		foot($buf);
		echo $buf;
	} else {
		if(!is_file($html_file)) MakeHtml();
		if(file_exists("mod_gzip.php")) $html_file="mod_gzip.php?".$html_file;
		echo "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=$html_file?".microtime()."\">";
		#header("Location: $html_file?");
	}
}
function past_log($data){//過去ログ作成
	global $past_no,$past_dir,$past_line,$autolink,$past_prefix;$dat='';

	if(!trim($data)) return 0;

	$fc = @fopen($past_no, "r") or die(__LINE__.$past_no."が開けません");
	$count = fgets($fc, 10);
	fclose($fc);
	$pastfile = $past_dir.$past_prefix.$count.".html";
	if(file_exists($pastfile)) $past = file($pastfile);

	if(@sizeof($past) > $past_line){
	$count++;
	$pf = fopen($past_no, "w");
	fputs($pf, $count);
	fclose($pf);
	$pastfile = $past_dir.$past_prefix.$count.".html";
	$past = "";
	}

	list($pno,$pdate,$pname,$pemail,$psub,$pcom,$purl,$pho,$ppw) = explode("<>", $data);

	if($purl){ $purl = "<a href=\"http://$purl\" target=\"_blank\">HP</a>";}
	if($pemail){ $pname = "<a href=\"mailto:$pemail\">$pname</a>";}
	// ＞がある時は色変更
	$pcom = preg_replace("/(&gt;)([^<]*)/i", "<font color=999999>\\1\\2</font>", $pcom);
	// URL自動リンク
	if ($autolink) { $pcom=auto_link($pcom); }

	$dat.="<hr>[$pno] <font color=\"#009900\"><b>$psub</b></font> Name：<b>$pname</b> <small>Date：$pdate</small> $purl<br><ul>$pcom</ul><!-- $pho -->\n";

	$np = fopen($pastfile, "w");
	fputs($np, $dat);
	if(isset($past)){
	while(list(, $val)=each($past)){ fputs($np, $val); }
	}
	fclose($np);
	@chmod($pastfile,0666);

}
function past_view(){
	global $past_no,$past_dir,$past_line,$past_prefix,$body,$pno,$PHP_SELF;

	$pno = htmlspecialchars($pno);

	$fc = @fopen($past_no, "r") or die(__LINE__.$past_no."が開けません");
	$count = fgets($fc, 10);
	fclose($fc);
	if(!$pno) $pno = $count;
	echo '<html><head><title>■ 過去ログ '.$pno.' ■</title>
<style type="text/css">
<!--
	body,tr,td,tbody,blockquote,tt,pre,textarea { font-size:12pt; font-family: "MS PGothic Ex+","MS-PGothic","MS PGothic","ＭＳ Ｐゴシック","Osaka-AA","Mona","mona-gothic-jisx0208.1990-0",sans-serif; line-height:1em; }
	a {text-decoration: none;}
	a:hover {text-decoration: underline;}
	code {border-bottom: 1px dotted; font-weight:normal;}
-->
</style></head>
'.$body.'<font size=2>[<a href="'.$PHP_SELF.'?">掲示板に戻る</a>]</font><br>
<center>■ 過去ログ '.$pno.' ■<P>new← ';
	$pastkey = $count;
	while ($pastkey > 0) {
	if ($pno == $pastkey) {
		echo "[<b>$pastkey</b>]";
	} else {
		echo "<a href=\"$PHP_SELF?mode=past&pno=$pastkey\">[$pastkey]</a>";
	}
	$pastkey--;
	}
	echo ' →old</center>'.$past_line.'件ずつ表示';
	$pastfile = $past_dir.$past_prefix.$pno.".html";
	if(!file_exists($pastfile)) error("<br>過去ログがみつかりません");
	readfile($pastfile);
	die("</body></html>");
}
function auto_link($proto){//自動リンク5/25修正
	$proto = preg_replace("#(https?|ftp|news)(://[[:alnum:]\+\$\;\?\.%,!#~*/:@&=_-]+)#","<a href=\"\\1\\2\" target=\"_blank\">\\1\\2</a>",$proto);
	return $proto;
}
function error($mes){	//エラーフォーマット
	global $body;
	?>
<html><head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8"></head>
<?php echo $body; ?>
<br><br><hr size=1><br><br>
<center><font color=red size=5><b><?php echo $mes; ?></b></font></center>
<br><br><hr size=1></body></html>
	<?php
	exit;
}

function get_comment($sno) {
	global $logfile,$gzlog,$PHP_SELF,$hostview;
	$view = $gzlog?ungzlog($logfile):file($logfile);
	$total2= count($view);
	$s=0;$no=0;

	while($s < $total2 && $no!=$sno){
	if(@!$view[$s]) break;
		list($no,$now,$name,$email,$sub,$com,$url,$host,$pw) = explode("<>", $view[$s]);
		$s++;
	}
	return $com;
}
/* 文字修整 */
function CleanStr(& $str){
	global $tag;
	$str = trim($str); // 去除前後多於空白
	if (get_magic_quotes_gpc()) {// "＼"斜線符號去除
		$str = stripslashes($str);
	}
	if(!$tag){ // 管理員可以使用HTML標籤
		$str = preg_replace("/&amp;(#x[0-9A-Fa-f]+|#[0-9]+|[a-z]+);/i", "&$1;", htmlspecialchars($str)); // 非管理員：HTML標籤禁用
	}
}
/*=====================
		 メイン
 ======================*/
//ini_set('display_errors', 1);
//ini_set('error_reporting', E_ALL);
ini_set("memory_limit", "32M");
switch($mode):
	case 'regist':
		regist();
		if($htmlw) MakeHtml();
		if(file_exists("mod_gzip.php")) $html_file="mod_gzip.php?".$html_file;
		echo "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=$PHP_SELF?\">";
#		header("Location: $PHP_SELF?");
		break;
	case 'txt2png':
		include_once('./txtrender.php');
		text2etc(get_comment($no),'utf8','png',$trans);
		break;
	case 'admin':
		admin();
		break;
	case 'remake':
		if($htmlw) MakeHtml();
		echo "<META HTTP-EQUIV=\"refresh\" content=\"0;URL=$PHP_SELF?\">";
//		ShowHtml();
		break;
	case 'usrdel':
		usrdel();
		if($htmlw) MakeHtml();
		ShowHtml();
		break;
	case 'past':
		past_view();
		break;
	default:
		ShowHtml();
		break;
	endswitch;
?>