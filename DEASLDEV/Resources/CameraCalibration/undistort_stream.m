% undistort_stream.m
% Undistort and display the video stream from UDPCamera

clear variables; %close all;
instrreset;



%%% EDIT PARAMETERS BELOW %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    path(path,'Z:\ASL_repos\development_scripts\Camera\Calibration\TOOLBOX_calib')
    image_IP = '127.0.0.1:10001';           %  image IP (127.0.0.1:1000# where # is the channel)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% DO NOT EDIT BELOW THIS LINE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



load Calib_Results fc cc kc alpha_c err_std
KK = [fc(1) alpha_c*fc(1) cc(1);0 fc(2) cc(2) ; 0 0 1];

%%% Compute the new KK matrix to fit as much data in the image (in order to
%%% accomodate large distortions:
now0 = clock;
ima_name = ['http://',image_IP,'/image.jpg'];
I = double(imread(ima_name));
if size(I,3)>1,
   I = I(:,:,2);
end;
[ny,nx] = size(I);

r2_extreme = (nx^2/(4*fc(1)^2) + ny^2/(4*fc(2)^2));
dist_amount = 1;%(1+kc(1)*r2_extreme + kc(2)*r2_extreme^2);
fc_new = dist_amount * fc;
KK_new = [fc_new(1) alpha_c*fc_new(1) cc(1);0 fc_new(2) cc(2) ; 0 0 1];

[I2] = rect(I,eye(3),fc,cc,kc,KK_new);

titlestr{1} = 'Undistorted Image';
image(I2);
colormap(gray(256));
title(titlestr);
drawnow;
    
tic
% for aaa = 1:100
while true
    now0 = clock;
    I = double(imread(ima_name));
    if size(I,3)>1,
       [I2] = rect(I(:,:,2),eye(3),fc,cc,kc,KK_new);
    else
        [I2] = rect(I,eye(3),fc,cc,kc,KK_new);
    end;
    
    figure(1);
    image(I2);
    colormap(gray(256));
    title(titlestr);
    drawnow;
    pause(0.00001)
    toc,tic
end