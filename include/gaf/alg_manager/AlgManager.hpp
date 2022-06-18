/**
 * @file AlgManager.hpp
 * 
 * @author chenhui.yu (yuchenhui153@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-01-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include "gaf/alg/common.hpp"
#include "gaf/visibility_control.h"

namespace gaf
{
    namespace manager
    {
        using CommonRetType = const std::pair<bool, std::string> &;

        class HUI_EXPORT AlgManager
        {
        private:
            ::gaf::algorithm::AlgorithmCommon*
            _get_alg_by_name(const std::string &module_name, const std::string &alg_name);

            static void _alg_deleter(::gaf::algorithm::AlgorithmCommon*);

            class ManagerImpl;
            ManagerImpl *_impl;


        public:
            template<class T> using AlgorithmUnique 
                = std::unique_ptr<T, std::function<void (::gaf::algorithm::AlgorithmCommon*)>>;

            AlgManager();

            virtual ~AlgManager();

            /**
             * @brief load a module from specific path
             * 
             * @param  path             a shared library
             * @return CommonRetType    
             */
            CommonRetType
            loadModule(const std::string &path, std::string &module_name);

            /**
             * @brief load all module from specific directory
             * 
             * @param  path      a directory
             * @param  recursive should recursive
             * @return CommonRetType 
             */
            CommonRetType
            loadAllModules(const std::string &path, std::vector<std::string> &success_path, bool recursive);

            /**
             * @brief try to unload a module
             * 
             * @param name 
             * @return CommonRetType 
             */
            CommonRetType
            unloadModule(const std::string &name);

            /**
             * @brief Get the Loaded Module object
             * 
             * @return std::vector<std::string> 
             */
            std::vector<std::string> getLoadedModule();

            /**
             * @brief Get the Algorithm By Name object
             * 
             * @tparam T            type derived from AlgorithmCommon
             * @param module_name   module's name
             * @param alg_name      algorithm' name
             * @return std::shared_ptr<T>
             */
            template <class T> AlgorithmUnique<T>
            getAlgorithmByName(const std::string &module_name, const std::string &alg_name)
            {
                T* ret_ptr = static_cast<T*>(_get_alg_by_name(module_name, alg_name));
                return AlgorithmUnique<T>( ret_ptr, &AlgManager::_alg_deleter);
            }
        };

    }
}