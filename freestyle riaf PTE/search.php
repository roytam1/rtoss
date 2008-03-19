<?php
require_once "./conf.php";
require_once "./bb2html.php";
echo '
<html><head><title>' . $tit . '</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
' . $style . '
</head>
' . $body;

echo "<a href='$PHP_SELF'>戻る</a> <a href=\"./\">【掲示板に戻る】</a><br>";

$word = $_GET['word'];
if (trim($word) != "") {
	$words = preg_split("/(　| )+/", stripslashes($word));

	$d = dir($ddir);
	while ($ent = $d->read()) {
		if (ereg("^[0-9]", $ent)) {
			$lines = file($ddir . $ent);
			list($oname, $oemail, $odate, $ocom, $sub) = explode(",", $lines[0]);
			$key = substr($ent, 0, strpos($ent, "."));
			$st = true;
			foreach($lines as $num => $line) {
				$find = false; //フラグ
				for($i = 0; $i < count($words); $i++) {
					if ($words[$i] == "") continue; //空は都バス
					if (stristr($line, $words[$i])) { // マッチです
						$find = true; //やった
						$line = str_replace($words[$i], "<b style='color:green;background-color:#ffff66'>$words[$i]</b>", $line);
					} elseif ($andor == "and") { // ANDの場合マッチしないなら次のログへ
						$find = false;
						break;
					} 
				} 
				if ($find) {
					if ($st) {
						echo "<hr><a href='read.php?key=$key'><font size=4>$sub</font></a>  $odate  by $oname</b><br>";
						$st = false;
					} 

					$num = $num + 1;
					list($name, $email, $date, $com) = explode(",", $line);
					if ($email != "") {
						$name = "<a href=\"mailto:$email\">$name</a>";
					} 
					$com = bb2html($com);
					echo "<br><a href='read.php?key=$key&st=$num&to=$num'>$num</a>
 <font color=\"forestgreen\"><b>$name</b></font> [ $date ]<br>$com<br>";
				} 
			} 
			// if(!$find2) echo "<br><br></b>「".htmlspecialchars($word)."」見つかんねぇよ(;´Д`) ﾀﾞｳｿ<br><br>";
		} 
	} 
	$d->close();
	exit;
} 

echo "<form action=\"$PHP_SELF\" method=GET></h3><ul>
<br><br>キーワード: <input type=text name=word size=10 value=" . htmlspecialchars($word) . ">
<input type=submit value=\"  検索する  \"><br><font color=red>$err</font><br>";

$d = dir($ddir);
while ($ent = $d->read()) {
	if (ereg("^[0-9]", $ent)&&ereg($ext."$", $ent))
		$tmp[] = substr($ent, 0, strpos($ent, "."));
} 
$d->close();
rsort($tmp);

$suball = file("subback.txt");
foreach($suball as $sub) {
	list($key, $subject) = explode(",", $sub);
	list($key,) = explode(".", $key);
	$up[$key] = $subject;
} 
foreach($tmp as $line) {
	echo " <a href=read.php?key=$line&ls=50>$line</a>";
	echo "　　$up[$line]<br>\n";
}
echo "</form></body></html>";
