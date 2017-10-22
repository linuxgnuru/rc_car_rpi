#include <stdio.h>
#include <stdlib.h>

#include "rc_header.h"

void printEnum(int e, _Bool b)
{
    char cr;

    cr = (b ? '\n' : ' ');
    switch (e)
    {
        case BAD_: printf("bad_%c", cr); break;
        case OK_: printf("ok_%c", cr); break;
        case CHECK_SONAR: printf("check_sonar%c", cr); break;
        case CHECK_SONAR_LEFT: printf("check_sonar_left%c", cr); break;
        case CHECK_SONAR_CENTER: printf("check_sonar_center%c", cr); break;
        case CHECK_SONAR_RIGHT: printf("check_sonar_right%c", cr); break;
        case CHECK_SONAR_REAR: printf("check_sonar_rear%c", cr); break;
        case GO_RIGHT: printf("go_right%c", cr); break;
        case GO_CENTER: printf("go_center%c", cr); break;
        case GO_LEFT: printf("go_left%c", cr); break;
        case GET_DIR: printf("get_dir%c", cr); break;
        case SONAR_LEFT: printf("sonar_left%c", cr); break;
        case SONAR_CENTER: printf("sonar_center%c", cr); break;
        case SONAR_RIGHT: printf("sonar_right%c", cr); break;
        case SONAR_REAR: printf("sonar_rear%c", cr); break;
        case NOCHANGE_: printf("nochange_%c", cr); break;
        case RIGHT_: printf("right_%c", cr); break;
        case LEFT_: printf("left_%c", cr); break;
        case CENTER_: printf("center_%c", cr); break;
        case FAR_RIGHT_: printf("far_right_%c", cr); break;
        case FAR_LEFT_: printf("far_left_%c", cr); break;
        case REMOTE_FORWARD: printf("remote_forward%c", cr); break;
        case REMOTE_BACKWARD: printf("remote_backward%c", cr); break;
        case REMOTE_STOP: printf("remote_stop%c", cr); break;
        case REMOTE_POWER: printf("remote_power%c", cr); break;
        case VOLTAGE: printf("voltage%c", cr); break;
        case CURRENT: printf("current%c", cr); break;
        case PERCENTAGE: printf("percentage%c", cr); break;
        case TEMPERATURE: printf("temperature%c", cr); break;
        case MOVE_F: printf("move_f%c", cr); break;
        case MOVE_B: printf("move_b%c", cr); break;
        case MOVE_R: printf("move_r%c", cr); break;
        case MOVE_L: printf("move_l%c", cr); break;
        case LED_BLUE_B: printf("led_blue_b%c", cr); break;
        case LED_BLUE_S: printf("led_blue_s%c", cr); break;
        case LED_GREEN_B: printf("led_green_b%c", cr); break;
        case LED_GREEN_S: printf("led_green_s%c", cr); break;
        case LED_YELLOW_B: printf("led_yellow_b%c", cr); break;
        case LED_YELLOW_S: printf("led_yellow_s%c", cr); break;
        case LED_RED_S: printf("led_red_s%c", cr); break;
        case LED_RED_B: printf("led_red_b%c", cr); break;
        default: printf("[%d]%c", e, cr); break;
    }
}
