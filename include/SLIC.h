//
// Created by Aiemu on 2021/11/17.
//

#ifndef FILTER_SLIC_H
#define FILTER_SLIC_H

#include <cmath>
#include <thread>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

struct P_Vec {
    P_Vec(int x, int y, int L, int a, int b) {
        this->x = x;
        this->y = y;
        this->L = L;
        this->a = a;
        this->b = b;
    }
    int x, y, L, a, b;
};

class SLIC {
private:
    int ori_w;
    int ori_h;
    int M;
    int dst_size;
    int src_size;
    int dst_pix_cnt;
    int src_pix_cnt;
    int seed_dis;
    Mat BGR_img;
    Mat Lab_img;
    std::vector<P_Vec> cluster_list;
    std::vector<std::vector<std::pair<int, int>>> pix_info; // <cluster_id, distance>

public:
    SLIC(int cluster_num, const char * path, int M);
    int get_distance(int cluster_id, int pix_x, int pix_y);
    void update_pix_info();
    void update_cluster_list();
    void display_cluster_list(int idx);
    void display_result(int idx);
    void run_an_epoch();
};


#endif //FILTER_SLIC_H
