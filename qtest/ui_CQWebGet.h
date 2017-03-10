/********************************************************************************
** Form generated from reading UI file 'CQWebGet.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CQWEBGET_H
#define UI_CQWEBGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QWebGet
{
public:
    QAction *action_Quit;
    QAction *action_Help;
    QWidget *centralwidget;
    QVBoxLayout *vboxLayout;
    QVBoxLayout *vboxLayout1;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QLineEdit *urlText;
    QTreeWidget *urlTree;
    QMenuBar *menubar;
    QMenu *menu_Help;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *QWebGet)
    {
        if (QWebGet->objectName().isEmpty())
            QWebGet->setObjectName(QStringLiteral("QWebGet"));
        QWebGet->resize(777, 572);
        action_Quit = new QAction(QWebGet);
        action_Quit->setObjectName(QStringLiteral("action_Quit"));
        action_Help = new QAction(QWebGet);
        action_Help->setObjectName(QStringLiteral("action_Help"));
        centralwidget = new QWidget(QWebGet);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        vboxLayout = new QVBoxLayout(centralwidget);
#ifndef Q_OS_MAC
        vboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout->setContentsMargins(9, 9, 9, 9);
#endif
        vboxLayout->setObjectName(QStringLiteral("vboxLayout"));
        vboxLayout1 = new QVBoxLayout();
#ifndef Q_OS_MAC
        vboxLayout1->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        vboxLayout1->setContentsMargins(0, 0, 0, 0);
#endif
        vboxLayout1->setObjectName(QStringLiteral("vboxLayout1"));
        hboxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
        hboxLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        hboxLayout->setContentsMargins(0, 0, 0, 0);
#endif
        hboxLayout->setObjectName(QStringLiteral("hboxLayout"));
        label = new QLabel(centralwidget);
        label->setObjectName(QStringLiteral("label"));

        hboxLayout->addWidget(label);

        urlText = new QLineEdit(centralwidget);
        urlText->setObjectName(QStringLiteral("urlText"));

        hboxLayout->addWidget(urlText);


        vboxLayout1->addLayout(hboxLayout);

        urlTree = new QTreeWidget(centralwidget);
        urlTree->setObjectName(QStringLiteral("urlTree"));
        urlTree->setColumnCount(2);

        vboxLayout1->addWidget(urlTree);


        vboxLayout->addLayout(vboxLayout1);

        QWebGet->setCentralWidget(centralwidget);
        menubar = new QMenuBar(QWebGet);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 777, 27));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QStringLiteral("menu_Help"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        QWebGet->setMenuBar(menubar);
        statusbar = new QStatusBar(QWebGet);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        statusbar->setGeometry(QRect(0, 550, 777, 22));
        QWebGet->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menu_Help->addAction(action_Help);
        menuFile->addAction(action_Quit);

        retranslateUi(QWebGet);
        QObject::connect(action_Quit, SIGNAL(triggered()), QWebGet, SLOT(close()));
        QObject::connect(urlText, SIGNAL(returnPressed()), QWebGet, SLOT(load()));

        QMetaObject::connectSlotsByName(QWebGet);
    } // setupUi

    void retranslateUi(QMainWindow *QWebGet)
    {
        QWebGet->setWindowTitle(QApplication::translate("QWebGet", "WebGet", 0));
        action_Quit->setText(QApplication::translate("QWebGet", "&Quit", 0));
        action_Help->setText(QApplication::translate("QWebGet", "&Help", 0));
        label->setText(QApplication::translate("QWebGet", "Url", 0));
        QTreeWidgetItem *___qtreewidgetitem = urlTree->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("QWebGet", "1", 0));
        ___qtreewidgetitem->setText(0, QApplication::translate("QWebGet", "0", 0));
        menu_Help->setTitle(QApplication::translate("QWebGet", "&Help", 0));
        menuFile->setTitle(QApplication::translate("QWebGet", "&File", 0));
    } // retranslateUi

};

namespace Ui {
    class QWebGet: public Ui_QWebGet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CQWEBGET_H
