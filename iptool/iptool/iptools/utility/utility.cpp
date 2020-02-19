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
void createHistogram(image &src, int startX, int endX, int startY, int endY, int iteration, bool beforeAfter) {

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
	
	strcpy_s(fileName, 256, "../output/");
	//True = before
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
void utility::grayHistoStretch(image &src, image &tgt) {

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
		createHistogram(src, ghsX, (ghsX + ghsSX), ghsY, (ghsY + ghsSY), i, 1);

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
		std::cout << "*********After Histo of ROI #" << i << "************\n";
		createHistogram(tgt, ghsX, (ghsX + ghsSX), ghsY, (ghsY + ghsSY), i, 0);
	}
}

/*-----------------------------------------------------------------------**/
void utility::optimalThreshGray(image &src, image &tgt, int a, int b) {

}

/*-----------------------------------------------------------------------**/
void colorHistogramStretch(image &src, image &tgt, int a, int b) {


}