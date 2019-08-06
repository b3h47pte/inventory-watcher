#pragma once
#include <boost/property_tree/ptree.hpp>
#include <memory>
#include <string>

namespace core
{

class Config
{
public:
    static const Config& get();

    template<typename T>
    T getVal(const std::string& k1) const;

protected:
    explicit Config(const std::string& fname);
    boost::property_tree::ptree _data;
};

template<typename T>
T
Config::getVal(const std::string& k1) const
{
    return _data.get<T>(k1);
}

}
