% Intrinsic and Extrinsic Camera Parameters
%
% This script file can be directly excecuted under Matlab to recover the camera intrinsic and extrinsic parameters.
% IMPORTANT: This file contains neither the structure of the calibration objects nor the image coordinates of the calibration points.
%            All those complementary variables are saved in the complete matlab data file Calib_Results.mat.
% For more information regarding the calibration model visit http://www.vision.caltech.edu/bouguetj/calib_doc/


%-- Focal length:
fc = [ 694.888063280957790 ; 693.780739351041920 ];

%-- Principal point:
cc = [ 326.443913218200180 ; 234.722398168528800 ];

%-- Skew coefficient:
alpha_c = 0.000000000000000;

%-- Distortion coefficients:
kc = [ -0.209532848162832 ; 0.107777509362195 ; -0.003009874188798 ; -0.004674258760730 ; 0.000000000000000 ];

%-- Focal length uncertainty:
fc_error = [ 4.017133407930996 ; 3.842937964990214 ];

%-- Principal point uncertainty:
cc_error = [ 4.091403764118927 ; 3.361262185328583 ];

%-- Skew coefficient uncertainty:
alpha_c_error = 0.000000000000000;

%-- Distortion coefficients uncertainty:
kc_error = [ 0.023651723878419 ; 0.206560282854057 ; 0.000948672206469 ; 0.001012574470555 ; 0.000000000000000 ];

%-- Image size:
nx = 640;
ny = 480;


%-- Various other variables (may be ignored if you do not use the Matlab Calibration Toolbox):
%-- Those variables are used to control which intrinsic parameters should be optimized

n_ima = 25;						% Number of calibration images
est_fc = [ 1 ; 1 ];					% Estimation indicator of the two focal variables
est_aspect_ratio = 1;				% Estimation indicator of the aspect ratio fc(2)/fc(1)
center_optim = 1;					% Estimation indicator of the principal point
est_alpha = 0;						% Estimation indicator of the skew coefficient
est_dist = [ 1 ; 1 ; 1 ; 1 ; 0 ];	% Estimation indicator of the distortion coefficients


%-- Extrinsic parameters:
%-- The rotation (omc_kk) and the translation (Tc_kk) vectors for every calibration image and their uncertainties

%-- Image #1:
omc_1 = [ -2.135638e+000 ; -2.124468e+000 ; 3.764964e-001 ];
Tc_1  = [ -9.380644e+001 ; -6.880772e+001 ; 4.550859e+002 ];
omc_error_1 = [ 4.635638e-003 ; 3.995020e-003 ; 8.714632e-003 ];
Tc_error_1  = [ 2.674215e+000 ; 2.202349e+000 ; 2.713254e+000 ];

%-- Image #2:
omc_2 = [ -2.014053e+000 ; -1.944940e+000 ; 4.856894e-001 ];
Tc_2  = [ -8.667034e+001 ; -7.163854e+001 ; 6.260771e+002 ];
omc_error_2 = [ 4.987398e-003 ; 4.298671e-003 ; 8.819107e-003 ];
Tc_error_2  = [ 3.687292e+000 ; 3.030537e+000 ; 3.505573e+000 ];

%-- Image #3:
omc_3 = [ 2.084092e+000 ; 2.118265e+000 ; -7.769789e-001 ];
Tc_3  = [ -9.012517e+001 ; -6.018311e+001 ; 5.415263e+002 ];
omc_error_3 = [ 3.044318e-003 ; 4.944641e-003 ; 8.818930e-003 ];
Tc_error_3  = [ 3.184340e+000 ; 2.619057e+000 ; 3.001901e+000 ];

%-- Image #4:
omc_4 = [ 2.116675e+000 ; 2.010355e+000 ; 2.403568e-001 ];
Tc_4  = [ -9.946488e+001 ; -7.762977e+001 ; 4.450576e+002 ];
omc_error_4 = [ 4.807550e-003 ; 4.300606e-003 ; 9.172482e-003 ];
Tc_error_4  = [ 2.656301e+000 ; 2.171307e+000 ; 2.743678e+000 ];

