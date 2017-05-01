<?php
require("./fav_settings.php");
require("./fav_strings.php");

function printAdmJs($id) {
	if(isset($_SESSION['isLogined']))
		return ' onmouseover="toggleAdmTool(\'adm'.$id.'\')" onmouseout="toggleAdmTool(\'adm'.$id.'\')"';
	else
		return '';
}

function printAdmTools($id) {
	global $SidebarSuffix2,$admAppend,$MyFav_Edit,$MyFav_Delete;
	return '<a href="'.text2xml("fav_action.php?action=edit&id=".$id.$SidebarSuffix2).'" class="admtool" '.$admAppend.' onclick="return true;">'.$MyFav_Edit.'</a>&nbsp;<a href="'.text2xml("fav_action.php?action=delete&id=".$id.$SidebarSuffix2).'" class="admtool" '.$admAppend.' onclick="return true;">'.$MyFav_Delete.'</a>';
}

if(isset($_POST['pwd'])||isset($_POST['logout'])) logInOut(val($_POST,'pwd'),isset($_POST['logout']));

// *** Read SQLite DB *** //
$db = new PDO('sqlite:./'.$sqlite_file, '', '', array(PDO::ATTR_PERSISTENT => true));

$qry="SELECT * FROM Fav WHERE cat = 1 ORDER BY ord,id";
$rs=$db->query($qry);

$ary=$rs->fetchAll(PDO::FETCH_ASSOC);

foreach($ary as &$acat) {
	$qry2="SELECT * FROM Fav WHERE cat = 0 AND catid = ".$acat['id']." ORDER BY ord,id";
	$rs2=$db->query($qry2);
	$ary2=$rs2->fetchAll(PDO::FETCH_ASSOC);
	$acat['childs']=$ary2;

}

$qry2="SELECT * FROM Fav WHERE cat = 0 AND catid = 0 ORDER BY ord,id";
$rs3=$db->query($qry2);
$ary3=$rs3->fetchAll(PDO::FETCH_ASSOC);
// *** Read SQLite DB *** //


if (!($oldNetscape || $noXML))
  header('Content-type: application/xhtml+xml');
echo '<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/2002/REC-xhtml1-20020801/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="zh-tw" lang="zh-tw">
<head>
<meta http-equiv="Content-Type" content="'.(($oldNetscape || $noXML)?'text/html':'application/xhtml+xml').'; charset=utf-8" />
<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7" /> <!-- IE8 workaround -->
<title>'.$MyFav_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css" />
<link rel="alternate" type="application/xml" title="RSS 1.0 Feed" href="rss.php" />
<link rel="alternate" type="application/xml" title="RSS 2.0 Feed" href="rss2.php" />
<link rel="alternate" type="application/xml" title="Atom Feed" href="atom.php" />
<style type="text/css">';
if (!($oldNetscape || $noXML)) echo '<![CDATA[';
if ($InSidebar) echo 'body {font-size: 9pt;}';
echo 'a.toggle { display:block; float:left; text-align:center; width:1em;}
a.admtool { font-size: 9pt;}
a.navi {
	font-size: 9pt;
	text-decoration: underline overline;
	line-height: 1em;
}
.dh {display:none;}
.dv {display:block;}
.divNavi {
	border: 1px dotted #999;
	background-color:#fff;
	top: 20px;
	right: 0px;
	position: absolute;
	text-align: right;
	line-height: 1em;
	padding: 2px;
	width:auto;
	height:auto;
	overflow:auto;
}
.divNavi-hide {
	border: 1px dotted #999;
	background-color:#fff;
	top: 20px;
	right: 0px;
	position: absolute;
	text-align: right;
	line-height: 1em;
	padding: 2px;
	width:1em;
	height:1em;
	overflow:hidden;
}
ul,li,dt,dl,dd { margin:0; padding:0; text-indent:0px; list-style-position:inside;}
dl dd div ul li {padding-left:0.35em;}
.admtools-hide {display:none;}
.admtools {display:inline; padding-left:1em;}
';
if (!($oldNetscape || $noXML)) echo ']]>';
echo '</style>
<script type="text/javascript">';
if (!($oldNetscape || $noXML)) echo '<![CDATA[';
echo 'var ie=window.clipboardData
var ns4=document.layers?1:0
var ns6=document.getElementById&&!document.all?1:0
var opera=window.opera?1:0
var geckoCSS2=(navigator.userAgent.indexOf("Gecko/")!=-1?navigator.userAgent.substr(navigator.userAgent.indexOf("Gecko/")+6,8):0)>=20011011?1:0;
var webkit=navigator.userAgent.indexOf("AppleWebKit/")!=-1?1:0;
var cookieName = "WebFavShow";
var inSidebar = '.(int)$InSidebar.';
var TimeoutTimer,TimerFlag;

