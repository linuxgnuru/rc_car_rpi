<?php
  $output = shell_exec("/opt/vc/bin/raspistill -t 1 -o /var/www/html/picture.jpg -n -w 800 -h 600");
header('Location: picture.jpg');

?>
