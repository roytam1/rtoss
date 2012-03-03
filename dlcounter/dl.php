<?php
if (phpversion()>="4.1.0"){
	extract($_SERVER);
	extract($_REQUEST);
}
/***********************************
	* DLカウンタ         by ToR
	*
	* http://php.s3.to/
	* 2001/10/23
	**********************************
	* DLカウンタです
	* ファイルに
	* ID|カウント|場所|登録日|タイトル|参照URL|を記録します
	* 管理モードはJavaScriptを使用しています。
	* 指定リンク元以外はDLしないように出来ます
	* http://www.proxy2.deのtop_dlを参考にしました
	*
	* 2002/08/19 認証強化。説明変更不可、化けバグ修正
	* 2002/09/13 ROOTやめ。相対ﾊﾟｽでも可
	*
	* ファイルの場所と解説ページは、dl.phpからの相対ﾊﾟｽか、フルURLを書きます
	*/
define(LOGFILE, "dllog.txt");		/* ログファイル名 */

define(ADMINPASS, "0123");		/* 管理パス */
define('SHOW_DEF','all');			/* ランクは最初何件表示？ */

define('BAR', 'aqua.gif');		/* バーの画像 */
define('HEIGHT', 10);			/* 画像の高さ */

define('NODETAILS', false);			/* 詳細データ表示？ */

define(REFCHECK, 0);			/* リンク元チェックする？する場合↓ */
$okurl = array('http://php.s3.to');	/* このリスト以外からのDLは不可 */

$xsendfile = false;   // use x-sendfile header?
                      // false: use PHP readfile_chunked()
                      // 'X-sendfile': for apache mod_xsendfile/lighttpd-1.5
                      // 'X-LIGHTTPD-send-file': for lighttpd-1.4.20+
                      // 'X-Accel-Redirect': for nginx
$xendfile_prefix = dirname(__FILE__).'/'; // path prefix for x-sendfile

