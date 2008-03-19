<?php
require("./fav_settings.php");

$noXML=false;
header("Content-type: text/xml"); 

$conn=sqlite_popen($sqlite_file);

if (isset($_GET["catid"])) {
  $catqstr="AND catid = ".intval($_GET["catid"]);
  $qry="SELECT * FROM Fav WHERE id = ".$_GET["catid"];
  $rs=sqlite_query($conn,$qry);
  $row = sqlite_fetch_array($rs);
  $homeTitle=$homeTitle." - ".$row["name"];
} else {
  $catqstr="";
}

$qry="SELECT * FROM Fav WHERE cat = 0 AND protected = 0 ".$catqstr." GROUP BY catid,id ORDER BY catid,ord";
$rs=sqlite_query($conn,$qry);

$atomDateTime=date('Y-m-d\TH:i:s\Z');

echo '<?xml version="1.0" encoding="utf-8"?>
<?xml-stylesheet href="atom.xsl" type="text/xsl" media="screen"?>

<feed version="0.3" xmlns="http://purl.org/atom/ns#">
  <title>'.$homeTitle.'</title>
  <link rel="alternate" type="text/html" href="'.$homeURL.'"/>

  <modified>'.$atomDateTime.'</modified>
  <author>
    <name>'.$authorName.'</name>
  </author>
  <tagline>'.$homeDesc.'</tagline>';
while($row = sqlite_fetch_array($rs)) {
  echo '<entry>
    <title>'.text2xml($row["name"]).'</title>
    <link rel="alternate" type="text/html" href="'.text2xml($row["addr"]).'"/>
    <id>tag:'.$domainName.','.date("Y").':'.$row["id"].'</id>
    <content>'.text2xml($itemDesc).'</content>
    <issued>'.$atomDateTime.'</issued>
    <modified>'.$atomDateTime.'</modified>
  </entry>';
}
?>
</feed>
