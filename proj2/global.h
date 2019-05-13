#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define FND(data) data[0] << 12 | data[1] << 8 | data[2] << 4 | data[0]

#define write_to_device(val,f_idx,f_dir,s_idx,s_dir) do{\
    outw((unsigned short int)(FND(fnd_data)),(unsigned int)iom_fpga_fnd_addr);\
    for(i = 0 ; i < 10; i++)\
        outw((unsigned short int)dot_data[val][i],(unsigned int)iom_fpga_dot_addr + i*2);\
    outw((unsigned short)(1 << (val-1)),(unsigned int)iom_fpga_led_addr);\
    new_idx = f_idx + f_dir;\
    if (0 >  new_idx || new_idx + strlen(name) >=16)\
        f_dir *= -1;\
    new_idx = f_idx + f_dir;\
    if (0 <=  new_idx && new_idx + strlen(name) <16){\
        for(i = 0 ; i < strlen(name); i++){\
            unsigned short int temp_data = ((name[i] & 0xFF) << 8 | (name[i+1] & 0xFF));\
            outw(temp_data,(unsigned int)iom_fpga_text_lcd_addr+i+new_idx);\
        }\
        f_idx = new_idx;\
    }\
    new_idx = s_idx + s_dir;\
    if (0 >  new_idx || new_idx + strlen(id) >=16){\
        s_dir *= -1;\
    }\
    new_idx = s_idx + s_dir;\
    if (0 <=  new_idx && new_idx + strlen(id) <16){  \
        for(i = 0 ; i < strlen(id); i++){\
            unsigned short int temp_data = ((id[i] & 0xFF) << 8 | (id[i+1] & 0xFF));\
            outw(temp_data,(unsigned int)iom_fpga_text_lcd_addr+16+i+new_idx);\
        }\
        s_idx = new_idx;\
    }\
}while(0)\

#define INIT do{\
    outw((unsigned short int)0,(unsigned int)iom_fpga_fnd_addr);\
    for(i = 0 ; i < 10; i++)\
        outw((unsigned short int)0,(unsigned int)iom_fpga_dot_addr + i*2);\
    outw((unsigned short)0,(unsigned int)iom_fpga_led_addr);\
    for(i = 0 ; i < 32; i++){\
        outw((unsigned short int)0,(unsigned int)iom_fpga_text_lcd_addr+i);\
    }\
}while(0)\

unsigned char dot_data[10][10] = {
	{0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e}, // 0
	{0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e}, // 1
	{0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f}, // 2
	{0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e}, // 3
	{0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06}, // 4
	{0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e}, // 5
	{0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e}, // 6
	{0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03}, // 7
	{0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e}, // 8
	{0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03} // 9
};


#endif
