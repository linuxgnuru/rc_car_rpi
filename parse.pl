#!/usr/bin/perl
# setup gpio
system("/usr/local/bin/rc_gpio");

system("gpio write 25 1");

# don't do anything if there is no camera
#$t_cmd = "/usr/bin/vcgencmd get_camera";
#chop($out = `$t_cmd`);
#($sup, $det) = split(/\s+/, $out);
#($s, $fs) = split(/=/, $sup);
#($d, $fd) = split(/=/, $det);

#if ($fs == 1 && $fd == 1) {

    $usb = "/mnt/USB";

    if (!-f $usb . "/stop.txt") {
        $file_name = $usb . "/robot.txt";
        if (-e $file_name) {
            open(F, $file_name);
            while (<F>) {
                chop($l = $_);
                ($dir, $dur) = split(/\s+/, $l);
                $dur *= 1000;
#system("/usr/local/bin/run_once $dir $dur no >/dev/null");
                system("/usr/local/bin/run_once $dir $dur >>/mnt/USB/OUTPUT.log");
                sleep(1);
            }
            close(F);
        } # end check if file_name exists
    } # end check if stop.txt exists
#} # end test for camera

system("gpio write 25 0");
