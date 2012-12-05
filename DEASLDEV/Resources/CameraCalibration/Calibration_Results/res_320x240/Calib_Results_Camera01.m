% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 384.450699224842990 ; 384.126576657265840 ];

%-- Principal point:
cc = [ 155.199853772775010 ; 101.564092352949270 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.444783023349018 ; 0.470696765129045 ; 0.000285193379365 ; 0.000031891638698 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 2.064139943246398 ; 1.893352240462898 ];

%-- Principal point uncertainty:
cc_error = [ 2.557792498307830 ; 2.822878496500680 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.030243902108028 ; 0.204751783412522 ; 0.001463159132544 ; 0.001144517494941 ; 0.000000000000000 ];

%-- Image size:
nx = 320;
ny = 240;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 20;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ 1.755264e+000 ; 1.698913e+000 ; 7.519393e-001 ];
Tc_1  = [ -6.234605e+001 ; -6.070592e+001 ; 2.802606e+002 ];
omc_error_1 = [ 6.889064e-003 ; 4.606239e-003 ; 9.169073e-003 ];
Tc_error_1  = [ 1.907978e+000 ; 2.077284e+000 ; 1.488041e+000 ];

%-- Image #2:
omc_2 = [ -2.577158e+000 ; -1.319637e+000 ; -6.159043e-001 ];
Tc_2  = [ -5.466594e+001 ; 4.613892e+001 ; 5.324710e+002 ];
omc_error_2 = [ 7.023084e-003 ; 6.884520e-003 ; 1.245942e-002 ];
Tc_error_2  = [ 3.570259e+000 ; 3.898210e+000 ; 2.977130e+000 ];

%-- Image #3:
omc_3 = [ 1.319273e-001 ; 2.750787e+000 ; 5.501785e-001 ];
Tc_3  = [ 6.343506e+001 ; -8.483189e+001 ; 4.923098e+002 ];
omc_error_3 = [ 3.447549e-003 ; 8.221964e-003 ; 1.141498e-002 ];
Tc_error_3  = [ 3.286650e+000 ; 3.662614e+000 ; 2.800212e+000 ];

%-- Image #4:
omc_4 = [ -6.124172e-001 ; -2.787837e+000 ; -4.451905e-001 ];
Tc_4  = [ 1.575576e+001 ; -7.027739e+001 ; 4.748768e+002 ];
omc_error_4 = [ 2.911552e-003 ; 9.131129e-003 ; 1.248068e-002 ];
Tc_error_4  = [ 3.160491e+000 ; 3.535309e+000 ; 2.893444e+000 ];

%-- Image #5:
omc_5 = [ 3.867034e-001 ; 2.326983e+000 ; 8.785880e-001 ];
Tc_5  = [ 4.582458e+001 ; -7.401970e+001 ; 3.377435e+002 ];
omc_error_5 = [ 5.254469e-003 ; 6.734273e-003 ; 8.890300e-003 ];
Tc_error_5  = [ 2.245366e+000 ; 2.511471e+000 ; 1.765028e+000 ];

%-- Image #6:
omc_6 = [ 2.366303e+000 ; 1.620318e+000 ; 1.526722e-001 ];
Tc_6  = [ -9.685806e+001 ; -1.283966e+001 ; 4.700392e+002 ];
omc_error_6 = [ 8.360823e-003 ; 5.283388e-003 ; 1.493183e-002 ];
Tc_error_6  = [ 3.163809e+000 ; 3.467488e+000 ; 2.554219e+000 ];

%-- Image #7:
omc_7 = [ 6.114485e-002 ; 2.652932e+000 ; 7.133819e-001 ];
Tc_7  = [ 6.940182e+001 ; -7.850924e+001 ; 4.377027e+002 ];
omc_error_7 = [ 3.833761e-003 ; 7.543880e-003 ; 1.036937e-002 ];
Tc_error_7  = [ 2.923456e+000 ; 3.262707e+000 ; 2.470987e+000 ];

%-- Image #8:
omc_8 = [ 1.068232e+000 ; 2.519081e+000 ; 8.091509e-001 ];
Tc_8  = [ 3.071741e+001 ; -6.167063e+001 ; 4.242620e+002 ];
omc_error_8 = [ 6.052215e-003 ; 6.584682e-003 ; 1.082457e-002 ];
Tc_error_8  = [ 2.815231e+000 ; 3.147702e+000 ; 2.399524e+000 ];

%-- Image #9:
omc_9 = [ -9.691787e-002 ; 2.463620e+000 ; 4.219947e-001 ];
Tc_9  = [ 9.414054e+001 ; -7.148878e+001 ; 4.117798e+002 ];
omc_error_9 = [ 3.961427e-003 ; 7.284805e-003 ; 9.463539e-003 ];
Tc_error_9  = [ 2.749113e+000 ; 3.062490e+000 ; 2.091004e+000 ];

