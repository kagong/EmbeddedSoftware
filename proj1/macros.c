#include "macros.h"
int _exp[4][4];
int max_notation[4];
/* NAME: SET_VALUE
 * it set the data
 * it will be called by the main()
 */
void set_value(){
    _exp[0][0] = 1 << 3;
    _exp[0][1] = 1 << 2;
    _exp[0][2] = 1 << 1;
    _exp[0][3] = 1;
    _exp[1][0] = 1000;
    _exp[1][1] = 100;
    _exp[1][2] = 10;
    _exp[1][3] = 1;
    _exp[2][0] = 1 << 9;
    _exp[2][1] = 1 << 6;
    _exp[2][2] = 1 << 3;
    _exp[2][3] = 1;
    _exp[3][0] = 1 << 6;
    _exp[3][1] = 1 << 4;
    _exp[3][2] = 1 << 2;
    _exp[3][3] = 1;
    max_notation[0]=2;
    max_notation[1]=10;
    max_notation[2]=8;
    max_notation[3]=4;

}