function gID(s) { return document.getElementById(s); }

function ToggleDiv(divID,aID) {
	if (gID(divID).className=="dv") ShrinkDiv(divID,aID);
	else ExpandDiv(divID,aID);
}

function getLinkText(aID) {
	if (ie)	return gID(aID).innerText;
	else	return gID(aID).firstChild.nodeValue;
}

function setLinkText(aID,newText) {
	if (ie)	gID(aID).innerText=newText;
	else	gID(aID).firstChild.nodeValue=newText;
}

function toggleAllDiv(aID,command) {
	var aText=getLinkText(aID);

	var DIVcoll = document.getElementsByTagName("div");
	if (command=="invert") {
		for (var LC=0; LC<DIVcoll.length; LC++){
  			if (DIVcoll[LC].className=="dv") 
     			ToggleDiv(DIVcoll[LC].id,DIVcoll[LC].id.toString().replace(/d/,"a"));
  			else if (DIVcoll[LC].className=="dh") 
     			ToggleDiv(DIVcoll[LC].id,DIVcoll[LC].id.toString().replace(/d/,"a"));
  		}
	} else if (command=="force") {
		if (aText=="'.$MyFav_ExpandAll.'") {
			for (var LC=0; LC<DIVcoll.length; LC++){
	  			if (DIVcoll[LC].className=="dh")
    	 			ToggleDiv(DIVcoll[LC].id,DIVcoll[LC].id.toString().replace(/d/,"a"));
			}
			setLinkText(aID,"'.$MyFav_ShrinkAll.'");
		} else {
			for (var LC=0; LC<DIVcoll.length; LC++){
	  			if (DIVcoll[LC].className=="dv")
    	 			ToggleDiv(DIVcoll[LC].id,DIVcoll[LC].id.toString().replace(/d/,"a"));
			}
			setLinkText(aID,"'.$MyFav_ExpandAll.'");
		}
	}
}
function ExpandDiv(divID,aID) {
	gID(divID).className="dv";
	add_submenu(divID);
	setLinkText(aID,"'.$MyFav_ShrinkMark.'");
}
function toggleAdmTool(divID) {
	if(gID(divID).className=="admtools-hide") gID(divID).className="admtools";
	else gID(divID).className="admtools-hide";
}
function ShrinkDiv(divID,aID) {
	gID(divID).className="dh";
	remove_submenu(divID);
	setLinkText(aID,"'.$MyFav_ExpandMark.'");
}
function add2Sidebar() {
var myHref=self.location.href.replace(/#.*/g,"");
if (window.sidebar&&window.sidebar.addPanel)
 window.sidebar.addPanel("My Web Faviourites",myHref+"?sidebar=Y","");
else 
 alert("Your browser does not support this particular feature.");
}
function moveNavi() {
	var offsetTop = 20; needRepeat = true;

	if (ie && !window.XMLHttpRequest) { //<IE7
		divNavi.style.pixelTop = document.body.scrollTop + offsetTop;
//		divNavi.style.pixelLeft = document.body.clientWidth + document.body.scrollLeft - divNavi.offsetWidth - offsetTop;
		divNavi.style.pixelRight = offsetTop;
	} else if (ns6||opera||ie) {
		var obj = document.getElementById("divNavi");
		if (!(geckoCSS2||opera||webkit||ie)) 
			obj.style.top=window.pageYOffset+offsetTop;
		else {
			obj.style.right="12px";
			obj.style.position="fixed";
			needRepeat = false;
		}
	} else if (ns4) {
		// it causes error in netscape
//		eval(document.divNavi.top=eval(window.pageYOffset+offsetTop));
	}
	if(needRepeat) setTimeout("moveNavi()",100); //IEでスレが大きい場合CPUを食うようなので変更。
}
function minNavi() { document.getElementById("divNavi").className = "divNavi-hide";TimerFlag=0; }
function NaviTimout() {
	if(inSidebar && !TimerFlag) {
		TimeoutTimer=setTimeout("minNavi()",1000);
		TimerFlag=1;
	}
}
function restoreNavi() {
	if(TimerFlag) {
		clearTimeout(TimeoutTimer);
		TimerFlag=0;
	}
	document.getElementById("divNavi").className = "divNavi";
}

function add_submenu(id) {
	var submenus = get_submenu();
	for (var i = 0; i<submenus.length; i++) if (submenus[i] == id) return;
	submenus.push(id);
	set_submenu(submenus);
}
function remove_submenu(id){
	var submenus = get_submenu();
	var new_submenus = new Array();
	for (var i=0; i<submenus.length; i++) if (submenus[i] != id) new_submenus.push(submenus[i]);
	set_submenu(new_submenus);
}
function get_submenu() {
	var cookie = get_cookie(cookieName);
	if (cookie) return cookie.split(/,/);
	else return [];
}
function set_submenu(submenus) {
	set_cookie(cookieName, submenus.join(","), 365);
}
function get_cookie(name) {
	with(document.cookie) {
		var index = indexOf(name + "=");
		if (index == -1) return "";
		index = indexOf("=", index) + 1;
		var endstr = indexOf(";", index);
		if (endstr == -1) endstr = length;
		return unescape(substring(index, endstr));
	}
}
function set_cookie(name, value, days) {
	if(days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires="+date.toGMTString();
	} else expires = "";
	document.cookie = name+"="+value+expires+"; path=/";
}';
if (!($oldNetscape || $noXML)) echo ']]>';
echo '</script>';
if($jscroll) {
	echo '<script type="text/javascript" src="jquery.min.js"></script>
<script type="text/javascript" src="jquery.nicescroll.min.js"></script>
<script type="text/javascript">';
	if (!($oldNetscape || $noXML)) echo '<![CDATA[';
	echo '$(document).ready(
	function() {
		$("html").niceScroll({cursorcolor:"#888",cursorwidth:7,cursoropacitymin:0.25,mousescrollstep:13,scrollspeed:40});
		if(location.hash) document.getElementById("a"+location.hash.substring(1)).scrollIntoView(true);
	}
);';
	if (!($oldNetscape || $noXML)) echo ']]>';
	echo '</script>';
}
echo '</head>
<body onload="moveNavi();">
<center><h2>'.$MyFav_Title.'</h2></center>
<a name="top" id="atop"></a>';
if (isset($_SESSION['isLogined'])) echo '<a href="'.text2xml("fav_action.php?action=order&id=-1".$SidebarSuffix2).'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_CatOrder.'</a> ';
if (!$oldNetscape) echo '<a id="aToggle" href="'.text2xml("javascript:toggleAllDiv('aToggle','force');").'" class="admtool" onclick="return true;">'.($shrinkFirst?$MyFav_ExpandAll:$MyFav_ShrinkAll).'</a> <a id="aToggle2" href="'.text2xml("javascript:toggleAllDiv('aToggle2','invert');").'" class="admtool" onclick="return true;">'.$MyFav_InvertAll.'</a>';
echo '<div class="'.($NoNavi?'dh':($DispNavi?'divNavi':'divNavi-hide')).'" id="divNavi" onmouseover="restoreNavi()" onmouseout="NaviTimout()">';
if (!$NoNavi) {
	echo '<a href="#bottom" style="font-size:x-small;" onmouseover="restoreNavi()" onclick="return true;">'.$MyFav_GotoBottom."</a><br />\n";
	foreach($ary as $a) {
		echo '<a href="#'.$a['id'].'" class="navi" onmouseover="restoreNavi()" '.((!$oldNetscape)?'onclick="'.text2xml("ExpandDiv('d".$a['id']."','a".$a['id']."');").'"':'').'>'.text2xml($a['name'])."</a><br />\n";
	}
	echo '<a href="#top" style="font-size:x-small;" onclick="return true;">'.$MyFav_GotoTop."</a><br />\n";
}
echo '</div>';

