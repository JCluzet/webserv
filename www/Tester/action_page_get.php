<?php
setcookie("cookie1", "oh voilà un ptit ");
setcookie("cookie2", "COOKIE !");
?>
<body>
    <section class="notFound">
    <link rel="stylesheet" href="/style/style.css">
		<!-- <link href="style.css" rel="stylesheet"> -->
        <div class="text">
            <h1><a href="/">HomePage</a></h1>
            <h3>WEBSERV 1.0</h3>
        <div class="img">
            <!-- <img src="https://assets.codepen.io/5647096/Delorean.png" alt="El Delorean, El Doc y Marti McFly"/> -->
            <img src="http://i.stack.imgur.com/SBv4T.gif" alt="this slowpoke moves"  width="250" />
            <!-- <img src="https://assets.codepen.io/5647096/Delorean.png" alt="El Delorean, El Doc y Marti McFly"/> -->
            <br><br>
            <br><br>
            <br><br>
            
            <?php
echo "Cookie1: " . $_COOKIE["cookie1"] . "<br>";
echo "Cookie2: " . $_COOKIE["cookie2"] . "<br><br><br>";
echo "Hello " . $_GET["lname"] . " " . $_GET["fname"] . " !";
?>
</div>
        
</section>
</body>


</html>