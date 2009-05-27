<?php 
if(isset($_GET['sitemap'])) {
 header('Content-Type: text/xml');
 echo '<'.'?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.google.com/schemas/sitemap/0.84" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.google.com/schemas/sitemap/0.84&#13;&#10;http://www.google.com/schemas/sitemap/0.84/sitemap.xsd">
';

 $f = glob('*');
 foreach ($f as $fi) {
  if(strpos($fi,'.php')===false && strpos($fi,'.html')===false)
   echo '<url>
<loc>http://'.$_SERVER['HTTP_HOST'].dirname($_SERVER['PHP_SELF']).'/'.$fi.'</loc>
</url>
';
 }
 echo '</urlset>';
}
else
 header('Location: home.html');
?>