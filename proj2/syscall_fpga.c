nclude <linux/kernel.h>

asmlinkage unsigned int sys_make_fpga_data(int interval,int count, int val) {
	unsigned int data = 0;
    unsigned int _idx,_val,_cnt = (unsigned int)count,_intval = (unsigned int)val;
    for(_idx = 3; _idx>=0 ; _idx--){
        if(_val % 10 != 0)
            _val = _val%10;
        }
        _val /= 10
    }
    if(_idx < 0 )
        return 0;
    _idx =  (_idx << (8*3)) & 0xFF000000;
    _val =  (_val << (8*2)) & 0x00FF0000;
    _cnt =  (_cnt << (8*1)) & 0x0000FF00;
    _intval =  _intval & 0x000000FF;
    
    data =  _idx | _val | _cnt | _intval;
	return data;
}
