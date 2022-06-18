/**
 * @file common.hpp
 * @author chenhui.yu (yuchenhui153@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include "gaf/visibility_control.h"
#include <boost/dll/alias.hpp> // for BOOST_DLL_ALIAS
#include <type_traits>
#include <vector>
#include <string>

namespace gaf
{
    namespace manager
    {
        class ModuleLibInstanceWrapper;
        class AlgManager;
    }

    namespace algorithm
    {

        enum class InterfaceIdentity
        {
            OPENCV_BASED,
            PCL_BASED,
            UNKNOWN
        };

        class HUI_EXPORT AlgorithmCommon
        {
        public:
            virtual ~AlgorithmCommon() {}

            virtual std::pair<bool, std::string> init(const std::vector<std::string> &args)
            {
                return {true, "success"};
            };

            virtual InterfaceIdentity identity()
            {
                return InterfaceIdentity::UNKNOWN;
            };

            virtual std::string name()
            {
                return std::string("UNKNOWN");
            };

            virtual std::string version()
            {
                return std::string("NOT_SET");
            };

            virtual std::string description()
            {
                return std::string("NOT_SET");
            };
        private:

            friend class ::gaf::manager::AlgManager;
            ::gaf::manager::ModuleLibInstanceWrapper* _module_lib;
        };

        template <class T>
        static AlgorithmCommon* create_module()
        {
            static_assert(
                std::is_base_of_v<AlgorithmCommon, T>,
                "type error!");

            return new T();
        }
    }
}

#define CreateAlgorithmModule(class_name) BOOST_DLL_ALIAS( \
    gaf::algorithm::create_module<class_name>,             \
    create_module_##class_name)

#define ModuleName(__module_name)                   \
    static const char *_module_name()               \
    {                                               \
        static const char *s_name = #__module_name; \
        return s_name;                              \
    }                                               \
    BOOST_DLL_ALIAS(_module_name, module_name)
