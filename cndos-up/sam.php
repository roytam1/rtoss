<?php
/***
* Reconstruction by Wastepaper Basket
* サムネイルカッター　（画像一覧）by ToR

* ※PHPオプションにGDが必要です（無料鯖ではダメなところが多い
* $_GET等使用してます。古いバージョンのPHPでは$_GET→$HTTP_GET_VARS $_SERVER→$HTTP_SERVER_VERS
**/

$title	= '中國DOS聯盟論壇專用上傳區';	//タイトル
$upphp		 = 'upload.php';		//アップローダ
$img_dir	 = "./img/";			//画像一覧ディレクトリ
$thumb_dir = "./imgs/";			//サムネイル保存ディレクトリ
$ext			 = ".+\.png$|.+\.jpe?g$";	//拡張子，GIFはGDのﾊﾞｰｼﾞｮﾝによっては無理
$W				 = 120;			//出力画像幅
$H				 = 120;			//出力画像高さ
$cols			= 2;				//1行に表示する画像数
$page_def	= 4;			//1ページに表示する画像数

if ($_GET["cmd"]=="min" && isset($_GET["pic"])) {
	$src = $img_dir.$_GET["pic"];

	// 画像の幅と高さとタイプを取得
	$thumbout = $thumb_dir.str_replace(strrchr($_GET["pic"],'.'),'.jpg',$_GET["pic"]);
	if(!file_exists($thumbout)) {
		$size = GetImageSize($src);
		switch ($size[2]) {
			case 1 : $im_in = ImageCreateFromGIF($src);	break;
			case 2 : $im_in = ImageCreateFromJPEG($src); break;
			case 3 : $im_in = ImageCreateFromPNG($src);	break;
		}
		// 読み込みエラー時
		if (!$im_in) {
			$im_in = ImageCreate($W,$H);
			$bgc = ImageColorAllocate($im_in, 0xff, 0xff, 0xff);
			$tc	= ImageColorAllocate($im_in, 0,0x80,0xff);
			ImageFilledRectangle($im_in, 0, 0, $W, $H, $bgc);
			ImageString($im_in,1,5,30,"Error loading {$_GET["pic"]}",$tc);
			header('Content-Type: image/png');
			ImagePNG($im_in);
			exit;
		 }
		// リサイズ
		if ($size[0] > $W || $size[1] > $H) {
			$key_w = $W / $size[0];
			$key_h = $H / $size[1];
			($key_w < $key_h) ? $keys = $key_w : $keys = $key_h;

			$out_w = $size[0] * $keys;
			$out_h = $size[1] * $keys;
		} else {
			$out_w = $size[0];
			$out_h = $size[1];
		}
		// 出力画像（サムネイル）のイメージを作成
		$im_out = ImageCreateTrueColor($out_w, $out_h);
		// 元画像を縦横とも コピーします。
		ImageCopyResampled($im_out, $im_in, 0, 0, 0, 0, $out_w, $out_h, $size[0], $size[1]);

		// ここでエラーが出る方は下２行と置き換えてください。(GD2.0以下
		//$im_out = ImageCreate($out_w, $out_h);
		//ImageCopyResized($im_out, $im_in, 0, 0, 0, 0, $out_w, $out_h, $size[0], $size[1]);

		// サムネイル画像をブラウザに出力、保存
		header('Content-Type: image/jpeg');
		ImageJPEG($im_out);
		ImageJPEG($im_out, $thumbout);
		// 作成したイメージを破棄
		ImageDestroy($im_out);
		ImageDestroy($im_in);
		exit;
	} else {
		header("Location: $thumbout");
		exit;
	}
}
// ディレクトリ一覧取得、ソート
$d = dir($img_dir);
while ($ent = $d->read()) {
	if (eregi($ext, $ent)) {
		$files[] = $ent;
	}
}
$d->close();
// ソート
@natsort($files);
$files2 = @array_reverse($files);
//ヘッダHTML
echo '<?xml version="1.0" encoding="utf-8"?>'."\n";
echo <<<HEAD
<!DOCTYPE html
	 PUBLIC "-//W3C//DTD XHTML 1.1//EN"
	 "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="zh">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>$title</title>
<link href="style.css" type="text/css" rel="stylesheet" media="all" />
</head>
<body>

<h1>$title</h1>
<p>請勿在論壇外轉載此區任何資料。</p>
<center><script type="text/javascript"><!--//--><![CDATA[//><!--
google_ad_client = "pub-8016619041936615";
//728x90, 已建立 2007/12/26
google_ad_slot = "8811687463";
google_ad_width = 728;
google_ad_height = 90;
//--><!]]></script>
<script type="text/javascript"
src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script></center>

<h2>圖像一覧</h2>
<table summary="縮圖一覧">
<tr>

HEAD;

//print_r($files);
$maxs = count($files)-1;
$start = $_GET['start'];
$ends = $start+$page_def-1;
$counter = 0;
while (list($line, $filename) = @each($files2)) {
	if (($line >= $start) && ($line <= $ends)) {
		$image = rawurlencode($filename);
		// サムネイルがある時はｻﾑﾈｲﾙへのﾘﾝｸ、それ以外はｻﾑﾈｲﾙ表示、作成
		if (file_exists($thumb_dir.$image)) $piclink = $thumb_dir.$image;
		else $piclink = $_SERVER["PHP_SELF"]."?cmd=min&pic=".$image;
		$counter++;
		if (((($counter) % $cols) == 1)&&(($counter) != 1)) {
	echo "</tr>\n<tr>\n";
		}//メインHTML
		echo <<<EOD
<td class="img"><a href="$img_dir$image"><img src="$piclink" alt="$filename" title="$filename" /><br />$filename</a></td>

EOD;
	}
}
echo "</tr>\n</table>\n";

//ﾍﾟｰｼﾞリンク
echo '<p class="tline"><a href="'.$upphp.'?">戻る</a> Page:';
if ($_GET["start"] > 0) {
	$prevstart = $_GET["start"] - $page_def;
	echo " <a href=\"$_SERVER[PHP_SELF]?start=$prevstart\">&lt;&lt;前へ</a>";
}
if ($ends < $maxs) {
	$nextstart = $ends+1;
	echo " <a href=\"$_SERVER[PHP_SELF]?start=$nextstart\">次へ&gt;&gt;</a>";
}
echo '</p>';

echo '
<center><script type="text/javascript"><!--//--><![CDATA[//><!--
google_ad_client = "pub-8016619041936615";
//468x60, 已建立 2007/12/17
google_ad_slot = "0207867936";
google_ad_width = 468;
google_ad_height = 60;
google_language = "zh-TW";
//--><!]]></script>
<script type="text/javascript"
src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script></center>

<h2>著作権表示</h2>
<p id="link"><a href="http://php.s3.to/">レッツPHP!</a> + <a href="http://utu.under.jp">Wastepaper Basket</a></p>

</body>
</html>
';
?>