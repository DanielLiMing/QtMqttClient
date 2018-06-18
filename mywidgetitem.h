#ifndef MYWIDGETITEM_H
#define MYWIDGETITEM_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>

class MyWidgetItem : public QWidget
{
    Q_OBJECT
public:
    explicit MyWidgetItem(QWidget *parent = nullptr);

public:
    //QLabel *label;
    QLineEdit *text;
    QPushButton *btn_del;

signals:

public slots:
};

#endif // MYWIDGETITEM_H
