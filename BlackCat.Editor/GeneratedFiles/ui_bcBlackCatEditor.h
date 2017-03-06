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
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QToolBox>
#include <QtWidgets/QToolButton>
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
    QVBoxLayout *verticalLayout_2;
    QToolBox *rightToolBox;
    QWidget *rightToolBoxTerrain;
    QFormLayout *formLayout;
    QLabel *terrainHeightLable;
    QLabel *terrainHeightValue;
    QLabel *terrainSmoothLable;
    QLabel *terrainSmoothValue;
    QSlider *terrainSmoothSlider;
    QLabel *terrainRadiusLable;
    QLabel *terrainRadiusValue;
    QSlider *terrainRadiusSlider;
    QSlider *terrainHeightSlider;
    QWidget *page_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *toolsDock;
    QWidget *toolsDockContents;
    QVBoxLayout *verticalLayout_3;
    QToolButton *objectSelectButton;
    QToolButton *terrainHeightButton;
    QToolButton *terrainSmoothButton;
    QToolButton *toolButton_3;
    QToolButton *toolButton;

    void setupUi(QMainWindow *bcBlackCatEditorClass)
    {
        if (bcBlackCatEditorClass->objectName().isEmpty())
            bcBlackCatEditorClass->setObjectName(QStringLiteral("bcBlackCatEditorClass"));
        bcBlackCatEditorClass->resize(1600, 900);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(bcBlackCatEditorClass->sizePolicy().hasHeightForWidth());
        bcBlackCatEditorClass->setSizePolicy(sizePolicy);
        bcBlackCatEditorClass->setMinimumSize(QSize(1600, 900));
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
        rightFrame->setMinimumSize(QSize(360, 0));
        rightFrame->setFrameShape(QFrame::StyledPanel);
        rightFrame->setFrameShadow(QFrame::Raised);
        rightFrame->setLineWidth(10);
        verticalLayout_2 = new QVBoxLayout(rightFrame);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        rightToolBox = new QToolBox(rightFrame);
        rightToolBox->setObjectName(QStringLiteral("rightToolBox"));
        rightToolBoxTerrain = new QWidget();
        rightToolBoxTerrain->setObjectName(QStringLiteral("rightToolBoxTerrain"));
        rightToolBoxTerrain->setGeometry(QRect(0, 0, 336, 721));
        formLayout = new QFormLayout(rightToolBoxTerrain);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QStringLiteral("formLayout"));
        terrainHeightLable = new QLabel(rightToolBoxTerrain);
        terrainHeightLable->setObjectName(QStringLiteral("terrainHeightLable"));

        formLayout->setWidget(0, QFormLayout::LabelRole, terrainHeightLable);

        terrainHeightValue = new QLabel(rightToolBoxTerrain);
        terrainHeightValue->setObjectName(QStringLiteral("terrainHeightValue"));

        formLayout->setWidget(0, QFormLayout::FieldRole, terrainHeightValue);

        terrainSmoothLable = new QLabel(rightToolBoxTerrain);
        terrainSmoothLable->setObjectName(QStringLiteral("terrainSmoothLable"));

        formLayout->setWidget(3, QFormLayout::LabelRole, terrainSmoothLable);

        terrainSmoothValue = new QLabel(rightToolBoxTerrain);
        terrainSmoothValue->setObjectName(QStringLiteral("terrainSmoothValue"));

        formLayout->setWidget(3, QFormLayout::FieldRole, terrainSmoothValue);

        terrainSmoothSlider = new QSlider(rightToolBoxTerrain);
        terrainSmoothSlider->setObjectName(QStringLiteral("terrainSmoothSlider"));
        terrainSmoothSlider->setMinimum(1);
        terrainSmoothSlider->setMaximum(5);
        terrainSmoothSlider->setPageStep(1);
        terrainSmoothSlider->setOrientation(Qt::Horizontal);

        formLayout->setWidget(4, QFormLayout::SpanningRole, terrainSmoothSlider);

        terrainRadiusLable = new QLabel(rightToolBoxTerrain);
        terrainRadiusLable->setObjectName(QStringLiteral("terrainRadiusLable"));

        formLayout->setWidget(5, QFormLayout::LabelRole, terrainRadiusLable);

        terrainRadiusValue = new QLabel(rightToolBoxTerrain);
        terrainRadiusValue->setObjectName(QStringLiteral("terrainRadiusValue"));

        formLayout->setWidget(5, QFormLayout::FieldRole, terrainRadiusValue);

        terrainRadiusSlider = new QSlider(rightToolBoxTerrain);
        terrainRadiusSlider->setObjectName(QStringLiteral("terrainRadiusSlider"));
        terrainRadiusSlider->setMinimum(1);
        terrainRadiusSlider->setMaximum(50);
        terrainRadiusSlider->setSingleStep(1);
        terrainRadiusSlider->setOrientation(Qt::Horizontal);
        terrainRadiusSlider->setInvertedAppearance(false);
        terrainRadiusSlider->setTickInterval(0);

        formLayout->setWidget(6, QFormLayout::SpanningRole, terrainRadiusSlider);

        terrainHeightSlider = new QSlider(rightToolBoxTerrain);
        terrainHeightSlider->setObjectName(QStringLiteral("terrainHeightSlider"));
        terrainHeightSlider->setMinimum(-20);
        terrainHeightSlider->setMaximum(20);
        terrainHeightSlider->setSingleStep(1);
        terrainHeightSlider->setPageStep(2);
        terrainHeightSlider->setOrientation(Qt::Horizontal);
        terrainHeightSlider->setInvertedAppearance(false);
        terrainHeightSlider->setTickInterval(0);

        formLayout->setWidget(2, QFormLayout::SpanningRole, terrainHeightSlider);

        QIcon icon;
        QString iconThemeName = QStringLiteral("areaChart");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QStringLiteral("."), QSize(), QIcon::Normal, QIcon::Off);
        }
        rightToolBox->addItem(rightToolBoxTerrain, icon, QStringLiteral("Terrain"));
        page_2 = new QWidget();
        page_2->setObjectName(QStringLiteral("page_2"));
        page_2->setGeometry(QRect(0, 0, 98, 28));
        rightToolBox->addItem(page_2, QStringLiteral("Page 2"));

        verticalLayout_2->addWidget(rightToolBox);


        mainLayout->addWidget(rightFrame);


        horizontalLayout->addLayout(mainLayout);

        bcBlackCatEditorClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(bcBlackCatEditorClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1600, 26));
        bcBlackCatEditorClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(bcBlackCatEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
        mainToolBar->setSizePolicy(sizePolicy4);
        mainToolBar->setMinimumSize(QSize(30, 30));
        mainToolBar->setBaseSize(QSize(0, 0));
        mainToolBar->setMovable(false);
        mainToolBar->setOrientation(Qt::Horizontal);
        bcBlackCatEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(bcBlackCatEditorClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        bcBlackCatEditorClass->setStatusBar(statusBar);
        toolsDock = new QDockWidget(bcBlackCatEditorClass);
        toolsDock->setObjectName(QStringLiteral("toolsDock"));
        QSizePolicy sizePolicy5(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(toolsDock->sizePolicy().hasHeightForWidth());
        toolsDock->setSizePolicy(sizePolicy5);
        toolsDock->setMinimumSize(QSize(55, 914));
        toolsDock->setMaximumSize(QSize(55, 524287));
        toolsDock->setFloating(false);
        toolsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        toolsDock->setWindowTitle(QStringLiteral(""));
        toolsDockContents = new QWidget();
        toolsDockContents->setObjectName(QStringLiteral("toolsDockContents"));
        verticalLayout_3 = new QVBoxLayout(toolsDockContents);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_3->setContentsMargins(5, 0, 5, 600);
        objectSelectButton = new QToolButton(toolsDockContents);
        objectSelectButton->setObjectName(QStringLiteral("objectSelectButton"));
        QSizePolicy sizePolicy6(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(objectSelectButton->sizePolicy().hasHeightForWidth());
        objectSelectButton->setSizePolicy(sizePolicy6);
        objectSelectButton->setCheckable(true);
        objectSelectButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(objectSelectButton);

        terrainHeightButton = new QToolButton(toolsDockContents);
        terrainHeightButton->setObjectName(QStringLiteral("terrainHeightButton"));
        sizePolicy6.setHeightForWidth(terrainHeightButton->sizePolicy().hasHeightForWidth());
        terrainHeightButton->setSizePolicy(sizePolicy6);
        terrainHeightButton->setMinimumSize(QSize(0, 0));
        terrainHeightButton->setSizeIncrement(QSize(0, 0));
        terrainHeightButton->setBaseSize(QSize(0, 0));
        terrainHeightButton->setCheckable(true);
        terrainHeightButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainHeightButton);

        terrainSmoothButton = new QToolButton(toolsDockContents);
        terrainSmoothButton->setObjectName(QStringLiteral("terrainSmoothButton"));
        sizePolicy6.setHeightForWidth(terrainSmoothButton->sizePolicy().hasHeightForWidth());
        terrainSmoothButton->setSizePolicy(sizePolicy6);
        terrainSmoothButton->setCheckable(true);
        terrainSmoothButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainSmoothButton);

        toolButton_3 = new QToolButton(toolsDockContents);
        toolButton_3->setObjectName(QStringLiteral("toolButton_3"));
        sizePolicy6.setHeightForWidth(toolButton_3->sizePolicy().hasHeightForWidth());
        toolButton_3->setSizePolicy(sizePolicy6);
        toolButton_3->setCheckable(true);
        toolButton_3->setAutoExclusive(true);

        verticalLayout_3->addWidget(toolButton_3);

        toolButton = new QToolButton(toolsDockContents);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        sizePolicy6.setHeightForWidth(toolButton->sizePolicy().hasHeightForWidth());
        toolButton->setSizePolicy(sizePolicy6);
        toolButton->setCheckable(true);
        toolButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(toolButton);

        toolsDock->setWidget(toolsDockContents);
        bcBlackCatEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), toolsDock);

        retranslateUi(bcBlackCatEditorClass);
        QObject::connect(terrainHeightSlider, SIGNAL(valueChanged(int)), terrainHeightValue, SLOT(setNum(int)));
        QObject::connect(terrainSmoothSlider, SIGNAL(valueChanged(int)), terrainSmoothValue, SLOT(setNum(int)));
        QObject::connect(terrainRadiusSlider, SIGNAL(valueChanged(int)), terrainRadiusValue, SLOT(setNum(int)));

        leftBottomTab->setCurrentIndex(0);
        rightToolBox->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(bcBlackCatEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *bcBlackCatEditorClass)
    {
        bcBlackCatEditorClass->setWindowTitle(QApplication::translate("bcBlackCatEditorClass", "BlackCatEditor", 0));
        leftBottomTab->setTabText(leftBottomTab->indexOf(consoleTab), QApplication::translate("bcBlackCatEditorClass", "Console", 0));
        rightToolBox->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "areachart,adjust", 0)));
        terrainHeightLable->setText(QApplication::translate("bcBlackCatEditorClass", "Height: ", 0));
        terrainHeightValue->setText(QApplication::translate("bcBlackCatEditorClass", "0", 0));
        terrainSmoothLable->setText(QApplication::translate("bcBlackCatEditorClass", "Smooth:", 0));
        terrainSmoothValue->setText(QApplication::translate("bcBlackCatEditorClass", "1", 0));
        terrainRadiusLable->setText(QApplication::translate("bcBlackCatEditorClass", "Radius: ", 0));
        terrainRadiusValue->setText(QApplication::translate("bcBlackCatEditorClass", "1", 0));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxTerrain), QApplication::translate("bcBlackCatEditorClass", "Terrain", 0));
        rightToolBox->setItemText(rightToolBox->indexOf(page_2), QApplication::translate("bcBlackCatEditorClass", "Page 2", 0));
#ifndef QT_NO_TOOLTIP
        objectSelectButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Object selection", 0));
#endif // QT_NO_TOOLTIP
        objectSelectButton->setText(QString());
        objectSelectButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "handpointero", 0)));
#ifndef QT_NO_TOOLTIP
        terrainHeightButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Height", 0));
#endif // QT_NO_TOOLTIP
        terrainHeightButton->setText(QString());
        terrainHeightButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "areachart", 0)));
#ifndef QT_NO_TOOLTIP
        terrainSmoothButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Smooth", 0));
#endif // QT_NO_TOOLTIP
        terrainSmoothButton->setText(QString());
        terrainSmoothButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "bullseye", 0)));
        toolButton_3->setText(QApplication::translate("bcBlackCatEditorClass", "...", 0));
        toolButton->setText(QApplication::translate("bcBlackCatEditorClass", "...", 0));
    } // retranslateUi

};

namespace Ui {
    class bcBlackCatEditorClass: public Ui_bcBlackCatEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BCBLACKCATEDITOR_H
