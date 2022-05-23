<body>
    <section class="notFound">
    <link rel="stylesheet" href="/style/style.css">
		<!-- <link href="style.css" rel="stylesheet"> -->
        <div class="text">
            <h1><a href="/">HomePage</a></h1>
            <h3>WEBSERV 1.0</h3>
        <div class="img">
            <img src="https://assets.codepen.io/5647096/Delorean.png" alt="El Delorean, El Doc y Marti McFly"/>
            <!-- <img src="https://assets.codepen.io/5647096/Delorean.png" alt="El Delorean, El Doc y Marti McFly"/> -->
            <br><br>
            <br><br>
            <br><br>
            

            <?php
$uploaddir = $_ENV["UPLOAD_PATH"];
$uploadfile = $uploaddir . "/" . basename($_FILES["uploadedfile"]["name"]);
$name = $_FILES["uploadedfile"]["name"];
echo "<br><br>";
if ($_FILES['uploadedfile']['tmp_name']) 
{
    if (move_uploaded_file($_FILES["uploadedfile"]["tmp_name"], $uploadfile)) 
    {
        echo "File " . basename($_FILES["uploadedfile"]["name"]) . " uploaded !";
        echo "<br><br>";
        echo "You can click   <a href='/uploads/$name'>here</a> to see the it.";
    }
    else 
{
        echo 'Voici quelques informations de débogage :';
        print_r($_FILES);
        echo '</pre>';
    }
    
}
else
{
    echo 'Aucun fichier à upload !';
}
?>
</div>

</section>
</body>


</html>