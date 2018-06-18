#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtMqtt/QMqttClient>
#include <QtMqtt/QMqttTopicName>
#include <QtMqtt/QMqttSubscription>
#include <QDebug>
#include <QDateTime>
#include <QLatin1Char>
#include <QLatin1Char>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QMap>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QHostInfo>
#include <unistd.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QMqttClient *m_client;
    QString hostName;
    int     hostPort;
    void pressAddress(QString addr);

private slots:
    void on_btn_connect_clicked(void);      //连接事件槽
    void on_btn_clr_send_buf_click(void);
    void on_btn_clr_recv_buf_click(void);
    void on_btn_exp2file_click(void);
    void on_btn_showtime_click(void);
    void on_btn_showtopic_click(void);
    void on_btn_unsub_all_click(void);
    void on_btn_subtopic_click(void);
    void on_btn_unsubtopic_click(void);
    void on_btn_publish_click(void);
    void on_btn_ping_click(void);

    void updateLogStateChange(QMqttClient::ClientState state);
    void brokenDisconnect(void);
    void topicMessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void pingResponseReceived(void);

    void lookedup(const QHostInfo &info);




};

#endif // MAINWINDOW_H
