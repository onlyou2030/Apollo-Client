#include "configure_listener.h"
#include "configure_client.h"




int main(int argc, char** argv)
{
    ConfigureInterface *configureClient = ConfigureClient::GetConfigureInstance();
    Listener *audioListener = new ConfigureListener("audioAppID");
    Listener *videoListener = new ConfigureListener("videoAppID");
    configureClient->RegisterListener(audioListener);
    configureClient->RegisterListener(videoListener);
    while (1)
    {
        if (audioListener->HasUpdated())
        {
            std::cout << audioListener->GetProperty() << std::endl << std::endl;
            audioListener->SetHasUpdated(false);
        }
        if (videoListener->HasUpdated())
        {
            std::cout << videoListener->GetProperty() << std::endl << std::endl;
            videoListener->SetHasUpdated(false);
        }
    }
    return 0;
}