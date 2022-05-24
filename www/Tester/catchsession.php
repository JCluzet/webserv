<?php
session_start();
session_destroy();

echo 'Bienvenue sur la page numéro 2<br />';

echo $_SESSION['favcolor']; // green
echo $_SESSION['animal'];   // cat
echo date('Y m d H:i:s', $_SESSION['time']);

// Vous pourriez utiliser la constante SID ici, tout comme dans la page page1.php
echo '<br /><a href="page1.php">page 1</a>';
?>