function ary2listOflinks($arr) {
	global $oldNetscape,$divShrink,$shrinkFirst,$MyFav_ExpandMark,$MyFav_ShrinkMark,$SidebarSuffix2,$admAppend,$MyFav_Add,$MyFav_Order,$aAppend;
	$txt='';
	foreach($arr as $a) {
		if($a['cat']=="1") {
			$txt.= '<dt>';
			if(isset($_SESSION['isLogined'])) $txt.= '<div onmouseover="toggleAdmTool(\'adm'.$a['id'].'\')" onmouseout="toggleAdmTool(\'adm'.$a['id'].'\')">';
			if (!$oldNetscape)
				$txt.= '<a id="a'.$a['id'].'" href="'.text2xml("javascript:ToggleDiv('d".$a['id']."','a".$a['id']."');").'" class="toggle">'.($shrinkFirst?$MyFav_ExpandMark:$MyFav_ShrinkMark).'</a> ';
			$txt.= '<a name="'.$a['id'].'"'.($a['addr']?' href="'.text2xml($a['addr']).'" '.$aAppend:'').'>'.text2xml($a['name']).'</a>';

			if (isset($_SESSION['isLogined'])) $txt.= '<span class="admtools-hide" id="adm'.$a['id'].'"><a href="'.text2xml("fav_action.php?action=add&catid=".$a['id'].$SidebarSuffix2).'" class="admtool" '.$admAppend.' onclick="return true;">'.$MyFav_Add.'</a>&nbsp;<a href="'.text2xml("fav_action.php?action=order&id=".$a['id'].$SidebarSuffix2).'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_Order.'</a>&nbsp;'.printAdmTools($a['id']).'</span></div>';

			
			$txt .='</dt><dd><div id="d'.$a['id'].'" class="'.$divShrink.'"><ul>'.ary2listOflinks($a['childs']).'</ul><br /></div></dd><dt><br /></dt>';
		} else {
			if ($a['protected']) $txt .= '<li type="circle"'.(printAdmJs($a['id'])).'><a href="'.text2xml("fav_action.php?action=go&id=".$a['id']).'" '.$aAppend.'>'.text2xml($a['name']).'</a>';
			else $txt .='<li'.(printAdmJs($a['id'])).'><a href="'.text2xml($a['addr']).'" '.$aAppend.'>'.text2xml($a['name']).'</a>';
			if (isset($_SESSION['isLogined'])) $txt .= '<span class="admtools-hide" id="adm'.$a['id'].'">'.printAdmTools($a['id']).'</span>';
		}
	}
	return $txt;
}

