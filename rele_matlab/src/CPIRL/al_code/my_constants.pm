$left_boundary = 120;
$right_boundary = 200;
$left_lane = 140;
$middle_lane = 160;
$right_lane = 180;
$height = 160 + 20;
$width = 300;
$car_length = 40;
$car_width = 20;
$step = 5; 					# This should evenly divide $left_boundary and $right_boundary
@speed = (0 .. 2);
@displace = (20, 40, 80);
@action = (-1 .. 1);
$road_left_boundary = $left_boundary + 20;
$road_right_boundary = $right_boundary - 20;
@my_car_x = map {$_ * $step} (($left_boundary/$step) .. ($right_boundary/$step));
@car_1_x = ($left_lane, $middle_lane, $right_lane);
@car_1_y = map {($_ * 20) + 10} (0 .. 9); 	# TODO: Make this less hard-code-y
$blue_car_bottom = $height - 10;
$red_car_start = 10;
