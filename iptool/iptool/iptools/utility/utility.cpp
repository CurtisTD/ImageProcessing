#include "utility.h"
#include <fstream>
#include <iostream>
#include <map>


#define MAXRGB 255
#define MINRGB 0

std::string utility::intToString(int number)
{
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

int utility::checkValue(int value)
{
	if (value > MAXRGB)
		return MAXRGB;
	if (value < MINRGB)
		return MINRGB;
	return value;
}

/*-----------------------------------------------------------------------**/
void utility::addGrey(image &src, image &tgt, int value)
{
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	for (int i=0; i<src.getNumberOfRows(); i++)
		for (int j=0; j<src.getNumberOfColumns(); j++)
		{
			tgt.setPixel(i,j,checkValue(src.getPixel(i,j)+value)); 
		}
}

/*-----------------------------------------------------------------------**/
void utility::binarize(image &src, image &tgt, int threshold)
{
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	for (int i=0; i<src.getNumberOfRows(); i++)
	{
		for (int j=0; j<src.getNumberOfColumns(); j++)
		{
			if (src.getPixel(i, j) < threshold)
				tgt.setPixel(i, j, MINRGB);
			else
				tgt.setPixel(i, j, MAXRGB);
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::scale(image &src, image &tgt, float ratio)
{
	int rows = (int)((float)src.getNumberOfRows() * ratio);
	int cols  = (int)((float)src.getNumberOfColumns() * ratio);
	tgt.resize(rows, cols);
	for (int i=0; i<rows; i++)
	{
		for (int j=0; j<cols; j++)
		{	
			/* Map the pixel of new image back to original image */
			int i2 = (int)floor((float)i/ratio);
			int j2 = (int)floor((float)j/ratio);
			if (ratio == 2) {
				/* Directly copy the value */
				tgt.setPixel(i,j,checkValue(src.getPixel(i2,j2)));
			}

			if (ratio == 0.5) {
				/* Average the values of four pixels */
				int value = src.getPixel(i2,j2) + src.getPixel(i2,j2+1) + src.getPixel(i2+1,j2) + src.getPixel(i2+1,j2+1);
				tgt.setPixel(i,j,checkValue(value/4));
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::doubleThreshold(image &src, image &tgt) {

	/* Opens the Double Threshold ROI file to get the regions & params */
	ifstream dtROIFile("../input/ROIDoubleThresh.txt");
	if (!dtROIFile.is_open()) {
		fprintf(stderr, "Can't open Double Threshold ROI file:\n");
	}
	int numROI; //number of doubleThreshold ROIs	
	dtROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);
	//For number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {
		/************************************************************************************/
		/* These parameters are obtained by their respective inputted ROI file              */
		/* These parameters are defined by the user in said files, and can be changed there.*/
		/************************************************************************************/
		int dtX, dtY, dtSX, dtSY, dtT1, dtT2; //Parameters of ROIs for intensity operations
		dtROIFile >> dtX >> dtY >> dtSX >> dtSY >> dtT1 >> dtT2;		

		//Double for loop - go through each ROI pixel and change pixel depending on thresholds
		for (int i = dtY; i < (dtY + dtSY); i++) {
			for (int j = dtX; j < (dtX + dtSX); j++) {
				if (src.getPixel(i, j) >= dtT1 && src.getPixel(i, j) <= dtT2) {
					tgt.setPixel(i, j, MAXRGB); //Set pixel to white
				} else {
					tgt.setPixel(i, j, MINRGB); //Set pixel to black
				}				
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::colorBinarize(image &src, image &tgt) {
	/* Opens the Double Threshold ROI file to get the regions & params */
	ifstream cbROIFile("../input/ROIColorBin.txt");
	if (!cbROIFile.is_open()) {
		fprintf(stderr, "Can't open ColorBin ROI file:\n");
	}
	int numROI; //number of ROIs	
	cbROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);
	//For number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {
		/************************************************************************************/
		/* These parameters are obtained by their respective input ROI file                 */
		/* These parameters are defined by the user in said files, and can be changed there.*/
		/************************************************************************************/
		int cbX, cbY, cbSX, cbSY; //Parameters of ROI
		int cR, cG, cB, tC, dC; //Three Colors, threshold, and intensity
		int newRed, newGreen, newBlue; //New channels when adding dC value to pixels
		cbROIFile >> cbX >> cbY >> cbSX >> cbSY >> cR >> cG >> cB >> tC >> dC;
		//Double for loop - go through each ROI pixel and change pixel depending on thresholds
		for (int i = cbY; i < (cbY + cbSY); i++) {
			for (int j = cbX; j < (cbX + cbSX); j++) {

				/*Calculates euclidean distance to color C in RGB*/				
				int eucDist = sqrt(
					pow((src.getPixel(i, j, RED) - cR), 2) +
					pow((src.getPixel(i, j, GREEN) - cG), 2) +
					pow((src.getPixel(i, j, BLUE) - cB), 2)
				);

				if (eucDist > tC) {
					tgt.setPixel(i, j, RED, MINRGB); //Set pixel to black if euclidean distance is < tC
					tgt.setPixel(i, j, GREEN, MINRGB);
					tgt.setPixel(i, j, BLUE, MINRGB);
				} else {
					newRed = (src.getPixel(i, j, RED) + dC);
					newGreen = (src.getPixel(i, j, GREEN) + dC);
					newBlue = (src.getPixel(i, j, BLUE) + dC);
					tgt.setPixel(i, j, RED, newRed);  //Add dC to all three channels of the pixel
					tgt.setPixel(i, j, GREEN, newGreen);
					tgt.setPixel(i, j, BLUE, newBlue);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::uniformSmooth(image &src, image &tgt, int smoothType) {
	/* Opens the Double Threshold ROI file to get the regions & params */
	ifstream smROIFile("../input/ROISmoothing.txt");
	if (!smROIFile.is_open()) {
		fprintf(stderr, "Can't open Smoothing ROI file:\n");
	}
	int numROI; //number of ROIs	
	smROIFile >> numROI;

	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	for (int k = 0; k < numROI; k++) {
		/************************************************************************************/
		/* These parameters are obtained by their respective input ROI file                 */
		/* These parameters are defined by the user in said files, and can be changed there.*/
		/************************************************************************************/
		int smX, smY, smSX, smSY, windowSize; //Parameters of ROIs for intensity operations
		smROIFile >> smX >> smY >> smSX >> smSY >> windowSize;

		/*2D Uniform smoothing*/
		if (smoothType == 2) { 
			for (int j = smY; j < (smY + smSY); j++) { //For each pixel
				for (int i = smX; i < (smX + smSX); i++) { //For each pixel cont.
					/* Calculate the neighboring average and use it for the pixel value */
					int sum = 0;
					int avg;
					for (int x = (i - (windowSize / 2)); x <= (i + (windowSize / 2)); ++x) {
						for (int y = (j - (windowSize / 2)); y <= (j + (windowSize / 2)); ++y) {
							if (x >= 0 && y >= 0) { //Doesnt allow for off the edge of the picture
								sum += src.getPixel(x, y);
							}
							else {
								sum = +0;
							}
						}
					}
					avg = sum / (windowSize * windowSize);
					tgt.setPixel(i, j, avg); //Set target pixel to average of those around it
				}
			}
		}
		/*1D Uniform Smoothing*/
		else if (smoothType == 1) { 
			int xSum = 0, ySum = 0, xAvg, yAvg;
			/*Horizontal Smoothing Pass*/
			for (int i = smX; i < (smX + smSX); i++) { //For each row
				for (int j = smY; j < (smY + smSY); j++) { //For each column		
					/*Horizontal Window Portion*/
					for (int x = (i - (windowSize / 2)); x <= (i + (windowSize / 2)); ++x) {
						if (x >= 0 && x < src.getNumberOfColumns()) {
							xSum += src.getPixel(x, j); //Adds up pixels next to (i,j) to get horizontal avg
						}
						else { //Off the edge pixels
							xSum += 0;
						}
					}
					xAvg = xSum / windowSize;
					tgt.setPixel(i, j, xAvg); //Change pixel to new average
					xSum = xAvg = 0; //Reset
				}
			}
			/*Vertical Smoothing Pass*/
			for (int j = smY; j < (smY + smSY); j++) { //For each column
				for (int i = smX; i < (smX + smSX); i++) { //For each row
					/*Vertical Window Portion*/
					for (int y = (j - (windowSize / 2)); y <= (j + (windowSize / 2)); ++y) {
						if (y >= 0 && y < src.getNumberOfRows()) {
							ySum += tgt.getPixel(i, y);
						}
						else { //Off the edge pixels
							ySum += 0;
						}
					}
					yAvg = ySum / windowSize;
					tgt.setPixel(i, j, yAvg); //Change pixel to new average
					ySum = yAvg = 0; //Reset
				}
			}
		}
		/*Incremental Smoothing*/
		else if(smoothType == 0) { 
			int xCurrSum = 0, yCurrSum = 0, xPrevSum = 0, yPrevSum = 0;
			int xAvg = 0, yAvg = 0;
			/*Horizontal Smoothing Pass*/
			for (int i = smX; i < (smX + smSX); i++) { //For each row
				for (int j = smY; j < (smY + smSY); j++) { //For each column		
					/*Horiz*/
					for (int x = (i - (windowSize / 2)); x <= (i + (windowSize / 2)); ++x) {
						if (x >= 0 && x < src.getNumberOfColumns()) {
							xCurrSum += src.getPixel(x, j);
						}
						else { //Off the edge pixels
							xCurrSum += 0;
						}
					}
					xAvg = xCurrSum / windowSize;
					tgt.setPixel(i, j, xAvg); //Change pixel
					xCurrSum = xAvg = 0; //Reset
				}
			}
			/*Vertical Smoothing Pass*/
			for (int j = smY; j < (smY + smSY); j++) { //For each column
				for (int i = smX; i < (smX + smSX); i++) { //For each row
					/*Vert*/
					for (int y = (j - (windowSize / 2)); y <= (j + (windowSize / 2)); ++y) {
						if (y >= 0 && y < src.getNumberOfRows()) {
							yCurrSum += tgt.getPixel(i, y);
						}
						else { //Off the edge pixels
							yCurrSum += 0;
						}
					}
					yAvg = yCurrSum / windowSize;
					tgt.setPixel(i, j, yAvg); //Change pixel
					yCurrSum = yAvg = 0; //Reset
				}
			}
		}
		else {
			std::cout << "Not a valid smoothing type for this call." << std::endl;
		}
	}
}


/*-----------------------------------------------------------------------**/
/*Helper function for getting intensity*/
double getColorIntensity(image &src, int x, int y) {
	double red_ch, green_ch, blue_ch;

	red_ch = src.getPixel(x, y, RED);
	green_ch = src.getPixel(x, y, GREEN);
	blue_ch = src.getPixel(x, y, BLUE);

	return (red_ch + green_ch + blue_ch) / (3 * 255); //Intensity calculation [0,1]
	
}
/*Helper function to generate a hisotgram*/
void createHistogram(image &src, int startX, int endX, int startY, int endY, int iteration, bool beforeAfter, char outfile[]) {

	int pixelIntensityCounts[256];

	int x = startX;
	int y = startY;
	int pixelIntensity;
	//Traverse the given ROI parameters
	for (x = startX; x < endX; x++) {
		for (y = startY; y < endY; y++) {
			pixelIntensity = src.getPixel(x, y);
			++pixelIntensityCounts[pixelIntensity]; //Increments the count of a pixel value
		}
	}
	//Get min/max counts of intensitites
	int minCount = pixelIntensityCounts[0];
	int maxCount = pixelIntensityCounts[0];
	for (int i = 0; i < 256; i++) {
		if (pixelIntensityCounts[i] > maxCount) {
			maxCount = pixelIntensityCounts[i];
		}
		if (pixelIntensityCounts[i] < minCount) {
			minCount = pixelIntensityCounts[i];
		}
	}

	//Normalize counts
	for (int l = 0; l < 256; l++) {
		pixelIntensityCounts[l] = ( (pixelIntensityCounts[l] - minCount) * 255 ) / (maxCount - minCount);
	}

	image histogram;
	histogram.resize(256, 256);

	//Creates a new file name
	std::string fileName;
	std::string s(outfile);
	std::string histoFile = s.substr(s.find_last_of("/") + 1);
	histoFile = histoFile.substr(0, histoFile.find(".", 0));

	//Create an output file for the histogram, named as a histogram by ROI
	fileName += "../output/";
	fileName += histoFile; //Puts src image name
	if (beforeAfter) {
		fileName += "ImgBefore";
	}
	else {
		fileName += "ImgAfter";
	}
	fileName += "_Histogram-ROI";
	fileName += to_string(iteration); //Puts ROI Iteration
	fileName += ".pgm";
	
	//Traverse the count of pixels and print them to the image
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < pixelIntensityCounts[x]; y++) {
			//At column number (x)
			histogram.setPixel(255-y, x, 255);
		}
	}
	histogram.save(fileName.c_str()); //Save output histogram
}
/*Main Gray Histogram Stretch*/
void utility::grayHistoStretch(image &src, image &tgt, char outfile[]) {

	/* Opens the Histogram Stretch ROI file to get the regions & params */
	ifstream ghsROIFile("../input/ROIGrayHistoStretch.txt");
	if (!ghsROIFile.is_open()) {
		fprintf(stderr, "Can't open Histogram Stretch ROI file:\n");
	}
	int numROI; //number of doubleThreshold ROIs	
	ghsROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	//For the number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {

		int ghsX, ghsY, ghsSX, ghsSY, givenMin, givenMax; //Parameters of ROIs for operations
		ghsROIFile >> ghsX >> ghsY >> ghsSX >> ghsSY >> givenMin >> givenMax;

		int intensity = 0;
		int maxIntensity = src.getPixel(1, 1); 
		int minIntensity = src.getPixel(1, 1);

		int pixelIntensityCounts[256];
		//First traversal of image to get min/max values and pixel intensity counts
		for (int x = ghsX; x < (ghsX + ghsSX); x++) {
			for (int y = ghsY; y < (ghsY + ghsSY); y++) {
				intensity = src.getPixel(x, y);
				pixelIntensityCounts[intensity]++;

				if (intensity > maxIntensity) {
					maxIntensity = intensity;
				}
				if (intensity < minIntensity) {
					minIntensity = intensity;
				}

			}
		}	

		/**Generate Histogram of ROI before (the source image ROI)**/
		createHistogram(src, ghsX, (ghsX + ghsSX), ghsY, (ghsY + ghsSY), i, 1, outfile);

		//Traversal to get new intensities
		int newIntensity;
		int b = 256; //Range for histogram stretching
		int a = 0; //Begin of range for histogram stretch
		for (int x = ghsX; x < (ghsX + ghsSX); x++) {
			for (int y = ghsY; y < (ghsY + ghsSY); y++) {
				//New intensity calculation
				if (src.getPixel(x, y) >= givenMin && src.getPixel(x, y) <= givenMax) {
					newIntensity = ((src.getPixel(x, y) - givenMin) * ((b - a) / (givenMax - givenMin)) + a);
				}
				else if (src.getPixel(x, y) < minIntensity) {
					newIntensity = 0;
				}
				else if (src.getPixel(x, y) > maxIntensity) {
					newIntensity = 255;
				}

				//Set new value
				tgt.setPixel(x, y, newIntensity); 
			}
		}

		/**Generate Histogram of ROI after (the target image)**/
		createHistogram(tgt, ghsX, (ghsX + ghsSX), ghsY, (ghsY + ghsSY), i, 0, outfile);
	}

	//End of GrayHistoStretch function
}

/*-----------------------------------------------------------------------**/
void utility::optimalThreshGray(image &src, image &tgt) {
	
	//Opens the Optimal Thresh ROI file to get the regions & params
	ifstream otgROIFile("../input/ROIOptimalThresh.txt");
	if (!otgROIFile.is_open()) {
		fprintf(stderr, "Can't open Optimal Thresh Gray Image ROI file:\n");
	}
	int numROI; //Number of doubleThreshold ROIs	
	otgROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	
	//For the number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {
		//Vars used to calculate avg value of original image -> Initial threshold value
		int sumValue = 0;
		int avgValue = 0;

		int otgX, otgY, otgSX, otgSY; //Parameters of ROIs for operations
		otgROIFile >> otgX >> otgY >> otgSX >> otgSY;

		int count = 0;
		//For the ROI, find avg value
		for (int x = otgX; x < (otgX + otgSX); x++) {
			for (int y = otgY; y < (otgY + otgSY); y++) {
				sumValue += tgt.getPixel(x, y);
				count++;
			}
		}
		avgValue = sumValue / count; //Avg value of ROI -> This is the initial threshold
		int initialThresh = avgValue;

		//Variable initilization
		int sumBack, sumFore, avgBack, avgFore, countBack, countFore;
		sumBack = sumFore = avgBack = avgFore = countBack = countFore = 0;
		int pixVal;
		//For the ROI, find background are foreground
		for (int x = otgX; x < (otgX + otgSX); x++) {
			for (int y = otgY; y < (otgY + otgSY); y++) {
				pixVal = tgt.getPixel(x, y);
				if (pixVal <= avgValue) {
					sumBack += pixVal;
					countBack++;
				} else {
					sumFore += pixVal;
					countFore++;
				}
			}
		}
		avgBack = sumBack / countBack;
		avgFore = sumFore / countFore;
		int newThreshold = (avgBack + avgFore) / 2;

		/********************THRESHOLD LIMIT*****************************/
		int threshDeltaLimit = 1; //Limit for threshold
		if (abs(initialThresh - newThreshold) >= threshDeltaLimit) {
			//Binarize as needed if difference is too big
			for (int x = otgX; x < (otgX + otgSX); x++) {
				for (int y = otgY; y < (otgY + otgSY); y++) {
					if (src.getPixel(x, y) <= newThreshold) {
						//Background set to black
						tgt.setPixel(x, y, MINRGB);
					}
					else {
						//Foreground set to white
						tgt.setPixel(x, y, MAXRGB);
					}
				}
			}

		} else {
			continue;
		}
	}
}

/*-----------------------------------------------------------------------**/
//Helper histoStretch function to the function of Combined Optimal Thresholding and Histogram Stretching
void histoStretch(image &src, image &tgt, int othsX, int othsY, int othsSX, int othsSY) {

	int intensity = 0;
	int newMaxIntensity = 0; //Set to 'impossible' max value
	int newMinIntensity = 255;
	//Traversal of image to new get min/max values after optimal Thresholding
	for (int x = othsX; x < (othsX + othsSX); x++) {
		for (int y = othsY; y < (othsY + othsSY); y++) {
			intensity = tgt.getPixel(x, y);
			if (intensity <= 0) {
				//Skip black pixels (not in region)
			}
			else if (intensity > newMaxIntensity) {
				newMaxIntensity = intensity;
			}
			else if (intensity < newMinIntensity) {
				newMinIntensity = intensity;
			}
		}
	}

	//Traversal to get new intensities
	int currIntensity, newIntensity;
	double a = newMinIntensity; //Min Intensity of this ROI
	double b = newMaxIntensity; //Max intensity of this ROI
	double c = 0; //Original min
	double d = 255; //Original max

	for (int x = othsX; x < (othsX + othsSX); x++) {
		for (int y = othsY; y < (othsY + othsSY); y++) {
			currIntensity = tgt.getPixel(x, y);
			//If current intensity if within old range, change them to new range
			newIntensity = ((currIntensity - a) * ((d - c) / (b - a))) + c; //Has to use float values

			if (currIntensity < a) { //If curr intensity below new min, set to 0
				newIntensity = 0;
			}
			else if (currIntensity > b) { //If above new max, set to 255
				newIntensity = 255;
			}

			if (currIntensity != 0) {
				//Set new value if current pixel isn't black
				tgt.setPixel(x, y, newIntensity); //only stretch the intended values
			}			
		}
	}

} //End of helper histo stretch function
/*Main Combined Optimal Thresh and Histo Stretch Function*/
void utility::optimalThresh_HistoStretch(image &src, image &tgt) {

	//Opens the Histogram Stretch ROI file to get the regions & params
	ifstream ot_hsROIFile("../input/ROIOptThreshHisto.txt");
	if (!ot_hsROIFile.is_open()) {
		fprintf(stderr, "Can't open Optimal Thresh & Histo Stretch ROI file:\n");
	}
	int numROI; //Number of doubleThreshold ROIs	
	ot_hsROIFile >> numROI;
	
	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	//Create separate images for background and foreground
	image foregroundImg, backgroundImg;
	foregroundImg.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
	foregroundImg.copyImage(tgt);
	backgroundImg.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
	backgroundImg.copyImage(tgt);

	//For the number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {
		//Vars used to calculate avg value of original image -> Initial threshold value
		int sumValue = 0;
		int avgValue = 0;

		int othsX, othsY, othsSX, othsSY; //Parameters of ROIs for operations
		ot_hsROIFile >> othsX >> othsY >> othsSX >> othsSY;

		/********************************************************/
		/*******************Optimal Thesholding******************/
		/********************************************************/
		int count = 0;
		//For the ROI, find avg value
		for (int x = othsX; x < (othsX + othsSX); x++) {
			for (int y = othsY; y < (othsY + othsSY); y++) {
				sumValue += tgt.getPixel(x, y);
				count++;
			}
		}
		avgValue = sumValue / count; //Avg value of ROI -> This is the initial threshold
		int initialThresh = avgValue;

		//Variable initilization
		int sumBack, sumFore, avgBack, avgFore, countBack, countFore;
		sumBack = sumFore = avgBack = avgFore = countBack = countFore = 0;
		int pixVal;

		//For the ROI, find background are foreground
		for (int x = othsX; x < (othsX + othsSX); x++) {
			for (int y = othsY; y < (othsY + othsSY); y++) {
				pixVal = tgt.getPixel(x, y);
				if (pixVal <= avgValue) {
					sumBack += pixVal;
					countBack++;
				}
				else {
					sumFore += pixVal;
					countFore++;
				}
			}
		}
		avgBack = sumBack / countBack;
		avgFore = sumFore / countFore;
		int newThreshold = (avgBack + avgFore) / 2;

		int threshDeltaLimit = 0; //Limit for threshold
		if (abs(initialThresh - newThreshold) >= threshDeltaLimit) {
			//Binarize as needed if difference is too big
			for (int x = othsX; x < (othsX + othsSX); x++) {
				for (int y = othsY; y < (othsY + othsSY); y++) {
					if (src.getPixel(x, y) <= newThreshold) {
						//Foreground image shows foreground in black pixels also
						foregroundImg.setPixel(x, y, MINRGB);						
					}
					else {
						//Background image shows background in black pixels
						backgroundImg.setPixel(x, y, MINRGB);
					}
				}
			}
		}
		//Save the images of background and foreground, both represented by black pixels in the image
		foregroundImg.save("../output/optimalAndStretched/black_foreground_img.pgm");
		backgroundImg.save("../output/optimalAndStretched/black_background_img.pgm");

		////////////////////////////////////////////////////////////////////////
		/********tgt now has had optimal thresholding done to it***************/
		////////////////////////////////////////////////////////////////////////


		/**********************************************************************/
		/******************Historgram Stretching for same ROI******************/
		/**********************************************************************/
		//Create new images to be stretched versions of the thresholded background 
		//and foreground images
		image foregroundStretched, backgroundStretched;
		foregroundStretched.resize(foregroundImg.getNumberOfRows(), foregroundImg.getNumberOfColumns());
		foregroundStretched.copyImage(foregroundImg);
		backgroundStretched.resize(backgroundImg.getNumberOfRows(), backgroundImg.getNumberOfColumns());
		backgroundStretched.copyImage(backgroundImg);

		//Stretch the respecctive regions
		histoStretch(src, foregroundStretched, othsX, othsY, othsSX, othsSY);
		histoStretch(src, backgroundStretched, othsX, othsY, othsSX, othsSY);

		foregroundStretched.save("../output/optimalAndStretched/stretched_foreground_img.pgm");
		backgroundStretched.save("../output/optimalAndStretched/stretched_background_img.pgm");

		tgt.copyImage(foregroundStretched); //Copy one image to start

		//Change final output file to what's needed
		int copyIntensity;
		for (int x = othsX; x < (othsX + othsSX); x++) {
			for (int y = othsY; y < (othsY + othsSY); y++) {
				copyIntensity = backgroundStretched.getPixel(x, y);
				if (copyIntensity > 0) {
					tgt.setPixel(x, y, copyIntensity);
				}
			}
		}
	}
}

/*-----------------------------------------------------------------------**/
void utility::sobelEdgeDetectGray(image &src, image &tgt, int threshold) {
	//Opens the Optimal Thresh ROI file to get the regions & params
	ifstream sedgROIFile("../input/ROIEdgeDetect.txt");
	if (!sedgROIFile.is_open()) {
		fprintf(stderr, "Can't open Sobel Edge Detect Gray ROI file:\n");
	}
	int numROI; //Number of doubleThreshold ROIs	
	sedgROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	image img2d;
	img2d.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
	img2d.copyImage(tgt);

	//Image horizontal orig
	image img2dhororg;
	img2dhororg.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());

	//Image vertical orig
	image img2dverorg;
	img2dverorg.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());

	//Image magnitude
	image img2damp;
	img2damp.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
	
	//For the number of ROIs, do the operation on the image
	for (int i = 0; i < numROI; i++) {
		std::cout << "\nROI num: " << i+1 << std::endl;
		int dtX, dtY, dtSX, dtSY; //Parameters of ROIs for intensity operations
		sedgROIFile >> dtX >> dtY >> dtSX >> dtSY;
		
		const int height = (dtY + dtSY);
		const int width = (dtX + dtSX);

		//Horizontal operations
		std::cout << "\tDoing horoizontal operations..." << std::endl;
		img2dhororg.copyImage(tgt); //Initially copy the image
		int max = -999, min = 256;
		for (int i = dtY + 1; i < height - 1; i++) {
			for (int j = dtX + 1; j < width - 1; j++) {
				int curr = img2d.getPixel(i - 1, j - 1) + 2 * img2d.getPixel(i - 1, j) + img2d.getPixel(i - 1, j + 1) - img2d.getPixel(i + 1, j - 1)
					- 2 * img2d.getPixel(i + 1, j) - img2d.getPixel(i + 1, j + 1);
				img2dhororg.setPixel(i, j, curr); //Set new value on original horizontal
				if (curr > max) {
					max = curr;
				}
				if (curr < min) {
					min = curr;
				}
			}
		}
		std::cout << "\tEnding horizontal operations..." << std::endl;

		//Vertical operations
		std::cout << "\tDoing vertical operations..." << std::endl;
		img2dverorg.copyImage(img2dhororg);
		max = -999; min = 256;
		for (int i = dtY + 1; i < height - 1; i++) {
			for (int j = dtX + 1; j < width - 1; j++) {
				int curr = img2d.getPixel(i - 1, j - 1) + 2 * img2d.getPixel(i, j - 1) + img2d.getPixel(i + 1, j - 1)
					- img2d.getPixel(i - 1, j + 1) - 2 * img2d.getPixel(i, j + 1) - img2d.getPixel(i + 1, j + 1);
				img2dverorg.setPixel(i, j, curr);
				if (curr > max) {
					max = curr;
				}
				if (curr < min) {
					min = curr;
				}
			}
		}
		std::cout << "\tEnding vertical operations..." << std::endl;

		//Operations of the amplitude
		std::cout << "\tDoing mag operations..." << std::endl;
		img2damp.copyImage(img2dverorg);
		max = -999; min = 256;
		for (int i = dtY; i < height; i++) {
			for (int j = dtX; j < width; j++) {
				img2damp.setPixel(i, j, sqrt(pow(img2dhororg.getPixel(i, j), 2) + pow(img2dverorg.getPixel(i, j), 2)));
				if (img2damp.getPixel(i, j) > max) {
					max = img2damp.getPixel(i, j);
				}

				if (img2damp.getPixel(i, j) < min) {
					min = img2damp.getPixel(i, j);
				}
			}
		}

		int diff = max - min; //Get difference
		for (int i = dtY; i < height; i++) {
			for (int j = dtX; j < width; j++) {
				float abc = (img2damp.getPixel(i, j) - min) / (diff*1.0);
				img2damp.setPixel(i, j, abc * 255);
			}
		}

		for (int i = dtY; i < height; i++) {
			for (int j = dtX; j < width; j++) {
				tgt.setPixel(i, j, RED, img2damp.getPixel(i, j));
				tgt.setPixel(i, j, GREEN, img2damp.getPixel(i, j));
				tgt.setPixel(i, j, BLUE, img2damp.getPixel(i, j));
			}
		}
		std::cout << "\tEnding magnitude operations..." << std::endl;

		//Binarize the image
		std::cout << "\tDoing binarizing operations..." << std::endl;
		for (int i = dtY; i < height - 1; i++) {
			for (int j = dtX; j < width - 1; j++) {
				if (img2damp.getPixel(i, j) < threshold) {
					tgt.setPixel(i, j, RED, MINRGB);
					tgt.setPixel(i, j, GREEN, MINRGB);
					tgt.setPixel(i, j, BLUE, MINRGB);
				}
				else {
					tgt.setPixel(i, j, RED, MAXRGB);
					tgt.setPixel(i, j, GREEN, MAXRGB);
					tgt.setPixel(i, j, BLUE, MAXRGB);
				}
			}
		}
		std::cout << "\tEnding binarizing operations..." << std::endl << std::endl;
	}
}


/*--------------------------------------------------------------------*/
void utility::edgeDetectColor(image &src, image &tgt) {
	//Opens the Optimal Thresh ROI file to get the regions & params
	ifstream sedgROIFile("../input/ROIEdgeDetect.txt");
	if (!sedgROIFile.is_open()) {
		fprintf(stderr, "Can't open Edge Detect Color ROI file:\n");
	}
	int numROI; //Number of doubleThreshold ROIs	
	sedgROIFile >> numROI;

	//Initially copy the image
	tgt.resize(src.getNumberOfRows(), src.getNumberOfColumns());
	tgt.copyImage(src);

	int threshold;
	std::cout << "\nEnter a threshold to be used for the \"Color Edge Detection\" algorithm: ";
	std::cin >> threshold;

	for (int i = 0; i < numROI; i++) {
		std::cout << "\nROI num: " << i + 1 << std::endl;
		int dtX, dtY, dtSX, dtSY; //Parameters of ROIs for intensity operations
		sedgROIFile >> dtX >> dtY >> dtSX >> dtSY;

		const int height = (dtY + dtSY);
		const int width = (dtX + dtSX);

		//Red channel image
		image redChannelSrc;
		image redEdge;
		redChannelSrc.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		redEdge.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		for (int i = dtY; i < height - 1; i++) {
			for (int j = dtX; j < width - 1; j++) {
				redChannelSrc.setPixel(i, j, RED, tgt.getPixel(i, j, RED));
				redChannelSrc.setPixel(i, j, GREEN, tgt.getPixel(i, j, RED));
				redChannelSrc.setPixel(i, j, BLUE, tgt.getPixel(i, j, RED));
			}
		}

		//Green channel image
		image greenChannelSrc;
		image greenEdge;
		greenChannelSrc.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		greenEdge.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		for (int i = dtY; i < height - 1; i++) {
			for (int j = dtX; j < width - 1; j++) {
				greenChannelSrc.setPixel(i, j, RED, tgt.getPixel(i, j, GREEN));
				greenChannelSrc.setPixel(i, j, GREEN, tgt.getPixel(i, j, GREEN));
				greenChannelSrc.setPixel(i, j, BLUE, tgt.getPixel(i, j, GREEN));
			}
		}

		//Blue channel image
		image blueChannelSrc;
		image blueEdge;
		blueChannelSrc.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		blueEdge.resize(tgt.getNumberOfRows(), tgt.getNumberOfColumns());
		for (int i = dtY; i < height - 1; i++) {
			for (int j = dtX; j < width - 1; j++) {
				blueChannelSrc.setPixel(i, j, RED, tgt.getPixel(i, j, BLUE));
				blueChannelSrc.setPixel(i, j, GREEN, tgt.getPixel(i, j, BLUE));
				blueChannelSrc.setPixel(i, j, BLUE, tgt.getPixel(i, j, BLUE));
			}
		}

		sobelEdgeDetectGray(redChannelSrc, redEdge, threshold);
		sobelEdgeDetectGray(greenChannelSrc, greenEdge, threshold);
		sobelEdgeDetectGray(blueChannelSrc, blueEdge, threshold);
		
		//Saves the images with just each color channel
		redChannelSrc.save("../output/redChanelSrc.ppm");
		greenChannelSrc.save("../output/greenChanelSrc.ppm");
		blueChannelSrc.save("../output/blueChannelSrc.ppm");

		redEdge.save("../output/redEdge.ppm");
		greenEdge.save("../output/greenEdge.ppm");
		blueEdge.save("../output/blueEdge.ppm");
		//All edge images have been binarized by this point from the sobel algorithm 
		//The images now need to be combined

		
		//Combine the images back
		for (int i = dtY; i < height - 1; i++) {
			for (int j = dtX; j < width - 1; j++) {
				if (redEdge.getPixel(i, j, RED) == MAXRGB || greenEdge.getPixel(i, j, GREEN) == MAXRGB || blueEdge.getPixel(i, j, BLUE) == MAXRGB) {
					tgt.setPixel(i, j, RED, MAXRGB);
					tgt.setPixel(i, j, GREEN, MAXRGB);
					tgt.setPixel(i, j, BLUE, MAXRGB);
					
				}
				else {
					tgt.setPixel(i, j, RED, MINRGB);
					tgt.setPixel(i, j, GREEN, MINRGB);
					tgt.setPixel(i, j, BLUE, MINRGB);	
				}
			}
		}
	}
}