<html>
	<body>
<pre>
<?php
$one=fopen("php://output","wb");
$file=popen("/var/www/html/sf2-sql/index2.c.out","r");
while(fputs($one,fgets($file)));
pclose($file);
fclose($one);
?>
</pre>
</body>
</html>
