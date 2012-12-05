% camera_calibrate_coords.m
% Find the camera coordinate frame wrt robot frame

clear variables; close all;
instrreset;

%%% EDIT PARAMETERS BELOW %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    path(path,'Z:\ASL_repos\rover_interface\UDPCommunicator')
    path(path,'Z:\ASL_repos\development_scripts\Camera\Calibration\TOOLBOX_calib')

    Pioneer.SubjectName = 'Segway01';      %  >>    subject name, as tracked by Vicon

    Pioneer.dX = 0;                         %  \\
    Pioneer.dY = 0;                         %   ||
    Pioneer.dZ = 0;%-0.1937;                   %   \\   get these values from 
    Pioneer.eX = 0;%-90;                       %   //   motioncapture2localize3d.cfg
    Pioneer.eY = 0;                         %   ||
    Pioneer.eZ = 0;%90;                        %  //

    image_IP = '127.0.0.1:10001';           %  >>    image IP (127.0.0.1:1000# where # is the channel)
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%% DO NOT EDIT BELOW THIS LINE %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

fprintf( 'Loading SDK...' );
Client.LoadViconDataStreamSDK();
fprintf( 'done\n' );
% Make a new client
MyClient = Client();

% Connect to a server
fprintf( 'Connecting...' );
while ~MyClient.IsConnected().Connected
  MyClient.Connect( '10.0.0.102:801' );
  fprintf( '.' );
end
fprintf( '\n' );

Output.EnableSegmentData         = MyClient.EnableSegmentData();
Output.EnableMarkerData          = MyClient.EnableMarkerData();
Output.EnableUnlabeledMarkerData = MyClient.EnableUnlabeledMarkerData();
Output.EnableDeviceData          = MyClient.EnableDeviceData();

Output.SetStreamMode = MyClient.SetStreamMode( StreamMode.ClientPull );
Output.SetAxisMapping = MyClient.SetAxisMapping( Direction.Forward, ...
                                                 Direction.Left,    ...
                                                 Direction.Up );    % Z-up
fprintf( 'Waiting for new frame...' );
while MyClient.GetFrame().Result.Value ~= Result.Success
    fprintf( '.' );
end% while
fprintf( '\n' );  

if MyClient.GetSubjectCount().SubjectCount == 0
    disp('ERROR: No Subjects Found');
    return
end

Pioneer.SegmentName = MyClient.GetSegmentName(Pioneer.SubjectName,1).SegmentName;
Pioneer.GlobalTranslation = MyClient.GetSegmentGlobalTranslation( Pioneer.SubjectName, Pioneer.SegmentName );
if sum(Pioneer.GlobalTranslation.Translation == [0;0;0])
    disp('ERROR: No Pioneer Data');
    return
end

CalibObject.SubjectName = 'cameracalibobject';
CalibObject.SegmentName = 'cameracalibobject';
CalibObject.GlobalTranslation = MyClient.GetSegmentGlobalTranslation( CalibObject.SubjectName, CalibObject.SegmentName );
if sum(CalibObject.GlobalTranslation.Translation == [0;0;0])
    disp('ERROR: No Calibration Object Data');
    return
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Pull a frame from Vicon and store segment locations and Occluded flags
for k = 1:100
    MyClient.GetFrame();
    
    Pioneer.GlobalTranslation = MyClient.GetSegmentGlobalTranslation( Pioneer.SubjectName, Pioneer.SegmentName );
    Pioneer.GlobalRotationMatrix = MyClient.GetSegmentGlobalRotationMatrix( Pioneer.SubjectName, Pioneer.SegmentName );
    Pioneer.GlobalRotationEulerXYZ = MyClient.GetSegmentGlobalRotationEulerXYZ( Pioneer.SubjectName, Pioneer.SegmentName );
        ViconPioneer(k).xyz = single(Pioneer.GlobalTranslation.Translation);
        ViconPioneer(k).o = uint8(Pioneer.GlobalTranslation.Occluded);
        ViconPioneer(k).RotationMatrix = single(Pioneer.GlobalRotationMatrix.Rotation);
        ViconPioneer(k).RotationEulerXYZ = single(Pioneer.GlobalRotationEulerXYZ.Rotation);
    
    CalibObject.GlobalTranslation = MyClient.GetSegmentGlobalTranslation( CalibObject.SubjectName, CalibObject.SegmentName );
    CalibObject.GlobalRotationMatrix = MyClient.GetSegmentGlobalRotationMatrix( CalibObject.SubjectName, CalibObject.SegmentName );
    CalibObject.GlobalRotationEulerXYZ = MyClient.GetSegmentGlobalRotationEulerXYZ( CalibObject.SubjectName, CalibObject.SegmentName );
        ViconCalibObject(k).xyz = single(CalibObject.GlobalTranslation.Translation);
        ViconCalibObject(k).o = uint8(CalibObject.GlobalTranslation.Occluded);
        ViconCalibObject(k).RotationMatrix = single(CalibObject.GlobalRotationMatrix.Rotation);
        ViconCalibObject(k).RotationEulerXYZ = single(CalibObject.GlobalRotationEulerXYZ.Rotation);
end

Pioneer.xyz = [];
Pioneer.EulerXYZ = [];
Pioneer.RotMat = [];
CalibObject.xyz = [];
CalibObject.EulerXYZ = [];
CalibObject.RotMat = [];

for fIndx = 1:k
    if ViconPioneer(fIndx).o(1) == 0
       Pioneer.xyz(:,end+1) = ViconPioneer(fIndx).xyz;
       Pioneer.EulerXYZ(:,end+1) = ViconPioneer(fIndx).RotationEulerXYZ;
       Pioneer.RotMat(:,:,end+1) = ViconPioneer(fIndx).RotationMatrix;
    end
    if ViconCalibObject(fIndx).o(1) == 0
       CalibObject.xyz(:,end+1) = ViconCalibObject(fIndx).xyz;
       CalibObject.EulerXYZ(:,end+1) = ViconCalibObject(fIndx).RotationEulerXYZ;
       CalibObject.RotMat(:,:,end+1) = ViconCalibObject(fIndx).RotationMatrix;
    end
end


% This needs to be changed to a LS fit...
Pioneer.xyz=mean(Pioneer.xyz,2);
Pioneer.EulerXYZ=mean(Pioneer.EulerXYZ,2);
Pioneer.RotMat = mean(Pioneer.RotMat(:,:,[2:end]),3);
CalibObject.xyz=mean(CalibObject.xyz,2);
CalibObject.EulerXYZ=mean(CalibObject.EulerXYZ,2);
CalibObject.RotMat = mean(CalibObject.RotMat(:,:,[2:end]),3);

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Take a picture of the camera's current view
ima_name = ['http://',image_IP,'/image.jpg'];
I = double(imread(ima_name));
if size(I,3)>1,
   I = I(:,:,2);
end;
[ny,nx] = size(I);

load Calib_Results fc cc kc alpha_c err_std

fprintf(1,'\nExtraction of the grid corners on the image\n');

wintx = 5;
winty = 5;

[x_ext,X_ext,n_sq_x,n_sq_y,ind_orig,ind_x,ind_y] = extract_grid(I,wintx,winty,fc,cc,kc,30,30);

%%% Computation of the Extrinsic Parameters attached to the grid:
[omc_ext,Tc_ext,Rc_ext,H_ext] = compute_extrinsic(x_ext,X_ext,fc,cc,kc,alpha_c);

%%% Reproject the points on the image:
[x_reproj] = project_points2(X_ext,omc_ext,Tc_ext,fc,cc,kc,alpha_c);
err_reproj = x_ext - x_reproj;
err_std2 = std(err_reproj')';

Basis = [X_ext(:,[ind_orig ind_x ind_orig ind_y ind_orig ])];

VX = Basis(:,2) - Basis(:,1);
VY = Basis(:,4) - Basis(:,1);
nX = norm(VX);
nY = norm(VY);
VZ = min(nX,nY) * cross(VX/nX,VY/nY);

Basis = [Basis VZ];

[x_basis] = project_points2(Basis,omc_ext,Tc_ext,fc,cc,kc,alpha_c);

dxpos = (x_basis(:,2) + x_basis(:,1))/2;
dypos = (x_basis(:,4) + x_basis(:,3))/2;
dzpos = (x_basis(:,6) + x_basis(:,5))/2;

figure(2);
image(I);
colormap(gray(256));
hold on;
plot(x_ext(1,:)+1,x_ext(2,:)+1,'r+');
plot(x_reproj(1,:)+1,x_reproj(2,:)+1,'yo');
h = text(x_ext(1,ind_orig)-25,x_ext(2,ind_orig)-25,'O');
set(h,'Color','g','FontSize',14);
h2 = text(dxpos(1)+1,dxpos(2)-30,'X');
set(h2,'Color','g','FontSize',14);
h3 = text(dypos(1)-30,dypos(2)+1,'Y');
set(h3,'Color','g','FontSize',14);
h4 = text(dzpos(1)-10,dzpos(2)-20,'Z');
set(h4,'Color','g','FontSize',14);
plot(x_basis(1,:)+1,x_basis(2,:)+1,'g-','linewidth',2);
title('Image points (+) and reprojected grid points (o)');
hold off;

% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
% fprintf(1,'\n\nExtrinsic parameters:\n\n');
% fprintf(1,'Translation vector: Tc_ext = [ %3.6f \t %3.6f \t %3.6f ]\n',Tc_ext);
% fprintf(1,'Rotation vector:   omc_ext = [ %3.6f \t %3.6f \t %3.6f ]\n',omc_ext);
% fprintf(1,'Rotation matrix:    Rc_ext = [ %3.6f \t %3.6f \t %3.6f\n',Rc_ext(1,:)');
% fprintf(1,'                               %3.6f \t %3.6f \t %3.6f\n',Rc_ext(2,:)');
% fprintf(1,'                               %3.6f \t %3.6f \t %3.6f ]\n',Rc_ext(3,:)');
% fprintf(1,'Pixel error:           err = [ %3.5f \t %3.5f ]\n\n',err_std2); 
% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

% Stores the results:
kk = 1;
n_ima = 1;

% Stores location of grid wrt camera:
eval(['omc_' num2str(kk) ' = omc_ext;']);
eval(['Tc_' num2str(kk) ' = Tc_ext;']);

% Stores the projected points:
eval(['y_' num2str(kk) ' = x_reproj;']);
eval(['X_' num2str(kk) ' = X_ext;']);
eval(['x_' num2str(kk) ' = x_ext;']);      
      
% Organize the points in a grid:
eval(['n_sq_x_' num2str(kk) ' = n_sq_x;']);
eval(['n_sq_y_' num2str(kk) ' = n_sq_y;']);

% Show extrinsic parameters
ext_calib


%% Calculate Transformation Matrices
%
% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
% Camera --> True Pioneer :
%     Camera --> True Grid
%                True Grid --> Vicon Grid
%                              Vicon Grid --> Vicon Global
%                                             Vicon Global --> Vicon Pioneer
%                                                              Vicon Pioneer --> True Pioneer
% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
% Transform pioneer coordinate frame outputted by Vicon to the coordinate frame we want
Rx = [1          0                       0 ; ...
      0 cos(Pioneer.eX*pi/180) -sin(Pioneer.eX*pi/180) ; ...
      0 sin(Pioneer.eX*pi/180) cos(Pioneer.eX*pi/180)];
  
Ry = [cos(Pioneer.eY*pi/180)  0  sin(Pioneer.eY*pi/180) ; ...
                0             1             0 ; ...
      -sin(Pioneer.eY*pi/180) 0  cos(Pioneer.eY*pi/180)];
  
Rz = [cos(Pioneer.eZ*pi/180) -sin(Pioneer.eZ*pi/180) 0 ; ...
      sin(Pioneer.eZ*pi/180)  cos(Pioneer.eZ*pi/180) 0 ; ...
               0                       0             1];
           
T = [Pioneer.dX;Pioneer.dY;Pioneer.dZ];

ViconPioneer_to_TruePioneer = inv([Rx,[0;0;0];0,0,0,1])*inv([Ry,[0;0;0];0,0,0,1])*inv([Rz,[0;0;0];0,0,0,1])*[eye(3),-T;0 0 0 1];
TruePioneer_to_ViconPioneer = inv(ViconPioneer_to_TruePioneer);
Global_to_ViconPioneer = [[Pioneer.RotMat,zeros(3,1)];0 0 0 1]*[[eye(3),-Pioneer.xyz./1000];0 0 0 1];
ViconPioneer_to_Global = inv(Global_to_ViconPioneer);
Global_to_TruePioneer = ViconPioneer_to_TruePioneer*Global_to_ViconPioneer;

Ry90 = [cos(pi/2) 0 sin(pi/2) ; ...
        0 1 0 ; ...
      -sin(pi/2) 0 cos(pi/2)];
  
ViconGrid_to_TrueGrid = [[inv(Ry90);0 0 0],[-.030;-.030;0;1]];
TrueGrid_to_ViconGrid = inv(ViconGrid_to_TrueGrid);
Global_to_ViconGrid = [[CalibObject.RotMat,zeros(3,1)];0 0 0 1]*[[eye(3),-CalibObject.xyz./1000];0 0 0 1];
ViconGrid_to_Global = inv(Global_to_ViconGrid);
TrueGrid_to_Camera = [[R_kk , Tc_kk./1000];0 0 0 1];   % units in meters
Camera_to_TrueGrid = inv(TrueGrid_to_Camera);

Camera_to_TruePioneer = ViconPioneer_to_TruePioneer*Global_to_ViconPioneer*ViconGrid_to_Global*TrueGrid_to_ViconGrid*Camera_to_TrueGrid;
TruePioneer_to_Camera = inv(Camera_to_TruePioneer); 

p2c = TruePioneer_to_Camera;
c2p = Camera_to_TruePioneer;

date = datestr(today);
pioneer = Pioneer.SubjectName;

README_str{1} = ['c2p = camera to pioneer coordinates  --->  [xp;yp;zp;1] = c2p*[xc;yc;zc;1]'];
README_str{2} = ['p2c = pioneer to camera coordinates  --->  [xc;yc;zc;1] = p2c*[xp;yp;zp;1]'];
README_str{3} = ['pioneer frame: origin between wheel hubs, +x forward, +y left, +z up'];
README_str{4} = ['camera frame: origin at image center (cc), +x right, +y down, +z out(forward)'];
README_str=README_str';

clear ans colors image_IP ViconPioneer ViconCalibObject BASE Basis H_ext N_kk Rkk VX VY VZ XX_kk X_ext YY_kk YYx YYy YYz a active_images b dxpos dypos dzpos fIndx h h2 h3 h4 h_switch h_switch2 hhh ind_orig ind_x ind_y k kk nX nY n_act n_ima n_sq_x n_sq_x_1 n_sq_y n_sq_y_1 no_grid nx ny omc_1 omc_ext omc_kk show_camera uu wintx winty x_1 x_basis x_ext x_reproj y_1 IP dX dY err_reproj ima_name ind_active err_std2 X_1 Rc_ext Tc_1 Tc_ext
save(['camera_calib_results_',date],'date','pioneer','fc','cc','alpha_c','kc','err_std','c2p','p2c','README_str');


% date : date the calibration was run
% pioneer : subject name of pioneer
% p2c (4x4 matrix) : transformation matrix from pioneer coordinates to camera coordinates
% c2p (4x4 matrix) : transformation matrix from camera coordinates to pioneer coordinates
% fc (2x1 vector) : focal length in pixels
% cc (2x1 vector) : principal point coordinates
% alpha_c (scalar) : skew coefficient defining the angle between the x and y pixel axes (default = 0)
% kc (5x1 vector) : image distortion coefficients (radial and tangential distortions) 
% err (2x1 vector) : standard deviation of the reprojection error (in pixels) in x and y directions, respectively


% Disconnect and dispose
MyClient.Disconnect();

% Unload the SDK
Client.UnloadViconDataStreamSDK();
















return
