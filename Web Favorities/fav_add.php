<?php
require("./fav_settings.php");
require("./fav_strings.php");

  $Command1__varcat=val($_POST,"newIsCat","0");
  $Command1__varprot=val($_POST,"newIsProtected","0");
  $Command1__varname=val($_POST,"newName");
  $Command1__varaddr=val($_POST,"newUrl");
  $Command1__varcatid=val($_POST,"catid",'0');
$MM_RedirectUrl=$homeURL.$SidebarSuffix1;

$db = new PDO('sqlite:./'.$sqlite_file, '', '', array(PDO::ATTR_PERSISTENT => true));
//$row = sqlite_fetch_array($rs);

if (isset($_SESSION['isLogined']) && isset($_POST["MM_insert"]) && $_POST["MM_insert"]=="form1"){
	$qry="SELECT COUNT(id) FROM Fav";
	$rs=$db->query($qry);
	$rcnt=$db->fetchColumn($rs);
	$sth = $db->prepare('INSERT INTO Fav (cat,name,addr,catid,protected,ord) VALUES (?, ?, ?, ?, ?, ?)');
	$sth->execute(array($Command1__varcat, $Command1__varname, $Command1__varaddr, $Command1__varcatid, $Command1__varprot, $rcnt));
//  $Command1_CommandText="INSERT INTO Fav (cat,name,addr,catid,protected,ord) VALUES (".sqlite_escape_string($Command1__varcat).",'".sqlite_escape_string($Command1__varname)."','".sqlite_escape_string($Command1__varaddr)."',".sqlite_escape_string($Command1__varcatid).",".sqlite_escape_string($Command1__varprot).",".sqlite_escape_string($rcnt).")";
//  sqlite_exec($Command1_CommandText,$conn);

  header("Location: ".$MM_RedirectUrl);
  exit();
} 

echo '<html>
<head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">
<title>'.$MyFav_Add_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css">';
if ($InSidebar){
echo '<STYLE type="text/css"><!--
body {font-size: 9pt;}
--></STYLE>';
}
echo '<script>
var orig_catid;
function detChk() {
	if (document.getElementById("newIsCat").checked) {
//		document.getElementById("newUrl").disabled=true;
//		document.getElementById("newUrl").value="";
		document.getElementById("newIsProtected").disabled=true;
		document.getElementById("catid").disabled=true;
		orig_catid=document.getElementById("catid").selectedIndex;
		document.getElementById("catid").selectedIndex="0";
	} else {
		document.getElementById("newIsProtected").disabled=false;
//		document.getElementById("newUrl").disabled=false;
		document.getElementById("catid").disabled=false;
		document.getElementById("catid").selectedIndex=orig_catid;
	}
}
function detSel() {
	if (document.getElementById("catid").selectedIndex != 0) {
		document.getElementById("newIsCat").disabled=true;
		document.getElementById("newIsCat").checked=false;
	} else {
		document.getElementById("newIsCat").disabled=false;
	}
}
</script>
</head>';
require("./fav_header.htm");
if (isset($_SESSION['isLogined'])) {
echo '<h2>'.$MyFav_Add_Header.'</h2>
<form name="form1" action="'.$_SERVER['PHP_SELF'].$SidebarSuffix1.'" method="POST">
  '.$MyFav_AddMod_Name.'
  <input name="newName" type="text" size="'.$TextboxWidth.'" value="'.val($_GET,'name').'">
  <br>
  '.$MyFav_AddMod_URL.'
  <input type="text" name="newUrl" id="newUrl" size="'.$TextboxWidth.'" value="'.val($_GET,'url').'">
  <br>
  '.$MyFav_AddMod_IsCategory.'
  <input name="newIsCat" id="newIsCat" onChange="detChk();" type="checkbox" value="1" '.val2($_GET,'catid','disabled="disabled"').'>
  <br>
  '.$MyFav_AddMod_IsPasswordProtected.'
  <input name="newIsProtected" id="newIsProtected" type="checkbox" value="1">
  <br>
  '.$MyFav_AddMod_ParentCategory.'
  <select name="catid" id="catid" onChange="detSel();">';
  
$qry="SELECT id,name FROM Fav WHERE cat = 1 ORDER BY ord,id";
$rs=$db->query($qry);
echo '    <option value="0" '.(isset($_GET["catid"])&&$_GET["catid"]==''?"selected":'').'>'.$MyFav_Cat_NoCategory.'</option>';
  while($row2 = $rs->fetch(PDO::FETCH_ASSOC)) {
echo '    <option value="'.($row2["id"]).'" '.(isset($_GET["catid"])&&$row2["id"]==$_GET["catid"]?"selected":'').'>'.($row2["name"]).'</option>';
  } 
echo '  </select>
  <input type="submit" name="Submit" value="'.$MyFav_AddMod_Submit.'">
  <input type="hidden" name="MM_insert" value="form1">
  '.$MyFav_BackHTML.'
</form>';
} else {
echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
}
require("./fav_footer.htm");
?>
</body>
</html>