%-- Image #10:
omc_10 = [ -1.059915e-003 ; 2.880177e+000 ; 3.440849e-001 ];
Tc_10  = [ 7.265185e+001 ; -7.619761e+001 ; 4.656711e+002 ];
omc_error_10 = [ 2.600999e-003 ; 8.986106e-003 ; 1.263490e-002 ];
Tc_error_10  = [ 3.108286e+000 ; 3.461536e+000 ; 2.693964e+000 ];

%-- Image #11:
omc_11 = [ -2.219281e+000 ; -2.200416e+000 ; -7.721749e-002 ];
Tc_11  = [ -1.005001e+002 ; -4.968122e+001 ; 4.429117e+002 ];
omc_error_11 = [ 7.682450e-003 ; 8.291773e-003 ; 1.659293e-002 ];
Tc_error_11  = [ 2.971584e+000 ; 3.294546e+000 ; 2.675159e+000 ];

%-- Image #12:
omc_12 = [ -1.850117e+000 ; -1.978295e+000 ; -5.861274e-001 ];
Tc_12  = [ -9.680498e+001 ; -2.427820e+001 ; 3.810340e+002 ];
omc_error_12 = [ 4.879725e-003 ; 7.298961e-003 ; 1.010393e-002 ];
Tc_error_12  = [ 2.540059e+000 ; 2.841896e+000 ; 2.216699e+000 ];

%-- Image #13:
omc_13 = [ -1.909656e+000 ; -1.868051e+000 ; 5.134834e-001 ];
Tc_13  = [ -1.070321e+002 ; -4.580639e+001 ; 4.675365e+002 ];
omc_error_13 = [ 7.213873e-003 ; 5.459314e-003 ; 9.735334e-003 ];
Tc_error_13  = [ 3.115830e+000 ; 3.463693e+000 ; 1.872946e+000 ];

%-- Image #14:
omc_14 = [ 1.799410e+000 ; 1.940266e+000 ; 7.954227e-001 ];
Tc_14  = [ -3.796638e+001 ; -4.684280e+001 ; 3.249307e+002 ];
omc_error_14 = [ 7.208642e-003 ; 4.729812e-003 ; 1.019358e-002 ];
Tc_error_14  = [ 2.174526e+000 ; 2.395380e+000 ; 1.733187e+000 ];

%-- Image #15:
omc_15 = [ -2.152471e+000 ; -1.851366e+000 ; -1.330975e+000 ];
Tc_15  = [ -4.736409e+001 ; -6.331392e+000 ; 2.737203e+002 ];
omc_error_15 = [ 4.184538e-003 ; 8.717726e-003 ; 1.040207e-002 ];
Tc_error_15  = [ 1.837428e+000 ; 2.020338e+000 ; 1.710554e+000 ];

%-- Image #16:
omc_16 = [ -1.883327e+000 ; -1.610870e+000 ; -3.180856e-001 ];
Tc_16  = [ -1.116867e+002 ; -1.813454e+001 ; 3.793650e+002 ];
omc_error_16 = [ 5.788908e-003 ; 6.314907e-003 ; 8.722977e-003 ];
Tc_error_16  = [ 2.529206e+000 ; 2.823073e+000 ; 1.975423e+000 ];

%-- Image #17:
omc_17 = [ 1.160049e-001 ; -2.995160e+000 ; -9.179467e-001 ];
Tc_17  = [ 6.845528e+001 ; -4.012753e+001 ; 3.560061e+002 ];
omc_error_17 = [ 3.984916e-003 ; 7.889556e-003 ; 1.146679e-002 ];
Tc_error_17  = [ 2.375668e+000 ; 2.651068e+000 ; 2.222262e+000 ];

%-- Image #18:
omc_18 = [ -4.460337e-002 ; -2.579665e+000 ; -5.111532e-001 ];
Tc_18  = [ 4.057978e+001 ; -6.435124e+001 ; 3.422502e+002 ];
omc_error_18 = [ 3.416478e-003 ; 7.918085e-003 ; 9.867116e-003 ];
Tc_error_18  = [ 2.292878e+000 ; 2.547823e+000 ; 2.056929e+000 ];

%-- Image #19:
omc_19 = [ -3.057903e-001 ; 2.415433e+000 ; 5.816354e-001 ];
Tc_19  = [ 1.167071e+002 ; -4.396832e+001 ; 4.091091e+002 ];
omc_error_19 = [ 4.272658e-003 ; 7.316933e-003 ; 9.373937e-003 ];
Tc_error_19  = [ 2.731625e+000 ; 3.058125e+000 ; 2.177550e+000 ];

%-- Image #20:
omc_20 = [ -2.110256e-002 ; 2.978334e+000 ; 2.250494e-001 ];
Tc_20  = [ 8.624467e+001 ; -7.565885e+001 ; 4.156809e+002 ];
omc_error_20 = [ 2.117940e-003 ; 9.134834e-003 ; 1.309562e-002 ];
Tc_error_20  = [ 2.777752e+000 ; 3.098292e+000 ; 2.468223e+000 ];

