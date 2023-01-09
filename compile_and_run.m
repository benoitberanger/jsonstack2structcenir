clc
clear

% mex -setup C++

mex jsonstack2structcenir.cpp

fname = 'stack_DEV_412_PROVOCAH_Pilote4_S6_Run_1.json';


%%

fprintf('\n')

tic
content = jsonstack2structcenir(fname)
fprintf('jsonstack2structcenir : %g \n', toc*1000)

tic
field_to_get={
    'RepetitionTime'
    'EchoTime'
    };
field_type={
    'double'
    'double'
    };
content2 = get_string_from_json( fname , field_to_get , field_type );
fprintf('get_string_from_json : %g \n', toc*1000)

% tic
% content3 = get_sequence_param_from_json(fname);
% fprintf('get_sequence_param_from_json : %g \n', toc*1000)

% tic
% content4 = spm_jsonread(fname);
% fprintf('spm_jsonread : %g \n', toc*1000)
