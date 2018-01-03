<?php
if (isset($_POST['power'])) {
    $power = $_POST['power'];
    $on_off = ($power == "on" ? 1 : 0);
    $output = shell_exec("gpio write 1 $on_off");
    echo "[power $power]";
    //echo "<pre>\n[$output - $on_off]\n</pre>\n";
}
?>
