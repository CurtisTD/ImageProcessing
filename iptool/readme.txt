/*******************
U29806512 Curtis Davis
This Read-me features past information, as well as the added functions,
parameters, and other functionality instructions; 
as to not leave out pertinent instructions.
*******************/

This software architecture is as follows:

../iptool/iptool.cpp- This file includes the main function.

../iptool/iptools -This folder hosts the files that are compiled into a static library. 
	image - This folder hosts the files that define an image.
	utility- this folder hosts the files that students store their implemented algorithms.

*** FUNCTIONS ***

1. Add intensity: add
Increase the intensity for a gray-level image.

2. Binarization: binarize
Binarize the pixels with the threshold.

3. Scaling: Scale
Reduce or expand the height and width with two scale factors.
Scaling factor = 2: double height and width of the input image.
Scaling factor = 0.5: half height and width of the input image.

4. Double Threshold Binarization: doubleThreshold
-This uses two given threshold parameters to binarize the image or ROI.
-The regions of interest(ROI) and two threshold
     parameters are found in the respective 'ROIDoubleThresh.txt' input file.
-This function takes a grayscale image and two thresholds, and binarizes the image,
    except the range to turn a pixel white is between the two given thresholds. The
    pixel is turned black otherwise.

5. Color Binarize+: colorBinarize
-This is a binarization algorithm to be used on colored images.
-The functions takes in an ROI, an RGB value, a threshold value, and a DC value
    from the 'ROIColorBin.txt' file in the input folder.
-The function calculates the euclidean distance from one pixel's color to the given
    RGB color in a 3D sense, and turns pixel black if it is above the threshold.
    Otherwise, the value DC is added to the pixel.

6. Uniform Smoothing: uniformSmooth
-This function has 3 different implementations, which one is chosen in 'parameters.txt'.
-The function takes in a ROI and windowSize parameters from the 'ROISmoothing.txt' input file.
-Each implementation of this function accomplishes the same goal. It smooths an image and its
    sharp details. It uses its neighborhood of pixels, whether it be 2-dimensional or 1-dimensional,
    calculates the average depending on window size, then uses that value as the pixels new value.


/*********New Functions below*********/

7. Gray Image Histogram Stretching
-This function takes a gray image and stretches the intensity 'histogram' of the image.
-This takes in the ROI (4 values), and the user defined min and max values for the new histogram's range.
-This function pulls from the 'ROIGrayHistoStretch.txt' input file and outputs three files:
	1. The histogram of the image before the stretch.
	2. The histogram of the image after the stretch.
	3. The image after the image has been stretched.

8. Optimal Thresholding
-This function implements the 'Optimal Thresholding' algorithm.
-It takes an image and calculates the average value of the image, giving the threshold value.
-It then applies the thresholding algorithm to 'binarize the image' as needed.
-It uses parameters from 'ROIOptimalThresh.txt', but only takes in the ROI(s).

9. Combining Gray Histogram Stretching and Optimal Thresholding
-This function applies both the Optimal Thresholding and Histogram Stretching operations to an image.
-The threshold is defined in the code for this function, and the function only takes the 4 ROI
	parameters from the ROI file 'ROIOptThreshHisto.txt'.
-It has a couple helper functions to ensure repetition is handled.
-This divides the 'foreground' and 'background' regions of an image through optimal thresholding
	and then applies histogram stretching to these regions separately. These images are saved 
	separately in another folder named 'optimalAndStretched' in the output folder for 'buffering'.
	The main combined image of the background and foreground is saved in the main output directory.

/******End of new functions******/



*** PARAMETERS ***

The first parameter of the parameters.txt is the number of operations (lines).
There are four parameters for each operation (line):
1. the input file name;
2. the output file name;
3. the name of the filter. Use "add", "binarize", and "scale" for your filters;
4. the value for adding intensity, threshold value for binarize filter, or the scaling factor for scale filter, or smoothing type.

There are now 'ROI' text files which contain the parameters for the newly implemented functions, located in the input folder.
These can be modified, and the parameters are labeled to be modified in their respective ROI.txt files.
In addition to the ROI files, the input folder also now has a '004.ppm' color image of a plant. This is to be used for
the colorBinarization function.

-ROIDoubleThresh.txt: 6 parameters
    ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI Threshold_1 Threshold_2
    These values specify the ROI and two threshold values for doubleThresh.
-ROIColorBin.txt: 9 parameters
    ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI colorRedValue cBlueValue colGreenValue Threshold DCvalue
    These values specify the ROI, RGB color values, threshold, and dcvalue.
-ROISmoothing.txt: 5 parameters
    ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI WindowSize
    These value specify the ROI and window size for uniformSmoothing.

/*****New parameters and functionality******/

-ROIGrayHistoStretch.txt: 6 parameters
	ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI min_param max_param
	These specify the ROI, max and min intensity values for stretching.
	This is currently set to the entire image of 'slope'. Set other parameter files accordingly.
-ROIOptimalThresh.txt: 4 parameters
	ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI
	These only parameters are to specify the ROI.
-ROIOptimalThreshHisto.txt: 4 parameters
	ROI_x-value ROI_y-value Size_x_ROI Size_y_ROI
	These only parameters are to specify the ROI.

/*****End of new parameters and functionality*****/


*** Run the program:

Open the iptools.sln file.
Directly debug in Visual Studio.
You can find the output image in output folder.