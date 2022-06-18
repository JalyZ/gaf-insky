#include "gaf/alg/image/CVBasedInterace.hpp"

class HUI_EXPORT CVTestClass : public gaf::algorithm::CVBasedInterface
{
public:
    std::string name() override
    {
        static std::string _name{"CVTestClass"};
        return _name;
    }

    void process(const cv::Mat& in, cv::Mat& out)
    {
        cv::cvtColor(in, out, cv::COLOR_BGR2GRAY);
    }
};

ModuleName(CV_TEST_MODULE)

CreateAlgorithmModule(CVTestClass)
