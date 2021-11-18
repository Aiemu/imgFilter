//
// Created by Aiemu on 2021/11/17.
//

#ifndef FILTER_LOMO_H
#define FILTER_LOMO_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class LOMO {
private:
    Mat ori_img;
    Mat saturation_img;
    Mat lomo_img;
    int ori_h;
    int ori_w;
    double halo_radius;

public:
    LOMO(const char * path);
    void set_saturation();
    void add_dark_halo();
    void display_saturation();
    void display_lomo_result();
};


#endif //FILTER_LOMO_H