/* へっだ部分 */
function head(){
	global $PHP_SELF;
	$this_day = gmdate("Y/m/d(D) H:i:s", time()+8*3600);//現在時刻
	?>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Top Downloads</title>
<style type=text/css>
td { font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
a:hover { color: #ff0000; text-decoration: underline }
select {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
input {  font-size: 8pt}
tt {  font-family: Verdana, Arial; font-size: 13pt; font-weight: bold;color:f08020;}
a {  text-decoration: none}
.bar {  height:1em; background-color:#6666CC;float:left;}
</style>
<script language="Javascript">
<!--
	function ViewDetail(target,selObj) {
		eval(target+".location='<? echo $PHP_SELF;?>?job=detail&detail="+selObj.options[selObj.selectedIndex].value+"'");
	}
// -->
</script>
</head>
<body bgcolor="FFFFDD" text="#000000" link="#000066" vlink="#000066" alink="#000066">
<br>
<form method="post" action="<? echo $PHP_SELF;?>">
<table width="560" border="0" cellspacing="0" cellpadding="2" height="55" align="center">
	<tr> 
		<td> <tt>レッツPHP! Downloads </tt></td>
		<td align="right"> <small style="text-color:blue">▶</small><a href="<? echo $PHP_SELF;?>?job=admin">Administration</a></td>
	</tr>
	<tr>
		<td><small style="text-color:blue">▶</small><b>Date: <? echo $this_day;?></b></td>
		<td align="right">
<?php
}
/* フッタ部分 */
function foot() {
	?>
 <table border=0 width="560" align="center" cellspacing="1" cellpadding="3">
	<tr> 
		<td valign="top"><a href="http://php.s3.to/" target="_blank">PHP Downloads</a></td>
			<td align="right">
			 <select name="option">
				 <option value="total" selected>Total</option>
				 <option value="perDay">Per day</option>
			 </select>
			 <select name="top">
				 <option value="10" selected>Top 10</option>
				 <option value="15">Top 15</option>
				 <option value="20">Top 20</option>
				 <option value="30">Top 30</option>
				 <option value="all">all</option>
			 </select>
			 <input type="submit" value="Submit">
		</td>
	</tr>
</table>
</form>
</body>
</html>
<?php
	exit;
}

// faster readfile()
function readfile_chunked($filename,$retbytes=true) {
   $chunksize = 1*(1024*1024); // how many bytes per chunk
   $buffer = '';
   $cnt =0;
   // $handle = fopen($filename, 'rb');
   $handle = fopen($filename, 'rb');
   if ($handle === false) return false;
   while (!feof($handle)) {
       $buffer = fread($handle, $chunksize);
       echo $buffer;
       ob_flush();flush();
       if ($retbytes) $cnt += strlen($buffer);
   }
   $status = fclose($handle);
   if ($retbytes && $status) {
       return $cnt; // return num. bytes delivered like readfile() does.
   }
   return $status;
}

/* カウントアップとDL */
function count_dl($id){
	global $xsendfile, $xendfile_prefix;

	$lines = file(LOGFILE);	//ファイル読み込み
	$find = FALSE;		//フラグ
	for($i=0; $i<count($lines); $i++){
		list($file_id,$count,$location,$stamp,$descript,$ref) = explode("|",$lines[$i]);
		if($id == $file_id){	//見つかったらカウントアップ
			$find=TRUE;
			$count++;
			$lines[$i] = "$file_id|$count|$location|$stamp|$descript|$ref|\n";//書き換え
			$redirect = $location;	//ファイル位置決定
			break;
		}
	}
	if($find){//ターゲットが見つかったらDL
		//ログ更新
		$fp = fopen(LOGFILE, "w");
		flock($fp, LOCK_EX);
		fputs($fp, implode("", $lines));
		fclose($fp);
		if(eregi("^(f|ht)tp:\/\/[._a-z0-9-]+",$redirect)){
			header("Location: $redirect");//URLの場合Location
		}else{//ローカルの場合ダイアログ
			header("Content-Type: application/octet-stream; name=".basename($redirect));
			header("Content-Disposition: attachment; filename=".basename($redirect));
			if($xsendfile == false) {
				readfile_chunked($redirect);
//				readfile($redirect);//読み込み
			} else {
				header($xsendfile.': '.$xendfile_prefix.$redirect);
			}
			exit;
		}
	}else{
		die("対象ファイルが見つかりません！");
	}
	exit;
}
/* ランキング表示 */
function show_table($top,$option){

	head();//ヘッダ表示
	//ページ処理
	$lines = file(LOGFILE);
	$now = time();

	if($top == 'all') $top = count($lines);
	else $top = (preg_match("/^(\d+)/",$top)) ? $top : (SHOW_DEF == 'all' ? count($lines) : SHOW_DEF);
	//ソート用に読み込む
	for($i=0; $i<count($lines); $i++){
		list($file_id,$num,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
		if($option=="perDay"){
			$dl_perday = ($now-$stamp>86400) ? sprintf("%.2f",($num*86400/($now-$stamp))) : 0;
			$num_arr[$descript] = $dl_perday;	//ランク用
			$name_arr[$file_id] = $descript;	//右上選択用
			$ref_arr[$descript] = $refurl;	//参照用
			$total += $dl_perday;			//合計カウント計算
		}else{
			$num_arr[$descript] = $num;	//ランク用
			$name_arr[$file_id] = $descript;	//右上選択用
			$ref_arr[$descript] = $refurl;	//参照用
			$total += $num;			//合計カウント計算
		}
	}
	arsort($num_arr);			//数字の大きい順
	asort($name_arr);			//名前の小さい順
	//右上部分作成
	if(!NODETAILS) {
		echo '       <select name="files" onChange="ViewDetail(\'self\',this)">
       <option selected>show details</option>
';
		while(list($fileid,$desc)=each($name_arr)){
			echo "        <option value=\"$fileid\">$desc</option>\n";
		}
		echo "      </select>\n";
	}
	//テーブルヘッダ
	echo "      </select>\n    </td>\n  </tr>\n</table>\n";
	echo "<table bgcolor=\"F7F7F7\" border=0 cellpadding=3 cellspacing=2 width=560 align=center>\n"; 
	echo "  <tr><td colspan=4 bgcolor=\"A8C1F1\"><font color=\"#000099\"><b>Top $top Downloads</b></font></td></tr>\n";
	//合計があったら
	if($total > 0){
		$flag = FALSE;	//一位フラグ
		while(list($title,$value)=each($num_arr)){
			if ($show < $top) {//上位何件
				$tmp = $rank;						//以前の数
				$rank = $value;						//新しい数
				if($tmp != $rank) $num = $show + 1;			//違うならランクアプ
				if(!$flag) $top_pos = $value;				//一位データ
				$per = sprintf("%.1f %%",($value *100 / $total));	//％フォーマット
				$img_width = ($value * 310 / $top_pos);			//バー幅決定
				if($ref_arr[$title]) $title = "<a href=\"$ref_arr[$title]\">$title</a>";
				echo "<tr><td align=center><b>$num</b></td><td>$title</td><td nowrap><div class=\"bar\" style=\"width:".$img_width."px\"></div> $value </td><td>$per</td></tr>\n";
				$show++;						//表示カウント
				$flag = TRUE;
			}else{
				break;
			}
		}
	}
	foot();//フッタ表示
}
/* 詳細データ表示 */
function show_detail($id){

	head();//ヘッダ表示
	if(!NODETAILS) {
		$now = time();
		//右上部分
		$lines = file(LOGFILE);
		for($i=0; $i<count($lines); $i++){
			list($file_id,$num,$location,$stamp,$descript,) = explode("|",$lines[$i]);
			$name_arr[$file_id] = $descript;
			$total += $num;
		}
		asort($name_arr);
		echo '       <select name="files" onChange="ViewDetail(\'self\',this)">
       <option selected>show details</option>
';
		while(list($fileid,$desc)=each($name_arr)){
			echo "        <option value=\"$fileid\">$desc</option>\n";
		}
		//テーブルヘッダ
		echo "      </select>\n    </td>\n  </tr>\n</table>\n";
		echo "<table bgcolor=\"F7F7F7\" border=0 cellpadding=3 cellspacing=2 width=560 align=center>\n"; 
		echo "  <tr><td colspan=4 bgcolor=\"A8C1F1\"><font color=\"#000099\"><b>Top $top Downloads</b></font></td></tr>\n";
		//項目名の配列
		$details = array('ファイル名','タイトル','登録日','経過日数','一日あたりのDL数','合計DL数','全体の割合','解説ページ');
		reset($lines);//一応リセット
		for($i=0; $i<count($lines); $i++){
			list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
			if($id == $file_id){
				$file_date = date("Y/m/d(D) H:i:s", $stamp);	//データ登録日
				$total_days = (int)(($now-$stamp)/86400);		//経過日数
				//一日あたりのDL数
				$dl_perday = ($now-$stamp>86400) ? sprintf("%.2f",($count*86400/($now-$stamp))) : 0;
				$percent = sprintf("%.2f%%",($count*100/$total));//全体における割合
				$link = "<a href=\"$refurl\">$refurl</a>";
				break;
			}
		}
		//データの配列
		$vals = array($file_id,$descript,$file_date,$total_days,$dl_perday,$count,$percent,$link);
		//対応させて表示
		for($c=0; $c<count($details); $c++){
			echo "  <tr>\n    <td width=35% bgcolor=ced5ff>$details[$c]</td>\n    <td bgcolor=e0e8ff>$vals[$c]</td>\n  </tr>\n";
		}
		
		echo "<tr><td colspan=2 align=left bgcolor=A8C1F1><a href=\"$GLOBALS[PHP_SELF]\">&lt;&lt;Back</a></td></tr>";
		echo "</table>\n";
	} else {
		echo "</td>\n  </tr>\n</table>\n
		<table border=0 cellpadding=3 cellspacing=2 width=560 align=center><tr><td>そんな機能ないもん。</td>\n  </tr>\n</table>";
	}
	foot();//フッタ表示
}
/* 管理画面表示　*/
function show_track(){
	global $PHP_SELF;

	$jump = $PHP_SELF."?job=admin&admin=".md5(ADMINPASS);

	?>
<html><head>
<meta http-equiv="Expires" content="Tue, 01 Jan 1980 1:00:00 GMT">
<meta http-equiv="Pragma" content="no-cache">
<title>Administration Mode</title>
<style type="text/css">
<!--
td {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
a {  text-decoration: none; color: #000000}
a:hover { color: #000099; text-decoration: underline }
.input {  font-size: 8pt}
tt {  font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13pt; font-weight: bold}
//-->
</style>
<script language="Javascript">
<!--
function check_char(strg) {
 var found = 0;
 for (char_pos=0; char_pos<strg.length; char_pos++) {
		if (strg.charAt(char_pos) == "|" || strg.charAt(char_pos) == "&" || strg.charAt(char_pos) == "=" || strg.charAt(char_pos) == '"' || strg.charAt(char_pos) == "'") {
			 found = 1;
			 break;
		}
 }
 return (found == 1) ? false : true;
}
function set_value(entry, old_value) {
 var new_value = window.prompt(entry+" の値を変更します",old_value)
 var found = 0;
 if (new_value != "" && new_value != null) {
		for (char_pos=0; char_pos<new_value.length; char_pos++) {
			 if (!(new_value.charAt(char_pos) >= '0' && new_value.charAt(char_pos) <= '9')) {
				  found = 1;
			 }
		}
		if (found == 1) {
			 alert ("数字を入力してください！");
		}
		else if (new_value == old_value) {
			 alert ("変更されてません！");
		}
		else { 
			 window.location.href = "<? echo $jump;?>&act=set_value&id="+entry+"&new="+new_value
		}
 }
}
function ChangeEntry(entry, old_name, feature) {
 var new_name = window.prompt(entry+"　の新しい "+feature+" を入力してください",old_name)
 var found = 1;
 if (new_name != "" && new_name != null) {
		if (check_char(new_name)) {
				 found = 0;
		}
		if (found == 1) {
			 alert ("Error! "+entry+" に許可されてない文字が含まれています!");
		}
		else if (new_name == old_name) {
			 alert ("変更されてません！");
		}
		else if (feature == "説明") {
			 window.location.href = "<? echo $jump;?>&act=ren_entry&id="+entry+"&new="+new_name
		}
		else if (feature == "FileID") {
			 window.location.href = "<? echo $jump;?>&act=ren_id&id="+entry+"&new="+new_name
		}
		else if (feature == "URL") {
			 window.location.href = "<? echo $jump;?>&act=set_loc&id="+entry+"&new="+new_name
		}
		else if (feature == "解説ページ") {
			 window.location.href = "<? echo $jump;?>&act=set_ref&id="+entry+"&new="+new_name
		}
 }
}
function del_entry(entry) {
 if (window.confirm("本当に削除してもよろしいですか？("+entry+")")) {
		window.location.href = "<? echo $jump;?>&act=del_entry&id="+entry
 }
}
function check_form() {
 var found = 1;
 des_field=document.form.description.value;
 id_field=document.form.new_id.value;
 if (des_field == "" || id_field == "") {
		alert ("IDとタイトルを埋めてください！");
		return false;
 }
 if (check_char(des_field) && check_char(id_field)) {
		found = 0;
 }
 if (found == 1) {
		alert ("Error! 許可されていない文字です");
		return false;
 }
 if (document.form.new_loc.value.indexOf(".")==-1) {
		alert("ファイルの場所が正しくありません！");
		document.form.new_loc.focus();
		return false;
 }
}
// -->
</script>
</head>
<body bgcolor="#FFFFFF">
 <table width="560" border="0" align="center">
	<tr>
		<td height="40"><tt>Administration Mode</tt></td>
		<td height="40" align="right" valign="bottom"><small style="text-color:blue">▶</small><a href="<? echo $PHP_SELF;?>?">Back to Top Downloads</a></td>
	</tr>
 </table>
 <table bgcolor="#F7F7F7" border=1 cellspacing=0 cellpadding=3 width="560" align="center">
		<tr bgcolor="#A8E1A8"> 
			<td><b>外ﾘﾝｸ</b></td>
			<td><b>説明</b></td>
			<td><b>ﾌｧｲﾙID</b></td>
			<td><b>DL数</b></td>
			<td><b>場所</b></td>
			<td><b>解説ﾍﾟｰｼﾞ</b></td>
			<td><b>削除</b></td>
		</tr>
<?php
$lines = file(LOGFILE);
$totalfiles = count($lines);
for($i=0; $i<count($lines); $i++){
	list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
	if($location=="") $location = "-";
	if($refurl=="") $refurl="-";
	print "    <tr>\n      <td><a href=\"$_SERVER[PHP_SELF]?dl=$file_id\">◆</a></td>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$descript','説明')\">$descript</a></td>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$file_id','FileID')\">$file_id</a></td>\n";
	print "      <td><a href=\"javascript:set_value('$file_id','$count')\">$count</a></td>\n      <td><a href=\"javascript:ChangeEntry('$file_id','$location','URL')\">$location</a></td>\n"; 
	print "      <td><a href=\"javascript:ChangeEntry('$file_id','$refurl','解説ページ')\">$refurl</a><td><a href=\"javascript:del_entry('$file_id')\">削除</a></td>\n    </tr>\n";
	$total+=$count;
}
echo '
		<tr> 
			<td colspan="2"><b><font color="#336600">&gt;&gt;Total Files:</font></b> <font color="#FF0000">'.$totalfiles.'</font></td>
			<td colspan="5"><b><font color="#336600">&gt;&gt;Total Downloads:</font></b> <font color="#FF0000">'.$total.'</font></td>
		</tr>
	</table>
	<form method="post" action="'.$PHP_SELF.'" name="form">
		<table width="560" border="0" align="center">
			<tr>
';?>
				<td>ファイルの場所： </td>
				<td><input type="text" name="new_loc" size="42"><small>dl.phpからのパスかURL</small>
				  <input type="hidden" name="job" value="admin">
				  <input type="hidden" name="admin" value="<? echo md5(ADMINPASS);?>">
			</tr>
			<tr> 
				<td>ファイル ID： </td>
				<td><input type="text" name="new_id" size="42"></td>
			</tr>
			<tr> 
				<td>表示タイトル： </td>
				<td><input type="text" name="description" size="42"></td>
			</tr>
			<tr> 
				<td>解説ページURL： </td>
				<td><input type="text" name="new_ref" size="42">
				  <input type="submit" name="submit" value="Add Entry" onclick="return check_form()" class="input"></td>
			</tr>
		</table>
	</form>
<br><center>リンクの仕方　dl.php?dl=[ファイルID]</center>
</body>
</html>
<?
exit;
}
/* 追加・変更・削除 */
function manage($manage,$id,$new){
	$lines = file(LOGFILE);//ログ読み込み
	$find = FALSE;//フラグ

	for($i=0; $i<count($lines); $i++){
		//データ分解
		list($file_id,$count,$location,$stamp,$descript,$refurl) = explode("|",$lines[$i]);
		if($id == $file_id){//同じIDだったら
			switch($manage){	//モード分岐
			case 'del_entry':	//削除
				$lines[$i] = "";
				$find = TRUE;
				break;
			case 'ren_entry':	//説明変更
				$new = chop($new);
				if(get_magic_quotes_gpc()) $new = stripslashes($new);
				$lines[$i] = "$file_id|$count|$location|$stamp|$new|$refurl|\n";
				$find = TRUE;
				break;
			case 'ren_id':	//ID変更
				$new = preg_replace("/\s*/","",$new);
				$lines[$i] = "$new|$count|$location|$stamp|$descript|$refurl|\n";
				$find = TRUE;
				break;
			case 'set_value':	//カウント変更
				$lines[$i] = "$file_id|$new|$location|$stamp|$descript|$refurl|\n";
				$find = TRUE;
				break;
			case 'set_loc':	//場所変更
				$new = preg_replace("/\s*/","",$new);
				$lines[$i] = "$file_id|$count|$new|$stamp|$descript|$refurl|\n";
				$find = TRUE;
				break;
			case 'set_ref':	//参照URL変更
				$lines[$i] = "$file_id|$count|$location|$stamp|$descript|$new|\n";
				$find = TRUE;
				break;
			}
		}
	}
	if($find){//ログ更新
		$fp = fopen(LOGFILE, "w");
		flock($fp, LOCK_EX);
		fputs($fp, implode("", $lines));
		fclose($fp);
	}
}
/* 新規追加 */
function add_data($url,$id,$desc,$ref){
	//整形
	$id = str_replace("|","",$id);
	$id = str_replace("'","",$id);
	$id = chop(str_replace('"','',$id));
	$url = chop($url);
	if(strstr($url,".") && !empty($id)) {
		$lines = file(LOGFILE);
		$find = FALSE;
		//同じIDが登録されていないか
		for($i=1; $i<count($lines); $i++){
			list($file_id,) = explode("|",$lines[$i]);
			if($id == $file_id){
				$find = TRUE;
				break;
			}
		}
		if(!$find){//ログ書き込み
			$desc = str_replace("|","",$desc);
			$desc = str_replace("'","",$desc);
			$desc = chop(str_replace('"','',$desc));
			if(get_magic_quotes_gpc()) $desc = stripslashes($desc);
			$ref = str_replace("|","",$ref);
			$ref = str_replace("'","",$ref);
			$ref = chop(str_replace('"','',$ref));
			$stamp = time();
			$newline = "$id|0|$url|$stamp|$desc|$ref|\n";//フォーマット
			$fp = fopen(LOGFILE, "a");
			fputs($fp, $newline);
			fclose($fp);
		}
	}else{
		die("URLかIDが不正です");
	}
}
/* 管理モードログイン */
function validation($admin){

	if($admin && ($admin != md5(ADMINPASS)))
		$err = "<font color=red>パスワードが違います！</font>";
	if(empty($admin) || $err){
		?>
<html>
	<head><title>PHP Downloads</title>
	<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<style type="text/css">
<!--
td {  font-family: Arial, Helvetica, sans-serif; font-size: 8pt}
tt {  font-family: Verdana, Arial, Helvetica, sans-serif; font-size: 13pt; font-weight: bold}
-->
</style>
</head>
<body bgcolor="FFFFDD">
<form method="post" action="<? echo $PHP_SELF;?>">
 <table width="560" border="0" align="center">
	<tr>
		<td height="40"><tt>Administration Mode</tt></td>
	</tr>
 </table>
 <table width="560" border="0" cellspacing="0" cellpadding="3" align="center">
	 <tr bgcolor="A8E1E1">
		 <td><b>パスワードを入力してください</b></td>
	 </tr>
	 <tr bgcolor="F7F7F7"> 
		 <td> 
			 <input type="password" name="admin_submit">
			 <input type="submit" value="Submit">
			 <input type="hidden" name="job" value="admin">
		 </td>
	 </tr>
	 <tr bgcolor="F7F7F7"> 
		 <td><? echo $err;?></td>
	 </tr>
	 <tr> 
		 <td><br>
			 <a href="http://php.s3.to" target="_blank">レッツPHP!</a> 
			 | <a href="<? echo $PHP_SELF;?>?">Back to Top Downloads</a></td>
	 </tr>
 </table>
</form>
</body>
</html>
<?php
exit;
	}
	return true;
}
/*************main******************/
//ダウンロード
if(!empty($dl)){
	if(REFCHECK){
		$c_okurl=count($okurl);
		$is_okurl=false;
		for($i=0; $i<$c_okurl; $i++){
			if(isset($_SERVER['HTTP_REFERER']) && ($is_okurl = strstr($_SERVER['HTTP_REFERER'],$okurl[$i]) !== FALSE))
				break;
			if(!$is_okurl && $i==$c_okurl-1)
				die("指定URL以外からはDLできません");
		}
	}
	count_dl($dl);
	exit;
}
switch($job){
case 'detail':
	if($detail) show_detail($detail);
	break;
case 'admin':
	if($admin_submit) $admin = md5($admin_submit);
	validation($admin);
	if($act) manage($act,$id,$new);
	if($submit=="Add Entry") add_data($new_loc,$new_id,$description,$new_ref);
	show_track();
	break;
default:
	if(time()-filemtime(LOGFILE) > 3600) @copy(LOGFILE,LOGFILE.".bak");
	show_table($top,$option);
}
?>
