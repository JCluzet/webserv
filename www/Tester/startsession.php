<?php
if (session_status() == PHP_SESSION_NONE) {
    session_start();
    $_SESSION["newsession"] = "newsession";
    $_SESSION["name"] = "test";
}
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
<?php

echo 'Session is now start!';

// $_SESSION['favcolor'] = 'green';
// $_SESSION['animal']   = 'cat';
// $_SESSION['time']     = time();

// // Fonctionne si le cookie a été accepté
// echo '<br /><a href="page2.php">page 2</a>';

// // Ou bien, en indiquant explicitement l'identfiant de session
// echo '<br /><a href="page2.php?' . SID . '">page 2</a>';
?>
<br><br><br>
</div>
</section>
</body>
</html>