/************************************************************/

/************************************************************/

#include "./iptools/core.h"
#include <string.h>
#include <fstream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>

using namespace std;

#define MAXLEN 256

int main (int argc, char** argv)
{
	image src, tgt;
	ifstream fp(argv[1]);
	char str[MAXLEN];
	rsize_t strmax = sizeof str;
	char outfile[MAXLEN];
	char *pch, *next_pch;
	int nOP;
	if (!fp.is_open()) {
		fprintf(stderr, "Can't open file: %s\n", argv[1]);
		exit(1);
	}

	
	fp >> nOP;
	for (int OP = 0; OP < nOP; OP++) {
		fp >> str;
		src.read(str); //Gets in file

		fp >> str;
		strcpy_s(outfile, MAXLEN, str); // Makes outfile string

		fp >> str;
        if (strncmp(str,"add",3)==0) {
			/* Add Intensity */
			fp >> str;
        	utility::addGrey(src,tgt,atoi(str));
        }

        else if (strncmp(str,"binarize",8)==0) {
			/* Thresholding */
			fp >> str;
			utility::binarize(src,tgt,atoi(str));
		}

		else if (strncmp(str,"scale", 5)==0) {
			/* Image scaling */
			fp >> str;
			utility::scale(src,tgt,atof(str));
		}
		
		else if (strncmp(str, "doubleT", 7) == 0) {
			/* Double Thresholding */				
			fp >> str;
			utility::doubleThreshold(src, tgt);
			std::cout << "Double threshold algorithm complete." << std::endl;
		}

		else if (strncmp(str, "colorBin", 8) == 0) {
			/* Color Binarization+ */	
			fp >> str;
			utility::colorBinarize(src, tgt);
			std::cout << "ColorBinarization+ algorithm complete." << std::endl;
		}		

		else if (strncmp(str, "uSmooth", 7) == 0) {
			/* Uniform Smoothing */
			fp >> str;
			utility::uniformSmooth(src, tgt, atof(str)); //Src image, target, type of smoothing
			std::cout << "Uniform smoothing algorithm complete." << std::endl;
		}

		else if (strncmp(str, "grayHistStretch", 15) == 0) {
			/* Histogram Stretching */
			fp >> str;
			utility::grayHistoStretch(src, tgt, outfile);
			std::cout << "Gray Histogram Stretching algorithm complete." << std::endl;
		}

		else if (strncmp(str, "optThreshGray", 13) == 0) {
			/* Optimal Thresholding of Gray Image */
			fp >> str;
			utility::optimalThreshGray(src, tgt);
			std::cout << "Optimal Thresholding of Gray Image algorithm complete." << std::endl;
		}

		else if (strncmp(str, "optThHisto", 10) == 0) {
			/* Optimal Thresholding of Gray Image combined with Histogram Stretching */
			fp >> str;
			utility::optimalThresh_HistoStretch(src, tgt);
			std::cout << "Optimal Thresholding of Gray Image combined with Histogram stretching algorithm complete." << std::endl;
		}

		else if (strncmp(str, "sobelEdgeGray", 10) == 0) {
			/* Edge detection of gray images */
			fp >> str;
			int threshold;
			std::cout << std::endl << "Please enter the threshold for the \"Sobel Edge Detect Gray\" algorithm: ";
			std::cin >> threshold;
			utility::sobelEdgeDetectGray(src, tgt, threshold);
			std::cout << "Sobel Edge Detection of Grayscale image algorithm complete." << std::endl;
		}

		else if (strncmp(str, "edgeDetectColor", 15) == 0) {
			/* Edge detection of color images */
			fp >> str;
			utility::edgeDetectColor(src, tgt);
			std::cout << "Edge Detection of Color image algorithm complete." << std::endl;
		}


		/////////////////////////////////
		//** New project 4 functions **//
		/////////////////////////////////

		/************************************************************************************************/
		else if (strncmp(str, "openCVHistoSt", 13) == 0) {
			/* OpenCV Histogram Stretch */
			fp >> str;
			utility::openCVHistogramStretch(src, tgt, outfile);
			std::cout << "OpenCV Histo Stretch Function Complete" << std::endl;
		}

		/************************************************************************************************/
		else if (strncmp(str, "openCVHistoEq", 13) == 0) {
			/* OpenCV Histogram Equalization */
			fp >> str;
			std::string path = "../input/slope.png";
			
			// Load image
			cv::Mat openCVSrc = cv::imread(path, -1);
			if (openCVSrc.empty()) {
				cout << "OpenCV Histo Equalize failed to open image" << endl;
				continue;
			}

			// Histogram Equalization
			cv::Mat openCVDst;
			cv::equalizeHist(openCVSrc, openCVDst);
			cv::imwrite("../output/slope_OpenCVHistoEqualized.png", openCVDst);
			std::cout << "OpenCV Histo Equalization Function Complete" << std::endl;
		}

		/************************************************************************************************/
		else if (strncmp(str, "openCVEdgeDet", 13) == 0) {
			/* OpenCV Edge Detection */
			fp >> str;
			std::string srcPath = "../input/baboon.pgm";
			std::string dstPath = "../output/baboon_dest.pgm";
			
			utility::openCVEdgeDetect(srcPath, dstPath);
		}

		/************************************************************************************************/
		else if (strncmp(str, "openCVEquaStr", 13) == 0) {
			/* Combination */
			fp >> str;
			std::string srcPath = "../input/baboon.pgm";
			std::string dstPath = "../output/baboon_dest.pgm";
			
			utility::combinedOCVEqualStretch(srcPath, dstPath);
		}

		/************************************************************************************************/
		else if (strncmp(str, "qrReaderFunct", 13) == 0) {
			/* QR Reader */
			fp >> str;
			std::string srcPath = "../input/QR/qr1.jpg";
			std::cout << "Performing QR code readings" << std::endl;
			utility::qrReaderFunction(srcPath);
		}


		else {
			printf("No function: %s\n", str);
			continue;
		}
       
		//Only save those with correct image file types
		std::string outfileString = outfile;
		if (outfileString.substr(outfileString.find_last_of(".") + 1) != "pgm" ||
			outfileString.substr(outfileString.find_last_of(".") + 1) != "ppm") {
				//Do nothing, usually for openCV which writes its images another way
		} else {
			tgt.save(outfile);
		}
	}
	//fclose(fp);
	fp.close();
	return 0;
}

