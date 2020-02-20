/************************************************************/

/************************************************************/

#include "./iptools/core.h"
#include <string.h>
#include <fstream>

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

		else {
			printf("No function: %s\n", str);
			continue;
		}
       
		tgt.save(outfile);
	}
	//fclose(fp);
	fp.close();
	return 0;
}

