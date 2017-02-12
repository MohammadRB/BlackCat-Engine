/********************************************************************************
** Form generated from reading UI file 'bcBlackCatEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BCBLACKCATEDITOR_H
#define UI_BCBLACKCATEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_bcBlackCatEditorClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QHBoxLayout *mainLayout;
    QFrame *leftFrame;
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QFrame *leftRenderFrame;
    QFrame *leftBottomFrame;
    QHBoxLayout *horizontalLayout_3;
    QTabWidget *leftBottomTab;
    QWidget *consoleTab;
    QFrame *rightFrame;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *bcBlackCatEditorClass)
    {
        if (bcBlackCatEditorClass->objectName().isEmpty())
            bcBlackCatEditorClass->setObjectName(QStringLiteral("bcBlackCatEditorClass"));
        bcBlackCatEditorClass->resize(1399, 809);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(bcBlackCatEditorClass->sizePolicy().hasHeightForWidth());
        bcBlackCatEditorClass->setSizePolicy(sizePolicy);
        bcBlackCatEditorClass->setMinimumSize(QSize(800, 500));
        bcBlackCatEditorClass->setBaseSize(QSize(1200, 750));
        centralWidget = new QWidget(bcBlackCatEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(3, 3, 3, 3);
        mainLayout = new QHBoxLayout();
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        leftFrame = new QFrame(centralWidget);
        leftFrame->setObjectName(QStringLiteral("leftFrame"));
        leftFrame->setFrameShape(QFrame::StyledPanel);
        leftFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(leftFrame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        splitter = new QSplitter(leftFrame);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setOrientation(Qt::Vertical);
        leftRenderFrame = new QFrame(splitter);
        leftRenderFrame->setObjectName(QStringLiteral("leftRenderFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(leftRenderFrame->sizePolicy().hasHeightForWidth());
        leftRenderFrame->setSizePolicy(sizePolicy2);
        leftRenderFrame->setMinimumSize(QSize(0, 400));
        leftRenderFrame->setFrameShape(QFrame::StyledPanel);
        leftRenderFrame->setFrameShadow(QFrame::Raised);
        splitter->addWidget(leftRenderFrame);
        leftBottomFrame = new QFrame(splitter);
        leftBottomFrame->setObjectName(QStringLiteral("leftBottomFrame"));
        sizePolicy1.setHeightForWidth(leftBottomFrame->sizePolicy().hasHeightForWidth());
        leftBottomFrame->setSizePolicy(sizePolicy1);
        leftBottomFrame->setBaseSize(QSize(0, 0));
        leftBottomFrame->setFrameShape(QFrame::StyledPanel);
        leftBottomFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(leftBottomFrame);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(3, 3, 3, 3);
        leftBottomTab = new QTabWidget(leftBottomFrame);
        leftBottomTab->setObjectName(QStringLiteral("leftBottomTab"));
        consoleTab = new QWidget();
        consoleTab->setObjectName(QStringLiteral("consoleTab"));
        leftBottomTab->addTab(consoleTab, QString());

        horizontalLayout_3->addWidget(leftBottomTab);

        splitter->addWidget(leftBottomFrame);

        verticalLayout->addWidget(splitter);


        mainLayout->addWidget(leftFrame);

        rightFrame = new QFrame(centralWidget);
        rightFrame->setObjectName(QStringLiteral("rightFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(rightFrame->sizePolicy().hasHeightForWidth());
        rightFrame->setSizePolicy(sizePolicy3);
        rightFrame->setMinimumSize(QSize(300, 0));
        rightFrame->setFrameShape(QFrame::StyledPanel);
        rightFrame->setFrameShadow(QFrame::Raised);
        rightFrame->setLineWidth(10);

        mainLayout->addWidget(rightFrame);


        horizontalLayout->addLayout(mainLayout);

        bcBlackCatEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(bcBlackCatEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1399, 26));
        bcBlackCatEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(bcBlackCatEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
        mainToolBar->setSizePolicy(sizePolicy4);
        mainToolBar->setMinimumSize(QSize(0, 30));
        mainToolBar->setBaseSize(QSize(0, 0));
        mainToolBar->setMovable(false);
        bcBlackCatEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(bcBlackCatEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        bcBlackCatEditorClass->setStatusBar(statusBar);

        retranslateUi(bcBlackCatEditorClass);

        leftBottomTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(bcBlackCatEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *bcBlackCatEditorClass)
    {
        bcBlackCatEditorClass->setWindowTitle(QApplication::translate("bcBlackCatEditorClass", "BlackCatEditor", 0));
        leftBottomTab->setTabText(leftBottomTab->indexOf(consoleTab), QApplication::translate("bcBlackCatEditorClass", "Console", 0));
    } // retranslateUi

};

namespace Ui {
    class bcBlackCatEditorClass: public Ui_bcBlackCatEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BCBLACKCATEDITOR_H
