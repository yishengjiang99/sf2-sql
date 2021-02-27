<?php
require_once("functions.php");
$info = dbrows("select p.name, sg.name, g.id from phdr p join pbag b on p.pbag_id=b.id join pgen g on b.pgen_id = g.id join sf_generators sg on g.id = sg.id where p.id=0"); 
var_dump($info);