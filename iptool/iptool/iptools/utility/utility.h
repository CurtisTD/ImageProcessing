#ifndef UTILITY_H
#define UTILITY_H

#include "../image/image.h"
#include <sstream>
#include <math.h>

class utility
{
	public:
		utility();
		virtual ~utility();
		static std::string intToString(int number);
		static int checkValue(int value);
		static void addGrey(image &src, image &tgt, int value);
		static void binarize(image &src, image &tgt, int threshold);
		static void scale(image &src, image &tgt, float ratio);
		static void doubleThreshold(image &src, image &tgt);
		static void colorBinarize(image &src, image &tgt);
		static void uniformSmooth(image &src, image &tgt, int smoothType);
		static void grayHistoStretch(image &src, image &tgt, char outfile[]);
		static void optimalThreshGray(image &src, image &tgt);
		static void optimalThresh_HistoStretch(image &src, image &tgt);

		static void sobelEdgeDetectGray(image &src, image &tgt, int threshold = 25);
		static void edgeDetectColor(image &src, image &tgt);
};

#endif

