#include "mshift.hpp"

using namespace std;
using namespace cv;
/* squares_tab[i]=(i-256)^2
s = ''
for y in range(16*2):
    for x in range(16):
        s+='%5d, '%((y*16 + x - 256)**2)
    s+='\n'

print s

*/
static const int squares_tab[] = {
        65536, 65025, 64516, 64009, 63504, 63001, 62500, 62001, 61504, 61009, 60516, 60025, 59536, 59049, 58564, 58081,
        57600, 57121, 56644, 56169, 55696, 55225, 54756, 54289, 53824, 53361, 52900, 52441, 51984, 51529, 51076, 50625,
        50176, 49729, 49284, 48841, 48400, 47961, 47524, 47089, 46656, 46225, 45796, 45369, 44944, 44521, 44100, 43681,
        43264, 42849, 42436, 42025, 41616, 41209, 40804, 40401, 40000, 39601, 39204, 38809, 38416, 38025, 37636, 37249,
        36864, 36481, 36100, 35721, 35344, 34969, 34596, 34225, 33856, 33489, 33124, 32761, 32400, 32041, 31684, 31329,
        30976, 30625, 30276, 29929, 29584, 29241, 28900, 28561, 28224, 27889, 27556, 27225, 26896, 26569, 26244, 25921,
        25600, 25281, 24964, 24649, 24336, 24025, 23716, 23409, 23104, 22801, 22500, 22201, 21904, 21609, 21316, 21025,
        20736, 20449, 20164, 19881, 19600, 19321, 19044, 18769, 18496, 18225, 17956, 17689, 17424, 17161, 16900, 16641,
        16384, 16129, 15876, 15625, 15376, 15129, 14884, 14641, 14400, 14161, 13924, 13689, 13456, 13225, 12996, 12769,
        12544, 12321, 12100, 11881, 11664, 11449, 11236, 11025, 10816, 10609, 10404, 10201, 10000,  9801,  9604,  9409,
         9216,  9025,  8836,  8649,  8464,  8281,  8100,  7921,  7744,  7569,  7396,  7225,  7056,  6889,  6724,  6561,
         6400,  6241,  6084,  5929,  5776,  5625,  5476,  5329,  5184,  5041,  4900,  4761,  4624,  4489,  4356,  4225,
         4096,  3969,  3844,  3721,  3600,  3481,  3364,  3249,  3136,  3025,  2916,  2809,  2704,  2601,  2500,  2401,
         2304,  2209,  2116,  2025,  1936,  1849,  1764,  1681,  1600,  1521,  1444,  1369,  1296,  1225,  1156,  1089,
         1024,   961,   900,   841,   784,   729,   676,   625,   576,   529,   484,   441,   400,   361,   324,   289,
          256,   225,   196,   169,   144,   121,   100,    81,    64,    49,    36,    25,    16,     9,     4,     1,
            0,     1,     4,     9,    16,    25,    36,    49,    64,    81,   100,   121,   144,   169,   196,   225,
          256,   289,   324,   361,   400,   441,   484,   529,   576,   625,   676,   729,   784,   841,   900,   961,
         1024,  1089,  1156,  1225,  1296,  1369,  1444,  1521,  1600,  1681,  1764,  1849,  1936,  2025,  2116,  2209,
         2304,  2401,  2500,  2601,  2704,  2809,  2916,  3025,  3136,  3249,  3364,  3481,  3600,  3721,  3844,  3969,
         4096,  4225,  4356,  4489,  4624,  4761,  4900,  5041,  5184,  5329,  5476,  5625,  5776,  5929,  6084,  6241,
         6400,  6561,  6724,  6889,  7056,  7225,  7396,  7569,  7744,  7921,  8100,  8281,  8464,  8649,  8836,  9025,
         9216,  9409,  9604,  9801, 10000, 10201, 10404, 10609, 10816, 11025, 11236, 11449, 11664, 11881, 12100, 12321,
        12544, 12769, 12996, 13225, 13456, 13689, 13924, 14161, 14400, 14641, 14884, 15129, 15376, 15625, 15876, 16129,
        16384, 16641, 16900, 17161, 17424, 17689, 17956, 18225, 18496, 18769, 19044, 19321, 19600, 19881, 20164, 20449,
        20736, 21025, 21316, 21609, 21904, 22201, 22500, 22801, 23104, 23409, 23716, 24025, 24336, 24649, 24964, 25281,
        25600, 25921, 26244, 26569, 26896, 27225, 27556, 27889, 28224, 28561, 28900, 29241, 29584, 29929, 30276, 30625,
        30976, 31329, 31684, 32041, 32400, 32761, 33124, 33489, 33856, 34225, 34596, 34969, 35344, 35721, 36100, 36481,
        36864, 37249, 37636, 38025, 38416, 38809, 39204, 39601, 40000, 40401, 40804, 41209, 41616, 42025, 42436, 42849,
        43264, 43681, 44100, 44521, 44944, 45369, 45796, 46225, 46656, 47089, 47524, 47961, 48400, 48841, 49284, 49729,
        50176, 50625, 51076, 51529, 51984, 52441, 52900, 53361, 53824, 54289, 54756, 55225, 55696, 56169, 56644, 57121,
        57600, 58081, 58564, 59049, 59536, 60025, 60516, 61009, 61504, 62001, 62500, 63001, 63504, 64009, 64516, 65025,
};

