<body>
    <section class="notFound">
    <link rel="stylesheet" href="/style/style.css">
        <div class="text">
            <h1><a href="/">HomePage</a></h1>
            <h3>WEBSERV 1.0</h3>
        <div class="img">
            <img src="http://i.stack.imgur.com/SBv4T.gif" alt="this slowpoke moves"  width="250" />
            <br><br>
            <br><br>
            <br><br>
            
            <h3>Here is the cookies you've just enter :</h3>
<?php
$cookie1 = htmlspecialchars($_COOKIE['cookie1']);
$cookie2 = htmlspecialchars($_COOKIE['cookie2']);
echo "Cookie1: " . $cookie1 . "<br>";
echo "Cookie2: " . $cookie2 . "<br><br><br>";
?>
<br><br><br>
</div>
</section>
</body>
</html>