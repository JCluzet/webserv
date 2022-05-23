<?php
$uploaddir = $_ENV["UPLOAD_PATH"];
$uploadfile = $uploaddir . "/" . basename($_FILES["uploadedfile"]["name"]);
echo "<br><br>";
if (move_uploaded_file($_FILES["uploadedfile"]["tmp_name"], $uploadfile))
{
    echo "File " . basename($_FILES["uploadedfile"]["name"]) . " uploaded !";
}
else
{    
    echo 'Voici quelques informations de débogage :';
    print_r($_FILES);
    echo '</pre>';
}
?>