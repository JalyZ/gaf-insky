/**
 * @file CVBasedInterace.hpp
 * @author chenhui.yu (yuchenhui153@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "opencv2/opencv.hpp"
#include "gaf/alg/common.hpp"

namespace gaf
{
    namespace algorithm
    {
        class HUI_EXPORT CVBasedInterface : public AlgorithmCommon
        {
        public:
            /**
            * @brief 处理一个图像，输出一个图像
            * 
            * @param in  输入数据
            * @param out 输出数据
            */
            virtual void process(const cv::Mat &in, cv::Mat &out) = 0;

            InterfaceIdentity identity() override
            {
                return InterfaceIdentity::OPENCV_BASED;
            }
        };
    }
}