std::pair<cv::Point, cv::Vec3b> meanShiftStep(cv::Point p, cv::Vec3b color, const Image& img, int width, int height) {
    int res_color[3] = {0, 0, 0};
    int res_pos[2] = {0, 0};
    int weight_sum = 0;

    height*=height;

    const int *const squares = squares_tab + 256;
    const int minx =  max(p.x-width, 0);
    const int miny = max(p.y-width, 0);
    const int maxx = min(p.x+width+1, img.cols);
    const int maxy = min(p.y+width+1, img.rows);

    for (int y = miny; y < maxy; y++) {
        const Vec3b* row = img[y] + minx;
        for (int x = minx; x < maxx; x++) {
            const Vec3b acolor = *row;
            row++;
            if (squares[acolor[0] - color[0]] + squares[acolor[1] - color[1]] +squares[acolor[2] - color[2]] > height)
                continue;
            weight_sum++;
            res_color[0] += acolor[0];
            res_color[1] += acolor[1];
            res_color[2] += acolor[2];
            res_pos[0]+=x;
            res_pos[1]+=y;
        }
    }
    if (weight_sum==0)
        return make_pair(p, color);
    for (int i=0; i<3; i++)
        res_color[i] /= weight_sum;
    return make_pair(
    	Point(res_pos[0]/weight_sum, res_pos[1]/weight_sum),
    	Vec3b(res_color[0], res_color[1], res_color[2])
    );

}

void meanShiftPoint(Image& src0, int x, int y, double radius, double threshold)
{
    pair<Point,Vec3b> val(Point(x, y), src0(y, x));
    // TODO: there can be faster stop criterion
    for(int i = 0;i < 3;i++)
        val = meanShiftStep(val.first, val.second, src0, radius, threshold);

    src0(y, x) = val.second;
}

void
meanShiftFiltering( Image src0,
                         double radius, double threshold)
{
    //return meanShiftFilteringOriginal(src0, src0, sp0, sr, 0, termcrit);
    for (int y=0;y<src0.rows;y++)
        for (int x=0;x<src0.cols;x++)
            meanShiftPoint(src0, x, y, radius, threshold);
}


/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include <opencv/cv.h>

