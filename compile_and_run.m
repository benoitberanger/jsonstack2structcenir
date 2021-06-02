clc
clear

mex -setup C++

mex jsonstack2structcenir.cpp

fname = 'stack_DEV_412_PROVOCAH_Pilote4_S6_Run_1.json';

tic
content = jsonstack2structcenir(fname);
toc*1000

tic
content = get_file_content_as_char( fname );
toc*1000