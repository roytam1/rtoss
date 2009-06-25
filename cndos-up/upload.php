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
if(phpversion()>="4.1.0"){//PHP4.1.0以降対応
  $_GET = array_map("_clean", $_GET);
  $_POST = array_map("_clean", $_POST);
  //$_POST = array_map("_clean", $_GET);
  extract($_GET);
  extract($_POST);
  extract($_COOKIE);
  extract($_SERVER);
  $upfile_type=_clean($_FILES['up']['type']);
  $upfile_size=$_FILES["upfile"]["size"];//某所で気づく・・・
  $upfile_name=_clean($_FILES["upfile"]["name"]);
  $upfile=$_FILES["upfile"]["tmp_name"];
}

  $title	= 'Wastepaper Uploader';	//タイトル
  $home		= 'http://utu.under.jp/';	//ホーム
  $logfile	= "up.log";	//ログファイル名（変更する事）
  $updir 	= "./img/";	//アップ用ディレクトリ
  $upsdir	= "./imgs/";	//サムネイル保存ディレクトリ
  $prefix	= '';		//接頭語（up001.txt,up002.jpgならup）
  $logmax	= 50;		//log保存行（これ以上は古いのから削除）
  $commax	= 250;		//コメント投稿量制限（バイト。全角はこの半分）
  $limitk	= 100;	//アップロード制限（KB キロバイト）
  $page_def	= 10;		//一ページの表示行数
  $admin	= "12345";	//削除管理パス
  $auto_link	= 1;		//コメントの自動リンク（Yes=1;No=0);
  $denylist	= array('192.168.0.1','sex.com','annony');	//アクセス拒否ホスト
  $arrowext	= array('zip','lzh','rar','avi','mpg','wmv','mp3','wma','swf','txt','bmp','jpg','png','gif');	//許可拡張子 小文字（それ以外はエラー

  $count_start	= "2004/10/24";	//カウンタ開始日
  $count_file	= "count.txt";  //カウンタファイル（空ファイルで666）

  $last_file	= "last.cgi";	//連続投稿制限用ファイル（空ファイルで666）
  $last_time	= 0;		//同一IPからの連続投稿許可する間隔（分）（0で無制限）

  /* 項目表示（環境設定）の初期状態 (表示ならChecked 表示しないなら空) */
  $f_act  = 'checked="checked"';	//ACT（削除リンク）
  $f_com  = 'checked="checked"';	//コメント
  $f_size = 'checked="checked"';	//ファイルサイズ
  $f_mime = '';				//MIMEタイプ
  $f_date = 'checked="checked"';	//日付け
  $f_orig = 'checked="checked"';	//元ファイル名

if($act=="envset"){
  $cookval = @implode(",", array($acte,$come,$sizee,$mimee,$datee,$orige)); 
  setcookie ("upcook", $cookval,time()+365*24*3600);
}
function _clean($str) {
  $str = htmlspecialchars($str);
  if (get_magic_quotes_gpc()) $str = stripslashes($str);
  return $str;
}
/* ここからヘッダー */
?>
<?php echo '<?xml version="1.0" encoding="euc-jp"?>'."\n" ?>
<!DOCTYPE html
   PUBLIC "-//W3C//DTD XHTML 1.1//EN"
   "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="ja">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=euc-jp" media="all" />
<title><?php echo "$title" ?></title>
<link href="style.css" type="text/css" rel="stylesheet" />
</head>
<body>

<h1><?php echo "$title" ?></h1>
<p>気になるネタなどヽ(´ー｀)ノ</p>

<?php
/* ヘッダーここまで */
$foot = <<<FOOT

<h2>著作権表示</h2>
<p id="link"><a href="http://php.s3.to/">レッツPHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a></p>

</body>
</html>
FOOT;

