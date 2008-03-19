<?php
require("./ptboard.config.php");
require("./pte.lib.php");

$PTE = new PTELibrary(THEMEFILE);

function addPost() {
	$pwdc = isset($_COOKIE["pwdc"]) ? $_COOKIE["pwdc"] : "";
	$url='';

	if($_SERVER["REQUEST_METHOD"] != "POST") error("請使用此版提供的表單來上傳",$dest);
	
	if(isset($_POST[$tmpField = getFieldName(true)])) {
		$name = isset($_POST[$tmpField]) ? $_POST[$tmpField] : '';
		$email = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
		$sub = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
		$com = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
	} else {
		$name = isset($_POST[$tmpField = getFieldName(gmdate("Ymd",date())-1)]) ? $_POST[$tmpField] : '';
		$email = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
		$sub = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
		$com = isset($_POST[$tmpField = getFieldName()]) ? $_POST[$tmpField] : '';
	}
	$pwd = isset($_POST['pwd']) ? $_POST['pwd'] : '';
	$res = isset($_POST['res']) ? $_POST['res'] : 0;

	// 欄位陷阱
	$FTname = isset($_POST['name']) ? $_POST['name'] : '';
	$FTemail = isset($_POST['email']) ? $_POST['email'] : '';
	$FTsub = isset($_POST['sub']) ? $_POST['sub'] : '';
	$FTcom = isset($_POST['com']) ? $_POST['com'] : '';
	$FTreply = isset($_POST['reply']) ? $_POST['reply'] : '';
	if($FTname != 'spammer' || $FTemail != 'foo@foo.bar' || $FTsub != 'DO NOT FIX THIS' || $FTcom != 'EID OG SMAPS' || $FTreply != '') error("Trapped.");

	// 時間
	$time = time();
	$tim = $time.substr(microtime(),2,3);

	// 讀取記錄檔
	$fp = fopen(DATAFILE,"r+");
	flock($fp, 2);
	rewind($fp);
	$buf = @fread($fp,filesize(DATAFILE));
	if($buf=='') error("讀取記錄檔時發生錯誤");
	$line = explode("\n",$buf);
	$countline = count($line);
	for($i = 0; $i < $countline; $i++){
		if($line[$i] != ""){
			list($artno,) = explode(",", rtrim($line[$i]));  // 作成逆轉換標籤
			$lineindex[$artno] = $i + 1;
			$line[$i] .= "\n";
		}
	}

	// 檢查表單內容
	if(!$name || preg_match("/^[ |　|]*$/",$name)) $name="";
	if(!$com || preg_match("/^[ |　|\t]*$/",$com)) $com="";
	if(!$sub || preg_match("/^[ |　|]*$/",$sub)) $sub="";

	$name = str_replace("管理",'"管理"',$name);
	$name = str_replace("刪除",'"刪除"',$name);

	if(strlen($com) > COM_MAX && $pwd!=ADMIN_PASS) error("內文過長");
	if(strlen($name) > 100) error("名稱過長");
	if(strlen($email) > 100) error("E-mail過長");
	if(strlen($sub) > 100) error("標題過長");
	if(strlen($res) > 10) error("欲回應的文章編號可能有誤");

	// 取得主機位置名稱
	$host = gethostbyaddr($_SERVER["REMOTE_ADDR"]);

	// 時間和密碼的樣式
	srand((double)microtime()*1000000);
	if($pwd==""){
		if($pwdc==""){
			$pwd = rand(); $pwd = substr($pwd,0,8);
		}else
			$pwd = $pwdc;
	}

	$c_pass = $pwd;
	$pass = ($pwd) ? substr(md5($pwd),2,8) : "*";
	$youbi = array('日','一','二','三','四','五','六');
	$yd = $youbi[gmdate("w", $time+TIME_ZONE*60*60)] ;
	$now = gmdate("y/m/d",$time+TIME_ZONE*60*60)."(".(string)$yd.")".gmdate("H:i",$time+TIME_ZONE*60*60);

	if(DISP_ID){ // 顯示ID
		if($email && DISP_ID==1)
			$now .= " ID:???";
		else
			$now .= " ID:".substr(crypt(md5($_SERVER["REMOTE_ADDR"].TITLE.gmdate("Ymd", $time+TIME_ZONE*60*60)),'id'),-8);
	}

	// 文字修整
	$email = CleanStr($email); $email = str_replace("\r\n","",$email);
	$sub = CleanStr($sub); $sub = str_replace("\r\n","",$sub);
	$url = CleanStr($url); $url = str_replace("\r\n","",$url);
	$res = CleanStr($res); $res = str_replace("\r\n","",$res);
	$com = CleanStr($com);
	// 換行字元統一
	$com = str_replace("\r\n","\n",$com); 
	$com = str_replace("\r","\n",$com);
	// 去除連續空白
	$com = preg_replace("/\n((　| )*\n){3,}/","\n",$com);
	$com = str_replace("\n",'<br/>',$com);		// 換行字元用<br />代替
	$com = str_replace("\n"," ", $com);	// 若還有\n換行字元則取消換行

	$name = str_replace("◆","◇",$name);// 防止トリップ偽造
	$name = str_replace("★","☆",$name); // 防止キャップ偽造
	$name = str_replace("\r\n","",$name);
	$name = CleanStr($name);
	$tripped_name = ""; // 給キャップ(Cap)機能讀取之Trip樣式
	if(preg_match("/(#|＃)(.*)/",$name,$regs)){ // 使用トリップ(Trip)機能 (ex：無名#abcd)
		$cap = $regs[2];
		$cap = strtr($cap,array("&amp;"=>"&","&#44;"=>","));
		$name = preg_replace("/(#|＃)(.*)/","",$name);
		$salt = substr($cap."H.",1,2);
		$salt = preg_replace("/[^\.-z]/",".",$salt);
		$salt = strtr($salt,":;<=>?@[\\]^_`","ABCDEFGabcdef");
		$trip=substr(crypt($cap,$salt),-10);
		$tripped_name = trim($name)."◆$trip";
		$name .= "<span class='nor'>◆$trip</span>";
	}
	if(strpos($name,"fusianasan")!==false) $name=str_replace("fusianasan","<span class='nor'>".$host."</span>",$name);
	if(strpos($name,"mokorikomo")!==false) $name=str_replace("mokorikomo","<span class='nor'>".$_SERVER['REMOTE_ADDR']."</span>",$name);

	if(!$name) $name = "無名氏";

	// 管理員投稿名稱特別色
	if($pwd==ADMIN_PASS) $name = "<span class='Cname_admin'>".$name."</span>";
//	if(!$com) $com = "無內文";
	if(!$com) error("請填入內文");
	if(!$sub) $sub = "無標題";

	list($lastno,) = explode(",", $line[0]);
	$no = $lastno + 1;
	isset($ext) ? 0 : $ext = "";
	isset($W) ? 0 : $W = "";
	isset($H) ? 0 : $H = "";
	isset($chk) ? 0 : $chk = "";
	$newline = "$no,$now,$name,$email,$sub,$com,$url,$host,$pass,$ext,$W,$H,$tim,$chk,\n";
	$newline .= implode('', $line);
	ftruncate($fp,0);
	set_file_buffer($fp, 0);
	rewind($fp);
	fputs($fp, $newline);

	// 樹狀結構記錄檔更新
	$newline = '';
	$tp = fopen(TREEFILE,"r+");
	set_file_buffer($tp, 0);
	rewind($tp);
	$buf = @fread($tp,filesize(TREEFILE));
	if($buf=='') error("更新樹狀結構記錄檔時發生錯誤");
	$treeline = explode("\n",$buf);
	$counttree = count($treeline);
	for($i = 0; $i < $counttree; $i++){
		if($treeline[$i] != ""){
			$treeline[$i] .= "\n";
			$j = explode(",", rtrim($treeline[$i]));
			if($lineindex[$j[0]]==0) $line[$i] = '';
		}
	}
	if($res){
		for($i = 0; $i < $counttree; $i++){
			$rtno = explode(",", rtrim($treeline[$i]));
			if($rtno[0]==$res){
				$treeline[$i] = rtrim($treeline[$i]).','.$no."\n";
				$j = explode(",", rtrim($treeline[$i]));
				if(!preg_match('/^sage/i',ltrim($email))){
					$newline = $treeline[$i];
					$treeline[$i] = '';
				}
				break;
			}
		}
	}

	if(!$res) $newline="$no\n";
	$newline .= implode('', $treeline);
	ftruncate($tp,0);
	set_file_buffer($tp, 0);
	rewind($tp);
	fputs($tp, $newline);
	fclose($tp);
	fclose($fp);
	
	error('<script type="text/javascript">
function redir(){
	location.href = "'.$_SERVER['PHP_SELF'].'";
}
setTimeout("redir()", 1000);
</script>
請等候跳轉……
');
}

/* 樹狀結構刪除 */
function treedel($delno){
	$fp = fopen(TREEFILE,"r+");
	set_file_buffer($fp, 0);
	flock($fp, 2);
	rewind($fp);
	$buf = @fread($fp,filesize(TREEFILE));
	if($buf=='') error("更新樹狀結構記錄檔時發生錯誤");
	$line = explode("\n",$buf);
	$countline = count($line);
	if($countline > 2){
		for($i = 0; $i < $countline; $i++) if($line[$i] != "") $line[$i] .= "\n";
		for($i = 0; $i < $countline; $i++){
			$treeline = explode(",", rtrim($line[$i]));
			$counttreeline = count($treeline);
			for($j = 0; $j < $counttreeline; $j++){
				if($treeline[$j] == $delno){
					$treeline[$j] = '';
					if($j==0) $line[$i] = '';
					else{
						$line[$i] = implode(',', $treeline);
						$line[$i] = str_replace(",,",",",$line[$i]);
						$line[$i] = preg_replace("/,$/","",$line[$i]);
						$line[$i] .= "\n";
					}
					break 2;
				}
			}
		}
		ftruncate($fp,0);
		set_file_buffer($fp, 0);
		rewind($fp);
		fputs($fp, implode('', $line));
	}
	fclose($fp);
}

function deletePost($no,$pwd) {
	$pwdc = isset($_COOKIE["pwdc"]) ? $_COOKIE["pwdc"] : "";
	$host = gethostbyaddr($_SERVER["REMOTE_ADDR"]);
	$delno = array("dummy");
	reset($_POST);
	while ($item = each($_POST))
		if($item[1]=='delete') array_push($delno,$item[0]);
	if($no) $delno=array_merge($delno,explode(',',$no));
	if($pwd=="" && $pwdc != "") $pwd = $pwdc;
	$fp = fopen(DATAFILE,"r+");
	set_file_buffer($fp, 0);
	flock($fp, 2);
	rewind($fp);
	$buf = @fread($fp,filesize(DATAFILE));
	if($buf=='') error("刪除發生錯誤");
	$line = explode("\n",$buf);
	$countline = count($line);
	for($i = 0; $i < $countline; $i++){ if($line[$i]!="") $line[$i] .= "\n"; }
	$flag = FALSE;
	for($i = 0; $i < $countline-1; $i++){
		list($dno,,,,,,,$dhost,$pass,$dext,,,$dtim,) = explode(",", $line[$i]);
		if(array_search($dno,$delno) && (substr(md5($pwd),2,8) == $pass || $dhost == $host||ADMIN_PASS==$pwd)){
			$flag = TRUE;
			$line[$i] = ""; // 密碼比對符合，此文章刪除
			treedel($dno);
		}
	}
	if($flag){ // 更新投稿文字記錄檔
		ftruncate($fp,0);
		set_file_buffer($fp, 0);
		rewind($fp);
		fputs($fp, implode('', $line));
	}
	fclose($fp);
	if(!$flag) error("無此文章或是密碼錯誤");
}

function showPost($res,$page=0) {
	global $PTE;
	$tmp_page=0;$p=0;$pte_vals=array();

	$tree = file(TREEFILE);
	$counttree = count($tree);
	$find = false;
	if($res){
		for($i = 0;$i<$counttree;$i++){
			list($artno,) = explode(",",rtrim($tree[$i]));
			if($artno==$res){ $st = $i; $find = true; break; } // 先搜尋欲回應之原文章
		}
		if(!$find) error("欲回應之文章並不存在！");
	}
	$line = file(DATAFILE);
	$countline = count($line);
	for($i = 0; $i < $countline; $i++){
		list($no,) = explode(",", $line[$i]);
		$lineindex[$no] = $i + 1; // 作成逆轉換標籤
	}

	if(!$res){ // php動態生成討論串用
		if($page < 0 || ($counttree && ($page * PAGE_AMOUNT) >= $counttree)) error("對不起，您所要求的頁數並不存在"); // $page 超過範圍則顯示錯誤畫面，與MySQL版不同 (但預設值0不必擋)
		$tmp_page = $page; // 進行跳頁所用
	}

	$dat = '';$tmprno=0;
	$pte_vals += array('{$TITLE}' => TITLE, '{$SELF}' => $_SERVER['PHP_SELF'],
		'{$FORM_NAME_FIELD}' => '<input class="hide" type="text" name="name" value="spammer" /><input type="text" name="'.getFieldName(true).'" id="fname" size="28" />',
		'{$FORM_EMAIL_FIELD}' => '<input type="text" name="'.getFieldName().'" id="femail" size="28" /><input type="text" class="hide" name="email" value="foo@foo.bar" />',
		'{$FORM_TOPIC_FIELD}' => '<input class="hide" value="DO NOT FIX THIS" type="text" name="sub" /><input type="text" name="'.getFieldName().'" id="fsub" size="28" />',
		'{$FORM_COMMENT_FIELD}' => '<textarea name="'.getFieldName().'" id="fcom" cols="48" rows="4" style="width: 400px; height: 80px;"></textarea><textarea name="com" class="hide" cols="48" rows="4">EID OG SMAPS</textarea>');

	$pte_vals['{$POSTS}'] = array();
	if(!$res) $st = $tmp_page * PAGE_AMOUNT;
	if($res) $pte_vals['{$RESTO}'] = $res;

	for($i = $st; $i < $st + PAGE_AMOUNT; $i++){
		$fil=''; $alert='';
		if(empty($tree[$i])) continue;
		$treeline = explode(",", rtrim($tree[$i]));
		$disptree = $treeline[0];
		$j = $lineindex[$disptree] - 1; // 在$j陣列搜尋該本文
		if(empty($line[$j])) continue;   // 在$j之範圍外
		list($no,$now,$name,$email,$sub,$com,$url,$host,$pwd,$ext,$w,$h,$time,$chk) = explode(",", $line[$j]);
		if($email) $name = "<a href=\"mailto:$email\">$name</a>";
		$com = auto_link(quoteLight($com));

		// 討論串所有回應生成
		if(!$res){
			$s = count($treeline) - PAGE_AMOUNT;
			if($s<1) $s = 1;
			elseif($s > 1)
				$alert .= "<font color=\"#707070\">有回應 ".($s - 1)." 篇被省略。要閱讀所有回應請按下回應連結。</font><br>\n";
			$RES_start = $s; // 分頁開始回應指標
			$RES_end = count($treeline); // 分頁結束回應指標
		}else{ // 回應模式
			$RES_start = 1; // 分頁開始回應指標
			$RES_end = count($treeline); // 分頁結束回應指標

			if(PAGE_AMOUNT > 0){ // PAGE_AMOUNT有設定 (開啟分頁)
				$countresALL = count($treeline) - 1; // 總回應數
				if($countresALL > 0){ // 有回應才做分頁動作
					if($page=="RE_PAGE_MAX" && is_string($page)){ // 特殊值：最末頁
						$page = intval($countresALL/PAGE_AMOUNT); // 最末頁資料指標位置
						if(!($countresALL % PAGE_AMOUNT)) $page--; // 如果回應數和一頁顯示取餘數=0，則-1
						$RES_end = $countresALL + 1; // 分頁結束回應指標
					}elseif($page=="all" && is_string($page)){ // 特殊值：全部
						$page=0;
						$RES_end = $countresALL + 1; // 分頁結束回應指標
					}else{
						if($page < 0) $page = 0; // 負數要求惡搞合理化
						if($page * PAGE_AMOUNT >= $countresALL) error("對不起，您所要求的頁數並不存在"); // 超過最大筆數，顯示錯誤
						$RES_end = ($page + 1) * PAGE_AMOUNT + 1; // 分頁結束回應指標
					}
					$RES_start = $page * PAGE_AMOUNT + 1; // 分頁開始回應指標
				}else // 沒有回應的情況只允許page = 0 或負數(惡搞合理化)
					if($page > 0) error("對不起，您所要求的頁數並不存在");
			}
		}
		$pte_vals['{$POSTS}'][$i] = array('{$NO}'=>$no, '{$SUB}'=>$sub, '{$NAME}'=>$name, '{$NOW}'=>$now, '{$COM}'=>$com, '{$ALERT}' => $alert);
		if(!$res) $pte_vals['{$POSTS}'][$i] += array('{$REPLYBTN}' => "[<a href=\"$_SERVER[PHP_SELF]?res=$no\">回應</a>]");

		$pte_vals['{$POSTS}'][$i]['{$RES}'] = array();
		for($k = $RES_start; $k < $RES_end; $k++){
			if($k<0 || $k>count($treeline)-1) continue;
			$disptree = $treeline[$k];
			$j = $lineindex[$disptree] - 1;
			if($line[$j]=="") continue;
			list($no,$now,$name,$email,$sub,$com,$url,$host,$pwd,$ext,$w,$h,$time,$chk) = explode(",", $line[$j]);
			if($email) $name = "<a href=\"mailto:$email\">$name</a>";
			$com = auto_link(quoteLight($com));
			$pte_vals['{$POSTS}'][$i]['{$RES}'][$k] = array('{$NO}'=>$no, '{$SUB}'=>$sub, '{$NAME}'=>$name, '{$NOW}'=>$now, '{$COM}'=>$com);
		}
		$p++;
		if($res) break; // 為回應模式時僅輸出單一討論串
	}

	$prev = ($res) ? (($page-1) * PAGE_AMOUNT) : ($st - PAGE_AMOUNT);
	$next = ($res) ? (($page+1) * PAGE_AMOUNT) : ($st + PAGE_AMOUNT);
	// 換頁判斷
	if($res){ // 回應分頁
		if(PAGE_AMOUNT > 0){ // 回應分頁開啟
			$dat .= "<table align='left' class='navi'><tr>";
			if($prev >= 0)
				 $dat .= "<td><form action=\"$_SERVER[PHP_SELF]?res=$res&amp;page=".$prev/PAGE_AMOUNT."\" method=post><input type=submit value=\"上一頁\"></form></td><td>";
			else $dat .= "<td nowrap>第一頁</td><td>";
			if($countresALL==0) // 無回應
				$dat .= "[<b>0</b>] ";
			else{
				$AllRes=isset($_GET['page']) && $_GET['page']=="all";
				for($i = 0; $i < $countresALL ; $i += PAGE_AMOUNT){
					if($page==$i/PAGE_AMOUNT && !$AllRes) $dat .= "[<b>".$i/PAGE_AMOUNT."</b>] ";
					else
						$dat .= "[<a href=\"$_SERVER[PHP_SELF]?res=$res&amp;page=".$i/PAGE_AMOUNT."\">".$i/PAGE_AMOUNT."</a>] ";
				}
				$dat .= $AllRes?"<span style='white-space: nowrap;'>[<b>全部</b>]</span> ":"<span style='white-space: nowrap;'>[<a href=\"$_SERVER[PHP_SELF]?res=$res&amp;page=all\">全部</a>]</span> ";
			}
			$dat .= "</td>";
			if($countresALL > $next && !$AllRes)
				 $dat .= "<td><form action=\"$_SERVER[PHP_SELF]?res=$res&amp;page=".$next/PAGE_AMOUNT."\" method=post><input type=submit value=\"下一頁\"></form></td><td>";
			else $dat .= "<td nowrap>最後一頁</td>";
			$dat .= "</tr></table><br clear='all'>\n";
		}
	}else{ // 一般分頁
		$dat .= "<table align='left' class=\"navi\"><tr>";
		if($prev >= 0){
			if($prev==0)
				$dat .= "<td><form action=\"$_SERVER[PHP_SELF]\" method='get'>";
			else
				$dat .= "<td><form action=\"$_SERVER[PHP_SELF]?page=".$prev/PAGE_AMOUNT."\" method=post>"; // 直接用php生成
			$dat .= "<input type=submit value=\"上一頁\">";
			$dat .= "</form></td>";
		}else{ $dat.="<td nowrap>第一頁</td>"; }
			$dat.="<td>";
			for($i = 0; $i < $counttree ; $i += PAGE_AMOUNT){
				if($st==$i) $dat .= "[<b>".($i/PAGE_AMOUNT)."</b>] ";
				else
					 $dat .= "[<a href=\"$_SERVER[PHP_SELF]?page=".$i/PAGE_AMOUNT."\">".$i/PAGE_AMOUNT."</a>] "; // 直接用php生成
			}

			$dat .= "</td>";
			if($p >= PAGE_AMOUNT && $counttree > $next){
				$dat .= "<td><form action=\"$_SERVER[PHP_SELF]?page=".$next/PAGE_AMOUNT."\" method=post>";
				$dat .= "<input type=submit value=\"下一頁\">";
				$dat .= "</form></td>";
			}else $dat .= "<td nowrap>最後一頁</td>";
			$dat .= "</tr></table><br clear=all>\n";
	}

	$pte_vals['{$PAGENAV}'] = $dat;
	$pte_vals['{$FOOTER}'] = 'Rev: 1.1.2';

	echo $PTE->ParseBlock('main',$pte_vals);
	exit();
}

/* 文字修整 */
function CleanStr($str){
	global $admin;
	$str = trim($str); // 去除前後多於空白
	if (get_magic_quotes_gpc()) // "＼"斜線符號去除
		$str = stripslashes($str);
	if($admin != ADMIN_PASS) // 管理員可以使用HTML標籤
		$str = preg_replace("/&amp;(#[0-9]+|[a-z]+);/i", "&$1;", htmlspecialchars($str)); // 非管理員：HTML標籤禁用
	return str_replace(",", "&#44;", $str); // ","逗號用參照碼方式取代
}

/* 網址自動連結 */
function auto_link($proto){
	return preg_replace('/(https?|ftp|news)(:\/\/[\w\+\$\;\?\.\{\}%,!#~*\/:@&=_-]+)/u', '<a href="$1$2" rel="_blank">$1$2</a>', $proto);
}

/* 引用標註 */
function quoteLight($comment){
	return preg_replace('/(^|<br\s*\/>\s*)((?:&gt;|＞).*?)(?=<br\s*\/>|$)/u', '$1<span class="quoteMsg">$2</span>', $comment);
}

function getFieldName($init=false) {
	$chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_+!%#@';

	if($init){ // Seed last for two days
		$seed=is_int($init)?$init:gmdate('Ymd',time());
		mt_srand($seed);
	}

	$str=$chars{mt_rand(0,51)};
	for($i=1;$i<FIELDNAME_LENGTH;$i++)
		$str.=$chars{mt_rand(0,68)};

	return $str;
}

function error($mesg) {
	global $PTE;
	echo $PTE->ParseBlock('error',array('{$mes}'=>$mesg, '{$TITLE}'=>TITLE, '{$FOOTER}'=>'', '{$SELF}'=>$_SERVER['PHP_SELF']));
	exit();
}


/* 程式首次執行之初始化 */
function init(){
	$err = "";
	$chkfile = array(DATAFILE,TREEFILE);
	if(!is_writable(realpath("./"))){ error("根目錄沒有寫入權限，請修改權限<br>"); }
	foreach($chkfile as $value){
		if(!file_exists(realpath($value))){
			$fp = fopen($value, "w");
			set_file_buffer($fp, 0);
			if($value==DATAFILE) fputs($fp,"1,05/01/01(六) 00:00 ID:00000000,無名氏,,無標題,無內文,,,,,,,,\n");
			if($value==TREEFILE) fputs($fp,"1\n");
			fclose($fp);
			if(file_exists(realpath($value))) @chmod($value,0666);
		}
		if(!is_writable(realpath($value))) $err .= "檔案：".$value."無法寫入<br>";
		if(!is_readable(realpath($value))) $err .= "檔案：".$value."無法讀取<br>";
	}
	if($err) error($err);
}

extract($_POST);
extract($_GET);
$iniv=array('action','res','no','pwd','page');
foreach($iniv as $iniva){
  if(!isset($$iniva)) $$iniva="";
}

init();

switch($action) {
  case 'post':
    addPost();
    break;
  case 'delete':
    deletePost($no,$pwd);
  default:
	if($res){ // 回應模式輸出
		$page = isset($_GET['page']) ? $_GET['page'] : 'RE_PAGE_MAX';
		if(!($page=='all' || $page=='RE_PAGE_MAX')) $page = intval($_GET['page']);
	}
    showPost($res,$page);
}
