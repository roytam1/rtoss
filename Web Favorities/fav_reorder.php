<?php
require("./fav_settings.php");
require("./fav_strings.php");

$conn=sqlite_popen($sqlite_file);
if($_GET['id']>0) {
	$qry="SELECT name FROM Fav WHERE id=".$_GET['id'];
	$container=sqlite_single_query($conn,$qry);
} elseif($_GET['id']==-1) $container=$MyFav_Order_CatOrd;
else $container=$MyFav_Order_RootOrd;

$qry="SELECT id,name FROM Fav WHERE ".($_GET['id']==-1?'cat = 1':'catid='.$_GET['id']).(!$_GET['id']?' AND cat = 0':'')." ORDER BY ord,id";
$rs=sqlite_query($conn,$qry);
$rcnt=sqlite_num_rows($rs);

$MM_RedirectUrl=$homeURL.$SidebarSuffix1;
if (isset($_SESSION['isLogined']) && isset($_POST["MM_reorder"]) && $_POST["MM_reorder"]=="form1"){
	$Command1_CommandText='';
	$order=explode("|",$_POST['newOrder']);
	array_pop($order);
	foreach($order as $ord => $id)
    	$Command1_CommandText.="UPDATE Fav SET ord = ".sqlite_escape_string($ord)." WHERE id = ".sqlite_escape_string($id).";";
    sqlite_exec($Command1_CommandText,$conn);

    header("Location: ".$MM_RedirectUrl);
    exit();
} 

echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>'.$MyFav_Order_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css">';
if ($InSidebar) {
echo '<STYLE type="text/css"><!--

body,select {font-size: 9pt;}

--></STYLE>';
}
echo '</head>';

require("./fav_header.htm");
echo "<h3>$MyFav_Order_Header$container</h3>";
if (isset($_SESSION['isLogined']) && $rcnt) {
echo '<script type="text/javascript">
function move(index,to) {
	var list = document.getElementById("ids");
	var total = list.options.length-1;
	if (index == -1) return false;
	if (to == +1 && index == total) return false;
	if (to == -1 && index == 0) return false;
	var items = new Array;
	var values = new Array;
	for (i = total; i >= 0; i--) {
		items[i] = list.options[i].text;
		values[i] = list.options[i].value;
	}
	for (i = total; i >= 0; i--) {
		if (index == i) {
			list.options[i + to] = new Option(items[i],values[i], 0, 1);
			list.options[i] = new Option(items[i + to], values[i +to]);
			i--;
		} else {
			list.options[i] = new Option(items[i], values[i]);
	   }
	}
	list.focus();
	return true;
}
function placeInHidden(delim, selStr, hidStr)
{
  var selObj = document.getElementById(selStr);
  var hideObj = document.getElementById(hidStr);
  hideObj.value = "";
  for (i = 0; i <= selObj.options.length-1; i++) { 
	hideObj.value += selObj.options[i].value + delim;
  }

}
</script>
<form action='.$_SERVER['PHP_SELF'].'?id='.$_GET['id'].$SidebarSuffix2.' method="post" onsubmit="placeInHidden(\'|\', \'ids\', \'nod\')">
<table>
<tr>
<td align="middle">';
echo '  <select id="ids" size="'.$rcnt.'">';
  while($row2 = sqlite_fetch_array($rs)) {
echo '    <option value="'.($row2["id"]).'" >'.$row2["name"].'</option>';
  } 
echo '  </select>
</td>
<td>
<input type="button" value="↑" 
onClick="move(document.getElementById(\'ids\').selectedIndex,-1)"><br><br>
<input type="button" value="↓"
onClick="move(document.getElementById(\'ids\').selectedIndex,+1)">
</td>
</tr>
<tr colspan="2"><td><input type="hidden" name="newOrder" id="nod" value="" />
  <input type="hidden" name="MM_reorder" value="form1">
  <input type="submit" name="Submit" value="'.$MyFav_AddMod_Submit.'">
  '.$MyFav_BackHTML.'
</td>
</tr>
</table></form>';
} elseif (!$rcnt) {
echo $MyFav_NotFound.'<br><center>'.$MyFav_BackHTML.'</center>';
} else {
echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
}
require("./fav_footer.htm");
?>
</body>
</html>
