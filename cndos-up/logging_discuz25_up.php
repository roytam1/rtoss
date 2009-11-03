<?php

/*
 ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 :: [DISCUZ!]  Crossday Discuz! Board                                    ::
 :: (c) 2001-2005 Comsenz Technology Ltd (www.discuz.com)                ::
 ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 :: Author:  Crossday (tech@discuz.com) Cnteacher (cnteacher@discuz.com) ::
 :: Version: 2.5F   2004/10/01 05:15                                     ::
 ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
*/

//fix:  BY pk0909
/*
1 在线列表重复的问题
*/

define('CURRSCRIPT', 'logging');
require './include/common.php';

if(isset($_POST['password'])) $password = $_POST['password'];
if(isset($_POST['username'])) $username = $_POST['username'];

		$discuz_uid = $adminid = 0;
		$discuz_user = $discuz_pw = $discuz_secques = '';

		$loginperm = logincheck();
		if(!$loginperm) {
			echo 'login_strike';
			exit;
		}

		$secques = ($questionid && $answer) ? quescrypt($questionid, $answer): '';
		$errorlog = "$timestamp\t$username\t".substr($password, 0, 2);
		for($i = 3; $i < strlen($password); $i++) {
			$errorlog .= '*';
		}
		$errorlog .= substr($password, -1)."\t".($secques ? "Ques #$questionid" : '')."\t$onlineip\n";

		$password = md5($password);

		$query = $db->query("SELECT m.uid AS discuz_uid, m.username AS discuz_user, m.password AS discuz_pw, m.adminid, m.groupid, m.credit, m.styleid AS styleidmem, m.lastvisit, m.lastpost, u.type as usertype, u.creditshigher, u.creditslower, u.allowinvisible
					FROM $table_members m LEFT JOIN $table_usergroups u USING (groupid)
					WHERE username='$username' AND password='$password' AND secques='$secques'");
		@extract($db->fetch_array($query));

		loginrecord($discuz_uid, $loginperm);

		if($discuz_uid) {
			echo 'OK '.$discuz_user;
			exit;
		} else {
			@$fp = fopen(DISCUZ_ROOT.'./forumdata/illegallog.php', 'a');
			@flock($fp, 2);
			@fwrite($fp, $errorlog);
			@fclose($fp);
			echo 'login_invalid';
			exit;
		}


function logincheck() {
	/* returned value
		1=nonexistence;
		2=within limitation;
		3=record expired
	*/

	global $db, $table_failedlogins, $onlineip, $timestamp;
	$query = $db->query("SELECT count, lastupdate FROM $table_failedlogins WHERE ip='$onlineip'");
	if($login = $db->fetch_array($query)) {
		if($timestamp - $login['lastupdate'] > 900) {
			return 3;
		} elseif($login['count'] < 5) {
			return 2;
		} else {
			return 0;
		}
	} else {
		return 1;
	}
}

function loginrecord($valid, $permission) {
	global $db, $table_failedlogins, $onlineip, $timestamp;
	if(!$valid) {
		switch($permission) {
			case 1:	$db->query("INSERT INTO $table_failedlogins (ip, count, lastupdate)
					VALUES ('$onlineip', '1', '$timestamp')");
				break;
			case 2: $db->query("UPDATE $table_failedlogins SET count=count+1, lastupdate='$timestamp' WHERE ip='$onlineip'");
				break;
			case 3: $db->query("UPDATE $table_failedlogins SET count='1', lastupdate='$timestamp' WHERE ip='$onlineip'");
				$db->query("DELETE FROM $table_failedlogins WHERE lastupdate<$timestamp-901", 'UNBUFFERED');
				break;
		}
	}
}

?>