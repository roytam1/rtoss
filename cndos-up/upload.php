<?php
/*********************************************
	Wastepaper Uploader v1.5
	http://utu.under.jp/
	PHPあぷろだを元にW3Cに準拠したXHTMLを目指すべく
	スクリプト及びHTML文書を改造いたしました。
	結構適当なXHTMLかもしれませんがW3C信者様お許しを…。
	------------------設置法-------------------
	設置ディレクトリは755
	img、imgsディレクトリは777
	upload.php、sum.phpは755
	up.log、last.cgi、count.txtは666
	------------------Based--------------------
	source by PHPあぷろだ v2.2
	http://php.s3.to
	
	source by ずるぽん
	http://zurubon.virtualave.net/

	もしもの為の.htaccess （CGI禁止SSI禁止Index表示禁止）
	Options -ExecCGI -Includes -Indexes
	.txtでも、中身がHTMLだと表示されちゃうので注意
	-------------------------------------------
 *********************************************/
error_reporting(0);
session_start();
if(phpversion()>='4.1.0'){//PHP4.1.0以降対応
	$_GET = array_map('_clean', $_GET);
	$_POST = array_map('_clean', $_POST);
	//$_POST = array_map('_clean', $_GET);
	extract($_GET);
	extract($_POST);
	extract($_COOKIE);
	extract($_SERVER);
	$upfile_type=_clean($_FILES['upfile']['type']);
	$upfile_size=$_FILES['upfile']['size'];//某所で気づく・・・
	$upfile_name=_clean($_FILES['upfile']['name']);
	$upfile=$_FILES['upfile']['tmp_name'];
}

include_once('./settings.php');

if($act=='envset'){
	$cookval = @implode(',', array($acte,$user,$come,$sizee,$mimee,$datee,$orige)); 
	setcookie('upcook', $cookval,time()+365*24*3600);
	$act='';
}
elseif($act=='logout') {
	unset($_SESSION['upuser']);
	$act='';
}
function _clean($str) {
	if(is_array($str)) return $str;
	$str = htmlspecialchars($str);
	if (get_magic_quotes_gpc()) $str = stripslashes($str);
	return $str;
}
$unique_id = uniqid('');
/* ここからヘッダー */
?>
<?php echo '<?xml version="1.0" encoding="utf-8"?>'."\n" ?>
<!DOCTYPE html
	 PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	 "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="zh">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" media="all" />
<title><?php echo "$title" ?></title>
<script type="text/javascript" src="upload.js"></script>
<script type="text/javascript"><!--//--><![CDATA[//><!--
var up_uid='<?php echo($unique_id)?>';
//--><!]]></script>
<link href="style.css" type="text/css" rel="stylesheet" />
</head>
<body onload="if(gID('pass'))gID('pass').value=getCookie('updelpass');if(gID('delpass'))gID('delpass').value=getCookie('updelpass');return true;">

<h1><?php echo $title; ?></h1>
<p>請勿在論壇外轉載此區任何資料。</p>
<?php
/* ヘッダーここまで */
$foot = <<<FOOT

<h2>著作権表示</h2>
<p id="link"><a href="http://php.s3.to/">レッツPHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a> + <a href="http://scrappedblog.blogspot.com/">Roytam1</a></p>

</body>
</html>
FOOT;

function FormatByte($size){//バイトのフォーマット（B→kB）
	$suffix=''; $suxAry=array('KB','MB','GB','TB');
	$ccnt=count($suxAry);
	for($cnt=0;$cnt<$ccnt;$cnt++)
		if($size > 1024) {$size/=1024; $suffix=$suxAry[$cnt];}
	return $suffix?sprintf("%.1f",$size).$suffix:$size.'B';
}
function paging($page, $total){//ページリンク作成
	global $PHP_SELF,$page_def,$act;

	$acts=$act?'&act='.$act:'';

	for ($j = 1; $j * $page_def < $total+$page_def; $j++) {
		if($page == $j){//今表示しているのはﾘﾝｸしない
			$next .= "[<strong>$j</strong>]";
		}else{
			$next .= sprintf('[<a href="%s?page=%d%s">%d</a>]', $PHP_SELF,$j,$acts,$j);//他はﾘﾝｸ
		}
	}
	if($page=="all") return sprintf('Page: %s [ALL]',$next);
	else return sprintf ('Page: %s [<a href="%s?page=all%s">ALL</a>]',$next,$PHP_SELF,$acts);
}
function error($mes1=""){//えっらーﾒｯｾｰｼﾞ
	global $foot;

	echo $mes1;
	echo $foot;
	exit;
}
function _get($type,$host,$port='80',$path='/',$data='') {
	$_err = 'lib sockets::'.__FUNCTION__.'(): ';
	switch($type) {
		case 'http':
			$type = '';
			break;
		case 'https':
			$type = 'ssl';
			break;
		case 'ssl':
			break;
		default:
			die($_err.'bad type '.$type);
	}
	if(!ctype_digit($port)) die($_err.'bad port '.$port);
	if(!empty($data)) foreach($data AS $k => $v) $str .= urlencode($k).'='.urlencode($v).'&'; $str = substr($str,0,-1);

	$fp = fsockopen($host,$port,$errno,$errstr,$timeout=30);
	if(!$fp) die($_err.$errstr.$errno); else {
			fputs($fp, "POST $path HTTP/1.1\r\n");
			fputs($fp, "Host: $host\r\n");
			fputs($fp, "Content-type: application/x-www-form-urlencoded\r\n");
			fputs($fp, "Content-length: ".strlen($str)."\r\n");
			fputs($fp, "Connection: close\r\n\r\n");
			fputs($fp, $str."\r\n\r\n");

			while(!feof($fp)) $d .= fgets($fp,4096);
			fclose($fp);
	} return $d;
} 