function FormatByte($size){//バイトのフォーマット（B→kB）
  if($size == 0)			$format = "";
  else if($size <= 1024)		$format = $size."B";
  else if($size <= (1024*1024))		$format = sprintf ("%dkB",($size/1024));
  else if($size <= (10*1024*1024))	$format = sprintf ("%.2fMB",($size/(1024*1024)));
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
if(!$upcook) $upcook=@implode(",",array($f_act,$f_com,$f_size,$f_mime,$f_date,$f_orig));
list($c_act,$c_com,$c_size,$c_mime,$c_date,$c_orig)=explode(",",$upcook);

/* アクセス制限 */
if(is_array($denylist)){
  while(list(,$line)=each($denylist)){
    if(strstr($host, $line)) error('<h2>エラーメッセージ</h2>
<p class="error">アクセス制限:アクセス権限がありません</p>');
  }
}
/* 削除実行 */
if($delid && $delpass!=""){
  $old = file($logfile);
  $find = false;
  for($i=0; $i<count($old); $i++){
    list($did,$dext,,,,,,$dpwd,)=explode("\t",$old[$i]);
    if($delid==$did){
      $find = true;
      $del_ext = $dext;
      $del_pwd = rtrim($dpwd);
    }else{
      $new[] = $old[$i];
    }
  }
  if(!$find) error('<h2>エラーメッセージ</h2>
<p class="error">削除エラー:該当ファイルが見つかりません</p>');
  if($delpass == $admin || substr(md5($delpass), 2, 7) == $del_pwd){
    if(file_exists($updir.$prefix.$delid.".$del_ext")) unlink($updir.$prefix.$delid.".$del_ext");
    if(file_exists($upsdir.$prefix.$delid.".$del_ext")) unlink($upsdir.$prefix.$delid.".$del_ext");
    $fp = fopen($logfile, "w");
    flock($fp, 2);
    fputs($fp, @implode("",$new));  
    fclose($fp);
  }else{
    error('<h2>エラーメッセージ</h2>
<p class="error">削除エラー:パスワードが違います</p>');
  }
}
/* 削除フォーム */
if($del){
  error("<h2>削除フォーム</h2>
<form method=\"post\" enctype=\"multipart/form-data\" action=\"$PHP_SELF\">
<p>削除パス入力<br />
<input type=\"hidden\" name=\"delid\" value=\"".htmlspecialchars($del)."\" />
<input type=\"password\" size=\"12\" name=\"delpass\" class=\"box\" tabindex=\"1\" accesskey=\"1\" />
<input type=\"submit\" value=\"削除\" tabindex=\"2\" accesskey=\"2\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">戻る</a></p>
");
}
/* 環境設定フォーム */
if($act=="env"){
  echo "<h2>環境設定</h2>
<form method=\"get\" action=\"$PHP_SELF\">
<p><input type=\"hidden\" name=\"act\" value=\"envset\" /></p>
<h3>表示設定</h3>
<ul>
<li><input type=\"checkbox\" name=\"acte\" value=\"checked\" tabindex=\"1\" accesskey=\"1\" $c_act />ACT</li>
<li><input type=\"checkbox\" name=\"come\" value=\"checked\" tabindex=\"2\" accesskey=\"2\" $c_com />COMMENT</li>
<li><input type=\"checkbox\" name=\"sizee\" value=\"checked\" tabindex=\"3\" accesskey=\"3\" $c_size />SIZE</li>
<li><input type=\"checkbox\" name=\"mimee\" value=\"checked\" tabindex=\"4\" accesskey=\"4\" $c_mime />MIME</li>
<li><input type=\"checkbox\" name=\"datee\" value=\"checked\" tabindex=\"5\" accesskey=\"5\" $c_date />DATE</li>
<li><input type=\"checkbox\" name=\"orige\" value=\"checked\" tabindex=\"6\" accesskey=\"6\" $c_orig />ORIG</li>
</ul>
<p>上記の設定にて訪問することができます（※cookie）</p>
<p><input type=\"submit\" value=\"登録\" tabindex=\"7\" accesskey=\"7\" /><input type=\"reset\" value=\"元に戻す\" tabindex=\"8\" accesskey=\"8\" /></p>
</form>
<p class=\"tline\"><a href=\"$PHP_SELF?\">戻る</a></p>

";
echo $foot;
exit;
}
$lines = file($logfile);
/* アプロード書き込み処理 */
if(file_exists($upfile) && $com && $upfile_size > 0){
  if(strlen($com) > $commax) error('<h2>エラーメッセージ</h2>
<p class="error">投稿エラー:コメントが長すぎます</p>
');
  if($upfile_size > $limitb)        error('<h2>エラーメッセージ</h2>
<p class="error">投稿エラー:ファイルがデカすぎます</p>');
  /* 連続投稿制限 */
  if($last_time > 0){
    $now = time();
    $last = @fopen($last_file, "r+") or die('<h2>エラーメッセージ</h2>
<p class="error">連続投稿用ファイル $last_file を作成してください</p>');
    $lsize = fgets($last, 1024);
    list($ltime, $lip) = explode("\t", $lsize);
    if($host == $lip && $last_time*60 > ($now-$ltime)){
      error('<h2>エラーメッセージ</h2>
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
    error('<h2>エラーメッセージ</h2>
<p class="error">拡張子エラー:その拡張子ファイルはアップロードできません</p>');
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

  $now = gmdate("Y/m/d(D)H:i", time()+9*60*60);	//日付のフォーマット
  $pwd = ($pass) ? substr(md5($pass), 2, 7) : "*";	//パスっ作成（無いなら*）

  $dat = @implode("\t", array($id,$ext,$com,$host,$now,$upfile_size,$upfile_type,$pwd,$upfile_name,));

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
echo '<h2>アップロードフォーム</h2>
<form method="post" enctype="multipart/form-data" action="'.$PHP_SELF.'" >
<p>Up File<strong>（'.$limitk.' KBまで）</strong><br />
<input type="hidden" name="MAX_FILE_SIZE" value="'.$limitb.'" />
<input type="file" size="40" name="upfile" class="box" tabindex="1" accesskey="1" />
 Del Pass : <input type="password" size="10" name="pass" maxlength="10" class="box" tabindex="2" accesskey="2" /></p>
<p>Comment（※コメント無記入はリロード/URIはオートリンク）<br />
<input type="text" size="45" name="com" value="" class="box" tabindex="3" accesskey="3" />
<input type="submit" value="Up/Reload" tabindex="4" accesskey="4" />
<input type="reset" value="Cancel" tabindex="5" accesskey="5" /></p>
<p>UP可能拡張子：'.$arrow.'</p>
</form>
';
/* カウンタ */
echo "<p class=\"uline\">$count_start から ";
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
（こわれにくさレベル1）D：投稿記事削除</p>
<p class="uline"><a href="'.$home.'">HOME</a> | <a href="'.$PHP_SELF.'?act=env">環境設定</a> | <a href="'.$PHP_SELF.'?">リロード</a> | <a href="sam.php">画像一覧</a></p>

<h2>ファイル一覧</h2>
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
if($c_act) echo "<th abbr=\"delete\" scope=\"col\">DEL</th>";
echo "<th abbr=\"name\" scope=\"col\">NAME</th>";
if($c_com) echo "<th abbr=\"comment\" scope=\"col\">COMMENT</th>";
if($c_size) echo "<th abbr=\"size\" scope=\"col\">SIZE</th>";
if($c_mime) echo "<th abbr=\"mime\" scope=\"col\">MIME</th>";
if($c_date) echo "<th abbr=\"date\" scope=\"col\">DATE</th>";
if($c_orig) echo "<th abbr=\"original name\" scope=\"col\">ORIG</th>";
echo "</tr>\n";
//メイン表示
for($i = $st; $i < $st+$page_def; $i++){
  if($lines[$i]=="") continue;
  list($id,$ext,$com,$host,$now,$size,$mtype,$pas,$orig,)=explode("\t",$lines[$i]);
  $fsize = FormatByte($size);
  if($auto_link) $com = ereg_replace("(https?|ftp|news)(://[[:alnum:]\+\$\;\?\.%,!#~*/:@&=_-]+)","<a href=\"\\1\\2\">\\1\\2</a>",$com);

  $filename = $prefix.$id.".$ext";
  $target = $updir.$filename;

  
  echo "<tr><!--$host-->\n";//ホスト表示
  if($c_act) echo "<td class=\"del\"><a href=\"$PHP_SELF?del=$id\">D</a></td>";
  echo "<td>[<a href=\"$target\">$filename</a>]</td>";
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

