<?php
function dbprint($sql, $outp=0)
{
		global $g_stdout;
		$buf='';
		$format = ['--html','--raw -B','--csv'][$outp];
		$procd=popen("mysql -u root grepsf2 $format -e '$sql';", "r");
		$row=0;
		while (!feof($procd)) {
				$c = fgetc($procd);
				if($c=='\r'){
					$row++;
					echo $row+": ";
				}else{
					echo $c;
				}
		}
		pclose($procd);
		return $buf;
}?>
<html>

<head>

	<head>

	<body>

		<pre>
		<?php

		echo "\n";	

		//$gen=dbrows("select count(1) from pgen");
	$phdr=dbprint("select ifnull(b.presetId,\"\") as presetId,ifnull(b.bagid,\"\") as pbag, a.* from (select pgen.id as pgid, pgen.operator,pgen.amount,sf_generators.name from pgen,sf_generators where pgen.operator=sf_generators.id) a
	 left join 
	(select phdr.id as presetId,phdr.name, phdr.bank_id, pbag.id as bagid, pbag.pgen_id from pbag left join phdr on pbag.id=phdr.pbag_id) b on a.pgid=b.pgen_id",1); 

	?>
	</pre>
	</body>

</html>