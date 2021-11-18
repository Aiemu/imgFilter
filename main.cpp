#include <iostream>
#include <opencv2/opencv.hpp>

#include "include/SLIC.h"
#include "include/LOMO.h"

using namespace cv;

int main() {
    const char *path = "../data/lenna.png";
//    LOMO lomo(path);
//    lomo.set_saturation();
//    lomo.display_saturation();
//    lomo.add_dark_halo();
//    lomo.display_lomo_result();
//    waitKey(0);

    SLIC slic(1600, path, 15);
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
