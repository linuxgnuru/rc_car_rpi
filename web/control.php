<html>
<head>
<title>linuxgnuru</title>
<script type="text/javascript" src="js/jquery.js"> </script>
</head>
<body>
<?php
 //session_start();
 //$d = (isset($_SESSION['direction']) ? $_SESSION['direction'] : "stopped");
 //$d = (isset($_POST['dir']) ? $_POST['dir'] : "stopped");
 /*
 $a = fopen("/dev/shm/angle.txt", "r") or die("unable to open file.");
 $angle = fread($a, filesize("/dev/shm/angle.txt"));
 fclose($a);
 */
?>

<!--form method=post action="index.php"-->
<p align=center>
<table border=0>
<caption><div id="result">[stop - 4]</div></caption>
<tr align=center>
<td><button name="power" value="on" onclick="power(this.value)">Turn On Remote</button></td>
<td><button name="dir" value="0" onclick="moveRobot(this.value)">Forward</button></td>
<td><button name="power" value="off" onclick="power(this.value)">Turn off Remote</button></td>
</tr>

<tr align=center>
<td><button name="dir" value="3" onclick="moveRobot(this.value)">Left</button></td>
<!--
<td><button name="dir" value="stop" onclick="moveRobot(this.value)">stop</button></td>
-->

<!--
<td><a href="take_pic.php" target="on_top">Take Picture</a></td>
<td><a href="t.html" target="on_top">Take Picture</a></td>
-->

<td><button name="dir" value="1" onclick="moveRobot(this.value)">Reverse</button></td>

<td><button name="dir" value="2" onclick="moveRobot(this.value)">Right</button></td>
</tr>

<tr align=center>
<td><button name="pic" value="take" onclick="takePicture(this.value)">Take Picture</button></td>
<td><button name="dir" value="4" onclick="moveRobot(this.value)">Stop</button></td>
<td><button name="pic" value="save" onclick="takePicture(this.value)">Save Picture</button></td>
</tr>
<!--
<tr> <th colspan=3><button name="dir" value="picture" onclick="moveRobot(this.value)">Take picture</button></th> </tr>
-->
</table>
</p>
<!--/form-->
<script>
    function moveRobot(val) {
       $.ajax({
         type: 'post',
         url: 'move.php',
         data: {
           dir:val
         },
         success: function (response) {
           document.getElementById("result").innerHTML=response; 
         }
       });
    }

    function power(val) {
       $.ajax({
         type: 'post',
         url: 'power.php',
         data: {
           power:val
         },
         success: function (response) {
           document.getElementById("result").innerHTML=response; 
         }
       });
    }

    function takePicture(val) {
        if (val == "take") {
            //window.frames['on_top'].document.location = "t.html";
            parent.frames[0].document.location = "t.html";
        } else {
            parent.frames[0].document.location = "save_pic.php";
        }
    }
</script>
</body>
</html>
</script>
</body>
</html>