void
meanShiftFilteringOriginal( const Image src0, Image dst0,
                         double sp0, double sr, int max_level,
                         CvTermCriteria termcrit )
{
    const int cn = 3;
    const int MAX_LEVELS = 8;
    cv::Mat* src_pyramid = new cv::Mat[MAX_LEVELS+1];
    cv::Mat* dst_pyramid = new cv::Mat[MAX_LEVELS+1];
    cv::Mat mask0;
    int i, j, level;
    //uchar* submask = 0;

    #define cdiff(ofs0) (tab[c0-dptr[ofs0]+255] + \
        tab[c1-dptr[(ofs0)+1]+255] + tab[c2-dptr[(ofs0)+2]+255] >= isr22)

    double sr2 = sr * sr;
    int isr2 = cvRound(sr2), isr22 = MAX(isr2,16);
    int tab[768];

    if( src0.type() != CV_8UC3 )
        CV_Error( CV_StsUnsupportedFormat, "Only 8-bit, 3-channel images are supported" );
    
    if( src0.type() != dst0.type() )
        CV_Error( CV_StsUnmatchedFormats, "The input and output images must have the same type" );

    if( src0.size() != dst0.size() )
        CV_Error( CV_StsUnmatchedSizes, "The input and output images must have the same size" );

    if( (unsigned)max_level > (unsigned)MAX_LEVELS )
        CV_Error( CV_StsOutOfRange, "The number of pyramid levels is too large or negative" );

    if( !(termcrit.type & CV_TERMCRIT_ITER) )
        termcrit.max_iter = 5;
    termcrit.max_iter = MAX(termcrit.max_iter,1);
    termcrit.max_iter = MIN(termcrit.max_iter,100);
    if( !(termcrit.type & CV_TERMCRIT_EPS) )
        termcrit.epsilon = 1.f;
    termcrit.epsilon = MAX(termcrit.epsilon, 0.f);

    for( i = 0; i < 768; i++ )
        tab[i] = (i - 255)*(i - 255);

    // 1. construct pyramid
    src_pyramid[0] = src0;
    dst_pyramid[0] = dst0;
    for( level = 1; level <= max_level; level++ )
    {
        src_pyramid[level].create( (src_pyramid[level-1].rows+1)/2,
                        (src_pyramid[level-1].cols+1)/2, src_pyramid[level-1].type() );
        dst_pyramid[level].create( src_pyramid[level].rows,
                        src_pyramid[level].cols, src_pyramid[level].type() );
        cv::pyrDown( src_pyramid[level-1], src_pyramid[level], src_pyramid[level].size() );
        //CV_CALL( cvResize( src_pyramid[level-1], src_pyramid[level], CV_INTER_AREA ));
    }

    mask0.create(src0.rows, src0.cols, CV_8UC1);
    //CV_CALL( submask = (uchar*)cvAlloc( (sp+2)*(sp+2) ));

    // 2. apply meanshift, starting from the pyramid top (i.e. the smallest layer)
    for( level = max_level; level >= 0; level-- )
    {
        cv::Mat src = src_pyramid[level];
        cv::Size size = src.size();
        uchar* sptr = src.data;
        int sstep = (int)src.step;
        uchar* mask = 0;
        int mstep = 0;
        uchar* dptr;
        int dstep;
        float sp = (float)(sp0 / (1 << level));
        sp = MAX( sp, 1 );

        if( level < max_level )
        {
            cv::Size size1 = dst_pyramid[level+1].size();
            cv::Mat m( size.height, size.width, CV_8UC1, mask0.data );
            dstep = (int)dst_pyramid[level+1].step;
            dptr = dst_pyramid[level+1].data + dstep + cn;
            mstep = (int)m.step;
            mask = m.data + mstep;
            //cvResize( dst_pyramid[level+1], dst_pyramid[level], CV_INTER_CUBIC );
            cv::pyrUp( dst_pyramid[level+1], dst_pyramid[level], dst_pyramid[level].size() );
            m.setTo(cv::Scalar::all(0));

            for( i = 1; i < size1.height-1; i++, dptr += dstep - (size1.width-2)*3, mask += mstep*2 )
            {
                for( j = 1; j < size1.width-1; j++, dptr += cn )
                {
                    int c0 = dptr[0], c1 = dptr[1], c2 = dptr[2];
                    mask[j*2 - 1] = cdiff(-3) || cdiff(3) || cdiff(-dstep-3) || cdiff(-dstep) ||
                        cdiff(-dstep+3) || cdiff(dstep-3) || cdiff(dstep) || cdiff(dstep+3);
                }
            }

            cv::dilate( m, m, cv::Mat() );
            mask = m.data;
        }

        dptr = dst_pyramid[level].data;
        dstep = (int)dst_pyramid[level].step;

        for( i = 0; i < size.height; i++, sptr += sstep - size.width*3,
                                          dptr += dstep - size.width*3,
                                          mask += mstep )
        {   
            for( j = 0; j < size.width; j++, sptr += 3, dptr += 3 )
            {               
                int x0 = j, y0 = i, x1, y1, iter;
                int c0, c1, c2;

                if( mask && !mask[j] )
                    continue;

                c0 = sptr[0], c1 = sptr[1], c2 = sptr[2];

                // iterate meanshift procedure
                for( iter = 0; iter < termcrit.max_iter; iter++ )
                {
                    uchar* ptr;
                    int x, y, count = 0;
                    int minx, miny, maxx, maxy;
                    int s0 = 0, s1 = 0, s2 = 0, sx = 0, sy = 0;
                    double icount;
                    int stop_flag;

                    //mean shift: process pixels in window (p-sigmaSp)x(p+sigmaSp)
                    minx = cvRound(x0 - sp); minx = MAX(minx, 0);
                    miny = cvRound(y0 - sp); miny = MAX(miny, 0);
                    maxx = cvRound(x0 + sp); maxx = MIN(maxx, size.width-1);
                    maxy = cvRound(y0 + sp); maxy = MIN(maxy, size.height-1);
                    ptr = sptr + (miny - i)*sstep + (minx - j)*3; 

                    for( y = miny; y <= maxy; y++, ptr += sstep - (maxx-minx+1)*3 )
                    {
                        int row_count = 0;
                        x = minx;
                        for( ; x + 3 <= maxx; x += 4, ptr += 12 )
                        {
                            int t0 = ptr[0], t1 = ptr[1], t2 = ptr[2];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x; row_count++;
                            }
                            t0 = ptr[3], t1 = ptr[4], t2 = ptr[5];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+1; row_count++;
                            }
                            t0 = ptr[6], t1 = ptr[7], t2 = ptr[8];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+2; row_count++;
                            }
                            t0 = ptr[9], t1 = ptr[10], t2 = ptr[11];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x+3; row_count++;
                            }
                        }
                        
                        for( ; x <= maxx; x++, ptr += 3 )
                        {      
                            int t0 = ptr[0], t1 = ptr[1], t2 = ptr[2];
                            if( tab[t0-c0+255] + tab[t1-c1+255] + tab[t2-c2+255] <= isr2 )
                            {                        
                                s0 += t0; s1 += t1; s2 += t2;
                                sx += x; row_count++;
                            }
                        }
                        count += row_count;
                        sy += y*row_count;
                    }

                    if( count == 0 )
                        break;

                    icount = 1./count;
                    x1 = cvRound(sx*icount);
                    y1 = cvRound(sy*icount);
                    s0 = cvRound(s0*icount);
                    s1 = cvRound(s1*icount);
                    s2 = cvRound(s2*icount);

                    stop_flag = (x0 == x1 && y0 == y1) || abs(x1-x0) + abs(y1-y0) +
                        tab[s0 - c0 + 255] + tab[s1 - c1 + 255] +
                        tab[s2 - c2 + 255] <= termcrit.epsilon;
                
                    x0 = x1; y0 = y1;
                    c0 = s0; c1 = s1; c2 = s2;

                    if( stop_flag )
                        break;
                }

                dptr[0] = (uchar)c0;
                dptr[1] = (uchar)c1;
                dptr[2] = (uchar)c2;
            }
        }
    }
    delete[] src_pyramid;
    delete[] dst_pyramid;
}

