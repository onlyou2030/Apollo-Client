#pragma once
#include <string>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Array.h>

class Listener
{
public:
    Listener(std::string appType)
        :_appType(appType),
        _hasUpdated(false)
    {

    }
    virtual ~Listener()
    {

    }
    virtual void Update(std::string property) = 0;
    virtual bool HasUpdated();
    virtual void SetHasUpdated(bool hasUpdated);
    virtual std::string GetAppType();
    virtual std::string GetProperty();
    virtual std::string ParsePropertyStr(std::string item, std::string default = "");
    virtual int64_t ParsePropertyInt(std::string item, int default = 0);
    virtual uint32_t ParsePropertyUint(std::string item, uint32_t default = 0);
protected:
    bool        _hasUpdated;
    std::string _property;
    std::string _appType;
};


class ConfigureListener :public Listener
{
public:
    virtual void Update(std::string property);
    ConfigureListener(std::string appType);
};