<?php
/*********************************************
	k-up SQL 20091030
	http://scrappedblog.blogspot.com/
	Use SQLite2 database instead of flatfile
	Add more useful functions

	Wastepaper Uploader v1.5
	http://utu.under.jp/
	PHPあぷろだを元にW3Cに準拠したXHTMLを目指すべく
	スクリプト及びHTML文書を改造いたしました。
	結構適当なXHTMLかもしれませんがW3C信者様お許しを…。
	------------------設置法-------------------
	設置ディレクトリは755
	fileディレクトリは777
	upload.phpは755
	k-up.sdbは666	
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
	$cookval = @implode(',', array($acte,$dlcnte,$come,$sizee,$mimee,$datee,$dlime,$orige)); 
	setcookie('upcook', $cookval,time()+365*24*3600);
	$act='';
}
$unique_id = uniqid('');
$conn = sqlite_popen($sqlite_file);

/* ここからヘッダー */
function htmlheader() {
	global $title,$banner,$unique_id,$act;
?>
<?php echo '<?xml version="1.0" encoding="utf-8"?>'."\n" ?>
<!DOCTYPE html
	 PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	 "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="zh">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" media="all" />
<title><?php echo $title; ?></title>
<script type="text/javascript" src="upload.js"></script>
<script type="text/javascript"><!--//--><![CDATA[//><!--
var up_uid='<?php echo($unique_id)?>';
//--><!]]></script>
<link href="style.css" type="text/css" rel="stylesheet" />
</head>
<body onload="if(gID('pass'))gID('pass').value=getCookie('updelpass');if(gID('delpass'))gID('delpass').value=getCookie('updelpass');return true;">

<h1><?php echo $title; ?></h1>
<p><?php echo $banner; ?></p>
<?php
}

if(!($act=='get' && $_SERVER['REQUEST_METHOD'] == 'POST')) htmlheader();
/* ヘッダーここまで */
$foot = <<<FOOT

<h2>著作権表示</h2>
<p id="link"><a href="http://php.s3.to/">レッツPHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a> + <a href="http://scrappedblog.blogspot.com/">Roytam1</a>(k-up SQL 20091103)</p>

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
function _clean($str) {
	if(is_array($str)) return $str;
	$str = htmlspecialchars($str);
	if (get_magic_quotes_gpc()) $str = stripslashes($str);
	return $str;
}
function sqlRun($str) {
	global $conn;
/*	$fp=fopen('sql.log','a+');
	fputs($fp,$str."\n");
	fclose($fp);*/
	return sqlite_exec($conn,$str);
}
function padNum($n) {
	global $countnumbers;
	return str_pad($n,$countnumbers,'0',STR_PAD_LEFT);
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

	echo $mes1."<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>";
	echo $foot;
	exit;
}
function expire($id) {
	global $conn,$updir,$prefix;
	$qry = 'SELECT * FROM upload WHERE id = '.$id;
	$rs = sqlite_query($conn,$qry);
	$row = sqlite_fetch_array($rs);

	if(file_exists($updir.$prefix.padNum($id).'.'.$row['ext'])) unlink($updir.$prefix.padNum($id).'.'.$row['ext']);

	$qry = 'UPDATE upload SET utime=0 WHERE id = '.$id;
	sqlRun($qry);
}

/* start */
$limitb = $limitk * 1024;
$host = @gethostbyaddr($REMOTE_ADDR);
if(!$upcook) $upcook=@implode(",",array($f_act,$f_dlcnt,$f_com,$f_size,$f_mime,$f_date,$f_dlim,$f_orig));
list($c_act,$c_dlcnt,$c_com,$c_size,$c_mime,$c_date,$c_dlim,$c_orig)=explode(',',$upcook);

