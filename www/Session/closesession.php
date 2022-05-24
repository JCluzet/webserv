<?php
session_start();
unset($_SESSION);
session_destroy();
echo "Session is now destroyed!";
echo '<br><br><br /><a href="/">Back to HomePage</a>';
?>