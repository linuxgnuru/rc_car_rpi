<?php
$output = shell_exec("/opt/vc/bin/raspistill -t 1 -o - -n -w 800 -h 600 -q 70");
header('Content-Type: image/jpeg');
header('Content-Disposition: inline; filename="webcam.jpg"');
echo "$output";
?>
