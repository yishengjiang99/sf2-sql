
<html>
	<head><link rel="stylesheet" href="https://cdn.rawgit.com/Chalarangelo/mini.css/v3.0.1/dist/mini-dark.min.css">
</head>
	<body class='container'>

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
