//
// Created by Aiemu on 2021/11/17.
//

#include "SLIC.h"

SLIC::SLIC(int cluster_num, const char *path, int M) {
    // get image data, resize src image and transfer BGR to Lab
    this->BGR_img = imread(path);
    if (!this->BGR_img.data) throw runtime_error("incorrect image path.");
    this->ori_h = this->BGR_img.rows;
    this->ori_w = this->BGR_img.cols;
    int d_size = sqrt(cluster_num);

    // basic image info
    int s_size = this->ori_h > this->ori_w ? this->ori_h : this->ori_w;
    this->M = M;
    this->dst_size = d_size;
    this->dst_pix_cnt = d_size * d_size;
    this->src_size = s_size;
    this->src_pix_cnt = s_size * s_size;
    this->seed_dis = sqrt(this->src_pix_cnt / this->dst_pix_cnt);

//    resize(this->BGR_img, this->BGR_img, Size(this->src_size, this->src_size));
    resize(this->BGR_img, this->BGR_img, Size(s_size / d_size * d_size,s_size / d_size * d_size));
    this->src_size = s_size / d_size * d_size;
    this->src_pix_cnt = this->src_size * this->src_size;

    cvtColor(this->BGR_img, this->Lab_img, COLOR_BGR2Lab);

    // init cluster list info
    for (int i = 0; i < this->dst_size; i++) {
        for (int j = 0; j < this->dst_size; j++) {
            P_Vec cluster(this->seed_dis / 2 + i * this->seed_dis, this->seed_dis / 2 + j * this->seed_dis,
                          this->Lab_img.at<Vec3b>(i, j)[0], this->Lab_img.at<Vec3b>(i, j)[1],
                          this->Lab_img.at<Vec3b>(i, j)[2]);
            this->cluster_list.push_back(cluster);
        }
    }

    // init pix's cluster and distance, <cluster_id, distance>
    for (int i = 0; i < this->src_size; i++) { // x, row_id
        std::vector<std::pair<int, int>> row;
        for (int j = 0; j < this->src_size; j++) { // y, column_id
            row.push_back(std::pair<int, int>(i / this->seed_dis * this->dst_size + j / this->seed_dis, -1));
        }
        this->pix_info.push_back(row);
    }
}

int SLIC::get_distance(int cluster_id, int pix_x, int pix_y) { // D = sqrt((d_c/N_c)^2 + (d_s/N_s)^2)
    int _x = this->cluster_list[cluster_id].x - pix_x;
    int _y = this->cluster_list[cluster_id].y - pix_y;
    int _l = this->cluster_list[cluster_id].L - this->Lab_img.at<Vec3b>(pix_x, pix_y)[0];
    int _a = this->cluster_list[cluster_id].a - this->Lab_img.at<Vec3b>(pix_x, pix_y)[1];
    int _b = this->cluster_list[cluster_id].b - this->Lab_img.at<Vec3b>(pix_x, pix_y)[2];

    int squ_d_s = _x * _x + _y * _y;
    int squ_d_c = _l * _l + _a * _a + _b * _b;
    int squ_n_c = this->M * this->M;
    int squ_n_s = this->src_pix_cnt / this->dst_pix_cnt;
    return squ_d_c / squ_n_c + squ_d_s / squ_n_s;
//    return squ_d_c + squ_d_s * 100;
}

void SLIC::update_pix_info() {
    for (int cluster_id = 0; cluster_id < this->dst_pix_cnt; cluster_id++) {
        for (int x = -this->seed_dis; x < this->src_size; x++) {
            int pix_x = this->cluster_list[cluster_id].x + x;
            for (int y = -this->seed_dis; y < this->src_size; y++) {
                int pix_y = this->cluster_list[cluster_id].y + y;
                if (pix_x < 0 || pix_y < 0 || pix_x >= this->src_size || pix_y >= this->src_size) continue;
                int dis = this->get_distance(cluster_id, pix_x, pix_y);
                if (this->pix_info[pix_x][pix_y].second == -1 || dis < this->pix_info[pix_x][pix_y].second) {
                    this->pix_info[pix_x][pix_y].first = cluster_id;
                    this->pix_info[pix_x][pix_y].second = dis;
                }
            }
        }
    }
}

