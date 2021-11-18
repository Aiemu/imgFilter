//
// Created by Aiemu on 2021/11/17.
//

#include "LOMO.h"


LOMO::LOMO(const char *path) {
    this->ori_img = imread(path);
    this->ori_h = this->ori_img.rows;
    this->ori_w = this->ori_img.cols;
    this->halo_radius = (this->ori_h > this->ori_w ? this->ori_h : this->ori_w) / 3.0;
}

void LOMO::set_saturation() {
    Mat transformer(1, 256, CV_8UC1);
    for (int i = 0; i < 256; i++) { // \frac{1}{1+e^{-\frac{x-0.5}{s}}}
        transformer.at<uchar>(i) = cvRound(256 * (1 / (1 + pow(exp(1.0), -(((double)i / 256.0 - 0.5) / 0.1)))));
    }

    vector<Mat> transformer_vec;
    split(this->ori_img, transformer_vec);
    LUT(transformer_vec[2], transformer, transformer_vec[2]);
    merge(transformer_vec, this->saturation_img);
}

void LOMO::add_dark_halo() {
    Mat dark_halo(this->ori_h, this->ori_w, CV_32FC3, Scalar(0.3, 0.3, 0.3));
    circle(dark_halo, Point(this->ori_w / 2, this->ori_h / 2), this->halo_radius,
           Scalar(1, 1, 1), -1);
    blur(dark_halo, dark_halo, Size(this->halo_radius, this->halo_radius));

    this->saturation_img.convertTo(this->lomo_img, CV_32FC3);
    multiply(this->lomo_img, dark_halo, this->lomo_img);
    this->lomo_img.convertTo(this->saturation_img, CV_8UC3);
    this->lomo_img.convertTo(this->lomo_img, CV_8UC1);
}

void LOMO::display_saturation() {
    imwrite("../output/lomo/saturation.jpg", this->saturation_img);
    imshow("Saturation", this->saturation_img);
}

void LOMO::display_lomo_result() {
    imwrite("../output/lomo/lomo.jpg", this->lomo_img);
    imshow("LOMO", this->lomo_img);
}