echo '<dl>'.ary2listOflinks($ary);
echo '<dd><ul>'.ary2listOflinks($ary3).'</ul></dd></dl>
<script type="text/javascript">';
if (!($oldNetscape || $noXML)) echo '<![CDATA[';
echo 'var showsubmenu = get_submenu();
for (submenu in showsubmenu) {
	ExpandDiv(showsubmenu[submenu],showsubmenu[submenu].replace(/d/,"a"));
}';
if (!($oldNetscape || $noXML)) echo ']]>';
echo '</script>
<a name="bottom" id="abottom"></a>';
if (isset($_SESSION['isLogined'])) echo '<a href="'.text2xml("fav_action.php?action=add".$SidebarSuffix2).'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_Add.'</a>&nbsp;<a href="'.text2xml("fav_action.php?action=order&id=0".$SidebarSuffix2).'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_Order.'</a> <a href="'.text2xml("fav_action.php?action=opt".$SidebarSuffix2).'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_Optimize.'</a> <a href="'.text2xml("javascript:location.href='".$BaseURL."fav_action.php?action=add&name='+escape(document.title)+'&url='+escape(document.location.href);").'" class="admtool" onclick="return true;" '.$admAppend.'>'.$MyFav_Bookmarklet.'</a>';
$uriSuffix=isset($_SERVER['QUERY_STRING']) && $_SERVER['QUERY_STRING']!=""?"?".$_SERVER['QUERY_STRING']:'';

echo '<form action="'.text2xml($_SERVER['PHP_SELF'].$uriSuffix).'" method="post">';
if (!isset($_SESSION['isLogined'])) {
	echo $MyFav_LoginPrompt;
    echo '<input type="password" name="pwd" />
    <input type="submit" name="Submit" value="'.$MyFav_LogIn.'" />';
} else echo '<input type="submit" name="logout" value="'.$MyFav_LogOut.'" />';
echo '</form>';
if (!$InSidebar) echo '<a href="javascript:add2Sidebar();" class="admtool">[Add to Sidebar]</a> ';
echo '<a href="atom.php" class="admtool">[Atom 0.3 Feed]</a> <a href="rss.php" class="admtool">[RSS 1.0 Feed]</a> <a href="rss2.php" class="admtool">[RSS 2.0 Feed]</a><br />';
require("./fav_footer.htm");
echo '</body>
</html>';
