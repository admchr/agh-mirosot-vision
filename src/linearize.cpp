#include "linearize.hpp"

uchar srgb_to_rgb(uchar ch){
    double x = ch/256.0;
    double res;
    if (x>0.04045)
        res =  pow((x+0.055)/(1.055), 2.4);
    else
        res = x / 12.92;
    return std::min(255, int(res*256+0.4));
}

uchar rgb_to_srgb(uchar ch){
    double x = ch/256.0;
    double res;
    if (x>0.00313)
        res =  1.055*pow(x, 1/2.4) - 0.055;
    else
        res = x * 12.92;
    return std::min(255, int(res*256+0.4));
}

static void do_convert(Image& img, bool de) {
	int tab[256];
	for(int i=0;i<256;i++) {
		if (de)
			tab[i] = rgb_to_srgb(i);
		else
			tab[i] = srgb_to_rgb(i);
	}
	for (int y=0; y<img.rows; y++)
		for (int x=0; x<img.cols; x++) {
			cv::Vec3b c = img(y, x);
			img(y, x)=cv::Vec3b(tab[c[0]], tab[c[1]], tab[c[2]]);
		}
}

void from_sRGB_to_RGB(Image& img) {
	do_convert(img, false);
}

void from_RGB_to_sRGB(Image& img) {
	do_convert(img, true);
}
