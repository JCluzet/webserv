<?php
// reset cookies
unset($_COOKIE['cookie1']);
unset($_COOKIE['cookie2']);
setcookie('cookie1', $_GET["fname"]);
setcookie('cookie2', $_GET["lname"]);
?>
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
echo "Cookie1: " . $_GET["fname"] . "<br>";
echo "Cookie2: " . $_GET["lname"] . "<br><br><br>";
?>
<br><br><br>
<h3>Click <a href="seecookies.html">here</a>to see your cookies.</h3>
</div>
</section>
</body>
</html>