%-- Image #5:
omc_5 = [ -1.869568e+000 ; -1.829610e+000 ; 7.736561e-001 ];
Tc_5  = [ -8.448768e+001 ; -6.729170e+001 ; 5.295628e+002 ];
omc_error_5 = [ 5.092458e-003 ; 3.903664e-003 ; 7.329645e-003 ];
Tc_error_5  = [ 3.125708e+000 ; 2.565363e+000 ; 2.737822e+000 ];

%-- Image #6:
omc_6 = [ NaN ; NaN ; NaN ];
Tc_6  = [ NaN ; NaN ; NaN ];
omc_error_6 = [ NaN ; NaN ; NaN ];
Tc_error_6  = [ NaN ; NaN ; NaN ];

%-- Image #7:
omc_7 = [ NaN ; NaN ; NaN ];
Tc_7  = [ NaN ; NaN ; NaN ];
omc_error_7 = [ NaN ; NaN ; NaN ];
Tc_error_7  = [ NaN ; NaN ; NaN ];

%-- Image #8:
omc_8 = [ NaN ; NaN ; NaN ];
Tc_8  = [ NaN ; NaN ; NaN ];
omc_error_8 = [ NaN ; NaN ; NaN ];
Tc_error_8  = [ NaN ; NaN ; NaN ];

%-- Image #9:
omc_9 = [ 1.842167e+000 ; 2.518381e+000 ; -6.278542e-002 ];
Tc_9  = [ -9.953674e+001 ; -9.719877e+001 ; 4.756337e+002 ];
omc_error_9 = [ 4.296139e-003 ; 6.127216e-003 ; 1.065672e-002 ];
Tc_error_9  = [ 2.814836e+000 ; 2.324943e+000 ; 2.928751e+000 ];

%-- Image #10:
omc_10 = [ -1.658388e+000 ; -2.068270e+000 ; 6.255820e-001 ];
Tc_10  = [ -1.083894e+002 ; -9.821399e+001 ; 4.887963e+002 ];
omc_error_10 = [ 4.801464e-003 ; 4.560929e-003 ; 7.366911e-003 ];
Tc_error_10  = [ 2.902500e+000 ; 2.389150e+000 ; 2.709117e+000 ];

%-- Image #11:
omc_11 = [ -1.537094e+000 ; -2.421034e+000 ; 3.270259e-001 ];
Tc_11  = [ -4.831360e+001 ; -1.234999e+002 ; 6.142159e+002 ];
omc_error_11 = [ 4.209325e-003 ; 5.902305e-003 ; 9.238905e-003 ];
Tc_error_11  = [ 3.627170e+000 ; 2.975531e+000 ; 3.559224e+000 ];

%-- Image #12:
omc_12 = [ -2.180917e+000 ; -1.877941e+000 ; 5.547948e-001 ];
Tc_12  = [ -1.331969e+002 ; -7.130320e+001 ; 6.141649e+002 ];
omc_error_12 = [ 5.522086e-003 ; 3.682527e-003 ; 9.105419e-003 ];
Tc_error_12  = [ 3.634295e+000 ; 2.991943e+000 ; 3.498202e+000 ];

%-- Image #13:
omc_13 = [ 2.595635e+000 ; 1.608845e+000 ; -6.325308e-001 ];
Tc_13  = [ -1.461488e+002 ; -2.624836e+001 ; 6.032739e+002 ];
omc_error_13 = [ 4.184684e-003 ; 4.393478e-003 ; 1.013909e-002 ];
Tc_error_13  = [ 3.561922e+000 ; 2.940067e+000 ; 3.509973e+000 ];

%-- Image #14:
omc_14 = [ 1.703452e+000 ; 2.115003e+000 ; 4.066300e-001 ];
Tc_14  = [ -1.147817e+002 ; -1.030604e+002 ; 4.502242e+002 ];
omc_error_14 = [ 4.225387e-003 ; 4.758645e-003 ; 7.880372e-003 ];
Tc_error_14  = [ 2.698373e+000 ; 2.215965e+000 ; 2.916887e+000 ];

%-- Image #15:
omc_15 = [ NaN ; NaN ; NaN ];
Tc_15  = [ NaN ; NaN ; NaN ];
omc_error_15 = [ NaN ; NaN ; NaN ];
Tc_error_15  = [ NaN ; NaN ; NaN ];

