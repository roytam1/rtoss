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

echo '<?xml version="1.0" encoding="utf-8"?>
<?xml-stylesheet href="rdf.xsl" type="text/xsl" media="screen"?>

<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns="http://purl.org/rss/1.0/">
	<channel rdf:about="'.$RSS_SELF.'">
		<title>'.$homeTitle.'</title>
		<link>'.$homeURL.'</link>
		<description>'.$homeDesc.'</description>
		<items>
			<rdf:Seq>';
while($row = sqlite_fetch_array($rs))
echo '				<rdf:li rdf:resource="'.text2xml($row["addr"]).'" />';
sqlite_rewind($rs);
echo '			</rdf:Seq>
		</items>
	</channel>';

while($row = sqlite_fetch_array($rs)) {
echo '	<item rdf:about="'.text2xml($row["addr"]).'">
		<title>'.text2xml($row["name"]).'</title>
		<link>'.text2xml($row["addr"]).'</link>
		<description>'.text2xml($itemDesc).'</description>
  	</item>';
}
?>
</rdf:RDF>
