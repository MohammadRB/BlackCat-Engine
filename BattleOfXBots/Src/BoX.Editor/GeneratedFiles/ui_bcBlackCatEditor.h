/********************************************************************************
** Form generated from reading UI file 'bcBlackCatEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.14.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BCBLACKCATEDITOR_H
#define UI_BCBLACKCATEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
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
    QAction *newSceneMenu;
    QAction *loadSceneMenu;
    QAction *saveSceneMenu;
    QAction *gameModeButton;
    QAction *editorModeButton;
    QAction *pauseModeButton;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout_4;
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
    QWidget *rightToolBoxObjectSelect;
    QLabel *entityNameLabel;
    QLabel *entityNameLabelValue;
    QPushButton *entityDeleteButton;
    QLabel *entityPositionLabel;
    QLineEdit *entityPositionXText;
    QLabel *entityPositionXLabel;
    QLabel *entityPositionYLabel;
    QLineEdit *entityPositionYText;
    QLabel *entityPositionZLabel;
    QLineEdit *entityPositionZText;
    QLineEdit *entityRotationZText;
    QLabel *entityRotationXLabel;
    QLabel *entityRotationYLabel;
    QLabel *entityRotationLabel;
    QLineEdit *entityRotationYText;
    QLabel *entityRotationZLabel;
    QLineEdit *entityRotationXText;
    QPushButton *entityUpdateButton;
    QWidget *rightToolBoxObjectInsert;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *entityNameList;
    QWidget *rightToolBoxTerrain;
    QFormLayout *formLayout;
    QLabel *terrainHeightLable;
    QLabel *terrainHeightValue;
    QSlider *terrainHeightSlider;
    QLabel *terrainSmoothLable;
    QSlider *terrainSmoothSlider;
    QLabel *terrainSmoothValue;
    QLabel *terrainMaterialLable;
    QComboBox *terrainMaterialCombo;
    QSlider *terrainRadiusSlider;
    QLabel *terrainRadiusLable;
    QLabel *terrainRadiusValue;
    QWidget *rightToolBoxDecalInsert;
    QHBoxLayout *horizontalLayout;
    QListWidget *decalNameList;
    QWidget *rightToolBoxDecalSelect;
    QPushButton *decalUpdateButton;
    QPushButton *decalDeleteButton;
    QLabel *decalNameLabel;
    QLineEdit *decalRotationZText;
    QLabel *decalRotationZLabel;
    QLineEdit *decalRotationXText;
    QLabel *decalRotationLabel;
    QLineEdit *decalRotationYText;
    QLabel *decalRotationXLabel;
    QLabel *decalRotationYLabel;
    QLabel *decalNameLabelValue;
    QMenuBar *mainMenuBar;
    QMenu *fileMenu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;
    QDockWidget *toolsDock;
    QWidget *toolsDockContents;
    QVBoxLayout *verticalLayout_3;
    QToolButton *objectSelectButton;
    QToolButton *objectInsertButton;
    QToolButton *terrainHeightButton;
    QToolButton *terrainSmoothButton;
    QToolButton *terrainMaterialButton;
    QToolButton *terrainMaterialSmoothButton;
    QToolButton *decalPaintButton;
    QToolButton *decalSelectButton;

    void setupUi(QMainWindow *bcBlackCatEditorClass)
    {
        if (bcBlackCatEditorClass->objectName().isEmpty())
            bcBlackCatEditorClass->setObjectName(QString::fromUtf8("bcBlackCatEditorClass"));
        bcBlackCatEditorClass->resize(1600, 900);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(bcBlackCatEditorClass->sizePolicy().hasHeightForWidth());
        bcBlackCatEditorClass->setSizePolicy(sizePolicy);
        bcBlackCatEditorClass->setMinimumSize(QSize(1600, 900));
        bcBlackCatEditorClass->setBaseSize(QSize(1200, 750));
        newSceneMenu = new QAction(bcBlackCatEditorClass);
        newSceneMenu->setObjectName(QString::fromUtf8("newSceneMenu"));
        loadSceneMenu = new QAction(bcBlackCatEditorClass);
        loadSceneMenu->setObjectName(QString::fromUtf8("loadSceneMenu"));
        saveSceneMenu = new QAction(bcBlackCatEditorClass);
        saveSceneMenu->setObjectName(QString::fromUtf8("saveSceneMenu"));
        gameModeButton = new QAction(bcBlackCatEditorClass);
        gameModeButton->setObjectName(QString::fromUtf8("gameModeButton"));
        gameModeButton->setCheckable(true);
        editorModeButton = new QAction(bcBlackCatEditorClass);
        editorModeButton->setObjectName(QString::fromUtf8("editorModeButton"));
        editorModeButton->setCheckable(true);
        editorModeButton->setChecked(true);
        pauseModeButton = new QAction(bcBlackCatEditorClass);
        pauseModeButton->setObjectName(QString::fromUtf8("pauseModeButton"));
        pauseModeButton->setCheckable(true);
        pauseModeButton->setChecked(false);
        centralWidget = new QWidget(bcBlackCatEditorClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        verticalLayout_4 = new QVBoxLayout(centralWidget);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(3, 3, 3, 3);
        mainLayout = new QHBoxLayout();
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        leftFrame = new QFrame(centralWidget);
        leftFrame->setObjectName(QString::fromUtf8("leftFrame"));
        leftFrame->setFrameShape(QFrame::StyledPanel);
        leftFrame->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(leftFrame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        splitter = new QSplitter(leftFrame);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        leftRenderFrame = new QFrame(splitter);
        leftRenderFrame->setObjectName(QString::fromUtf8("leftRenderFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(leftRenderFrame->sizePolicy().hasHeightForWidth());
        leftRenderFrame->setSizePolicy(sizePolicy2);
        leftRenderFrame->setMinimumSize(QSize(0, 550));
        leftRenderFrame->setFrameShape(QFrame::StyledPanel);
        leftRenderFrame->setFrameShadow(QFrame::Raised);
        splitter->addWidget(leftRenderFrame);
        leftBottomFrame = new QFrame(splitter);
        leftBottomFrame->setObjectName(QString::fromUtf8("leftBottomFrame"));
        sizePolicy1.setHeightForWidth(leftBottomFrame->sizePolicy().hasHeightForWidth());
        leftBottomFrame->setSizePolicy(sizePolicy1);
        leftBottomFrame->setBaseSize(QSize(0, 0));
        leftBottomFrame->setFrameShape(QFrame::StyledPanel);
        leftBottomFrame->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(leftBottomFrame);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(3, 3, 3, 3);
        leftBottomTab = new QTabWidget(leftBottomFrame);
        leftBottomTab->setObjectName(QString::fromUtf8("leftBottomTab"));
        consoleTab = new QWidget();
        consoleTab->setObjectName(QString::fromUtf8("consoleTab"));
        leftBottomTab->addTab(consoleTab, QString());

        horizontalLayout_3->addWidget(leftBottomTab);

        splitter->addWidget(leftBottomFrame);

        verticalLayout->addWidget(splitter);


        mainLayout->addWidget(leftFrame);

        rightFrame = new QFrame(centralWidget);
        rightFrame->setObjectName(QString::fromUtf8("rightFrame"));
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
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        rightToolBox = new QToolBox(rightFrame);
        rightToolBox->setObjectName(QString::fromUtf8("rightToolBox"));
        rightToolBoxObjectSelect = new QWidget();
        rightToolBoxObjectSelect->setObjectName(QString::fromUtf8("rightToolBoxObjectSelect"));
        rightToolBoxObjectSelect->setGeometry(QRect(0, 0, 340, 663));
        entityNameLabel = new QLabel(rightToolBoxObjectSelect);
        entityNameLabel->setObjectName(QString::fromUtf8("entityNameLabel"));
        entityNameLabel->setGeometry(QRect(10, 10, 81, 16));
        entityNameLabelValue = new QLabel(rightToolBoxObjectSelect);
        entityNameLabelValue->setObjectName(QString::fromUtf8("entityNameLabelValue"));
        entityNameLabelValue->setGeometry(QRect(100, 10, 231, 16));
        entityDeleteButton = new QPushButton(rightToolBoxObjectSelect);
        entityDeleteButton->setObjectName(QString::fromUtf8("entityDeleteButton"));
        entityDeleteButton->setEnabled(false);
        entityDeleteButton->setGeometry(QRect(160, 180, 81, 31));
        entityPositionLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionLabel->setObjectName(QString::fromUtf8("entityPositionLabel"));
        entityPositionLabel->setGeometry(QRect(10, 40, 81, 16));
        entityPositionXText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionXText->setObjectName(QString::fromUtf8("entityPositionXText"));
        entityPositionXText->setEnabled(false);
        entityPositionXText->setGeometry(QRect(30, 70, 91, 25));
        entityPositionXText->setReadOnly(false);
        entityPositionXLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionXLabel->setObjectName(QString::fromUtf8("entityPositionXLabel"));
        entityPositionXLabel->setGeometry(QRect(10, 70, 16, 16));
        entityPositionYLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionYLabel->setObjectName(QString::fromUtf8("entityPositionYLabel"));
        entityPositionYLabel->setGeometry(QRect(10, 100, 16, 16));
        entityPositionYText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionYText->setObjectName(QString::fromUtf8("entityPositionYText"));
        entityPositionYText->setEnabled(false);
        entityPositionYText->setGeometry(QRect(30, 100, 91, 25));
        entityPositionYText->setReadOnly(false);
        entityPositionZLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionZLabel->setObjectName(QString::fromUtf8("entityPositionZLabel"));
        entityPositionZLabel->setGeometry(QRect(10, 130, 16, 16));
        entityPositionZText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionZText->setObjectName(QString::fromUtf8("entityPositionZText"));
        entityPositionZText->setEnabled(false);
        entityPositionZText->setGeometry(QRect(30, 130, 91, 25));
        entityPositionZText->setReadOnly(false);
        entityRotationZText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationZText->setObjectName(QString::fromUtf8("entityRotationZText"));
        entityRotationZText->setEnabled(false);
        entityRotationZText->setGeometry(QRect(160, 130, 91, 25));
        entityRotationZText->setReadOnly(false);
        entityRotationXLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationXLabel->setObjectName(QString::fromUtf8("entityRotationXLabel"));
        entityRotationXLabel->setGeometry(QRect(140, 70, 16, 16));
        entityRotationYLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationYLabel->setObjectName(QString::fromUtf8("entityRotationYLabel"));
        entityRotationYLabel->setGeometry(QRect(140, 100, 16, 16));
        entityRotationLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationLabel->setObjectName(QString::fromUtf8("entityRotationLabel"));
        entityRotationLabel->setGeometry(QRect(140, 40, 81, 16));
        entityRotationYText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationYText->setObjectName(QString::fromUtf8("entityRotationYText"));
        entityRotationYText->setEnabled(false);
        entityRotationYText->setGeometry(QRect(160, 100, 91, 25));
        entityRotationYText->setReadOnly(false);
        entityRotationZLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationZLabel->setObjectName(QString::fromUtf8("entityRotationZLabel"));
        entityRotationZLabel->setGeometry(QRect(140, 130, 16, 16));
        entityRotationXText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationXText->setObjectName(QString::fromUtf8("entityRotationXText"));
        entityRotationXText->setEnabled(false);
        entityRotationXText->setGeometry(QRect(160, 70, 91, 25));
        entityRotationXText->setReadOnly(false);
        entityUpdateButton = new QPushButton(rightToolBoxObjectSelect);
        entityUpdateButton->setObjectName(QString::fromUtf8("entityUpdateButton"));
        entityUpdateButton->setEnabled(false);
        entityUpdateButton->setGeometry(QRect(30, 180, 81, 31));
        rightToolBox->addItem(rightToolBoxObjectSelect, QString::fromUtf8("Object Select"));
        rightToolBoxObjectInsert = new QWidget();
        rightToolBoxObjectInsert->setObjectName(QString::fromUtf8("rightToolBoxObjectInsert"));
        rightToolBoxObjectInsert->setGeometry(QRect(0, 0, 340, 663));
        horizontalLayout_2 = new QHBoxLayout(rightToolBoxObjectInsert);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        entityNameList = new QListWidget(rightToolBoxObjectInsert);
        entityNameList->setObjectName(QString::fromUtf8("entityNameList"));

        horizontalLayout_2->addWidget(entityNameList);

        rightToolBox->addItem(rightToolBoxObjectInsert, QString::fromUtf8("Object Insert"));
        rightToolBoxTerrain = new QWidget();
        rightToolBoxTerrain->setObjectName(QString::fromUtf8("rightToolBoxTerrain"));
        rightToolBoxTerrain->setGeometry(QRect(0, 0, 340, 663));
        formLayout = new QFormLayout(rightToolBoxTerrain);
        formLayout->setSpacing(6);
        formLayout->setContentsMargins(11, 11, 11, 11);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        terrainHeightLable = new QLabel(rightToolBoxTerrain);
        terrainHeightLable->setObjectName(QString::fromUtf8("terrainHeightLable"));

        formLayout->setWidget(0, QFormLayout::LabelRole, terrainHeightLable);

        terrainHeightValue = new QLabel(rightToolBoxTerrain);
        terrainHeightValue->setObjectName(QString::fromUtf8("terrainHeightValue"));

        formLayout->setWidget(0, QFormLayout::FieldRole, terrainHeightValue);

        terrainHeightSlider = new QSlider(rightToolBoxTerrain);
        terrainHeightSlider->setObjectName(QString::fromUtf8("terrainHeightSlider"));
        terrainHeightSlider->setMinimum(-20);
        terrainHeightSlider->setMaximum(20);
        terrainHeightSlider->setSingleStep(1);
        terrainHeightSlider->setPageStep(2);
        terrainHeightSlider->setOrientation(Qt::Horizontal);
        terrainHeightSlider->setInvertedAppearance(false);
        terrainHeightSlider->setTickInterval(0);

        formLayout->setWidget(2, QFormLayout::SpanningRole, terrainHeightSlider);

        terrainSmoothLable = new QLabel(rightToolBoxTerrain);
        terrainSmoothLable->setObjectName(QString::fromUtf8("terrainSmoothLable"));

        formLayout->setWidget(6, QFormLayout::LabelRole, terrainSmoothLable);

        terrainSmoothSlider = new QSlider(rightToolBoxTerrain);
        terrainSmoothSlider->setObjectName(QString::fromUtf8("terrainSmoothSlider"));
        terrainSmoothSlider->setMinimum(1);
        terrainSmoothSlider->setMaximum(6);
        terrainSmoothSlider->setPageStep(1);
        terrainSmoothSlider->setOrientation(Qt::Horizontal);

        formLayout->setWidget(7, QFormLayout::SpanningRole, terrainSmoothSlider);

        terrainSmoothValue = new QLabel(rightToolBoxTerrain);
        terrainSmoothValue->setObjectName(QString::fromUtf8("terrainSmoothValue"));

        formLayout->setWidget(6, QFormLayout::FieldRole, terrainSmoothValue);

        terrainMaterialLable = new QLabel(rightToolBoxTerrain);
        terrainMaterialLable->setObjectName(QString::fromUtf8("terrainMaterialLable"));

        formLayout->setWidget(10, QFormLayout::LabelRole, terrainMaterialLable);

        terrainMaterialCombo = new QComboBox(rightToolBoxTerrain);
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->addItem(QString());
        terrainMaterialCombo->setObjectName(QString::fromUtf8("terrainMaterialCombo"));

        formLayout->setWidget(11, QFormLayout::SpanningRole, terrainMaterialCombo);

        terrainRadiusSlider = new QSlider(rightToolBoxTerrain);
        terrainRadiusSlider->setObjectName(QString::fromUtf8("terrainRadiusSlider"));
        terrainRadiusSlider->setMinimum(1);
        terrainRadiusSlider->setMaximum(50);
        terrainRadiusSlider->setSingleStep(1);
        terrainRadiusSlider->setValue(25);
        terrainRadiusSlider->setOrientation(Qt::Horizontal);
        terrainRadiusSlider->setInvertedAppearance(false);
        terrainRadiusSlider->setTickInterval(0);

        formLayout->setWidget(4, QFormLayout::SpanningRole, terrainRadiusSlider);

        terrainRadiusLable = new QLabel(rightToolBoxTerrain);
        terrainRadiusLable->setObjectName(QString::fromUtf8("terrainRadiusLable"));

        formLayout->setWidget(3, QFormLayout::LabelRole, terrainRadiusLable);

        terrainRadiusValue = new QLabel(rightToolBoxTerrain);
        terrainRadiusValue->setObjectName(QString::fromUtf8("terrainRadiusValue"));

        formLayout->setWidget(3, QFormLayout::FieldRole, terrainRadiusValue);

        QIcon icon;
        QString iconThemeName = QString::fromUtf8("areaChart");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QString::fromUtf8("C:/Users/moham/.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        rightToolBox->addItem(rightToolBoxTerrain, icon, QString::fromUtf8("Terrain"));
        rightToolBoxDecalInsert = new QWidget();
        rightToolBoxDecalInsert->setObjectName(QString::fromUtf8("rightToolBoxDecalInsert"));
        rightToolBoxDecalInsert->setGeometry(QRect(0, 0, 340, 663));
        horizontalLayout = new QHBoxLayout(rightToolBoxDecalInsert);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        decalNameList = new QListWidget(rightToolBoxDecalInsert);
        decalNameList->setObjectName(QString::fromUtf8("decalNameList"));

        horizontalLayout->addWidget(decalNameList);

        rightToolBox->addItem(rightToolBoxDecalInsert, QString::fromUtf8("Decal Paint"));
        rightToolBoxDecalSelect = new QWidget();
        rightToolBoxDecalSelect->setObjectName(QString::fromUtf8("rightToolBoxDecalSelect"));
        rightToolBoxDecalSelect->setGeometry(QRect(0, 0, 340, 663));
        decalUpdateButton = new QPushButton(rightToolBoxDecalSelect);
        decalUpdateButton->setObjectName(QString::fromUtf8("decalUpdateButton"));
        decalUpdateButton->setEnabled(false);
        decalUpdateButton->setGeometry(QRect(30, 160, 81, 31));
        decalDeleteButton = new QPushButton(rightToolBoxDecalSelect);
        decalDeleteButton->setObjectName(QString::fromUtf8("decalDeleteButton"));
        decalDeleteButton->setEnabled(false);
        decalDeleteButton->setGeometry(QRect(160, 160, 81, 31));
        decalNameLabel = new QLabel(rightToolBoxDecalSelect);
        decalNameLabel->setObjectName(QString::fromUtf8("decalNameLabel"));
        decalNameLabel->setGeometry(QRect(10, 0, 81, 16));
        decalRotationZText = new QLineEdit(rightToolBoxDecalSelect);
        decalRotationZText->setObjectName(QString::fromUtf8("decalRotationZText"));
        decalRotationZText->setEnabled(false);
        decalRotationZText->setGeometry(QRect(30, 120, 91, 25));
        decalRotationZText->setReadOnly(false);
        decalRotationZLabel = new QLabel(rightToolBoxDecalSelect);
        decalRotationZLabel->setObjectName(QString::fromUtf8("decalRotationZLabel"));
        decalRotationZLabel->setGeometry(QRect(10, 120, 16, 16));
        decalRotationXText = new QLineEdit(rightToolBoxDecalSelect);
        decalRotationXText->setObjectName(QString::fromUtf8("decalRotationXText"));
        decalRotationXText->setEnabled(false);
        decalRotationXText->setGeometry(QRect(30, 60, 91, 25));
        decalRotationXText->setReadOnly(false);
        decalRotationLabel = new QLabel(rightToolBoxDecalSelect);
        decalRotationLabel->setObjectName(QString::fromUtf8("decalRotationLabel"));
        decalRotationLabel->setGeometry(QRect(10, 30, 81, 16));
        decalRotationYText = new QLineEdit(rightToolBoxDecalSelect);
        decalRotationYText->setObjectName(QString::fromUtf8("decalRotationYText"));
        decalRotationYText->setEnabled(false);
        decalRotationYText->setGeometry(QRect(30, 90, 91, 25));
        decalRotationYText->setReadOnly(false);
        decalRotationXLabel = new QLabel(rightToolBoxDecalSelect);
        decalRotationXLabel->setObjectName(QString::fromUtf8("decalRotationXLabel"));
        decalRotationXLabel->setGeometry(QRect(10, 60, 16, 16));
        decalRotationYLabel = new QLabel(rightToolBoxDecalSelect);
        decalRotationYLabel->setObjectName(QString::fromUtf8("decalRotationYLabel"));
        decalRotationYLabel->setGeometry(QRect(10, 90, 16, 16));
        decalNameLabelValue = new QLabel(rightToolBoxDecalSelect);
        decalNameLabelValue->setObjectName(QString::fromUtf8("decalNameLabelValue"));
        decalNameLabelValue->setGeometry(QRect(100, 0, 231, 16));
        rightToolBox->addItem(rightToolBoxDecalSelect, QString::fromUtf8("Decal Select"));

        verticalLayout_2->addWidget(rightToolBox);


        mainLayout->addWidget(rightFrame);


        verticalLayout_4->addLayout(mainLayout);

        bcBlackCatEditorClass->setCentralWidget(centralWidget);
        mainMenuBar = new QMenuBar(bcBlackCatEditorClass);
        mainMenuBar->setObjectName(QString::fromUtf8("mainMenuBar"));
        mainMenuBar->setGeometry(QRect(0, 0, 1600, 22));
        fileMenu = new QMenu(mainMenuBar);
        fileMenu->setObjectName(QString::fromUtf8("fileMenu"));
        bcBlackCatEditorClass->setMenuBar(mainMenuBar);
        mainToolBar = new QToolBar(bcBlackCatEditorClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(mainToolBar->sizePolicy().hasHeightForWidth());
        mainToolBar->setSizePolicy(sizePolicy4);
        mainToolBar->setMinimumSize(QSize(0, 0));
        mainToolBar->setMaximumSize(QSize(16777215, 30));
        mainToolBar->setBaseSize(QSize(0, 0));
        mainToolBar->setMovable(false);
        mainToolBar->setOrientation(Qt::Horizontal);
        mainToolBar->setIconSize(QSize(22, 22));
        mainToolBar->setFloatable(false);
        bcBlackCatEditorClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(bcBlackCatEditorClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(statusBar->sizePolicy().hasHeightForWidth());
        statusBar->setSizePolicy(sizePolicy5);
        bcBlackCatEditorClass->setStatusBar(statusBar);
        toolsDock = new QDockWidget(bcBlackCatEditorClass);
        toolsDock->setObjectName(QString::fromUtf8("toolsDock"));
        QSizePolicy sizePolicy6(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(toolsDock->sizePolicy().hasHeightForWidth());
        toolsDock->setSizePolicy(sizePolicy6);
        toolsDock->setMinimumSize(QSize(55, 914));
        toolsDock->setMaximumSize(QSize(55, 524287));
        toolsDock->setFloating(false);
        toolsDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
        toolsDock->setWindowTitle(QString::fromUtf8(""));
        toolsDockContents = new QWidget();
        toolsDockContents->setObjectName(QString::fromUtf8("toolsDockContents"));
        verticalLayout_3 = new QVBoxLayout(toolsDockContents);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout_3->setContentsMargins(5, 0, 5, 600);
        objectSelectButton = new QToolButton(toolsDockContents);
        objectSelectButton->setObjectName(QString::fromUtf8("objectSelectButton"));
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(objectSelectButton->sizePolicy().hasHeightForWidth());
        objectSelectButton->setSizePolicy(sizePolicy7);
        objectSelectButton->setCheckable(true);
        objectSelectButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(objectSelectButton);

        objectInsertButton = new QToolButton(toolsDockContents);
        objectInsertButton->setObjectName(QString::fromUtf8("objectInsertButton"));
        sizePolicy7.setHeightForWidth(objectInsertButton->sizePolicy().hasHeightForWidth());
        objectInsertButton->setSizePolicy(sizePolicy7);
        objectInsertButton->setCheckable(true);
        objectInsertButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(objectInsertButton);

        terrainHeightButton = new QToolButton(toolsDockContents);
        terrainHeightButton->setObjectName(QString::fromUtf8("terrainHeightButton"));
        sizePolicy7.setHeightForWidth(terrainHeightButton->sizePolicy().hasHeightForWidth());
        terrainHeightButton->setSizePolicy(sizePolicy7);
        terrainHeightButton->setMinimumSize(QSize(0, 0));
        terrainHeightButton->setSizeIncrement(QSize(0, 0));
        terrainHeightButton->setBaseSize(QSize(0, 0));
        terrainHeightButton->setCheckable(true);
        terrainHeightButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainHeightButton);

        terrainSmoothButton = new QToolButton(toolsDockContents);
        terrainSmoothButton->setObjectName(QString::fromUtf8("terrainSmoothButton"));
        sizePolicy7.setHeightForWidth(terrainSmoothButton->sizePolicy().hasHeightForWidth());
        terrainSmoothButton->setSizePolicy(sizePolicy7);
        terrainSmoothButton->setCheckable(true);
        terrainSmoothButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainSmoothButton);

        terrainMaterialButton = new QToolButton(toolsDockContents);
        terrainMaterialButton->setObjectName(QString::fromUtf8("terrainMaterialButton"));
        sizePolicy7.setHeightForWidth(terrainMaterialButton->sizePolicy().hasHeightForWidth());
        terrainMaterialButton->setSizePolicy(sizePolicy7);
        terrainMaterialButton->setCheckable(true);
        terrainMaterialButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainMaterialButton);

        terrainMaterialSmoothButton = new QToolButton(toolsDockContents);
        terrainMaterialSmoothButton->setObjectName(QString::fromUtf8("terrainMaterialSmoothButton"));
        sizePolicy7.setHeightForWidth(terrainMaterialSmoothButton->sizePolicy().hasHeightForWidth());
        terrainMaterialSmoothButton->setSizePolicy(sizePolicy7);
        terrainMaterialSmoothButton->setCheckable(true);
        terrainMaterialSmoothButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainMaterialSmoothButton);

        decalPaintButton = new QToolButton(toolsDockContents);
        decalPaintButton->setObjectName(QString::fromUtf8("decalPaintButton"));
        sizePolicy7.setHeightForWidth(decalPaintButton->sizePolicy().hasHeightForWidth());
        decalPaintButton->setSizePolicy(sizePolicy7);
        decalPaintButton->setCheckable(true);
        decalPaintButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(decalPaintButton);

        decalSelectButton = new QToolButton(toolsDockContents);
        decalSelectButton->setObjectName(QString::fromUtf8("decalSelectButton"));
        sizePolicy7.setHeightForWidth(decalSelectButton->sizePolicy().hasHeightForWidth());
        decalSelectButton->setSizePolicy(sizePolicy7);
        decalSelectButton->setCheckable(true);
        decalSelectButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(decalSelectButton);

        toolsDock->setWidget(toolsDockContents);
        bcBlackCatEditorClass->addDockWidget(Qt::LeftDockWidgetArea, toolsDock);

        mainMenuBar->addAction(fileMenu->menuAction());
        fileMenu->addAction(newSceneMenu);
        fileMenu->addAction(loadSceneMenu);
        fileMenu->addAction(saveSceneMenu);
        mainToolBar->addAction(gameModeButton);
        mainToolBar->addAction(pauseModeButton);
        mainToolBar->addAction(editorModeButton);

        retranslateUi(bcBlackCatEditorClass);
        QObject::connect(terrainHeightSlider, SIGNAL(valueChanged(int)), terrainHeightValue, SLOT(setNum(int)));
        QObject::connect(terrainSmoothSlider, SIGNAL(valueChanged(int)), terrainSmoothValue, SLOT(setNum(int)));
        QObject::connect(terrainRadiusSlider, SIGNAL(valueChanged(int)), terrainRadiusValue, SLOT(setNum(int)));

        leftBottomTab->setCurrentIndex(0);
        rightToolBox->setCurrentIndex(4);


        QMetaObject::connectSlotsByName(bcBlackCatEditorClass);
    } // setupUi

    void retranslateUi(QMainWindow *bcBlackCatEditorClass)
    {
        bcBlackCatEditorClass->setWindowTitle(QCoreApplication::translate("bcBlackCatEditorClass", "BlackCatEditor", nullptr));
        newSceneMenu->setText(QCoreApplication::translate("bcBlackCatEditorClass", "NewScene", nullptr));
        loadSceneMenu->setText(QCoreApplication::translate("bcBlackCatEditorClass", "LoadScene", nullptr));
        saveSceneMenu->setText(QCoreApplication::translate("bcBlackCatEditorClass", "SaveScene", nullptr));
        gameModeButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "GameMode", nullptr));
#if QT_CONFIG(tooltip)
        gameModeButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "GameMode", nullptr));
#endif // QT_CONFIG(tooltip)
        gameModeButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "play", nullptr)));
        editorModeButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "EditorMode", nullptr));
#if QT_CONFIG(tooltip)
        editorModeButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "EditorMode", nullptr));
#endif // QT_CONFIG(tooltip)
        editorModeButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "stop", nullptr)));
        pauseModeButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "PauseMode", nullptr));
#if QT_CONFIG(tooltip)
        pauseModeButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "PauseMode", nullptr));
#endif // QT_CONFIG(tooltip)
        pauseModeButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "pause", nullptr)));
        leftBottomTab->setTabText(leftBottomTab->indexOf(consoleTab), QCoreApplication::translate("bcBlackCatEditorClass", "Console", nullptr));
        rightToolBox->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "handpointero,cubes,areachart,pencil,pencilsquareo", nullptr)));
        entityNameLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Entity Name:", nullptr));
        entityNameLabelValue->setText(QString());
        entityDeleteButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Delete", nullptr));
        entityPositionLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Entity Position:", nullptr));
        entityPositionXLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "x:", nullptr));
        entityPositionYLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "y:", nullptr));
        entityPositionZLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "z:", nullptr));
        entityRotationXLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "x:", nullptr));
        entityRotationYLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "y:", nullptr));
        entityRotationLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Entity Rotation:", nullptr));
        entityRotationZLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "z:", nullptr));
        entityUpdateButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Update", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxObjectSelect), QCoreApplication::translate("bcBlackCatEditorClass", "Object Select", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxObjectInsert), QCoreApplication::translate("bcBlackCatEditorClass", "Object Insert", nullptr));
        terrainHeightLable->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Height: ", nullptr));
        terrainHeightValue->setText(QCoreApplication::translate("bcBlackCatEditorClass", "0", nullptr));
        terrainSmoothLable->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Smooth:", nullptr));
        terrainSmoothValue->setText(QCoreApplication::translate("bcBlackCatEditorClass", "1", nullptr));
        terrainMaterialLable->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Material: ", nullptr));
        terrainMaterialCombo->setItemText(0, QCoreApplication::translate("bcBlackCatEditorClass", "Material 1", nullptr));
        terrainMaterialCombo->setItemText(1, QCoreApplication::translate("bcBlackCatEditorClass", "Material 2", nullptr));
        terrainMaterialCombo->setItemText(2, QCoreApplication::translate("bcBlackCatEditorClass", "Material 3", nullptr));
        terrainMaterialCombo->setItemText(3, QCoreApplication::translate("bcBlackCatEditorClass", "Material 4", nullptr));
        terrainMaterialCombo->setItemText(4, QCoreApplication::translate("bcBlackCatEditorClass", "Material 5", nullptr));
        terrainMaterialCombo->setItemText(5, QCoreApplication::translate("bcBlackCatEditorClass", "Material 6", nullptr));
        terrainMaterialCombo->setItemText(6, QCoreApplication::translate("bcBlackCatEditorClass", "Material 7", nullptr));
        terrainMaterialCombo->setItemText(7, QCoreApplication::translate("bcBlackCatEditorClass", "Material 8", nullptr));

        terrainRadiusLable->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Radius: ", nullptr));
        terrainRadiusValue->setText(QCoreApplication::translate("bcBlackCatEditorClass", "25", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxTerrain), QCoreApplication::translate("bcBlackCatEditorClass", "Terrain", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxDecalInsert), QCoreApplication::translate("bcBlackCatEditorClass", "Decal Paint", nullptr));
        decalUpdateButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Update", nullptr));
        decalDeleteButton->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Delete", nullptr));
        decalNameLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "Decal Name:", nullptr));
        decalRotationZLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "z:", nullptr));
        decalRotationLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "D\331\220ecal Rotation:", nullptr));
        decalRotationXLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "x:", nullptr));
        decalRotationYLabel->setText(QCoreApplication::translate("bcBlackCatEditorClass", "y:", nullptr));
        decalNameLabelValue->setText(QString());
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxDecalSelect), QCoreApplication::translate("bcBlackCatEditorClass", "Decal Select", nullptr));
        fileMenu->setTitle(QCoreApplication::translate("bcBlackCatEditorClass", "File", nullptr));
#if QT_CONFIG(tooltip)
        objectSelectButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Object Select", nullptr));
#endif // QT_CONFIG(tooltip)
        objectSelectButton->setText(QString());
        objectSelectButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "handpointero", nullptr)));
#if QT_CONFIG(tooltip)
        objectInsertButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Object Insert", nullptr));
#endif // QT_CONFIG(tooltip)
        objectInsertButton->setText(QString());
        objectInsertButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "cubes", nullptr)));
#if QT_CONFIG(tooltip)
        terrainHeightButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Terrain Height", nullptr));
#endif // QT_CONFIG(tooltip)
        terrainHeightButton->setText(QString());
        terrainHeightButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "areachart", nullptr)));
#if QT_CONFIG(tooltip)
        terrainSmoothButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Terrain Smooth", nullptr));
#endif // QT_CONFIG(tooltip)
        terrainSmoothButton->setText(QString());
        terrainSmoothButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "bullseye", nullptr)));
#if QT_CONFIG(tooltip)
        terrainMaterialButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Terrain Material", nullptr));
#endif // QT_CONFIG(tooltip)
        terrainMaterialButton->setText(QString());
        terrainMaterialButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "envira", nullptr)));
#if QT_CONFIG(tooltip)
        terrainMaterialSmoothButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Terrain Material Smooth", nullptr));
#endif // QT_CONFIG(tooltip)
        terrainMaterialSmoothButton->setText(QString());
        terrainMaterialSmoothButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "bullseye", nullptr)));
#if QT_CONFIG(tooltip)
        decalPaintButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Decal Paint", nullptr));
#endif // QT_CONFIG(tooltip)
        decalPaintButton->setText(QString());
        decalPaintButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "pencil", nullptr)));
#if QT_CONFIG(tooltip)
        decalSelectButton->setToolTip(QCoreApplication::translate("bcBlackCatEditorClass", "Decal Select", nullptr));
#endif // QT_CONFIG(tooltip)
        decalSelectButton->setText(QString());
        decalSelectButton->setProperty("fontAwesome", QVariant(QCoreApplication::translate("bcBlackCatEditorClass", "pencilsquareo", nullptr)));
    } // retranslateUi

};

namespace Ui {
    class bcBlackCatEditorClass: public Ui_bcBlackCatEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BCBLACKCATEDITOR_H
