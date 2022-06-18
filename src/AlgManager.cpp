#include "gaf/alg_manager/AlgManager.hpp"
#include "gaf/alg/common.hpp"
#include <unordered_map>
#include <boost/dll/import.hpp> // for import_alias
#include <boost/function.hpp>
#include <atomic>
#include <mutex>

namespace gaf{
namespace manager{

typedef const char*                      (module_name_t)();
typedef gaf::algorithm::AlgorithmCommon* (algorithm_creator_t)();

using ModuleLibInstance         = boost::dll::shared_library;
using ModuleLibInstanceUnique   = std::unique_ptr<ModuleLibInstance>;

struct ModuleLibInstanceWrapper
{
    ModuleLibInstanceUnique instance;
    std::atomic<size_t>     using_count;

    bool is_using()
    {
        return using_count != 0;
    }
};

using MWrapperUnique            = std::unique_ptr<ModuleLibInstanceWrapper>;
using ModuleLibMap              = std::unordered_map<std::string, MWrapperUnique>;

static const CommonRetType common_success           {true,  "succss"};
static const CommonRetType module_no_not_exise      {false, "module not exist"};
static const CommonRetType module_no_function       {false, "module no function"};
static const CommonRetType module_has_loaded        {false, "module has loaded"};
static const CommonRetType module_not_loaded        {false, "module nor loaded"};
static const CommonRetType module_not_been_loaded   {false, "module not been loaded"};
static const CommonRetType module_is_using          {false, "module is using"};
static const CommonRetType directory_not_exist      {false, "directory not exist"};
static const CommonRetType unknown_error            {false, "unknown error"};

namespace fs = boost::filesystem;

class AlgManager::ManagerImpl
{
    friend class AlgManager;
public:
    ManagerImpl(AlgManager* _owner)
    :owner(_owner){}

    CommonRetType load_module(const fs::path& module_path, std::string& module_name)
    {
        /* default name of module name function */
        static const char* name_of_module_name = "module_name";
        MWrapperUnique instance_wrapper = std::make_unique<ModuleLibInstanceWrapper>();

        auto absolute_path = boost::filesystem::absolute(module_path);

        if(!boost::filesystem::exists(absolute_path))
            return module_no_not_exise;

        /* create module lib instance */
        instance_wrapper->instance = std::make_unique<ModuleLibInstance>(
            absolute_path, 
            boost::dll::load_mode::append_decorations
        );

        /* no name function */
        if(!instance_wrapper->instance->has(name_of_module_name)) 
            return module_no_function;

        /* get module name reader */
        boost::function<module_name_t> module_name_reader = 
            instance_wrapper->instance->get_alias<module_name_t>(name_of_module_name);

        /* get name */
        module_name = module_name_reader();

        /* is exist */
        if(module_lib_instance_map.count(module_name)) 
            return module_has_loaded;

        {
            std::scoped_lock lck(map_mutex);
            module_lib_instance_map[module_name] = std::move(instance_wrapper);
        }

        return common_success;
    }

    CommonRetType unloadModule(const std::string& name)
    {
        std::scoped_lock lck(map_mutex);

        if(!module_lib_instance_map.count(name))        return module_not_been_loaded;
        if( module_lib_instance_map[name]->is_using())  return module_is_using;

        module_lib_instance_map.erase(name);
        return common_success;
    }

    gaf::algorithm::AlgorithmCommon*
    getAlgorithmByName(const std::string& module_name, const std::string& alg_name)
    {
        std::scoped_lock lck(map_mutex);

        if(!module_lib_instance_map.count(module_name)) 
            return nullptr;

        auto& instance_wrapper = module_lib_instance_map[module_name];

        auto real_name = "create_module_" + alg_name;

        if(!instance_wrapper->instance->has(real_name))
            return nullptr;

        /* get alg creator */
        boost::function<algorithm_creator_t> algorithm_creator = 
            instance_wrapper->instance->get_alias<algorithm_creator_t>(real_name);

        auto algorithm_instance = algorithm_creator();
        instance_wrapper->using_count++;
        algorithm_instance->_module_lib = instance_wrapper.get();

        return algorithm_instance;
    }

    bool isSharedLibrary(const fs::path& path)
    {
        auto file_ext = path.extension();
        return file_ext == ".so" || file_ext == ".dll";
    }

    std::vector<fs::path> findAllLibraryRecursive(const fs::path& root)
    {
        std::vector<fs::path> paths;
        for (auto const & entry : fs::recursive_directory_iterator(root))
        {
            if (fs::is_regular_file(entry) && isSharedLibrary(entry.path()))
                paths.emplace_back(entry.path());
        }
        return paths;
    }

    std::vector<fs::path> findAllLibrary(const fs::path& root)
    {
        std::vector<fs::path> paths;
        for (auto const & entry : fs::directory_iterator(root))
        {
            if (fs::is_regular_file(entry) && isSharedLibrary(entry.path()))
                paths.emplace_back(entry.path().filename());
        }
        return paths;
    }

    std::vector<std::string> getLoadedModule()
    {
        std::vector<std::string> ret_vector;
        std::scoped_lock lck(map_mutex);

        for(const auto& elem : module_lib_instance_map)
        {
            ret_vector.push_back(elem.first);
        }
        return ret_vector;
    }

private:
    AlgManager*         owner;
    std::mutex          map_mutex;
    ModuleLibMap        module_lib_instance_map;
};

AlgManager::AlgManager()
{
    _impl = new ManagerImpl(this);
}

AlgManager::~AlgManager()
{
    delete _impl;
}

CommonRetType 
AlgManager::loadModule(const std::string& path, std::string& module_name)
{
    return _impl->load_module(fs::path(path), module_name);
}

CommonRetType
AlgManager::loadAllModules(const std::string& path, std::vector<std::string>& success_path, bool recursive)
{
    if(!fs::exists(path) || !fs::is_directory(path)) return directory_not_exist;

    auto list_of_file = recursive ? 
        _impl->findAllLibraryRecursive(path) : _impl->findAllLibrary(path);

    std::string loaded_module_name;

    for(const auto& lib_path : list_of_file)
    {
        auto ret = _impl->load_module(lib_path, loaded_module_name);
        if(ret.first)
        {
            success_path.push_back(loaded_module_name);
        }
    }

    return common_success;
}

CommonRetType 
AlgManager::unloadModule(const std::string& name)
{
    return _impl->unloadModule(name);
}

gaf::algorithm::AlgorithmCommon*
AlgManager::_get_alg_by_name(const std::string& module_name, const std::string& alg_name)
{
    return _impl->getAlgorithmByName(module_name, alg_name);
}

std::vector<std::string> AlgManager::getLoadedModule()
{
    return _impl->getLoadedModule();
}

void AlgManager::_alg_deleter(::gaf::algorithm::AlgorithmCommon* ptr)
{
    ptr->_module_lib->using_count--;
    delete ptr;
}

}}
