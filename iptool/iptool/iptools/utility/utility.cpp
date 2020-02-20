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
			if (src.getPixel(i,j) < threshold)
				tgt.setPixel(i,j,MINRGB);
			else
				tgt.setPixel(i,j,MAXRGB);
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
	char fileName[256];
	char stringIteration[256];
	
	//Create an output file for the histogram, named as a histogram by ROI
	strcpy_s(fileName, 256, "../output/");
	//True = before
	strcat_s(fileName, 256, outfile);
	if (beforeAfter) {
		strcat_s(fileName, 256, "ImgBefore");
	}
	else {
		strcat_s(fileName, 256, "ImgAfter");
	}
	strcat_s(fileName, 256, "_Histogram-ROI");
	sprintf_s(stringIteration, "%d", iteration);
	strcat_s(fileName, 256, stringIteration);
	strcat_s(fileName, 256, ".pgm");

	//Traverse the count of pixels and print them to the image
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < pixelIntensityCounts[x]; y++) {
			//At column number (x)
			histogram.setPixel(255-y, x, 255);
		}
	}
	histogram.save(fileName); //Save output histogram
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
void histoStretch(image &tgt, int othsX, int othsY, int othsSX, int othsSY, int origMin, int origMax) {
	
	int intensity = 0;
	int newMaxIntensity = tgt.getPixel(1, 1);
	int newMinIntensity = tgt.getPixel(1, 1);

	//Traversal of image to new get min/max values after optimal Thresholding
	for (int x = othsX; x < (othsX + othsSX); x++) {
		for (int y = othsY; y < (othsY + othsSY); y++) {
			intensity = tgt.getPixel(x, y);

			if (intensity > newMaxIntensity) {
				newMaxIntensity = intensity;
			}
			if (intensity < newMinIntensity) {
				newMinIntensity = intensity;
			}
		}
	}

	//Traversal to get new intensities
	int currIntensity, newIntensity;
	double a = newMinIntensity; //Min Intensity of this ROI
	double b = newMaxIntensity; //Max intensity of this ROI
	double c = origMin;
	double d = origMax;
	for (int x = othsX; x < (othsX + othsSX); x++) {
		for (int y = othsY; y < (othsY + othsSY); y++) {
			currIntensity = tgt.getPixel(x, y);

			if (currIntensity >= c && currIntensity <= d) {
				newIntensity = ( (currIntensity - c) * ((b - a) / (d - c)) ) + a; //Has to use float values
/*
std::cout << "a,b,c,d " << a << "," << b << "," << c << "," << d << ": " << currIntensity << "--> New intensity: " << 
	"( (" << currIntensity << " - " << c << " ) * ((" << b << " - " << a << ") / (" << d << " - " << c << ")) ) + " << a <<
	" = " << newIntensity << std::endl;
*/
				//Cap values
				if (newIntensity > 255) { newIntensity = 255; }
				if (newIntensity < 0) { newIntensity = 0; }
			}
			else if (currIntensity <= a) {
				newIntensity = 0;
			}
			else if (currIntensity >= b) {
				newIntensity = 255;
			}

			//Set new value
			tgt.setPixel(x, y, newIntensity);
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

		int threshDeltaLimit = 1; //Limit for threshold
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
//		foregroundImg.save("../output/optimalAndStretched/black_foreground_img.pgm");
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

		//Get min and max intensities of original image; 'C' and 'D'
		int origMin, origMax;
		origMin = origMax = src.getPixel(0, 0);
		for (int x = othsX; x < (othsX + othsSX); x++) {
			for (int y = othsY; y < (othsY + othsSY); y++) {
				int currInten = src.getPixel(x, y);
				if (origMax < currInten) { origMax = currInten; }
				if (origMin > currInten) { origMin = currInten; }
			}
		}

		histoStretch(foregroundStretched, othsX, othsY, othsSX, othsSY, origMin, origMax);
		histoStretch(backgroundStretched, othsX, othsY, othsSX, othsSY, origMin, origMax);

//		foregroundStretched.save("../output/optimalAndStretched/stretched_foreground_img.pgm");
		backgroundStretched.save("../output/optimalAndStretched/stretched_background_img.pgm");
	}
}

/*-----------------------------------------------------------------------**/
void utility::colorHistogramStretch(image &src, image &tgt, int a, int b) {

	
}