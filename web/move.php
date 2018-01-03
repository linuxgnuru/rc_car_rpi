<?php
if (isset($_POST['dir'])) {
    $dir = $_POST['dir'];
    // todo
    // add stuff that makes rc car run and stuff
    //$rd = ($dir == "forward" ? 0 : ($dir == "backward" ? 1 : ($dir == "left" ? 2 : ($dir == "right" ? 3 : 4))));
    //$output = shell_exec("/usr/local/bin/rc_web $rd");
    $output = shell_exec("/usr/local/bin/rc_web $dir");
    if ($output) { echo "<pre>\ndir: [$dir]\n[$output]\n</pre>\n"; }
    else { echo "[$dir]"; }
} else {
    echo "[invalid command]";
}
?>
