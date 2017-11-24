#include "configure_listener.h"
#include <iostream>


std::string Listener::GetProperty()
{
    return _property;
}

std::string Listener::GetAppType()
{
    return _appType;
}

bool Listener::HasUpdated()
{
    return _hasUpdated;
}

void Listener::SetHasUpdated(bool hasUpdated)
{
    _hasUpdated = hasUpdated;
}

std::string Listener::ParsePropertyStr(std::string item, std::string default /* = "" */)
{
    std::string re;
    if (_property.empty())
    {
        return default;
    }
    Poco::JSON::Parser parser;
    Poco::Dynamic::Var json = parser.parse(_property);
    Poco::JSON::Object::Ptr pObj = json.extract<Poco::JSON::Object::Ptr>();
    Poco::Dynamic::Var subJson = pObj->get("configurations");
    if (subJson.isEmpty())
    {
        return default;
    }
    Poco::JSON::Object::Ptr subObj = subJson.extract<Poco::JSON::Object::Ptr>();
    Poco::Dynamic::Var tempVar = subObj->get(item);
    if (tempVar.isEmpty())
    {
        return default;
    }
    re = tempVar.toString();
    return re;
}

int64_t Listener::ParsePropertyInt(std::string item, int default /* = 0 */)
{
    std::string str = ParsePropertyStr(item);
    if (str == "")
    {
        return default;
    }
    return Poco::NumberParser::parse64(str);
}

uint32_t Listener::ParsePropertyUint(std::string item, uint32_t default /* = 0 */)
{
    std::string str= ParsePropertyStr(item);
    if (str == "")
    {
        return default;
    }
    return Poco::NumberParser::parseUnsigned(str);
}


ConfigureListener::ConfigureListener(std::string appType)
    :Listener(appType)
{

}

void ConfigureListener::Update(std::string property)
{
    _property = property;
    //do something
}