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
if(phpversion()>="4.1.0"){//PHP4.1.0以降対応
  $_GET = array_map("_clean", $_GET);
  $_POST = array_map("_clean", $_POST);
  //$_POST = array_map("_clean", $_GET);
  extract($_GET);
  extract($_POST);
  extract($_COOKIE);
  extract($_SERVER);
  $upfile_type=_clean($_FILES['upfile']['type']);
  $upfile_size=$_FILES["upfile"]["size"];//某所で気づく・・・
  $upfile_name=_clean($_FILES["upfile"]["name"]);
  $upfile=$_FILES["upfile"]["tmp_name"];
}

  $title	= '上傳區';	//タイトル
  $home		= 'http://www.cn-dos.net/';	//ホーム
  $logfile	= "up.log";	//ログファイル名（変更する事）
  $updir 	= "./img/";	//アップ用ディレクトリ
  $upsdir	= "./imgs/";	//サムネイル保存ディレクトリ
  $prefix	= '';		//接頭語（up001.txt,up002.jpgならup）
  $logmax	= 5000;		//log保存行（これ以上は古いのから削除）
  $usrmax	= 25;		//User投稿量制限（バイト。全角はこの半分）
  $commax	= 250;		//コメント投稿量制限（バイト。全角はこの半分）
  $limitk	= 100000;	//アップロード制限（KB キロバイト）
  $page_def	= 15;		//一ページの表示行数
  $admin	= "del";	//削除管理パス
  $auto_link	= 1;		//コメントの自動リンク（Yes=1;No=0);
  $denylist	= array('192.168.0.1','sex.com','annony');	//アクセス拒否ホスト
  $arrowext	= array('7z','zip','lzh','rar','avi','mpg','wmv','mp3','wma','swf','txt','bmp','jpg','png','gif');	//許可拡張子 小文字（それ以外はエラー

  $count_start	= "2008/02/11";	//カウンタ開始日
  $count_file	= "count.txt";  //カウンタファイル（空ファイルで666）

  $last_file	= "last.cgi";	//連続投稿制限用ファイル（空ファイルで666）
  $last_time	= 0;		//同一IPからの連続投稿許可する間隔（分）（0で無制限）

  /* 項目表示（環境設定）の初期状態 (表示ならChecked 表示しないなら空) */
  $f_act  = 'checked="checked"';	//ACT（削除リンク）
  $f_usr  = 'checked="checked"';	//コメント
  $f_com  = 'checked="checked"';	//コメント
  $f_size = 'checked="checked"';	//ファイルサイズ
  $f_mime = '';				//MIMEタイプ
  $f_date = 'checked="checked"';	//日付け
  $f_orig = 'checked="checked"';	//元ファイル名

if($act=="envset"){
  $cookval = @implode(",", array($acte,$user,$come,$sizee,$mimee,$datee,$orige)); 
  setcookie ("upcook", $cookval,time()+365*24*3600);
}
function _clean($str) {
  $str = htmlspecialchars($str);
  if (get_magic_quotes_gpc()) $str = stripslashes($str);
  return $str;
}
$unique_id = uniqid("");
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
<script type="text/javascript"><!--//--><![CDATA[//><!--
function gID(s) { return document.getElementById(s); }
/* 建立XMLHttpRequest物件 */
function JSONXMLHttpReq(){
	var objxml = false;
	/*@cc_on @*/
	/*@if (@_jscript_version >= 5)
	try{
		objxml = new ActiveXObject("Msxml2.XMLHTTP");
	}catch(e){
		try{
			objxml = new ActiveXObject("Microsoft.XMLHTTP");
		}catch(e2){ objxml = false; }
	}
	@end @*/
	if(!objxml && typeof XMLHttpRequest!='undefined') {
		objxml = new XMLHttpRequest();
		if(objxml.overrideMimeType) objxml.overrideMimeType('text/plain');
	}
	return objxml;
}
var xhttpjson=JSONXMLHttpReq();

function ParseProgress(){
	if(xhttpjson.readyState==4){ // 讀取完成
		percent = xhttpjson.responseText;
		if(percent > 0) {
			gID("progressbarborder").style.display="block";
			gID("progressbartext").innerHTML = percent+"%";
			gID("progressbar").style.width = percent+"%";
		}
		if(percent < 100){
			setTimeout("getProgress()", 200);
		}
	}
}
function getProgress(){
	if(xhttpjson){
		xhttpjson.open('GET','progress.php?progress_key=<?php echo($unique_id)?>', true);
		xhttpjson.onreadystatechange = ParseProgress;
		xhttpjson.send(null);
	}
}
 
