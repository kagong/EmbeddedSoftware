#include <linux/kernel.h>

asmlinkage unsigned int sys_make_fpga_data(int interval,int count, int val) {
	unsigned int data = 0;
    unsigned int t_idx,t_val,t_cnt,t_intval;
    t_val = (unsigned int)val;
    t_cnt = (unsigned int)count;
    t_intval = (unsigned int)interval;
    for(t_idx = 3; t_idx>=0 ; t_idx--){
        if(t_val % 10 != 0){
            t_val = t_val%10;
            break;
        }
        t_val /= 10;
    }
    if(t_idx < 0 )
        return 0;
    t_idx =  (t_idx << (8*3)) & 0xFF000000;
    t_val =  (t_val << (8*2)) & 0x00FF0000;
    t_cnt =  (t_cnt << (8*1)) & 0x0000FF00;
    t_intval =  t_intval & 0x000000FF;
    
    data =  t_idx | t_val | t_cnt | t_intval;
	return data;
}
