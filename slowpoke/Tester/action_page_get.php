<?php
setcookie("cookie1", "oh voilà un ptit ");
setcookie("cookie2", "COOKIE !");
?>
<body>
    <section class="notFound">
        <style>
            body {
              background-color: rgb(68, 54, 40);
            }
            @import url('https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap');

* {
    padding: 0;
    margin: 0;
    box-sizing: border-box;
    font-family: 'Press Start 2P';
    color: #ffffff;
    text-align: center;
}

body {
    background-color: #000000;
    background-image: url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='42' height='58' viewBox='0 0 42 58'%3E%3Cg fill='%23dddcdd' fill-opacity='0.23'%3E%3Cpath fill-rule='evenodd' d='M12 18h12v18h6v4H18V22h-6v-4zm-6-2v-4H0V0h36v6h6v36h-6v4h6v12H6v-6H0V16h6zM34 2H2v8h24v24h8V2zM6 8a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm8 0a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm8 0a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm8 0a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm0 8a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm0 8a2 2 0 1 0 0-4 2 2 0 0 0 0 4zm0 8a2 2 0 1 0 0-4 2 2 0 0 0 0 4zM2 50h32v-8H10V18H2v32zm28-6a2 2 0 1 0 0 4 2 2 0 0 0 0-4zm-8 0a2 2 0 1 0 0 4 2 0 0-4z'/%3E%3C/g%3E%3C/svg%3E");
}

section.notFound {
    display: flex;
    justify-content: center;
    align-items: center;
    margin: 0 5%;
    height: 60vh;
}

section.notFound h1 {
    color: red;
    font-size: 100px;
}

section.submit {
    background-color: #000000;
    border: none;
    color: #5d3434;
    padding: 10px 20px;
    text-align: center;
    text-decoration: none;
    display: inline-block;
    font-size: 16px;
    margin: 4px 2px;
    cursor: pointer;
}

section.notFound h2 {
    font-size: 50px;
}

section.notFound h1, h2, h3 {
    margin-bottom: 40px;
}

div.text {
    height: 50vh;
}

div.text a {
    text-decoration: none;
    margin-right: 20px;
}

div.text a:hover {
    color: red;
    text-decoration: underline; 
}

@media only screen and (max-width: 768px) {
    section.notFound {
        flex-direction: column;
        justify-content: space-around;
    }
    section.notFound div.img img {
        width: 70vw;
        height: auto;
    }
    section.notFound h1 {
        font-size: 50px;
    }
    section.notFound h2 {
        font-size: 25px;
    }
    div.text a:active {
    color: red;
    text-decoration: underline; 
  }
}
            h1 {
              color: maroon;
              margin-left: 40px;
            }
            </style>
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
echo "Hello " . $_GET["lname"] . " " . $_GET["fname"] . " !";
?>
</div>
        
</section>
</body>


</html>