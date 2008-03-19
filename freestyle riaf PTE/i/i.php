<?php
require_once "../conf.php";
require_once "../bb2html.php";
//require_once "../htmltemplate.inc";
require_once "../lib_pte.php";

$arg = array();

if ($_GET['mode'] == "post") {
    $arg['post'] = true;
} else {
    $arg['main'] = true;
} 

$arg['dir_path'] = $dir_path;
// スレ読み込み
$subject = file("../" . $subj_file);
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
    $res = array();
    if ($subject[$i] == "") break;
    list($key_dat, $title) = explode(",", $subject[$i]);
    list($key,) = explode(".", $key_dat);
    $ddir = str_replace("./", "", $ddir);
    $logfile = "../" . $ddir . $key_dat;
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
    } else {
        $rst = 2;
    } 
    for($j = $rst;$j <= $resnum; $j++) {
        $over = false;
        if ($log[$j-1] == "") break;
        list($name, $email, $now, $com, , $host) = explode(",", $log[$j-1]);
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
        $resarr = compact('name', 'email', 'now', 'com', 'host', 'maillink', 'over');
        $resarr['no'] = $j;
        $res[] = $resarr;
    } 
    // メインHTML
    $oya = compact('fname', 'femail', 'fdate', 'fcom', 'fsub', 'fhost', 'fmaillink', 'next', 'prev', 'no', 'key', 'resnum', 'res');
    $arg['oya'][] = $oya;
} 

$next = $page + 1;
$prev = $page-1;

$arg['page'] = '<br>';
$arg['dir_path'] = $dir_path;
$arg['read_def'] = $read_def;
$arg['tit'] = $tit;
if ($prev > 0) $arg['prev'] = $prev;
if ($no < $sub_num) $arg['next'] = $next;
// print_r($arg);
//HtmlTemplate::t_include("../" . $bbs_skin_mb, $arg);
$pteargs=toPTEargs($arg,$utf82sjis);
$pc_PTE=new PTELibrary("../$bbs_skin_mb");
echo $pc_PTE->ParseBlock('MAIN',$pteargs);

// header("Location: index.html");

?>
