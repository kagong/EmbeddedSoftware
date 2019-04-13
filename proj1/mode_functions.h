#ifndef MODE_FUNCTIONS_H
#define MODE_FUNCTIONS_H
#include "global.h"
void mode_clock(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_clock_checker(fpga_devices *now,msg_output *omsg);
void mode_counter(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_text_editor(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_draw_board(msg_input *imsg, fpga_devices *now, msg_output *omsg);
void mode_foo(msg_input *imsg, fpga_devices *now, msg_output *omsg);

#endif