function startProgress(){
    gID("up_progress").style.display="block";
    setTimeout("getProgress()", 1000);
}
//--><!]]></script>
<link href="style.css" type="text/css" rel="stylesheet" />
</head>
<body>

<h1><?php echo "$title" ?></h1>
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
  if($size == 0)			$format = "";
  else if($size <= 1024)		$format = $size."B";
  else if($size <= (1024*1024))		$format = sprintf ("%dkB",($size/1024));
  else if($size <= (1024*1024*1024))	$format = sprintf ("%.2fMB",($size/(1024*1024)));
  else if($size <= (1024*1024*1024*1024))	$format = sprintf ("%.2fGB",($size/(1024*1024*1024)));
  else					$format = $size."B";

  return $format;
}
function paging($page, $total){//ページリンク作成
  global $PHP_SELF,$page_def;

    for ($j = 1; $j * $page_def < $total+$page_def; $j++) {
      if($page == $j){//今表示しているのはﾘﾝｸしない
        $next .= "[<strong>$j</strong>]";
      }else{
        $next .= sprintf("[<a href=\"%s?page=%d\">%d</a>]", $PHP_SELF,$j,$j);//他はﾘﾝｸ
      }
    }
    if($page=="all") return sprintf ("Page: %s [ALL]",$next,$PHP_SELF);
    else return sprintf ("Page: %s [<a href=\"%s?page=all\">ALL</a>]",$next,$PHP_SELF);
}
function error($mes1=""){//えっらーﾒｯｾｰｼﾞ
  global $foot;

  echo $mes1;
  echo $foot;
  exit;
}
/* start */
$limitb = $limitk * 1024;
$host = @gethostbyaddr($REMOTE_ADDR);
if(!$upcook) $upcook=@implode(",",array($f_act,$f_usr,$f_com,$f_size,$f_mime,$f_date,$f_orig));
list($c_act,$c_usr,$c_com,$c_size,$c_mime,$c_date,$c_orig)=explode(",",$upcook);