%-- Image #16:
omc_16 = [ NaN ; NaN ; NaN ];
Tc_16  = [ NaN ; NaN ; NaN ];
omc_error_16 = [ NaN ; NaN ; NaN ];
Tc_error_16  = [ NaN ; NaN ; NaN ];

%-- Image #17:
omc_17 = [ -1.873236e+000 ; -2.396962e+000 ; 3.237582e-001 ];
Tc_17  = [ -1.706780e+002 ; -1.347941e+002 ; 6.300327e+002 ];
omc_error_17 = [ 5.564879e-003 ; 5.307172e-003 ; 1.057172e-002 ];
Tc_error_17  = [ 3.759229e+000 ; 3.104359e+000 ; 3.894553e+000 ];

%-- Image #18:
omc_18 = [ -1.682364e+000 ; -2.647256e+000 ; 1.594687e-002 ];
Tc_18  = [ 1.985428e+001 ; -9.152611e+001 ; 6.755202e+002 ];
omc_error_18 = [ 5.817300e-003 ; 1.018106e-002 ; 1.688222e-002 ];
Tc_error_18  = [ 3.987759e+000 ; 3.282043e+000 ; 4.092722e+000 ];

%-- Image #19:
omc_19 = [ 1.627364e+000 ; 2.664306e+000 ; 9.850430e-003 ];
Tc_19  = [ -2.626815e+001 ; -1.519537e+002 ; 6.066015e+002 ];
omc_error_19 = [ 5.199796e-003 ; 8.738949e-003 ; 1.436140e-002 ];
Tc_error_19  = [ 3.605391e+000 ; 2.950219e+000 ; 3.647687e+000 ];

%-- Image #20:
omc_20 = [ -1.946607e+000 ; -2.406698e+000 ; 1.834959e-001 ];
Tc_20  = [ -7.682347e+001 ; -1.020689e+002 ; 6.168672e+002 ];
omc_error_20 = [ 5.894208e-003 ; 7.048243e-003 ; 1.345931e-002 ];
Tc_error_20  = [ 3.637508e+000 ; 2.987830e+000 ; 3.694469e+000 ];

%-- Image #21:
omc_21 = [ -2.156006e+000 ; -2.131515e+000 ; 3.370552e-002 ];
Tc_21  = [ -1.246652e+002 ; -7.373980e+001 ; 5.725578e+002 ];
omc_error_21 = [ 5.874833e-003 ; 5.880159e-003 ; 1.256103e-002 ];
Tc_error_21  = [ 3.377857e+000 ; 2.795998e+000 ; 3.579957e+000 ];

%-- Image #22:
omc_22 = [ -2.325473e+000 ; -2.027745e+000 ; 4.726182e-001 ];
Tc_22  = [ -1.321503e+002 ; -8.179302e+001 ; 6.206985e+002 ];
omc_error_22 = [ 5.958522e-003 ; 3.753923e-003 ; 1.036681e-002 ];
Tc_error_22  = [ 3.667347e+000 ; 3.018972e+000 ; 3.667317e+000 ];

%-- Image #23:
omc_23 = [ NaN ; NaN ; NaN ];
Tc_23  = [ NaN ; NaN ; NaN ];
omc_error_23 = [ NaN ; NaN ; NaN ];
Tc_error_23  = [ NaN ; NaN ; NaN ];

%-- Image #24:
omc_24 = [ 2.519107e+000 ; 1.420599e+000 ; -6.906959e-001 ];
Tc_24  = [ -1.552560e+002 ; 2.461381e+001 ; 6.667560e+002 ];
omc_error_24 = [ 4.433209e-003 ; 4.361362e-003 ; 9.580532e-003 ];
Tc_error_24  = [ 3.931589e+000 ; 3.257739e+000 ; 3.898033e+000 ];

%-- Image #25:
omc_25 = [ 2.296230e+000 ; 1.896306e+000 ; -3.594558e-001 ];
Tc_25  = [ -1.470156e+002 ; -4.414278e+001 ; 5.427647e+002 ];
omc_error_25 = [ 4.388436e-003 ; 4.816878e-003 ; 9.724169e-003 ];
Tc_error_25  = [ 3.195317e+000 ; 2.651313e+000 ; 3.323404e+000 ];

