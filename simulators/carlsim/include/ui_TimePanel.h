/********************************************************************************
** Form generated from reading UI file 'TimePanel.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIMEPANEL_H
#define UI_TIMEPANEL_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
//#include <QtGui/QAction>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

QT_BEGIN_NAMESPACE

class Ui_TimePanel
{
public:
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QSpinBox *stepMsEdit;
    QDoubleSpinBox *speedEdit;
    QWidget *widget_2;
    QLabel *realSpeed;
    QLabel *ssnTimeLabel;
    QLineEdit *snnTimeDiff;
    QLabel *modelSpeed;
    QLabel *snnSpeed;
    QLabel *realTimeLabel;
    QLabel *stepCounter;
    QLabel *modelTimeLabel;
    QDateTimeEdit *modelTimeEdit;
    QDateTimeEdit *realTimeEdit;
    QLineEdit *modelTimeDiff;
    QLineEdit *realTimeDiff;
    QPushButton *pushButton;
    QLabel *stepLabel;

    void setupUi(QWidget *TimePanel)
    {
        if (TimePanel->objectName().isEmpty())
            TimePanel->setObjectName(QString::fromUtf8("TimePanel"));
        TimePanel->setEnabled(true);
        TimePanel->resize(526, 91);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TimePanel->sizePolicy().hasHeightForWidth());
        TimePanel->setSizePolicy(sizePolicy);
        TimePanel->setMinimumSize(QSize(1, 91));
        TimePanel->setMaximumSize(QSize(16777215, 91));
        horizontalLayout = new QHBoxLayout(TimePanel);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        widget = new QWidget(TimePanel);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy1);
        widget->setMaximumSize(QSize(90, 16777215));
        widget->setAutoFillBackground(true);
        widget->setStyleSheet(QString::fromUtf8(""));
        stepMsEdit = new QSpinBox(widget);
        stepMsEdit->setObjectName(QString::fromUtf8("stepMsEdit"));
        stepMsEdit->setGeometry(QRect(10, 30, 71, 22));
        stepMsEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        stepMsEdit->setMaximum(10000);
		//stepMsEdit->setValue(50);        
		stepMsEdit->setValue(1);
        speedEdit = new QDoubleSpinBox(widget);
        speedEdit->setObjectName(QString::fromUtf8("speedEdit"));
        speedEdit->setGeometry(QRect(10, 60, 71, 22));
        speedEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        speedEdit->setMinimum(0.001);
        speedEdit->setMaximum(1000);
        speedEdit->setSingleStep(0.001);  
        speedEdit->setValue(0.001);
		speedEdit->setDecimals(3); 

        horizontalLayout->addWidget(widget);

        widget_2 = new QWidget(TimePanel);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        QSizePolicy sizePolicy2(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(widget_2->sizePolicy().hasHeightForWidth());
        widget_2->setSizePolicy(sizePolicy2);
        widget_2->setMinimumSize(QSize(100, 0));
        widget_2->setAutoFillBackground(false);
        realSpeed = new QLabel(widget_2);
        realSpeed->setObjectName(QString::fromUtf8("realSpeed"));
        realSpeed->setGeometry(QRect(355, 60, 71, 22));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(realSpeed->sizePolicy().hasHeightForWidth());
        realSpeed->setSizePolicy(sizePolicy3);
        realSpeed->setMinimumSize(QSize(0, 22));
        realSpeed->setMaximumSize(QSize(120, 22));
        realSpeed->setStyleSheet(QString::fromUtf8("QLabel { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        realSpeed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ssnTimeLabel = new QLabel(widget_2);
        ssnTimeLabel->setObjectName(QString::fromUtf8("ssnTimeLabel"));
        ssnTimeLabel->setGeometry(QRect(20, 1, 20, 21));
        ssnTimeLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/neuralnetwork")));
        snnTimeDiff = new QLineEdit(widget_2);
        snnTimeDiff->setObjectName(QString::fromUtf8("snnTimeDiff"));
        snnTimeDiff->setGeometry(QRect(217, 1, 141, 22));
        sizePolicy3.setHeightForWidth(snnTimeDiff->sizePolicy().hasHeightForWidth());
        snnTimeDiff->setSizePolicy(sizePolicy3);
        snnTimeDiff->setMinimumSize(QSize(0, 0));
        snnTimeDiff->setStyleSheet(QString::fromUtf8("QLineEdit { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        snnTimeDiff->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        snnTimeDiff->setReadOnly(true);
        modelSpeed = new QLabel(widget_2);
        modelSpeed->setObjectName(QString::fromUtf8("modelSpeed"));
        modelSpeed->setGeometry(QRect(355, 31, 71, 22));
        sizePolicy3.setHeightForWidth(modelSpeed->sizePolicy().hasHeightForWidth());
        modelSpeed->setSizePolicy(sizePolicy3);
        modelSpeed->setMinimumSize(QSize(0, 22));
        modelSpeed->setMaximumSize(QSize(120, 22));
        modelSpeed->setStyleSheet(QString::fromUtf8("QLabel { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        modelSpeed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        snnSpeed = new QLabel(widget_2);
        snnSpeed->setObjectName(QString::fromUtf8("snnSpeed"));
        snnSpeed->setGeometry(QRect(355, 1, 71, 22));
        sizePolicy3.setHeightForWidth(snnSpeed->sizePolicy().hasHeightForWidth());
        snnSpeed->setSizePolicy(sizePolicy3);
        snnSpeed->setMinimumSize(QSize(0, 22));
        snnSpeed->setMaximumSize(QSize(120, 22));
        snnSpeed->setStyleSheet(QString::fromUtf8("QLabel { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        snnSpeed->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        realTimeLabel = new QLabel(widget_2);
        realTimeLabel->setObjectName(QString::fromUtf8("realTimeLabel"));
        realTimeLabel->setGeometry(QRect(20, 61, 21, 20));
        realTimeLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/system")));
        stepCounter = new QLabel(widget_2);
        stepCounter->setObjectName(QString::fromUtf8("stepCounter"));
        stepCounter->setGeometry(QRect(100, 1, 120, 22));
        sizePolicy3.setHeightForWidth(stepCounter->sizePolicy().hasHeightForWidth());
        stepCounter->setSizePolicy(sizePolicy3);
        stepCounter->setMinimumSize(QSize(120, 22));
        stepCounter->setMaximumSize(QSize(120, 22));
        stepCounter->setStyleSheet(QString::fromUtf8("QLabel { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        stepCounter->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        modelTimeLabel = new QLabel(widget_2);
        modelTimeLabel->setObjectName(QString::fromUtf8("modelTimeLabel"));
        modelTimeLabel->setGeometry(QRect(20, 31, 21, 20));
        modelTimeLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/model")));
        modelTimeEdit = new QDateTimeEdit(widget_2);
        modelTimeEdit->setObjectName(QString::fromUtf8("modelTimeEdit"));
        modelTimeEdit->setGeometry(QRect(42, 31, 178, 22));
        modelTimeEdit->setMinimumSize(QSize(178, 0));
        modelTimeEdit->setLayoutDirection(Qt::LeftToRight);
        modelTimeEdit->setStyleSheet(QString::fromUtf8("QDateTimeEdit { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        modelTimeEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        modelTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        modelTimeEdit->setDate(QDate(2013, 6, 1));
        modelTimeEdit->setTime(QTime(22, 30, 15));
        modelTimeEdit->setCurrentSection(QDateTimeEdit::YearSection);
        realTimeEdit = new QDateTimeEdit(widget_2);
        realTimeEdit->setObjectName(QString::fromUtf8("realTimeEdit"));
        realTimeEdit->setGeometry(QRect(42, 61, 178, 22));
        realTimeEdit->setMinimumSize(QSize(178, 0));
        realTimeEdit->setLayoutDirection(Qt::LeftToRight);
        realTimeEdit->setStyleSheet(QString::fromUtf8("QDateTimeEdit { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        realTimeEdit->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        realTimeEdit->setButtonSymbols(QAbstractSpinBox::NoButtons);
        realTimeEdit->setDate(QDate(2013, 6, 1));
        realTimeEdit->setTime(QTime(22, 30, 15));
        realTimeEdit->setCurrentSection(QDateTimeEdit::YearSection);
        modelTimeDiff = new QLineEdit(widget_2);
        modelTimeDiff->setObjectName(QString::fromUtf8("modelTimeDiff"));
        modelTimeDiff->setGeometry(QRect(217, 31, 141, 22));
        sizePolicy3.setHeightForWidth(modelTimeDiff->sizePolicy().hasHeightForWidth());
        modelTimeDiff->setSizePolicy(sizePolicy3);
        modelTimeDiff->setMinimumSize(QSize(0, 0));
        modelTimeDiff->setStyleSheet(QString::fromUtf8("QLineEdit { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        modelTimeDiff->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        modelTimeDiff->setReadOnly(true);
        realTimeDiff = new QLineEdit(widget_2);
        realTimeDiff->setObjectName(QString::fromUtf8("realTimeDiff"));
        realTimeDiff->setGeometry(QRect(217, 60, 141, 22));
        sizePolicy3.setHeightForWidth(realTimeDiff->sizePolicy().hasHeightForWidth());
        realTimeDiff->setSizePolicy(sizePolicy3);
        realTimeDiff->setMinimumSize(QSize(0, 0));
        realTimeDiff->setStyleSheet(QString::fromUtf8("QLineEdit { margin-left: 5px; background-color: black; color: red; border-color: #555555; border-width: 2px; border-style: inset; border-radius: 4; font-weight: bold;}"));
        realTimeDiff->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        realTimeDiff->setReadOnly(true);
        pushButton = new QPushButton(widget_2);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(0, 0, 12, 81));
        pushButton->setIconSize(QSize(12, 16));
        pushButton->setCheckable(true);
        pushButton->setChecked(false);
        stepLabel = new QLabel(widget_2);
        stepLabel->setObjectName(QString::fromUtf8("stepLabel"));
        stepLabel->setGeometry(QRect(80, 0, 21, 21));
        stepLabel->setPixmap(QPixmap(QString::fromUtf8(":/icon/step")));

        horizontalLayout->addWidget(widget_2);


        retranslateUi(TimePanel);
        QObject::connect(pushButton, SIGNAL(clicked(bool)), widget, SLOT(setVisible(bool)));

        QMetaObject::connectSlotsByName(TimePanel);
    } // setupUi

    void retranslateUi(QWidget *TimePanel)
    {
        TimePanel->setWindowTitle(QApplication::translate("TimePanel", "Form")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        stepMsEdit->setToolTip(QApplication::translate("TimePanel", "Precision (ms/step)")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        stepMsEdit->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        stepMsEdit->setSuffix(QApplication::translate("TimePanel", " ms")); // , 0, QApplication::UnicodeUTF8));
        stepMsEdit->setPrefix(QString());
#ifndef QT_NO_TOOLTIP
        speedEdit->setToolTip(QApplication::translate("TimePanel", "Speedfactor (Modelime / Realtime)")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        speedEdit->setPrefix(QApplication::translate("TimePanel", "x")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        realSpeed->setToolTip(QApplication::translate("TimePanel", "No. of Neural Processing Steps")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        realSpeed->setText(QApplication::translate("TimePanel", "x 0.0")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ssnTimeLabel->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        ssnTimeLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        snnTimeDiff->setToolTip(QApplication::translate("TimePanel", "<html><head/><body><p>Internal Time of the Spiking Neural Network <a href=\"http://google.com\"><span style=\" text-decoration: underline; color:#0000ff;\">Google</span></a></p></body></html>")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        snnTimeDiff->setText(QApplication::translate("TimePanel", "00.0  00:00:00.000")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        modelSpeed->setToolTip(QApplication::translate("TimePanel", "No. of Neural Processing Steps")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        modelSpeed->setText(QApplication::translate("TimePanel", "x 0.0")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        snnSpeed->setToolTip(QApplication::translate("TimePanel", "No. of Neural Processing Steps")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        snnSpeed->setText(QApplication::translate("TimePanel", "x 0.0")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        realTimeLabel->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        realTimeLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        stepCounter->setToolTip(QApplication::translate("TimePanel", "No. of Neural Processing Steps")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        stepCounter->setText(QApplication::translate("TimePanel", "35,503")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        modelTimeLabel->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        modelTimeLabel->setText(QString());
#ifndef QT_NO_TOOLTIP
        modelTimeEdit->setToolTip(QApplication::translate("TimePanel", "External Time of the underlying Model")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        modelTimeEdit->setDisplayFormat(QApplication::translate("TimePanel", "yyyy-MM-dd hh:mm:ss.zzz")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        realTimeEdit->setToolTip(QApplication::translate("TimePanel", "Systemtime (Local and Processor)")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        realTimeEdit->setDisplayFormat(QApplication::translate("TimePanel", "yyyy-MM-dd hh:mm:ss.zzz")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        modelTimeDiff->setToolTip(QApplication::translate("TimePanel", "<html><head/><body><p>Internal Time of the Spiking Neural Network <a href=\"http://google.com\"><span style=\" text-decoration: underline; color:#0000ff;\">Google</span></a></p></body></html>")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        modelTimeDiff->setText(QApplication::translate("TimePanel", "00.0  00:00:00.000")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        realTimeDiff->setToolTip(QApplication::translate("TimePanel", "<html><head/><body><p>Internal Time of the Spiking Neural Network <a href=\"http://google.com\"><span style=\" text-decoration: underline; color:#0000ff;\">Google</span></a></p></body></html>")); // , 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        realTimeDiff->setText(QApplication::translate("TimePanel", "00.0  00:00:00.000")); // , 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("TimePanel", "<")); // , 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        stepLabel->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        stepLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TimePanel: public Ui_TimePanel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIMEPANEL_H
