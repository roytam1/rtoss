<?php
require("./fav_settings.php");
require("./fav_strings.php");

  $Command1__varid=val($_POST,"id");

$Recordset1__MMColParam="1";
if (isset($_GET["id"])) $Recordset1__MMColParam=intval($_GET["id"]);


$MM_RedirectUrl=$homeURL.$SidebarSuffix1;

$conn=sqlite_popen($sqlite_file);
$qry="SELECT * FROM Fav WHERE id = ".sqlite_escape_string($Recordset1__MMColParam);
$rs=sqlite_query($conn,$qry);
$rcnt=sqlite_num_rows($rs);
$row = sqlite_fetch_array($rs);

if (isset($_SESSION['isLogined']) && isset($_POST["MM_delete"]) && $_POST["MM_delete"]=="form1"){
  $Command1_CommandText="DELETE FROM Fav WHERE id = ".sqlite_escape_string($Command1__varid);
  sqlite_exec($Command1_CommandText,$conn);
  header("Location: ".$MM_RedirectUrl);
  exit();
} elseif (isset($_POST["MM_delete"]) && $_POST["MM_delete"]!=""){
  header("Location: ".$MM_RedirectUrl);
  exit();
} 

echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>'.$MyFav_Del_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css">';
if ($InSidebar) {
echo '<STYLE type="text/css"><!--
body {font-size: 9pt;}
--></STYLE>';
}
echo '</head>';

require("./fav_header.htm");
if (isset($_SESSION['isLogined']) && $rcnt==1) {
echo '<form name="form1" action="'.$_SERVER['PHP_SELF'].$SidebarSuffix1.'" method="POST"><center>
<font size=4>'.$MyFav_Del_Prompt.'</font><br><table><tr>
  <td>'.$MyFav_Del_Code.'</td><td>'.($row["id"]).'</td></tr>
  <td>'.$MyFav_AddMod_Name.'</td><td>'.($row["name"]).'</td></tr>
  <td>'.$MyFav_AddMod_URL.'</td><td>'.($row["addr"]).'</td></tr>
  <td>'.$MyFav_AddMod_IsCategory.'</td><td>'.($row["cat"]?$MyFav_Del_Yes:$MyFav_Del_No).'</td></tr>
  <td>'.$MyFav_Del_ParentCategoryCode.'</td><td>'.($row["catid"]).'</td></tr></table><br>
  <input type="submit" id="Submit" name="Submit" value="'.$MyFav_Del_Yes.'">&nbsp;&nbsp;&nbsp;&nbsp;
  <a href="'.$BackUrl.'" '.$admAppend.'>['.$MyFav_Del_No.']</a></center>
  <input type="hidden" name="MM_delete" value="form1">
  <input type="hidden" name="id" value="'.($row["id"]).'">
</form>';
} elseif ($rcnt!=1) {
echo $MyFav_NotFound.'<br><center>'.$MyFav_BackHTML.'</center>';
} else {
echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
}
require("./fav_footer.htm");
?>
</body>
</html>
