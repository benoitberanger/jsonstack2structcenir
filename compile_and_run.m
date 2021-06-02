clc
clear

mex -setup C++

mex jsonstack2structcenir.cpp

jsonstack2structcenir('stack_DEV_412_PROVOCAH_Pilote4_S6_Run_1.json')
