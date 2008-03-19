<?php
require_once "./conf.php";
require_once "./bb2html.php";
require_once "./htmltemplate.inc";

$arg = array();

$arg['main'] = true;

$arg['dir_path'] = $dir_path;
$arg['tit'] = $tit;
$arg['rmes'] = $rmes;
$arg['tmes'] = $tmes;
$arg['kanban'] = $kanban;
$arg['pc_index'] = $pc_index;
$arg['NIch_index'] = $NIch_index;
// スレ読み込み
$subject = @file($subj_file);
$sub_num = sizeof($subject);

$st = 0;
$page = 1;
// ヘッドライン
$headto = ($thre_def < count($subject)) ? $thre_def : count($subject);
$n = 1;
for($h = $st;$h < $headto;$h++, $n++) {
	list($kdate, $title1) = explode(",", $subject[$h]);
	list($kdate,) = explode(".", $kdate);
	$title = "";
	$hno = $h + 1;
	// $resnum = chop($resnum);
	// if(strlen($title1) > $thre_cut) $title = substr($title1,0,$thre_cut) ."..";
	// else
	$title = trim($title1);
	$tr = ($n % 3 == 0) ? true : false;
	if ($hno <= ($page - 1) * $page_def + $page_def) {
		$arg['head'][] = array('title' => $title, 'key' => $kdate, 'no' => $hno, 'tr' => $tr);
	} else {
		$arg['over'][] = array('title' => $title, 'key' => $kdate, 'no' => $hno, 'tr' => $tr);
	} 
} 
// メインループ
for($i = $st;$i < $st + $page_def;$i++) {
    $res = array(); $skip=0;
    if ($subject[$i] == "") break;
    list($key_dat, $title) = explode(",", $subject[$i]);
    list($key,) = explode(".", $key_dat);
    $logfile = $ddir . $key_dat;
    if (!file_exists($logfile)) $main .= "ファイルが見つかりません";
    $log = file($logfile);
    $resnum = count($log);
    list($fname, $femail, $fdate, $fcom, $fsub, $fhost) = explode(",", $log[0]); //親
    $no = $i + 1;
    $next = $no + 1;
    $prev = $no - 1;
    $fcom = bb2html($fcom);
    $fmaillink = ($femail != "") ? true : false;
	/* レス */
	if ($resnum > $res_def) {
		$rst = $resnum - $res_def + 1;
		$skip = $rst - 2;
	} else {
		$rst = 2;
	} 
    for($j = $rst;$j <= $resnum; $j++) {
        $over = false;$show = true;
        if ($log[$j-1] == "") break;
        list($name, $email, $now, $com, , $host) = explode(",", $log[$j-1]);
        if($taborn > 1 && $now == $aborn) $show = false;
        if($taborn > 0 && $now == $aborn && trim($com) == $aborn) $show = false;
        if (substr_count($com, "<br>") > $viewline) {
            $comarr = explode("<br>", $com);
            $com = "";
            for($br = 0; $br < $viewline; $br++) { // 投稿行数オーバー
                $com .= $comarr[$br] . "<br>";
            } 
            $over = true;
        } 
        if (strlen($com) > $viewbyte) { // 投稿文字数オーバー
            $com = substr($com, 0, $viewbyte) . "...";
            $over = true;
        } 
        $com = bb2html($com);
        $maillink = ($email != "") ? true : false; 
        // レスHTML
        $resarr = compact('name', 'email', 'now', 'com', 'host', 'maillink', 'over', 'show');
        $resarr['no'] = $j;
        $res[] = $resarr;
    } 
    // メインHTML
    $oya = compact('fname', 'femail', 'fdate', 'fcom', 'fsub', 'fhost', 'fmaillink', 'next', 'prev', 'no', 'key', 'resnum', 'skip', 'res');
    $arg['oya'][] = $oya;
} 

$pc_html = HtmlTemplate::t_buffer("$bbs_skin_pc", $arg);
$NIch_html = HtmlTemplate::t_buffer("$NIch_skin_pc", $arg);
$mb_html = HtmlTemplate::t_buffer("$bbs_skin_mb", $arg);

$fp = fopen($pc_index, "w") or error("Cannot open Index file $pc_index");
fputs($fp, $pc_html);
fclose($fp);

$fp = fopen($NIch_index, "w") or error("Cannot open Index file $NIch_index");
fputs($fp, $NIch_html);
fclose($fp);

$fm = fopen($mb_index, "w") or error("Cannot open Index file $mb_index");
fputs($fm, $mb_html);
fclose($fm);
// header("Location: index.html");

?>
