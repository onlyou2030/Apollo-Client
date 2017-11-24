#include "configure_client.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPSessionFactory.h"
#include "Poco/Net/HTTPSessionInstantiator.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/StreamCopier.h"

ConfigureInterface *ConfigureClient::_sConfigureInstance = NULL;

ConfigureClient::ConfigureClient()
{
    try
    {
        _pCof = new Poco::Util::IniFileConfiguration("app.ini");
        _apolloServerIp = _pCof->getString("apolloServerIp");
        _apolloServerPort = _pCof->getUInt("apolloServerPort");
        _requestRate = _pCof->getUInt("requestRate");
    }
    catch (Poco::Exception& e)
    {
        exit(e.code());
    }
    _timer.schedule(this, 0, _requestRate*1000);
}

ConfigureClient::~ConfigureClient()
{
    _timer.cancel(true);
}

ConfigureInterface* ConfigureClient::GetConfigureInstance()
{
    if (!_sConfigureInstance)
    {
        _sConfigureInstance = new ConfigureClient();
    }
    return _sConfigureInstance;
}

void ConfigureClient::RegisterListener(Listener *listener)
{
    if (listener)
    {
        _listenerList.push_back(listener);
        std::string appType = listener->GetAppType();
        if (_appIDList.find(appType) == _appIDList.end())
        {
            std::string appID;
            try
            {
                appID = _pCof->getString(appType);
            }
            catch (Poco::Exception& e)
            {
                exit(e.code());
            }
            _appIDList.insert(std::pair<std::string, std::string>(appType, appID));
            while (!listener->HasUpdated());//等待获得更新
        }
    }
}

void ConfigureClient::RemoveListener(Listener *listener)
{
    std::vector<Listener *>::iterator it = _listenerList.begin();
    for (; it != _listenerList.end(); ++it)
    {
        if (*it == listener)
        {
            it = _listenerList.erase(it);
            return;
        }
    }
}

void ConfigureClient::NotifyListeners(std::string propertyType,std::string property)
{
    std::vector<Listener *>::iterator it = _listenerList.begin();
    for (; it != _listenerList.end(); ++it)
    {
        if ((*it)->GetAppType() == propertyType)
        {
            (*it)->Update(property);
            (*it)->SetHasUpdated(true);
        }
    }
}


void ConfigureClient::run()
{
    std::map<std::string, std::string>::iterator it = _appIDList.begin();
    for (; it != _appIDList.end(); ++it)
    {
        std::string tempResponse;
        if (RequestConfiguration(it->second, tempResponse))  //有最新数据
        {
            for (int i = 0; i < _listenerList.size(); i++)
            {
                if (_listenerList[i]->GetAppType() == it->first) //通知类型相符的
                {
                    _listenerList[i]->Update(tempResponse);
                    _listenerList[i]->SetHasUpdated(true);
                }
            }
        }
    }
}

std::string ConfigureClient::LoadAppID(std::string appType)
{
    if (_appIDList.find(appType) == _appIDList.end())
    {
        return "";
    }
    return _appIDList[appType];
}

bool ConfigureClient::RequestConfiguration(std::string appID, std::string &configuraton)
{
    std::string url = "/configs/" + appID + "/default/application?ip=" + _apolloServerIp;
    Poco::Net::HTTPClientSession session(_apolloServerIp, _apolloServerPort);
    Poco::Net::HTTPRequest httpRequest(Poco::Net::HTTPRequest::HTTP_GET, url, Poco::Net::HTTPMessage::HTTP_1_1);
    Poco::Net::HTTPResponse httpResponse;

    session.sendRequest(httpRequest);
    std::istream &rs = session.receiveResponse(httpResponse);
    if (httpResponse.getStatus() != 200)
    {
        return false;
    }
    Poco::StreamCopier::copyToString(rs, configuraton);
    if (_lastResponse.find(appID) == _lastResponse.end())
    {
        _lastResponse.insert(std::pair<std::string, std::string>(appID, configuraton));
        return true;
    }
    if (_lastResponse[appID] != configuraton)
    {
        _lastResponse[appID] = configuraton;
        return true;
    }
    return false;
}