/* アクセス制限 */
if(is_array($denylist)){
  foreach($denylist as $line){
    if(strstr($host, $line)) error('<h2>錯誤</h2>
<p class="error">存取限制:您沒有使用權限</p>');
  }
}
/* 削除実行 */
if($delid && $delpass!=""){
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
    if(file_exists($updir.$prefix.$delid.".$del_ext")) unlink($updir.$prefix.$delid.".$del_ext");
    if(file_exists($upsdir.$prefix.$delid.".$del_ext")) unlink($upsdir.$prefix.$delid.".$del_ext");
    $fp = fopen($logfile, "w");
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
<input type=\"password\" size=\"12\" name=\"delpass\" class=\"box\" tabindex=\"1\" accesskey=\"1\" />
<input type=\"submit\" value=\"刪除\" tabindex=\"2\" accesskey=\"2\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>
");
}
/* 環境設定フォーム */
if($act=="env"){
  echo "<h2>環境設定</h2>
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
<p class=\"tline\"><a href=\"$PHP_SELF?\">返回</a></p>

";
echo $foot;
exit;
}
$lines = file($logfile);
/* アプロード書き込み処理 */
if(file_exists($upfile) && $com && $usr && $upfile_size > 0){
  if(isset($usr{$usrmax+1})) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:用戶名稱過長</p>
');
  if(isset($com{$commax+1})) error('<h2>錯誤</h2>
<p class="error">上傳錯誤:備註過長</p>
');
  if($upfile_size > $limitb)        error('<h2>錯誤</h2>
<p class="error">上傳錯誤:此檔案過大</p>');
  /* 連続投稿制限 */
  if($last_time > 0){
    $now = time();
    $last = @fopen($last_file, "r+") or die('<h2>錯誤</h2>
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
  $pos = strrpos($upfile_name,".");	//拡張子取得
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
  list($id,) = explode("\t", $lines[0]);//No取得
  $id = sprintf("%03d", ++$id);		//インクリ
  $newname = $prefix.$id.".".$ext;

  /* 自鯖転送 */
  move_uploaded_file($upfile, $updir.$newname);//3.0.16より後のバージョンのPHP 3または 4.0.2 後
  //copy($upfile, $updir.$newname);
  chmod($updir.$newname, 0604);

  /* MIMEタイプ */
  if(!$upfile_type) $upfile_type = "text/plain";//デフォMIMEはtext/plain

  /* コメント他 */
  $com = htmlspecialchars($com);	//タグ変換
  if(get_magic_quotes_gpc()) $com = stripslashes($com);	//￥除去
  $usr = htmlspecialchars($usr);	//タグ変換
  if(get_magic_quotes_gpc()) $usr = stripslashes($usr);	//￥除去

  $now = gmdate("Y/m/d(D)H:i", time()+9*60*60);	//日付のフォーマット
  $pwd = ($pass) ? substr(md5($pass), 2, 7) : "*";	//パスっ作成（無いなら*）

  $dat = @implode("\t", array($id,$ext,$usr,$com,$host,$now,$upfile_size,$upfile_type,$pwd,$upfile_name,));

  if(count($lines) >= $logmax){		//ログオーバーならデータ削除
    for($d = count($lines)-1; $d >= $logmax-1; $d--){
      list($did,$dext,)=explode("\t", $lines[$d]);
      if(file_exists($updir.$prefix.$did.".".$dext)) {
        unlink($updir.$prefix.$did.".".$dext);
      }
    }
  }

  $fp = fopen ($logfile , "w");		//書き込みモードでオープン
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
<form method="post" enctype="multipart/form-data" action="'.$PHP_SELF.'" onsubmit="startProgress();return true;">
<p>檔案<strong>（最大 '.$limitk.' KB）</strong><br />
<input type="hidden" name="MAX_FILE_SIZE" value="'.$limitb.'" />
<input type="hidden" name="APC_UPLOAD_PROGRESS" id="progress_key" value="'.$unique_id.'"/>
<input type="file" size="40" name="upfile" class="box" tabindex="1" accesskey="1" />
<p>用戶名稱（※沒輸入的話檔案將不會被儲存。）<br />
<input type="text" size="20" name="usr" value="" class="box" tabindex="1" accesskey="2" />
 Del Pass : <input type="password" size="10" name="pass" maxlength="10" class="box" tabindex="3" accesskey="3" /></p>
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
  $fp = fopen($count_file,"r+");//読み書きモードでオープン
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
<p class="uline"><a href="'.$home.'">主頁</a> | <a href="'.$PHP_SELF.'?act=env">環境設定</a> | <a href="'.$PHP_SELF.'?">重新整理</a> | <a href="sam.php">圖像一覧</a></p>

<h2>檔案一覧</h2>
<p class="uline">';
/* ログ開始位置 */
$st = ($page) ? ($page - 1) * $page_def : 0;
if(!$page) $page = 1;
if($page == "all"){
  $st = 0;
  $page_def = count($lines);
}
echo paging($page, count($lines));//ページリンク
//メインヘッダ
echo "</p>\n";
echo "<table summary=\"files\">\n<tr>";
if($c_act) echo "<th abbr=\"delete\" scope=\"col\">刪</th>";
echo "<th abbr=\"name\" scope=\"col\">檔名</th>";
if($c_usr) echo "<th abbr=\"user\" scope=\"col\">用戶</th>";
if($c_com) echo "<th abbr=\"comment\" scope=\"col\">備註</th>";
if($c_size) echo "<th abbr=\"size\" scope=\"col\">大小</th>";
if($c_mime) echo "<th abbr=\"mime\" scope=\"col\">MIME</th>";
if($c_date) echo "<th abbr=\"date\" scope=\"col\">日期</th>";
if($c_orig) echo "<th abbr=\"original name\" scope=\"col\">原檔名</th>";
echo "</tr>\n";
//メイン表示
for($i = $st; $i < $st+$page_def; $i++){
  if($lines[$i]=="") continue;
  list($id,$ext,$usr,$com,$host,$now,$size,$mtype,$pas,$orig,)=explode("\t",$lines[$i]);
  $fsize = FormatByte($size);
  if($auto_link) $com = ereg_replace("(https?|ftp|news)(://[[:alnum:]\+\$\;\?\.%,!#~*/:@&=_-]+)","<a href=\"\\1\\2\">\\1\\2</a>",$com);

  $filename = $prefix.$id.".$ext";
  $target = $updir.$filename;

  
  echo "<tr><!--$host-->\n";//ホスト表示
  if($c_act) echo "<td class=\"del\"><a href=\"$PHP_SELF?del=$id\">D</a></td>";
  echo "<td>[<a href=\"$target\">$filename</a>]</td>";
  if($c_usr) echo "<td>$usr</td>";
  if($c_com) echo "<td>$com</td>";
  if($c_size) echo "<td class=\"size\">$fsize</td>";
  if($c_mime) echo "<td>$mtype</td>";
  if($c_date) echo "<td>$now</td>\n";
  if($c_orig) echo "<td class=\"orig\">\n$orig</td>\n";
  echo "</tr>\n";
  }

echo "</table>\n<p class=\"tline\">";
echo paging($page,count($lines));
echo "</p>\n";
echo $foot;
?>

