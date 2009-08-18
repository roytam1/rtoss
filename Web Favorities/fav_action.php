<?php
require("./fav_settings.php");
require("./fav_strings.php");

$conn=sqlite_popen($sqlite_file);

$iPass=val($_POST,"pwd");
$iAction=val($_GET,'action');

echo '<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<title>'.$MyFav_Action_Title.'</title>
<link href="./style.css" rel="stylesheet" type="text/css">';
if ($InSidebar) {	echo '<STYLE type="text/css"><!--
body {font-size: 9pt;}
--></STYLE>';
}
echo '</head>';

require("./fav_header.htm");

if (!isset($_SESSION['isLogined']) && !viewAuth()) {
	if (($iAction=="go") && ($iPass) && isset($_POST["h_id"]) && (($iPass==$FavPasswd) || ($iPass==$ViewPassword))) {
		viewAuth('login',$iPass);
    	$qry="SELECT * FROM Fav WHERE id = ".$_POST["h_id"];
		$rs=sqlite_query($conn,$qry);
		$row = sqlite_fetch_array($rs);
		header("Location: ".$row["addr"]);
	} else {
		if (($iAction=="go") && (!$iPass)) {
			echo $MyFav_PasswdPrompt.'<form action="'.$_SERVER['PHP_SELF'].'?action=go" method="post">
	<input type="hidden" name="h_id" value="'.$_GET["id"].'">
    <input type="password" name="pwd">
    <input type="submit" name="Submit" value="登入">
    '.$MyFav_BackHTML.'
</form>';
		} else {
			if(!isset($_POST['pwd'])) {
				echo $MyFav_PasswdPrompt.'<form action="'.$_SERVER['PHP_SELF'].'?'.$_SERVER['QUERY_STRING'].'" method="post">
	    <input type="password" name="pwd">
	    <input type="submit" name="Submit" value="登入">
	    '.$MyFav_BackHTML.'
	</form>';
			} else
				if(!logInOut(val($_POST,'pwd')))
					echo $MyFav_AccessDeny.'<br><center>'.$MyFav_BackHTML.'</center>';
		}
	}
}
if (isset($_SESSION['isLogined']) || viewAuth()) {
   switch ($iAction) {
    case "add":
      $_GET['url']=urlencode($_GET['url']); // encodes url again
      $_GET['name']=urlencode(jsUCEsc2utf8($_GET['name'])); // encodes name again
      header("Location: ".$BaseURL."fav_add.php?".toQueryString("catid",'name','url').$SidebarSuffix2);
      break;
    case "edit":
      header("Location: ".$BaseURL."fav_edit.php?".toQueryString('id').$SidebarSuffix2);
      break;
    case "delete":
      header("Location: ".$BaseURL."fav_del.php?".toQueryString('id').$SidebarSuffix2);
      break;
    case "order":
      header("Location: ".$BaseURL."fav_reorder.php?".toQueryString('id').$SidebarSuffix2);
      break;
    case "go":
      $qry="SELECT * FROM Fav WHERE id = ".$_GET["id"];
	  $rs=sqlite_query($conn,$qry);
	  $row = sqlite_fetch_array($rs);
      header("Location: ".$row["addr"]);
      break;
    case "opt":
      $qry="VACUUM Fav";
	  $rs=sqlite_exec($conn,$qry);
      echo $MyFav_Action_Optimized.'<br><center>'.$MyFav_BackHTML.'</center>';
      break;
    default:
      echo $MyFav_Action_UnknowOpCode.'<br><center>'.$MyFav_BackHTML.'</center>';
      break;
  }
}
require("./fav_footer.htm"); ?>
</body>
</html>

