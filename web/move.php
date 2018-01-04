<?php
if (isset($_POST['dir'])) {
    $dir = $_POST['dir'];
    // todo
    // add stuff that makes rc car run and stuff
    $rd = ($dir == 0 ? "forward" : ($dir == 1 ? "backward" : ($dir == 2 ? "right" : ($dir == 3 ? "left" : "stop"))));
    //$output = shell_exec("/usr/local/bin/rc_web $rd");
    $gp = shell_exec("gpio read 1");
    if ($gp == 1) {
      echo "[$rd";
      //$output = shell_exec("/usr/local/bin/rc_web $dir");
      $cmd = "/usr/local/bin/rc_web $dir";
      $outputfile = "/dev/shm/output.txt";
      $pidfile = "/dev/shm/pid.txt";
      exec(sprintf("%s > %s 2>&1 & echo $! >> %s", $cmd, $outputfile, $pidfile));
      //try { $result = shell_exec(sprintf("ps %d", $pid)); }
      //$output = shell_exec("/usr/local/bin/rc_web $dir");
      if ($output) {
          echo " - $output";
      }
      echo "]";
    } else {
        echo "[no power]";
    }
} else {
    echo "[invalid command]";
}
?>
