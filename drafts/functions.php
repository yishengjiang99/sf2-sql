<?php
function dbrows($sql,$args=[]){
	global $sfname;
	$sql=sprintf($sql,$args);
	exec(sprintf("mysql -u root groupmidi -e '%s'", $sql),$ret); 
	$fields=preg_split('/[\s]+/',array_shift($ret));
	$rows=[];	
	foreach($ret as $kk=>$r){
		$row_arr=preg_split('/[\s]+/', $r);
		$row = new stdClass;
		foreach($fields as $k=>$f){
			$row->$f = $row_arr[$k];
		}
		$rows[]=$row;
	}
	return $rows;
}