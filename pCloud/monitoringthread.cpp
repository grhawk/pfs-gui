#include "monitoringthread.h"
#include "pcloudapp.h"

MonitoringThread::MonitoringThread(PCloudApp *a)
{
    app=a;
    connect(this, SIGNAL(sendMessageSignal(QString, QString)), app, SLOT(showTrayMessage(QString, QString)));
    connect(this, SIGNAL(setOnlineStatus(bool)), app, SLOT(setOnlineStatus(bool)));
}

typedef struct {
    uint32_t type;
    uint32_t length;
    char buff[4096];
} msg;

void MonitoringThread::run()
{
    int unsigned cnt;
    msg m;
    sleep(2);
    cnt=0;
    while (1){
        QFile file(app->settings->get("path")+"/.pfs_settings/events");
        app->setOnlineStatus(file.exists());

        if (file.open(QFile::ReadOnly)){
            if (file.read((char *)&m, sizeof(m))>=8){
                if (m.type&1 || cnt>2){
                    m.buff[m.length]=0;
                    app->lastMessageType=m.type;
                    emit sendMessageSignal(m.buff, "");
                }
                file.close();
                continue;
            }
            file.close();
        }
        sleep(1);
        cnt++;
    }
}

