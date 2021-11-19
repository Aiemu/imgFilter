#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>

#include "include/SLIC.h"
#include "include/LOMO.h"

using namespace cv;
using namespace std;

struct Config {
    int M;
    int cluster_num;
    std::string path;
};

Config get_config();

int main() {
//    const char *path = "../data/tch.jpg";
    Config config = get_config();
    LOMO lomo(config.path.c_str());
    lomo.set_saturation();
    lomo.display_saturation();
    lomo.add_dark_halo();
    lomo.display_lomo_result();

    SLIC slic(config.cluster_num, config.path.c_str(), config.M);
    int loop = 10;
    for (int i = 0; i < loop; i++) {
        std::cout << "Loop: " << i + 1 << std::endl;
        slic.run_an_epoch();
        slic.display_cluster_list(i);
        slic.display_result(i);
        waitKey(100);
    }

    return 0;
}

Config get_config() {
    ifstream fin;
    fin.open("../config.txt", ios::in);
    std::string tmp_line;
    Config ret;
    while (getline(fin, tmp_line)) {
        tmp_line.erase(std::remove(tmp_line.begin(), tmp_line.end(), ' '), tmp_line.end());
        std::string content = tmp_line.substr(tmp_line.find("=") + 1);
        if (tmp_line.find("filename") == 0) {
            ret.path = content;
        } else if (tmp_line.find("M") == 0) {
            ret.M = stoi(content);
        } else if (tmp_line.find("cluster") == 0) {
            ret.cluster_num = stoi(content);
        }
    }
    return ret;
}