void SLIC::update_cluster_list() {
    std::vector<P_Vec> sum_vec = std::vector<P_Vec>(this->dst_pix_cnt, P_Vec(0, 0, 0, 0, 0));
    std::vector<int> cnt_vec = std::vector<int>(this->dst_pix_cnt, 0);
    for (int i = 0; i < this->src_size; i++) {
        for (int j = 0; j < this->src_size; j++) {
            sum_vec[this->pix_info[i][j].first].x += i;
            sum_vec[this->pix_info[i][j].first].y += j;
            sum_vec[this->pix_info[i][j].first].L += this->Lab_img.at<Vec3b>(i, j)[0];
            sum_vec[this->pix_info[i][j].first].a += this->Lab_img.at<Vec3b>(i, j)[1];
            sum_vec[this->pix_info[i][j].first].b += this->Lab_img.at<Vec3b>(i, j)[2];
            cnt_vec[this->pix_info[i][j].first]++;
        }
    }
    for (int i = 0; i < this->dst_pix_cnt; i++) {
        if (cnt_vec[i] == 0) continue;
        this->cluster_list[i].x = round(sum_vec[i].x / cnt_vec[i]);
        this->cluster_list[i].y = round(sum_vec[i].y / cnt_vec[i]);
        this->cluster_list[i].L = round(sum_vec[i].L / cnt_vec[i]);
        this->cluster_list[i].a = round(sum_vec[i].a / cnt_vec[i]);
        this->cluster_list[i].b = round(sum_vec[i].b / cnt_vec[i]);
    }
}

void SLIC::display_cluster_list(int idx) {
    Mat tmp = this->Lab_img.clone();
    for (int i = 0; i < this->dst_pix_cnt; i++) {
        circle(tmp, Point(this->cluster_list[i].x, this->cluster_list[i].y), 1,
               Scalar(0, 255, 0), 1);
    }
    cvtColor(tmp, tmp, COLOR_Lab2BGR);
    char filename[100];
    sprintf(filename, "../output/crystallization/clusters/cluster-%d.jpg", idx);
    if (this->ori_h != this->src_size || this->ori_w != this->src_size) {
        resize(tmp, tmp, Size(this->ori_w, this->ori_h));
    }
    imwrite(filename, tmp);
    imshow("Cluster", tmp);
}

void SLIC::display_result(int idx) {
    Mat tmp = this->Lab_img.clone();
    for (int i = 0; i < this->src_size; i++) {
        for (int j = 0; j < this->src_size; j++) {
            int cluster_x = this->cluster_list[this->pix_info[i][j].first].x;
            int cluster_y = this->cluster_list[this->pix_info[i][j].first].y;
            tmp.at<Vec3b>(i, j)[0] = this->Lab_img.at<Vec3b>(cluster_x, cluster_y)[0];
            tmp.at<Vec3b>(i, j)[1] = this->Lab_img.at<Vec3b>(cluster_x, cluster_y)[1];
            tmp.at<Vec3b>(i, j)[2] = this->Lab_img.at<Vec3b>(cluster_x, cluster_y)[2];
        }
    }
    cvtColor(tmp, tmp, COLOR_Lab2BGR);
    char filename[100];
    sprintf(filename, "../output/crystallization/results/result-%d.jpg", idx);
    if (this->ori_h != this->src_size || this->ori_w != this->src_size) {
        resize(tmp, tmp, Size(this->ori_w, this->ori_h));
    }
    imwrite(filename, tmp);
    imshow("Result", tmp);
}

void SLIC::run_an_epoch() {
    this->update_pix_info();
    this->update_cluster_list();
}


