### 一、客户端设计

#### 1.实现原理

![客户端架构](https://github.com/onlyou2030/Apollo-Client/blob/master/image/%E5%88%9B%E5%BB%BA%E9%A1%B9%E7%9B%AE%E6%88%AA%E5%9B%BE.png)

客户端实现原理如下：

1.客户端会定时从Apollo配置中心拉取最新配置，并通知给观察对象。

2.应用程序可以从Apollo客户端获取最新的配置、订阅配置更新通知。

#### 2.客户端框架设计

![客户端UML](image\客户端UML.png)

客户端框架基于观察者模式实现。

1.ConfigureClient会定期从Apollo配置中心拉取最新配置，并与上一次获得的配置进行比较，如果有更新，则会回调用应的Listener的Update接口，以及SetHasUpdated接口。

2.ConfigureListener实现的Update接口中只是将配置进行更新，并没有执行其他操作，因此应用程序如果想获得最新的配置，需要轮询HasUpdated接口。

3.如果可以实现自定义的CustomizeListener，重写接口Update即可。在该接口中，可以更新配置，同时执行自定义操作。这样，就不需要轮询HasUpdated接口。

4.用户可以在app.ini配置文件中设置拉取配置的频率。



### 三、客户端的使用

####1.配置文件

假设app.ini配置文件内容如下：

```
apolloServerIp = 172.18.70.54
apolloServerPort = 33410
requestRate = 5

confAppID = 172-18-156-31-conf
audioAppID = 172-18-156-31-audio
videoAppID = 172-18-156-31-video
dataAppID = 172-18-156-31-data
```

其中apolloServerIp和apolloServerPort分别是Apollo配置中心的IP和端口，requestRate是客户端请求更新的频率，单位为秒（s）。这三个参数是必须要有的。

####2.应用ID

应用ID在配置文件中的存储格式为：

```
appType=YOUR-APP-ID
```



这样客户端可以通过用户类别获得应用类别（appType）获得该主机该应用的ID（appID）。用户可以根据实际的应用来进行添加。

注意：appID是用来识别应用身份的唯一ID，且必须与配置中心设置的AppId一致。在Apollo配置中心创建项目时，填入的应用Id必须与app.ini里面的一致。（后续可用CMS来进行管理）

![创建项目截图](image\创建项目截图.png)

#### 3.客户端API使用

#####注册观察者

（1）首先获得ApolloClient的单例。

```
ConfigureInterface *configure = ConfigureClient::GetConfigureInstance();
```

（2）接着创建一个观察者，创建时需要传入观察的应用类别（appType），需要与app.ini中的appType一致。

```
Listener *confListener = new ConfigureListener("confAppID");
```

（3）将创建的观察者注册到ApolloClient中。

```
configure->RegisterListener(confListener);
```

##### 获得全部配置信息

如果想获得所有配置信息，可以调用以下接口获得。

```
std::string property = confListener->GetProperty();
```

注意：调用此接口返回的是JSON格式的字符串，返回内容的sample如下，需要进行解析。

```
{
  "appId": "100004458",
  "cluster": "default",
  "namespaceName": "application",
  "configurations": {
    "portal.elastic.document.type":"biz",
    "portal.elastic.cluster.name":"hermes-es-fws"
  },
  "releaseKey": "20170430092936-dee2d58e74515ff3"
}
```

##### 获得单个配置信息

如果只想获得单个配置信息，且返回字符串结果，可以调用以下接口，即可获得对应的值。

```
confListener->ParsePropertyStr("monitor");
```

除了返回字符串外，还支持返回int64和unsigned int32，分别对应如下接口。

```
confListener->ParsePropertyInt("ConfServerID");
confListener->ParsePropertyUint("monitorPort");
```

#### 4.应用端实时获得最新配置

目前应用端实时获得最新配置，有两种方法：

（1）对Listener的HasUpdated进行轮询，有更新的话就执行相应操作。

```
while(1)
{
   if(confListener->HasUpdated())
   {
      //执行更新后的操作
   }
}
```

（2）继承Listener类，自己实现Update方法，让Apollo客户端回调Update。

````
class MyListener :public Listener
{
public:
    virtual void Update(std::string property);
    {
        _property = property;
        //执行更新后的操作
    }
    MyListener(std::string appType)
        :Listener(appType)
    {

    }
};
````

