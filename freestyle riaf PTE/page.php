<?php
require_once('./driver.inc');
$skin = isset($_COOKIE['interface']) && $_COOKIE['interface'] == '2ch'? $NIch_skin_pc : $bbs_skin_pc;
outputHTML($skin,$pteargs);
?>