/* アクセス制限 */
if(is_array($denylist)){
	foreach($denylist as $line){
		if(strstr($host, $line)) error('<h2>錯誤</h2>
<p class="error">存取限制：您沒有使用權限</p>');
	}
}
/* 削除実行 */
if($delid && $delpass!=''){
	$qry = 'SELECT * FROM upload WHERE id = '.$delid;
	$rs = sqlite_query($conn,$qry);
	$row = sqlite_fetch_array($rs);

	if(!$row) error('<h2>錯誤</h2>
<p class="error">刪除錯誤：此檔案找不到</p>');
	if($delpass == $admin || substr(md5($delpass), 2, 7) == $row['pwd']){
		if(file_exists($updir.$prefix.padNum($delid).'.'.$row['ext'])) unlink($updir.$prefix.padNum($delid).'.'.$row['ext']);

		$qry = 'DELETE FROM upload WHERE id = '.$delid;
		sqlRun($qry);

	}else{
		error('<h2>錯誤</h2>
<p class="error">刪除錯誤：密碼錯誤</p>');
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
<li><input type=\"checkbox\" name=\"dlcnte\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_com />下載次數</li>
<li><input type=\"checkbox\" name=\"come\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_com />備註</li>
<li><input type=\"checkbox\" name=\"sizee\" value=\"checked\" tabindex=\"4\" accesskey=\"4\" $c_size />大小</li>
<li><input type=\"checkbox\" name=\"mimee\" value=\"checked\" tabindex=\"5\" accesskey=\"5\" $c_mime />MIME</li>
<li><input type=\"checkbox\" name=\"datee\" value=\"checked\" tabindex=\"6\" accesskey=\"6\" $c_date />日期</li>
<li><input type=\"checkbox\" name=\"dlime\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_com />下載限制</li>
<li><input type=\"checkbox\" name=\"orige\" value=\"checked\" tabindex=\"7\" accesskey=\"7\" $c_orig />原檔名</li>
</ul>
<p>以上設定將會以 cookie 保存以便再次使用。</p>
<p><input type=\"submit\" value=\"儲存\" tabindex=\"8\" accesskey=\"8\" /><input type=\"reset\" value=\"還原\" tabindex=\"9\" accesskey=\"9\" /></p>
</form>
");
}
elseif($act=='mdel') {
	if(!isset($_POST['mdid'])) error('<h2>錯誤</h2>
<p class="error">刪除錯誤:未選擇檔案</p>');
	if($delpass==$admin) {
		$qry = 'SELECT * FROM upload WHERE id IN ('.implode(',',$_POST['mdid']).')';
		$rs = sqlite_query($conn,$qry);
		while($row = sqlite_fetch_array($rs)) {
			if(file_exists($updir.$prefix.padNum($row['id']).'.'.$row['ext'])) unlink($updir.$prefix.padNum($row['id']).'.'.$row['ext']);
		}

		$qry = 'DELETE FROM upload WHERE id IN ('.implode(',',$_POST['mdid']).')';
		sqlRun($qry);
		$find = true;
	} else {
		$delids = array();
		$qry = 'SELECT * FROM upload WHERE id IN ('.implode(',',$_POST['mdid']).')';
		$rs = sqlite_query($conn,$qry);
		while($row = sqlite_fetch_array($rs)) {
			if(substr(md5($delpass), 2, 7) == rtrim($row['pwd'])){
				$delids[] = $row['id'];
				if(file_exists($updir.$prefix.padNum($row['id']).'.'.$row['ext'])) unlink($updir.$prefix.padNum($row['id']).'.'.$row['ext']);
			}
		}
		$find = count($delids);

		$qry = 'DELETE FROM upload WHERE id IN ('.implode(',',$delids).')';
		sqlRun($qry);
	}
	if(!$find) error('<h2>錯誤</h2>
<p class="error">刪除錯誤:密碼錯誤</p>');
	$act='';
}elseif($act=='down') {
	$qry = 'SELECT * FROM upload WHERE id = '.intval($id);
	$rs = sqlite_query($conn,$qry);
	$row = sqlite_fetch_array($rs);

	if(!$row) error('<h2>錯誤</h2>
<p class="error">下載錯誤：此檔案找不到</p>');

	if($row['tlim']||$row['dlim']) {
		if(($row['tlim'] && (time()+$tz*60*60>=($row['utime']+$row['tlim']*60))) || ($row['dlim'] && ($row['dcnt']>=$row['dlim']))) expire($id);
	}

	$qry = 'SELECT * FROM upload WHERE id = '.$id;
	$rs = sqlite_query($conn,$qry);
	$row = sqlite_fetch_array($rs);

	if(!$row['utime']) error('<h2>錯誤</h2>
<p class="error">下載錯誤：此檔案已失效</p>');

	$txt="<h2>下載</h2>
<form method=\"post\" action=\"$PHP_SELF\">
<p><input type=\"hidden\" name=\"act\" value=\"get\" /><input type=\"hidden\" name=\"id\" value=\"$row[id]\" /></p>
<h3>下載檔案</h3>
<p>您將要下載檔案 $prefix".padNum($id).".$row[ext]。</p>";
	if($row['dpwd'] != '*') $txt.='<p>請輸入下載密碼：<input type="password" size="10" name="downpass" maxlength="10" class="box" tabindex="3" accesskey="3" /></p>';
	$txt.="<p>檔案名稱：<label><input type='radio' name='name' value='gen' checked='checked'/>$prefix".padNum($id).".$row[ext]</label> ".($row['upfile_name']!='*'?"<label><input type='radio' name='name' value='user'/>$row[upfile_name]</label> </p>":'')."
<p><input type=\"submit\" value=\"下載\" tabindex=\"8\" accesskey=\"8\" /></p>
</form>
";
error($txt);
}elseif($act=='get' && $_SERVER['REQUEST_METHOD'] == 'POST') {
	$qry = 'SELECT * FROM upload WHERE id = '.$id;
	$rs = sqlite_query($conn,$qry);
	$row = sqlite_fetch_array($rs);

	if(!$row) {
		htmlheader();
		error('<h2>錯誤</h2>
<p class="error">下載錯誤：此檔案找不到</p>');
	}
	if(!$row['utime']) {
		htmlheader();
		error('<h2>錯誤</h2>
<p class="error">下載錯誤：此檔案已失效</p>');
	}

	if($row['dpwd'] == '*' || ($row['dpwd'] != '*' && $row['dpwd'] == substr(md5($downpass), 2, 7))) {
		$qry = 'UPDATE upload SET dcnt=dcnt+1 WHERE id = '.$id;
		sqlRun($qry);

		$fname = $name == 'gen' ? $prefix.padNum($id).'.'.$row['ext'] : $row['upfile_name'];
		header("Content-Disposition: attachment; filename=$fname");
		header("Content-type: $row[upfile_type]; name=$fname");
		readfile($updir.$prefix.padNum($id).'.'.$row['ext']);
		exit;
	} else {
		htmlheader();
		error('<h2>錯誤</h2>
<p class="error">下載錯誤：密碼錯誤</p>');
	}
}
/* アプロード書き込み処理 */
if(file_exists($upfile) && $com && $upfile_size > 0){
	if(isset($com{$commax+1})) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:備註過長</p>
');
	if($upfile_size > $limitb) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:此檔案過大</p>');
	/* 連続投稿制限 */
	if($last_time > 0){
		$now = time()+$tz*60*60;

		$qry = 'SELECT utime,host FROM upload ORDER BY utime desc LIMIT 1';
		$rs = sqlite_query($conn,$qry);
		$row = sqlite_fetch_array($rs);
		if($row) {
			if($host == $row['host'] && $last_time*60 > ($now-$row['utime'])){
				error('<h2>錯誤</h2>
<p class="error">連續上傳限制:請在'.($row['utime']+$last_time*60-$now).'秒後再嘗試上傳</p>');
			}
		}
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
	
	/* MIMEタイプ */
	if(!$upfile_type) $upfile_type = 'text/plain';//デフォMIMEはtext/plain

	/* コメント他 */
	$com = htmlspecialchars($com);	//タグ変換
	if(get_magic_quotes_gpc()) $com = stripslashes($com);	//￥除去

	$utime = time()+$tz*60*60;
	$now = gmdate('Y/m/d(D)H:i', $utime);	//日付のフォーマット
	$pwd = ($pass) ? substr(md5($pass), 2, 7) : '*';	//パスっ作成（無いなら*）
	$dpwd = ($downpass) ? substr(md5($downpass), 2, 7) : '*';	//パスっ作成（無いなら*）
	if($noorig) $upfile_name = '*';
	
	$qry=sprintf("INSERT INTO upload (ext,com,host,now,upfile_size,upfile_type,pwd,upfile_name,dpwd,utime,tlim,dlim) VALUES ('%s','%s','%s','%s',%s,'%s','%s','%s','%s',%s,%s,%s)",sqlite_escape_string($ext),sqlite_escape_string($com),$host,$now,$upfile_size,sqlite_escape_string($upfile_type),$pwd,sqlite_escape_string($upfile_name),$dpwd,$utime,intval($tlim),intval($dlim));
	sqlRun($qry,$qerr);
	$id = sqlite_last_insert_rowid($conn);

	$newname = $prefix.padNum($id).'.'.$ext;

	/* 自鯖転送 */
	move_uploaded_file($upfile, $updir.$newname);//3.0.16より後のバージョンのPHP 3または 4.0.2 後
	//copy($upfile, $updir.$newname);
	chmod($updir.$newname, 0604);

	$qry = 'SELECT count(*) FROM upload';
	$rs = sqlite_query($conn,$qry);
	$rows = sqlite_fetch_single($rs);

	if($rows > $logmax) {
		$delids = array();
		$qry = 'SELECT * FROM upload ORDER BY utime desc LIMIT '.$logmax.','.($logmax-$rows);
		$rs = sqlite_query($conn,$qry);
		while($row = sqlite_fetch_array($rs)) {
			$delids[] = $row['id'];
			if(file_exists($updir.$prefix.padNum($row['id']).'.'.$row['ext'])) unlink($updir.$prefix.padNum($row['id']).'.'.$row['ext']);
		}
		$qry = 'DELETE FROM upload WHERE id IN ('.implode(',',$delids).')';
		sqlRun($qry);
	}
}
/* 投稿フォーム */
echo '<h2>上傳檔案</h2>
<form method="post" enctype="multipart/form-data" action="'.$PHP_SELF.'" onsubmit="setCookie(\'updelpass\',gID(\'pass\').value);startProgress();return true;">
<p>檔案<strong>（最大 '.$limitk.' KB，共 '.$logmax.' 個檔案）</strong><br />
<input type="hidden" name="MAX_FILE_SIZE" value="'.$limitb.'" />
<input type="hidden" name="APC_UPLOAD_PROGRESS" id="progress_key" value="'.$unique_id.'"/>
<input type="file" size="40" name="upfile" class="box" tabindex="1" accesskey="1" />
刪除密碼：<input type="password" size="10" name="pass" id="pass" maxlength="10" class="box" tabindex="2" accesskey="2" />
下載密碼(選填)：<input type="password" size="10" name="downpass" maxlength="10" class="box" tabindex="3" accesskey="3" />
時限(選填，單位：分)：<input type="text" size="10" maxlength="5" name="tlim" value="" class="box" tabindex="4" accesskey="4" />
下載次數限制(選填)：<input type="text" size="10" maxlength="5" name="dlim" value="" class="box" tabindex="5" accesskey="5" /></p>
<p>備註(<strong class="rb">必須</strong>)
<input type="text" size="45" name="com" value="" class="box" tabindex="6" accesskey="6" />
<label><input type="checkbox" name="noorig" value="1" />隱藏原檔名</label>
<input type="submit" value="上傳" tabindex="7" accesskey="7" />
<input type="reset" value="Cancel" tabindex="8" accesskey="8" /></p>
<p>可以上傳的檔尾：'.implode(' ',$arrowext).'</p>
<div id="up_progress">上傳中… <span id="progressbartext"></span><div id="progressbarborder"><div id="progressbar"></div></div></div>
</form>
';
/* カウンタ */
echo "<p class=\"uline\">$count_start 開始存取數: ";

$qry = 'UPDATE counter SET counter=counter+1';
sqlRun($qry,$conn);
$qry = 'SELECT counter FROM counter';
$rs = sqlite_query($conn,$qry);
echo sqlite_fetch_single($rs);

/* モードリンク */
echo '
　D：刪除檔案</p>
<p class="uline"><a href="'.$home.'">主頁</a> | <a href="'.$PHP_SELF.'?act=mult">多檔刪除</a> | <a href="'.$PHP_SELF.'?act=env">環境設定</a> | <a href="'.$PHP_SELF.'?">重新整理</a></p>

<h2>檔案一覧</h2>
<p class="uline">';

$qry = 'SELECT count(*) FROM upload';
$rs = sqlite_query($conn,$qry);
$rows = sqlite_fetch_single($rs);

/* ログ開始位置 */
$st = ($page) ? ($page - 1) * $page_def : 0;
if(!$page) $page = 1;
if($page == 'all'){
	$st = 0;
	$page_def = $rows;
}
echo paging($page, $rows);//ページリンク
//メインヘッダ
echo "</p>\n";
if($act=='mult')	echo "<form action=\"$PHP_SELF\" method=POST><input type='hidden' name='act' value='mdel'/>";
echo "<table summary=\"files\">\n<tr>";
if($c_act) echo '<th abbr="delete" scope="col">刪</th>';
echo '<th abbr="name" scope="col">檔名</th>';
if($c_dlcnt) echo '<th abbr="count" scope="col">次數</th>';
if($c_com) echo '<th abbr="comment" scope="col">備註</th>';
if($c_size) echo '<th abbr="size" scope="col">大小</th>';
if($c_mime) echo '<th abbr="mime" scope="col">MIME</th>';
if($c_date) echo '<th abbr="date" scope="col">日期</th>';
if($c_dlim) echo '<th abbr="limit" scope="col">限制</th>';
if($c_orig) echo '<th abbr="original name" scope="col">原檔名</th>';
echo "</tr>\n";
//メイン表示
$qry = 'SELECT * FROM upload ORDER BY id desc LIMIT '.$st.','.$page_def;
$rs = sqlite_query($conn,$qry);
while($row = sqlite_fetch_array($rs)) {
	$fsize = FormatByte($row['upfile_size']);
	if($auto_link) $com = preg_replace('/(https?|ftp|news)(:\/\/[\w\+\$\;\?\.\{\}%,!#~*\/:@&=_-]+)/u', '<a href="$1$2">$1$2</a>',$com);

	$pmark = $row['dpwd'] != '*' ? '<span class="rb">*</span>' : '';
	$filename = $prefix.padNum($row['id']).'.'.$row['ext'];

	echo "<tr><!--$row[host]-->\n";//ホスト表示
	if($c_act) {
		if($act=='mult') echo "<td class=\"del\"><input type=\"checkbox\" name=\"mdid[]\" value=\"$row[id]\"/></td>";
		else echo "<td class=\"del\"><a href=\"$PHP_SELF?del=$row[id]\">D</a></td>";
	}
	$dlink = $row['utime'] ? "<a href=\"$PHP_SELF?act=down&amp;id=$row[id]\">$filename</a>" : $filename;
	echo "<td>[$dlink]</td>";
	if($c_dlcnt) echo "<td>$row[dcnt]</td>";
	if($c_com) echo "<td>$pmark$row[com]</td>";
	if($c_size) echo "<td class=\"size\">$fsize</td>";
	if($c_mime) echo "<td>$row[upfile_type]</td>";
	if($c_date) echo "<td>$row[now]</td>\n";
	if($c_dlim) {
		$dlimit='';
		if($row['tlim']) $dlimit=$row['tlim'].'分';
		if($row['dlim']) $dlimit.=$dlimit ? ' 或 '.$row['dlim'].'次' : $row['dlim'].'次';
		if(!$dlimit) $dlimit='無';
		echo "<td>$dlimit</td>";
	}
	if($c_orig) echo "<td class=\"orig\">\n$row[upfile_name]</td>\n";
	echo "</tr>\n";
	}

echo "</table>\n";
if($act=='mult')	echo '<div align="right">密碼:<input type="password" name="delpass" id="delpass"/><input type="submit" name="submit" value="刪除"/></div></form>';
echo '<p class="tline">'.paging($page,$rows)."</p>\n";
echo $foot;
?>

