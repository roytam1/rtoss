<?php
require("./fav_settings.php");
require("./fav_strings.php");

$db = new PDO('sqlite:./'.$sqlite_file, '', '', array(PDO::ATTR_PERSISTENT => true));

$getid=intval($_GET['id']);
if($getid>0) {
	$qry="SELECT name FROM Fav WHERE id=".$getid;
	$rs=$db->query($qry);
	$container=$rs->fetchColumn();
} elseif($_GET['id']==-1) $container=$MyFav_Order_CatOrd;
else $container=$MyFav_Order_RootOrd;

$qry='SELECT id,name,addr FROM Fav WHERE '.($getid==-1?'cat = 1':'catid='.$getid).(!$getid?' AND cat = 0':'').' ORDER BY ord,id';
$rs=$db->query($qry);
$row = $rs->fetch(PDO::FETCH_ASSOC);
$rcnt=is_array($row) ? 1 : 0;
$rs=$db->query($qry); // There is no rewind() in PDO, requery instead.

$MM_RedirectUrl=$homeURL.$SidebarSuffix1;
if (isset($_SESSION['isLogined']) && isset($_POST["MM_reorder"]) && $_POST["MM_reorder"]=="form1"){
	$Command1_CommandText='';
	$order=explode('|',$_POST['newOrder']);
//	array_pop($order);
	foreach($order as $ord => $id)
    	$Command1_CommandText.="UPDATE Fav SET ord = ".intval($ord)." WHERE id = ".intval($id).";";
    $db->exec($Command1_CommandText);

    header("Location: ".$MM_RedirectUrl);
    exit();
} 

echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>'.$MyFav_Order_Title.'</title>

<script language="JavaScript" type="text/javascript" src="tool-man-nocookie.js"></script>
<script language="JavaScript" type="text/javascript">
function ws(s){window.status = s}
</script>

<link href="./style.css" rel="stylesheet" type="text/css" />
<STYLE type="text/css"><!--
ul#ids {
	list-style-type: none;
	padding: 0 0 1em 1em;
	margin: 0px;
	width: 20em;
}
ul#ids li {
	font-size:9pt;
	cursor:move;
	padding: 2px 2px;
	border: 1px solid #ccc;
	background-color: #eee;
}
ul#ids li:before {
	content:"↕";
}
--></STYLE>';
if ($InSidebar) {
echo '<STYLE type="text/css"><!--

body,select {font-size: 9pt;}

--></STYLE>';
}
echo '</head>';

require("./fav_header.htm");
echo "<h3>$MyFav_Order_Header$container</h3>";
if (isset($_SESSION['isLogined']) && $rcnt) {
echo '<script language="JavaScript" type="text/javascript"><!--
	var dragsort = ToolMan.dragsort()
	var junkdrawer = ToolMan.junkdrawer()

	window.onload = function() {
		dragsort.makeListSortable(document.getElementById("ids"),
				verticalOnly, saveOrder)
	}

	function verticalOnly(item) {
		item.toolManDragGroup.verticalOnly()
	}

	function saveOrder(item) {}

	function placeInHidden(id,hid) { document.getElementById(hid).value = ToolMan.junkdrawer().serializeList(document.getElementById(id)); }
//-->
</script>

<form action='.$_SERVER['PHP_SELF'].'?id='.$_GET['id'].$SidebarSuffix2.' method="post" onsubmit="placeInHidden(\'ids\',\'nod\')">';
echo '  <ul id="ids">';
  while($row2 = $rs->fetch(PDO::FETCH_ASSOC)) {
echo '    <li itemID="'.($row2["id"]).'"><span onmouseover="ws(\''.$row2["addr"].'\')" onmouseout="ws(\'\')">'.$row2["name"].'</span></li>
';
  } 
echo '  </ul>
<input type="hidden" name="newOrder" id="nod" value="" />
  <input type="hidden" name="MM_reorder" value="form1">
  <input type="submit" name="Submit" value="'.$MyFav_AddMod_Submit.'">
  '.$MyFav_BackHTML.'
</form>';
} elseif (!$rcnt) {
echo $MyFav_NotFound.'<br><center>'.$MyFav_BackHTML.'</center>';
} else {
echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
}
require("./fav_footer.htm");
?>
</body>
</html>
