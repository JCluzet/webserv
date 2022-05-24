<?php
session_start();
/*session is started if you don't write this line can't use $_Session  global variable*/
$_SESSION[$_POST["tag"]]=$_POST["value"];

// output the new value
echo $_POST["tag"] . "=" . $_POST["value"] . " is now set!";

/*it is my new session*/
// $_SESSION[$_POST["session2"]]=$updatedvalue;
/*session updated*/
echo '<br><br><br /><a href="/">Back to HomePage</a>';
?>