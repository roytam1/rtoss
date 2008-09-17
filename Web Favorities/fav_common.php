<?php
$NoNavi=$oldNetscape=$noXML=false;$mozver=$iever=$SidebarSuffix1=$admAppend=$aAppend='';

$ua=strtolower($_SERVER["HTTP_USER_AGENT"]);
if(strpos($ua,'mozilla/')!==false) {
  preg_match('#mozilla/(\d+\.*\d+)#',$ua,$m);
  $mozver=$m[1];
}
if(strpos($ua,'msie ')!==false) {
  preg_match('#msie (\d+\.*\d+)#',$ua,$m);
  $iever=$m[1];
}

if ((strpos($ua,"mozilla/4.0")===false && strpos($ua,"mozilla/5.0")===false && strpos($ua,"opera/")===false) ||
	(strpos($ua,"mozilla/5.0")!==false && strpos($ua,"opera")===false && strpos($ua,"gecko")===false && strpos($ua,"webkit")===false && strpos($ua,"khtml")===false) ||
	($mozver && $mozver < '4.0') || ($iever && $iever < '5.5'))
    $oldNetscape=true;

if (isset($_GET["F"])) {
  if ($_GET["F"]=="Y") $oldNetscape=false;
  if ($_GET["F"]=="N") $oldNetscape=true;
}
if ($oldNetscape) $NoNavi=true;

if (!isset($_GET["noxml"]))
  if ((strpos(strtoupper($_SERVER["HTTP_USER_AGENT"]),"MSIE") ? strpos(strtoupper($_SERVER["HTTP_USER_AGENT"]),"MSIE")+1 : 0)>0)
    $noXML=true;

if (isset($_GET["noxml"]) && $_GET["noxml"]=="Y") $noXML=true;
if (isset($_GET["noxml"]) && $_GET["noxml"]=="N") $noXML=false;
if (isset($_GET["sidebar"]) && $_GET["sidebar"]=="Y") {
  $InSidebar=true;
  $DispNavi=false;
} else {
  $InSidebar=false;
  $DispNavi=true;
}
if ($InSidebar) {
  $aAppend="target=\"_content\"";
  $admAppend="target=\"_search\"";
  $SidebarSuffix1="?sidebar=Y";
  $SidebarSuffix2="&sidebar=Y";
  $TextboxWidth=30;
} else {
  $aAppend=$admAppend=$SidebarSuffix1=$SidebarSuffix2="";
}

if ($NewWin && !$InSidebar) $aAppend="target=\"_blank\"";

if ($shrinkFirst && $oldNetscape) $shrinkFirst=!$oldNetscape;
if ($shrinkFirst==true) $divShrink="dh";
else $divShrink="dv";

function text2xml($StrIn) {
  global $noXML;
  return $noXML?$StrIn:htmlspecialchars($StrIn);
}

function fullURL() {
	$filename=preg_replace('/.*\/+/',"",$_SERVER['PHP_SELF']);
	$path=preg_replace("/$filename$/","",$_SERVER['PHP_SELF']);
	return "http://".$_SERVER["HTTP_HOST"].$path;
}

function val($arr,$v,$nullval='') {
	return isset($arr[$v])?$arr[$v]:$nullval;
}
function val2($arr,$v,$wholeval='%2$s',$nullval='') {
	return isset($arr[$v])?sprintf($wholeval,$v,$arr[$v]):$nullval;
}
function toQueryString(/*$arg1, $arg2...$argN*/) {
	if (!($argcnt = func_num_args())) // called with no arg
		return '';
	$arg_list = func_get_args();
	$qrystr = '';
	for($i=0;$i<$argcnt;$i++) {
		$qrystr.=isset($_GET[$arg_list[$i]])?($qrystr?'&':'').$arg_list[$i].'='.$_GET[$arg_list[$i]]:'';
	}
	return $qrystr;
}

function logInOut($pass,$logout=false) {
	global $FavPasswd,$ViewPassword;
	if (isset($_SESSION["pwd"]) && ($_SESSION['pwd']!=$FavPasswd) || $logout) {
	  unset($_SESSION["pwd"]);
	  unset($_SESSION["isLogined"]);
	  return true;
	} elseif ($pass && ($pass==$FavPasswd)) {
	  $_SESSION['isLogined']=1;
	  $_SESSION['pwd']=$_POST["pwd"];
	  return true;
	} else return false;
}

function viewAuth($action='auth',$pass='') {
	global $FavPasswd,$ViewPassword;
	switch($action) {
		case 'auth':
			if(isset($_COOKIE['favsess'])) {
				@list($hash,$exptime,$type) = @explode('.',$_COOKIE['favsess']);
				if($type == 2 && md5('favsess'.$FavPasswd.$exptime) == $hash && time() <= $exptime) return true;
				if($type == 1 && md5('favsess'.$ViewPassword.$exptime) == $hash && time() <= $exptime) return true;
				viewAuth('logout');
			}
			return false;
		case 'login':
			$exptime = time()+3600;
			setcookie("favsess", md5('favsess'.$pass.$exptime).'.'.$exptime.'.'.(($pass==$FavPasswd)?2:1), time()+3600);
			break;
		case 'logout':
			setcookie("favsess", '', time()-3600);
	}
}
?>