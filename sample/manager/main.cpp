#include <iostream>
#include "gaf/alg_manager/AlgManager.hpp"
#include "gaf/alg/image/CVBasedInterace.hpp"
#include <chrono>


int main(int argc, char* argv[])
{
    using namespace gaf::algorithm;
    if(argc < 2)
    {
        std::cout << "error input" << std::endl;
        return -1;
    }

    gaf::manager::AlgManager manager;
    std::string loaded_module;
    auto&&[load_success, detail] = manager.loadModule(argv[1], loaded_module);
    if(!load_success)
    {
        std::cout << "load library failed! detail:" << detail << std::endl;
        return -1;
    }
    std::cout << "load library success" << std::endl;
    auto alg = manager.getAlgorithmByName<CVBasedInterface>("CV_TEST_MODULE", "CVTestClass");

    if(!alg)
    {
        std::cout << "can not get algorithm:" << "CVTestClass" << std::endl;
        return -1;
    }

    std::cout   << "algorithm detail:\n" 
                << "name:\t\t"         << alg->name()          << "\n"
                << "description:\t"    << alg->description()   << "\n"
                << "version:\t"        << alg->version()
                << std::endl;

    cv::Mat in_mat = cv::imread(argv[2]);
    cv::Mat out_mat;

    /*
    auto begin = std::chrono::system_clock::now();
    for(int i = 0; i < 50000 ; i++)
        alg->process(in_mat , out_mat);
    auto use_time = std::chrono::system_clock::now() - begin;
    std::cout   << "shared library invike time:" 
                << std::chrono::duration_cast<std::chrono::milliseconds>(use_time).count()
                << std::endl;

    // direct invoke
    begin = std::chrono::system_clock::now();
    for(int i = 0; i < 50000 ; i++)
        cv::cvtColor(in_mat, out_mat, cv::COLOR_BGR2GRAY);
    use_time = std::chrono::system_clock::now() - begin;
    std::cout   << "shared library invike time:" 
                << std::chrono::duration_cast<std::chrono::milliseconds>(use_time).count()
                << std::endl;
    */

    alg->process(in_mat , out_mat);
    cv::namedWindow("out image", cv::WINDOW_NORMAL);
    cv::imshow("out image", out_mat);

    cv::waitKey();

    return 0;
}