/* start */
$limitb = $limitk * 1024;
$host = @gethostbyaddr($REMOTE_ADDR);
if(!$upcook) $upcook=@implode(",",array($f_act,$f_usr,$f_com,$f_size,$f_mime,$f_date,$f_orig));
list($c_act,$c_usr,$c_com,$c_size,$c_mime,$c_date,$c_orig)=explode(',',$upcook);

/* アクセス制限 */
if(is_array($denylist)){
	foreach($denylist as $line){
		if(strstr($host, $line)) error('<h2>錯誤</h2>
<p class="error">存取限制:您沒有使用權限</p>');
	}
}
/* 削除実行 */
if($delid && $delpass!=''){
	$old = file($logfile);
	$find = false;
	for($i=0; $i<count($old); $i++){
		list($did,$dext,,,,,,,$dpwd,)=explode("\t",$old[$i]);
		if($delid==$did){
			$find = true;
			$del_ext = $dext;
			$del_pwd = rtrim($dpwd);
		}else{
			$new[] = $old[$i];
		}
	}
	if(!$find) error('<h2>錯誤</h2>
<p class="error">刪除錯誤:此檔案找不到</p>');
	if($delpass == $admin || substr(md5($delpass), 2, 7) == $del_pwd){
		if(file_exists($updir.$prefix.$delid.'.'.$del_ext)) unlink($updir.$prefix.$delid.'.'.$del_ext);
		if(file_exists($upsdir.$prefix.$delid.'.'.$del_ext)) unlink($upsdir.$prefix.$delid.'.'.$del_ext);
		$fp = fopen($logfile, 'w');
		flock($fp, 2);
		fputs($fp, @implode("",$new));
		fclose($fp);
	}else{
		error('<h2>錯誤</h2>
<p class="error">刪除錯誤:密碼錯誤</p>');
	}
}
/* 削除フォーム */
if($del){
	error("<h2>刪除檔案</h2>
<form method=\"post\" enctype=\"multipart/form-data\" action=\"$PHP_SELF\">
<p>請輸入密碼<br />
<input type=\"hidden\" name=\"delid\" value=\"".htmlspecialchars($del)."\" />
<input type=\"password\" size=\"12\" name=\"delpass\" id=\"delpass\" class=\"box\" tabindex=\"1\" accesskey=\"1\" />
<input type=\"submit\" value=\"刪除\" tabindex=\"2\" accesskey=\"2\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>
");
}
/* 環境設定フォーム */
if($act=="env"){
	error("<h2>環境設定</h2>
<form method=\"get\" action=\"$PHP_SELF\">
<p><input type=\"hidden\" name=\"act\" value=\"envset\" /></p>
<h3>顯示設定</h3>
<ul>
<li><input type=\"checkbox\" name=\"acte\" value=\"checked\" tabindex=\"1\" accesskey=\"1\" $c_act />刪</li>
<li><input type=\"checkbox\" name=\"user\" value=\"checked\" tabindex=\"2\" accesskey=\"2\" $c_com />用戶</li>
<li><input type=\"checkbox\" name=\"come\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_com />備註</li>
<li><input type=\"checkbox\" name=\"sizee\" value=\"checked\" tabindex=\"4\" accesskey=\"4\" $c_size />大小</li>
<li><input type=\"checkbox\" name=\"mimee\" value=\"checked\" tabindex=\"5\" accesskey=\"5\" $c_mime />MIME</li>
<li><input type=\"checkbox\" name=\"datee\" value=\"checked\" tabindex=\"6\" accesskey=\"6\" $c_date />日期</li>
<li><input type=\"checkbox\" name=\"orige\" value=\"checked\" tabindex=\"7\" accesskey=\"7\" $c_orig />原檔名</li>
</ul>
<p>以上設定將會以 cookie 保存以便再次使用。</p>
<p><input type=\"submit\" value=\"儲存\" tabindex=\"8\" accesskey=\"8\" /><input type=\"reset\" value=\"還原\" tabindex=\"9\" accesskey=\"9\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>");
}
elseif($act=='login') {
	error("<h2>登入</h2>
<form method=\"post\" enctype=\"multipart/form-data\" action=\"$PHP_SELF\">
<p><input type=\"hidden\" name=\"act\" value=\"logging\" />
請以論壇帳戶登入。<br/>
用戶名稱：<input type=\"text\" size=\"12\" name=\"upuser\" class=\"box\" tabindex=\"1\" accesskey=\"1\" /><br />
密碼：<input type=\"password\" size=\"12\" name=\"usrpass\" class=\"box\" tabindex=\"2\" accesskey=\"2\" />
<input type=\"submit\" value=\"登入\" tabindex=\"3\" accesskey=\"3\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>");
}
elseif($act=='logging') {
	$urlparts = parse_url($login_backend);
	if(!isset($urlparts['port'])) {
			if($urlparts['scheme'] == 'http') $urlparts['port'] = '80';
			if($urlparts['scheme'] == 'https') $urlparts['port'] = '443';
	}
	$backend_raw = _get($urlparts['scheme'],$urlparts['host'],$urlparts['port'],$urlparts['path'],array('username'=>$upuser,'password'=>$usrpass));
	$backend_res = substr(strstr($backend_raw,"\r\n\r\n"),4);
	$backend_res = explode("\r\n",$backend_res); array_shift($backend_res); array_pop($backend_res); array_pop($backend_res); array_pop($backend_res); $backend_res = implode("\r\n",$backend_res);

	if(substr($backend_res,0,3)=='OK ') $_SESSION['upuser'] = rtrim(substr($backend_res,3));
	else error($backend_res);

	$act='';
}
elseif($act=='mdel') {
	if(!isset($_POST['mdid'])) error('<h2>錯誤</h2>
<p class="error">刪除錯誤:未選擇檔案</p>');
	if($delpass==$admin) {
		$mdid = array_merge(array('dummy'), $_POST['mdid']);
		$old = file($logfile);
		$find = false;
		for($i=0; $i<count($old); $i++){
			list($did,$dext,)=explode("\t",$old[$i]);
				if(array_search($did,$mdid)){
					$find = true;
					if(file_exists($updir.$prefix.$did.'.'.$dext)) unlink($updir.$prefix.$did.'.'.$dext);
					if(file_exists($upsdir.$prefix.$did.'.'.$dext)) unlink($upsdir.$prefix.$did.'.'.$dext);
				}else{
					$new[] = $old[$i];
				}
		}
	} else {
		$mdid = array_merge(array('dummy'), $_POST['mdid']);
		$old = file($logfile);
		$find = false;
		for($i=0; $i<count($old); $i++){
			list($did,$dext,,,,,,,$dpwd,)=explode("\t",$old[$i]);
				if(array_search($did,$mdid)){
					if(substr(md5($delpass), 2, 7) == rtrim($dpwd)){
						$find = true;
						if(file_exists($updir.$prefix.$did.'.'.$dext)) unlink($updir.$prefix.$did.'.'.$dext);
						if(file_exists($upsdir.$prefix.$did.'.'.$dext)) unlink($upsdir.$prefix.$did.'.'.$dext);
					}
				}else{
					$new[] = $old[$i];
				}
		}
	}
	if(!$find) error('<h2>錯誤</h2>
<p class="error">刪除錯誤:密碼錯誤</p>');
		$fp = fopen($logfile, 'w');
		flock($fp, 2);
		fputs($fp, @implode("",$new));
		fclose($fp);
	$act='';
}
$lines = file($logfile);
/* アプロード書き込み処理 */
if(file_exists($upfile) && !isset($_SESSION['upuser'])) error('<h2>錯誤</h2>
<p class="error">驗證錯誤:請先登入</p>
');
if(file_exists($upfile) && $com && $_SESSION['upuser'] && $upfile_size > 0){
	$usr = $_SESSION['upuser'];
	if(isset($com{$commax+1})) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:備註過長</p>
');
	if($upfile_size > $limitb) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:此檔案過大</p>');
	/* 連続投稿制限 */
	if($last_time > 0){
		$now = time();
		$last = @fopen($last_file, 'r+') or die('<h2>錯誤</h2>
<p class="error">連続投稿用ファイル $last_file を作成してください</p>');
		$lsize = fgets($last, 1024);
		list($ltime, $lip) = explode("\t", $lsize);
		if($host == $lip && $last_time*60 > ($now-$ltime)){
			error('<h2>錯誤</h2>
<p class="error">連続投稿制限中:時間を置いてやり直してください</p>');
		}
		rewind($last);
		fputs($last, "$now\t$host\t");
		fclose($last);
	}
	/* 拡張子と新ファイル名 */
	$pos = strrpos($upfile_name,'.');	//拡張子取得
	$ext = substr($upfile_name,$pos+1,strlen($upfile_name)-$pos);
	$ext = strtolower($ext);//小文字化
	if(!in_array($ext, $arrowext))
		error('<h2>錯誤</h2>
<p class="error">檔尾錯誤:此檔尾無法上傳</p>');
	/* 拒否拡張子はtxtに変換
	for($i=0; $i<count($denyext); $i++){
		if(strstr($ext,$denyext[$i])) $ext = 'txt';
	}
	*/
	foreach($denyupload as $noup) {
		if($upfile_name==$noup) {
			echo '<p class="tline"><script type="text/javascript">location.href="'.$PHP_SELF.'";</script></p>';
			exit();
		}
	}
	
	list($id,) = explode("\t", $lines[0]);//No取得
	$id = sprintf('%03d', ++$id);		//インクリ
	$newname = $prefix.$id.'.'.$ext;

	/* 自鯖転送 */
	move_uploaded_file($upfile, $updir.$newname);//3.0.16より後のバージョンのPHP 3または 4.0.2 後
	//copy($upfile, $updir.$newname);
	chmod($updir.$newname, 0604);

	/* MIMEタイプ */
	if(!$upfile_type) $upfile_type = 'text/plain';//デフォMIMEはtext/plain

	/* コメント他 */
	$com = htmlspecialchars($com);	//タグ変換
	if(get_magic_quotes_gpc()) $com = stripslashes($com);	//￥除去
	$usr = htmlspecialchars($usr);	//タグ変換
	if(get_magic_quotes_gpc()) $usr = stripslashes($usr);	//￥除去

	$now = gmdate('Y/m/d(D)H:i', time()+9*60*60);	//日付のフォーマット
	$pwd = ($pass) ? substr(md5($pass), 2, 7) : '*';	//パスっ作成（無いなら*）

	$dat = @implode("\t", array($id,$ext,$usr,$com,$host,$now,$upfile_size,$upfile_type,$pwd,$upfile_name,));

	if(count($lines) >= $logmax){		//ログオーバーならデータ削除
		for($d = count($lines)-1; $d >= $logmax-1; $d--){
			list($did,$dext,)=explode("\t", $lines[$d]);
			if(file_exists($updir.$prefix.$did.'.'.$dext)) {
				unlink($updir.$prefix.$did.'.'.$dext);
			}
		}
	}

	$fp = fopen ($logfile , 'w');		//書き込みモードでオープン
	flock($fp ,2);
	fputs ($fp, "$dat\n");		//先頭に書き込む
	for($i = 0; $i < $logmax-1; $i++)	//いままでの分を追記
		fputs($fp, $lines[$i]);
	fclose ($fp);
	reset($lines);
	$lines = file($logfile);		//入れなおし
}
foreach($arrowext as $list) $arrow .= $list." ";
/* 投稿フォーム */
echo '<h2>上傳檔案</h2>
<form method="post" enctype="multipart/form-data" action="'.$PHP_SELF.'" onsubmit="setCookie(\'updelpass\',gID(\'pass\').value);startProgress();return true;">
<p>檔案<strong>（最大 '.$limitk.' KB）</strong><br />
<input type="hidden" name="MAX_FILE_SIZE" value="'.$limitb.'" />
<input type="hidden" name="APC_UPLOAD_PROGRESS" id="progress_key" value="'.$unique_id.'"/>
<input type="file" size="40" name="upfile" class="box" tabindex="1" accesskey="1" />
<p>用戶名稱：[<strong>'.(isset($_SESSION['upuser'])?'<a href="'.$PHP_SELF.'?act=logout">'.$_SESSION['upuser'].'</a>':'<a href="'.$PHP_SELF.'?act=login">登入</a>').'</strong>] Del Pass : <input type="password" size="10" name="pass" id="pass" maxlength="10" class="box" tabindex="3" accesskey="3" /></p>
<p>備註（※沒輸入的話檔案將不會被儲存。）<br />
<input type="text" size="45" name="com" value="" class="box" tabindex="4" accesskey="4" />
<input type="submit" value="上傳" tabindex="5" accesskey="5" />
<input type="reset" value="Cancel" tabindex="6" accesskey="6" /></p>
<p>可以上傳的檔尾：'.$arrow.'</p>
<div id="up_progress">上傳中… <span id="progressbartext"></span><div id="progressbarborder"><div id="progressbar"></div></div></div>
</form>
';
/* カウンタ */
echo "<p class=\"uline\">$count_start 開始存取數: ";
if(file_exists($count_file)){
	$fp = fopen($count_file,'r+');//読み書きモードでオープン
	$count = fgets($fp, 64);	//64バイトorEOFまで取得、カウントアップ
	$count++;
	fseek($fp, 0);			//ポインタを先頭に、ロックして書き込み
	flock($fp,2);
	fputs($fp, $count);
	fclose($fp);			//ファイルを閉じる
	echo $count;			//カウンタ表示
}
/* モードリンク */
echo '
　D：刪除檔案</p>
<p class="uline"><a href="'.$home.'">主頁</a> | <a href="'.$PHP_SELF.'?act=mult">多檔刪除</a> | <a href="'.$PHP_SELF.'?act=env">環境設定</a> | <a href="'.$PHP_SELF.'?">重新整理</a> | <a href="sam.php">圖像一覧</a></p>

<h2>檔案一覧</h2>
<p class="uline">';
/* ログ開始位置 */
$st = ($page) ? ($page - 1) * $page_def : 0;
if(!$page) $page = 1;
if($page == 'all'){
	$st = 0;
	$page_def = count($lines);
}
echo paging($page, count($lines));//ページリンク
//メインヘッダ
echo "</p>\n";
if($act=='mult')	echo "<form action=\"$PHP_SELF\" method=POST><input type='hidden' name='act' value='mdel'/>";
echo "<table summary=\"files\">\n<tr>";
if($c_act) echo '<th abbr="delete" scope="col">刪</th>';
echo '<th abbr="name" scope="col">檔名</th>';
if($c_usr) echo '<th abbr="user" scope="col">用戶</th>';
if($c_com) echo '<th abbr="comment" scope="col">備註</th>';
if($c_size) echo '<th abbr="size" scope="col">大小</th>';
if($c_mime) echo '<th abbr="mime" scope="col">MIME</th>';
if($c_date) echo '<th abbr="date" scope="col">日期</th>';
if($c_orig) echo '<th abbr="original name" scope="col">原檔名</th>';
echo "</tr>\n";
//メイン表示
for($i = $st; $i < $st+$page_def; $i++){
	if($lines[$i]=='') continue;
	list($id,$ext,$usr,$com,$host,$now,$size,$mtype,$pas,$orig,)=explode("\t",$lines[$i]);
	$fsize = FormatByte($size);
	if($auto_link) $com = preg_replace('/(https?|ftp|news)(:\/\/[\w\+\$\;\?\.\{\}%,!#~*\/:@&=_-]+)/u', '<a href="$1$2">$1$2</a>',$com);

	$filename = $prefix.$id.'.'.$ext;
	$target = $updir.$filename;

	
	echo '<tr><!--'.str_replace('--','- -',$host)."-->\n";//ホスト表示
	if($c_act) {
		if($act=='mult') echo "<td class=\"del\"><input type=\"checkbox\" name=\"mdid[]\" value=\"$id\"/></td>";
		else echo "<td class=\"del\"><a href=\"$PHP_SELF?del=$id\">D</a></td>";
	}
	echo "<td>[<a href=\"$target\">$filename</a>]</td>";
	if($c_usr) echo "<td>$usr</td>";
	if($c_com) echo "<td>$com</td>";
	if($c_size) echo "<td class=\"size\">$fsize</td>";
	if($c_mime) echo "<td>$mtype</td>";
	if($c_date) echo "<td>$now</td>\n";
	if($c_orig) echo "<td class=\"orig\">\n$orig</td>\n";
	echo "</tr>\n";
	}

echo "</table>\n";
if($act=='mult') echo '<div align="right">密碼:<input type="password" name="delpass" id="delpass"/><input type="submit" name="submit" value="刪除"/></div></form>';
echo '<p class="tline">'.paging($page,count($lines))."</p>\n";
echo $foot;
?>

