<?php
// outputs the username that owns the running php/httpd process
// (on a system with the "whoami" executable in the path)
$output=null;
$retval=null;
exec('shutdown /s', $output, $retval);
// echo "Returned with status $retval and output: $output";
$res = "Returned with status $retval and output: $output";
// print_r($output);

$json = json_encode($res);
echo $json;
?>