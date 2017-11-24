#pragma once
#include "configure_listener.h"
#include <vector>
#include <map>
#include "Poco/Util/TimerTask.h"
#include "Poco/Util/Timer.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/AutoPtr.h"

class ConfigureInterface
{
public:
    virtual void RegisterListener(Listener *listener) = 0;
    virtual void RemoveListener(Listener *listener) = 0;
    virtual void NotifyListeners(std::string propertyType, std::string property) = 0;
protected:
    std::vector<Listener *> _listenerList;
};



class ConfigureClient :public ConfigureInterface, public Poco::Util::TimerTask
{
public:
    ~ConfigureClient();
    virtual void RegisterListener(Listener *listener);
    virtual void RemoveListener(Listener *listener);
    virtual void NotifyListeners(std::string propertyType, std::string property);
    static ConfigureInterface *GetConfigureInstance();    //获得单例

    virtual void run() override;
    std::string LoadAppID(std::string appType);    //根据appType获得appID

protected:
    bool RequestConfiguration(std::string appID, std::string &configuraton);
private:
    ConfigureClient();
    Poco::Util::Timer                      _timer;
    static ConfigureInterface              *_sConfigureInstance;
    std::map<std::string, std::string>     _appIDList;
    std::map<std::string, std::string>     _lastResponse;
    Poco::AutoPtr<Poco::Util::IniFileConfiguration> _pCof;
    std::string                            _apolloServerIp;
    uint16_t                               _apolloServerPort;
    uint32_t                               _requestRate;             //请求更新频率（s）
};