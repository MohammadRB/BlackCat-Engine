/********************************************************************************
** Form generated from reading UI file 'bcBlackCatEditor.ui'
**
** Created by: Qt User Interface Compiler version 5.11.2
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
    QLineEdit *decalRotationText;
    QLabel *decalNameLabel;
    QLabel *decalRotationLabel;
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
            bcBlackCatEditorClass->setObjectName(QStringLiteral("bcBlackCatEditorClass"));
        bcBlackCatEditorClass->resize(1600, 900);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(bcBlackCatEditorClass->sizePolicy().hasHeightForWidth());
        bcBlackCatEditorClass->setSizePolicy(sizePolicy);
        bcBlackCatEditorClass->setMinimumSize(QSize(1600, 900));
        bcBlackCatEditorClass->setBaseSize(QSize(1200, 750));
        newSceneMenu = new QAction(bcBlackCatEditorClass);
        newSceneMenu->setObjectName(QStringLiteral("newSceneMenu"));
        loadSceneMenu = new QAction(bcBlackCatEditorClass);
        loadSceneMenu->setObjectName(QStringLiteral("loadSceneMenu"));
        saveSceneMenu = new QAction(bcBlackCatEditorClass);
        saveSceneMenu->setObjectName(QStringLiteral("saveSceneMenu"));
        gameModeButton = new QAction(bcBlackCatEditorClass);
        gameModeButton->setObjectName(QStringLiteral("gameModeButton"));
        gameModeButton->setCheckable(true);
        editorModeButton = new QAction(bcBlackCatEditorClass);
        editorModeButton->setObjectName(QStringLiteral("editorModeButton"));
        editorModeButton->setCheckable(true);
        editorModeButton->setChecked(true);
        centralWidget = new QWidget(bcBlackCatEditorClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        verticalLayout_4 = new QVBoxLayout(centralWidget);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(3, 3, 3, 3);
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
        leftRenderFrame->setMinimumSize(QSize(0, 550));
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
        rightToolBoxObjectSelect = new QWidget();
        rightToolBoxObjectSelect->setObjectName(QStringLiteral("rightToolBoxObjectSelect"));
        rightToolBoxObjectSelect->setGeometry(QRect(0, 0, 340, 670));
        entityNameLabel = new QLabel(rightToolBoxObjectSelect);
        entityNameLabel->setObjectName(QStringLiteral("entityNameLabel"));
        entityNameLabel->setGeometry(QRect(10, 10, 81, 16));
        entityNameLabelValue = new QLabel(rightToolBoxObjectSelect);
        entityNameLabelValue->setObjectName(QStringLiteral("entityNameLabelValue"));
        entityNameLabelValue->setGeometry(QRect(100, 10, 231, 16));
        entityDeleteButton = new QPushButton(rightToolBoxObjectSelect);
        entityDeleteButton->setObjectName(QStringLiteral("entityDeleteButton"));
        entityDeleteButton->setEnabled(false);
        entityDeleteButton->setGeometry(QRect(160, 180, 81, 31));
        entityPositionLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionLabel->setObjectName(QStringLiteral("entityPositionLabel"));
        entityPositionLabel->setGeometry(QRect(10, 40, 81, 16));
        entityPositionXText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionXText->setObjectName(QStringLiteral("entityPositionXText"));
        entityPositionXText->setEnabled(false);
        entityPositionXText->setGeometry(QRect(30, 70, 91, 25));
        entityPositionXText->setReadOnly(false);
        entityPositionXLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionXLabel->setObjectName(QStringLiteral("entityPositionXLabel"));
        entityPositionXLabel->setGeometry(QRect(10, 70, 16, 16));
        entityPositionYLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionYLabel->setObjectName(QStringLiteral("entityPositionYLabel"));
        entityPositionYLabel->setGeometry(QRect(10, 100, 16, 16));
        entityPositionYText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionYText->setObjectName(QStringLiteral("entityPositionYText"));
        entityPositionYText->setEnabled(false);
        entityPositionYText->setGeometry(QRect(30, 100, 91, 25));
        entityPositionYText->setReadOnly(false);
        entityPositionZLabel = new QLabel(rightToolBoxObjectSelect);
        entityPositionZLabel->setObjectName(QStringLiteral("entityPositionZLabel"));
        entityPositionZLabel->setGeometry(QRect(10, 130, 16, 16));
        entityPositionZText = new QLineEdit(rightToolBoxObjectSelect);
        entityPositionZText->setObjectName(QStringLiteral("entityPositionZText"));
        entityPositionZText->setEnabled(false);
        entityPositionZText->setGeometry(QRect(30, 130, 91, 25));
        entityPositionZText->setReadOnly(false);
        entityRotationZText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationZText->setObjectName(QStringLiteral("entityRotationZText"));
        entityRotationZText->setEnabled(false);
        entityRotationZText->setGeometry(QRect(160, 130, 91, 25));
        entityRotationZText->setReadOnly(false);
        entityRotationXLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationXLabel->setObjectName(QStringLiteral("entityRotationXLabel"));
        entityRotationXLabel->setGeometry(QRect(140, 70, 16, 16));
        entityRotationYLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationYLabel->setObjectName(QStringLiteral("entityRotationYLabel"));
        entityRotationYLabel->setGeometry(QRect(140, 100, 16, 16));
        entityRotationLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationLabel->setObjectName(QStringLiteral("entityRotationLabel"));
        entityRotationLabel->setGeometry(QRect(140, 40, 81, 16));
        entityRotationYText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationYText->setObjectName(QStringLiteral("entityRotationYText"));
        entityRotationYText->setEnabled(false);
        entityRotationYText->setGeometry(QRect(160, 100, 91, 25));
        entityRotationYText->setReadOnly(false);
        entityRotationZLabel = new QLabel(rightToolBoxObjectSelect);
        entityRotationZLabel->setObjectName(QStringLiteral("entityRotationZLabel"));
        entityRotationZLabel->setGeometry(QRect(140, 130, 16, 16));
        entityRotationXText = new QLineEdit(rightToolBoxObjectSelect);
        entityRotationXText->setObjectName(QStringLiteral("entityRotationXText"));
        entityRotationXText->setEnabled(false);
        entityRotationXText->setGeometry(QRect(160, 70, 91, 25));
        entityRotationXText->setReadOnly(false);
        entityUpdateButton = new QPushButton(rightToolBoxObjectSelect);
        entityUpdateButton->setObjectName(QStringLiteral("entityUpdateButton"));
        entityUpdateButton->setEnabled(false);
        entityUpdateButton->setGeometry(QRect(30, 180, 81, 31));
        rightToolBox->addItem(rightToolBoxObjectSelect, QStringLiteral("Object Select"));
        rightToolBoxObjectInsert = new QWidget();
        rightToolBoxObjectInsert->setObjectName(QStringLiteral("rightToolBoxObjectInsert"));
        rightToolBoxObjectInsert->setGeometry(QRect(0, 0, 340, 670));
        horizontalLayout_2 = new QHBoxLayout(rightToolBoxObjectInsert);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        entityNameList = new QListWidget(rightToolBoxObjectInsert);
        entityNameList->setObjectName(QStringLiteral("entityNameList"));

        horizontalLayout_2->addWidget(entityNameList);

        rightToolBox->addItem(rightToolBoxObjectInsert, QStringLiteral("Object Insert"));
        rightToolBoxTerrain = new QWidget();
        rightToolBoxTerrain->setObjectName(QStringLiteral("rightToolBoxTerrain"));
        rightToolBoxTerrain->setGeometry(QRect(0, 0, 340, 670));
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

        terrainSmoothLable = new QLabel(rightToolBoxTerrain);
        terrainSmoothLable->setObjectName(QStringLiteral("terrainSmoothLable"));

        formLayout->setWidget(6, QFormLayout::LabelRole, terrainSmoothLable);

        terrainSmoothSlider = new QSlider(rightToolBoxTerrain);
        terrainSmoothSlider->setObjectName(QStringLiteral("terrainSmoothSlider"));
        terrainSmoothSlider->setMinimum(1);
        terrainSmoothSlider->setMaximum(6);
        terrainSmoothSlider->setPageStep(1);
        terrainSmoothSlider->setOrientation(Qt::Horizontal);

        formLayout->setWidget(7, QFormLayout::SpanningRole, terrainSmoothSlider);

        terrainSmoothValue = new QLabel(rightToolBoxTerrain);
        terrainSmoothValue->setObjectName(QStringLiteral("terrainSmoothValue"));

        formLayout->setWidget(6, QFormLayout::FieldRole, terrainSmoothValue);

        terrainMaterialLable = new QLabel(rightToolBoxTerrain);
        terrainMaterialLable->setObjectName(QStringLiteral("terrainMaterialLable"));

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
        terrainMaterialCombo->setObjectName(QStringLiteral("terrainMaterialCombo"));

        formLayout->setWidget(11, QFormLayout::SpanningRole, terrainMaterialCombo);

        terrainRadiusSlider = new QSlider(rightToolBoxTerrain);
        terrainRadiusSlider->setObjectName(QStringLiteral("terrainRadiusSlider"));
        terrainRadiusSlider->setMinimum(1);
        terrainRadiusSlider->setMaximum(50);
        terrainRadiusSlider->setSingleStep(1);
        terrainRadiusSlider->setValue(25);
        terrainRadiusSlider->setOrientation(Qt::Horizontal);
        terrainRadiusSlider->setInvertedAppearance(false);
        terrainRadiusSlider->setTickInterval(0);

        formLayout->setWidget(4, QFormLayout::SpanningRole, terrainRadiusSlider);

        terrainRadiusLable = new QLabel(rightToolBoxTerrain);
        terrainRadiusLable->setObjectName(QStringLiteral("terrainRadiusLable"));

        formLayout->setWidget(3, QFormLayout::LabelRole, terrainRadiusLable);

        terrainRadiusValue = new QLabel(rightToolBoxTerrain);
        terrainRadiusValue->setObjectName(QStringLiteral("terrainRadiusValue"));

        formLayout->setWidget(3, QFormLayout::FieldRole, terrainRadiusValue);

        QIcon icon;
        QString iconThemeName = QStringLiteral("areaChart");
        if (QIcon::hasThemeIcon(iconThemeName)) {
            icon = QIcon::fromTheme(iconThemeName);
        } else {
            icon.addFile(QStringLiteral("C:/Users/moham/.designer/backup"), QSize(), QIcon::Normal, QIcon::Off);
        }
        rightToolBox->addItem(rightToolBoxTerrain, icon, QStringLiteral("Terrain"));
        rightToolBoxDecalInsert = new QWidget();
        rightToolBoxDecalInsert->setObjectName(QStringLiteral("rightToolBoxDecalInsert"));
        rightToolBoxDecalInsert->setGeometry(QRect(0, 0, 340, 670));
        horizontalLayout = new QHBoxLayout(rightToolBoxDecalInsert);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        decalNameList = new QListWidget(rightToolBoxDecalInsert);
        decalNameList->setObjectName(QStringLiteral("decalNameList"));

        horizontalLayout->addWidget(decalNameList);

        rightToolBox->addItem(rightToolBoxDecalInsert, QStringLiteral("Decal Paint"));
        rightToolBoxDecalSelect = new QWidget();
        rightToolBoxDecalSelect->setObjectName(QStringLiteral("rightToolBoxDecalSelect"));
        rightToolBoxDecalSelect->setGeometry(QRect(0, 0, 340, 670));
        decalUpdateButton = new QPushButton(rightToolBoxDecalSelect);
        decalUpdateButton->setObjectName(QStringLiteral("decalUpdateButton"));
        decalUpdateButton->setEnabled(false);
        decalUpdateButton->setGeometry(QRect(30, 80, 81, 31));
        decalDeleteButton = new QPushButton(rightToolBoxDecalSelect);
        decalDeleteButton->setObjectName(QStringLiteral("decalDeleteButton"));
        decalDeleteButton->setEnabled(false);
        decalDeleteButton->setGeometry(QRect(160, 80, 81, 31));
        decalRotationText = new QLineEdit(rightToolBoxDecalSelect);
        decalRotationText->setObjectName(QStringLiteral("decalRotationText"));
        decalRotationText->setEnabled(false);
        decalRotationText->setGeometry(QRect(100, 30, 91, 25));
        decalRotationText->setReadOnly(false);
        decalNameLabel = new QLabel(rightToolBoxDecalSelect);
        decalNameLabel->setObjectName(QStringLiteral("decalNameLabel"));
        decalNameLabel->setGeometry(QRect(10, 0, 81, 16));
        decalRotationLabel = new QLabel(rightToolBoxDecalSelect);
        decalRotationLabel->setObjectName(QStringLiteral("decalRotationLabel"));
        decalRotationLabel->setGeometry(QRect(10, 30, 81, 16));
        decalNameLabelValue = new QLabel(rightToolBoxDecalSelect);
        decalNameLabelValue->setObjectName(QStringLiteral("decalNameLabelValue"));
        decalNameLabelValue->setGeometry(QRect(100, 0, 231, 16));
        rightToolBox->addItem(rightToolBoxDecalSelect, QStringLiteral("Decal Select"));

        verticalLayout_2->addWidget(rightToolBox);


        mainLayout->addWidget(rightFrame);


        verticalLayout_4->addLayout(mainLayout);

        bcBlackCatEditorClass->setCentralWidget(centralWidget);
        mainMenuBar = new QMenuBar(bcBlackCatEditorClass);
        mainMenuBar->setObjectName(QStringLiteral("mainMenuBar"));
        mainMenuBar->setGeometry(QRect(0, 0, 1600, 21));
        fileMenu = new QMenu(mainMenuBar);
        fileMenu->setObjectName(QStringLiteral("fileMenu"));
        bcBlackCatEditorClass->setMenuBar(mainMenuBar);
        mainToolBar = new QToolBar(bcBlackCatEditorClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
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
        statusBar->setObjectName(QStringLiteral("statusBar"));
        QSizePolicy sizePolicy5(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(statusBar->sizePolicy().hasHeightForWidth());
        statusBar->setSizePolicy(sizePolicy5);
        bcBlackCatEditorClass->setStatusBar(statusBar);
        toolsDock = new QDockWidget(bcBlackCatEditorClass);
        toolsDock->setObjectName(QStringLiteral("toolsDock"));
        QSizePolicy sizePolicy6(QSizePolicy::Maximum, QSizePolicy::Preferred);
        sizePolicy6.setHorizontalStretch(0);
        sizePolicy6.setVerticalStretch(0);
        sizePolicy6.setHeightForWidth(toolsDock->sizePolicy().hasHeightForWidth());
        toolsDock->setSizePolicy(sizePolicy6);
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
        QSizePolicy sizePolicy7(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy7.setHorizontalStretch(0);
        sizePolicy7.setVerticalStretch(0);
        sizePolicy7.setHeightForWidth(objectSelectButton->sizePolicy().hasHeightForWidth());
        objectSelectButton->setSizePolicy(sizePolicy7);
        objectSelectButton->setCheckable(true);
        objectSelectButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(objectSelectButton);

        objectInsertButton = new QToolButton(toolsDockContents);
        objectInsertButton->setObjectName(QStringLiteral("objectInsertButton"));
        sizePolicy7.setHeightForWidth(objectInsertButton->sizePolicy().hasHeightForWidth());
        objectInsertButton->setSizePolicy(sizePolicy7);
        objectInsertButton->setCheckable(true);
        objectInsertButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(objectInsertButton);

        terrainHeightButton = new QToolButton(toolsDockContents);
        terrainHeightButton->setObjectName(QStringLiteral("terrainHeightButton"));
        sizePolicy7.setHeightForWidth(terrainHeightButton->sizePolicy().hasHeightForWidth());
        terrainHeightButton->setSizePolicy(sizePolicy7);
        terrainHeightButton->setMinimumSize(QSize(0, 0));
        terrainHeightButton->setSizeIncrement(QSize(0, 0));
        terrainHeightButton->setBaseSize(QSize(0, 0));
        terrainHeightButton->setCheckable(true);
        terrainHeightButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainHeightButton);

        terrainSmoothButton = new QToolButton(toolsDockContents);
        terrainSmoothButton->setObjectName(QStringLiteral("terrainSmoothButton"));
        sizePolicy7.setHeightForWidth(terrainSmoothButton->sizePolicy().hasHeightForWidth());
        terrainSmoothButton->setSizePolicy(sizePolicy7);
        terrainSmoothButton->setCheckable(true);
        terrainSmoothButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainSmoothButton);

        terrainMaterialButton = new QToolButton(toolsDockContents);
        terrainMaterialButton->setObjectName(QStringLiteral("terrainMaterialButton"));
        sizePolicy7.setHeightForWidth(terrainMaterialButton->sizePolicy().hasHeightForWidth());
        terrainMaterialButton->setSizePolicy(sizePolicy7);
        terrainMaterialButton->setCheckable(true);
        terrainMaterialButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainMaterialButton);

        terrainMaterialSmoothButton = new QToolButton(toolsDockContents);
        terrainMaterialSmoothButton->setObjectName(QStringLiteral("terrainMaterialSmoothButton"));
        sizePolicy7.setHeightForWidth(terrainMaterialSmoothButton->sizePolicy().hasHeightForWidth());
        terrainMaterialSmoothButton->setSizePolicy(sizePolicy7);
        terrainMaterialSmoothButton->setCheckable(true);
        terrainMaterialSmoothButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(terrainMaterialSmoothButton);

        decalPaintButton = new QToolButton(toolsDockContents);
        decalPaintButton->setObjectName(QStringLiteral("decalPaintButton"));
        sizePolicy7.setHeightForWidth(decalPaintButton->sizePolicy().hasHeightForWidth());
        decalPaintButton->setSizePolicy(sizePolicy7);
        decalPaintButton->setCheckable(true);
        decalPaintButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(decalPaintButton);

        decalSelectButton = new QToolButton(toolsDockContents);
        decalSelectButton->setObjectName(QStringLiteral("decalSelectButton"));
        sizePolicy7.setHeightForWidth(decalSelectButton->sizePolicy().hasHeightForWidth());
        decalSelectButton->setSizePolicy(sizePolicy7);
        decalSelectButton->setCheckable(true);
        decalSelectButton->setAutoExclusive(true);

        verticalLayout_3->addWidget(decalSelectButton);

        toolsDock->setWidget(toolsDockContents);
        bcBlackCatEditorClass->addDockWidget(static_cast<Qt::DockWidgetArea>(1), toolsDock);

        mainMenuBar->addAction(fileMenu->menuAction());
        fileMenu->addAction(newSceneMenu);
        fileMenu->addAction(loadSceneMenu);
        fileMenu->addAction(saveSceneMenu);
        mainToolBar->addAction(gameModeButton);
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
        bcBlackCatEditorClass->setWindowTitle(QApplication::translate("bcBlackCatEditorClass", "BlackCatEditor", nullptr));
        newSceneMenu->setText(QApplication::translate("bcBlackCatEditorClass", "NewScene", nullptr));
        loadSceneMenu->setText(QApplication::translate("bcBlackCatEditorClass", "LoadScene", nullptr));
        saveSceneMenu->setText(QApplication::translate("bcBlackCatEditorClass", "SaveScene", nullptr));
        gameModeButton->setText(QApplication::translate("bcBlackCatEditorClass", "GameMode", nullptr));
#ifndef QT_NO_TOOLTIP
        gameModeButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "GameMode", nullptr));
#endif // QT_NO_TOOLTIP
        gameModeButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "play", nullptr)));
        editorModeButton->setText(QApplication::translate("bcBlackCatEditorClass", "EditorMode", nullptr));
#ifndef QT_NO_TOOLTIP
        editorModeButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "EditorMode", nullptr));
#endif // QT_NO_TOOLTIP
        editorModeButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "stop", nullptr)));
        leftBottomTab->setTabText(leftBottomTab->indexOf(consoleTab), QApplication::translate("bcBlackCatEditorClass", "Console", nullptr));
        rightToolBox->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "handpointero,cubes,areachart,pencil,pencilsquareo", nullptr)));
        entityNameLabel->setText(QApplication::translate("bcBlackCatEditorClass", "Entity Name:", nullptr));
        entityNameLabelValue->setText(QString());
        entityDeleteButton->setText(QApplication::translate("bcBlackCatEditorClass", "Delete", nullptr));
        entityPositionLabel->setText(QApplication::translate("bcBlackCatEditorClass", "Entity Position:", nullptr));
        entityPositionXLabel->setText(QApplication::translate("bcBlackCatEditorClass", "x:", nullptr));
        entityPositionYLabel->setText(QApplication::translate("bcBlackCatEditorClass", "y:", nullptr));
        entityPositionZLabel->setText(QApplication::translate("bcBlackCatEditorClass", "z:", nullptr));
        entityRotationXLabel->setText(QApplication::translate("bcBlackCatEditorClass", "x:", nullptr));
        entityRotationYLabel->setText(QApplication::translate("bcBlackCatEditorClass", "y:", nullptr));
        entityRotationLabel->setText(QApplication::translate("bcBlackCatEditorClass", "Entity Rotation:", nullptr));
        entityRotationZLabel->setText(QApplication::translate("bcBlackCatEditorClass", "z:", nullptr));
        entityUpdateButton->setText(QApplication::translate("bcBlackCatEditorClass", "Update", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxObjectSelect), QApplication::translate("bcBlackCatEditorClass", "Object Select", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxObjectInsert), QApplication::translate("bcBlackCatEditorClass", "Object Insert", nullptr));
        terrainHeightLable->setText(QApplication::translate("bcBlackCatEditorClass", "Height: ", nullptr));
        terrainHeightValue->setText(QApplication::translate("bcBlackCatEditorClass", "0", nullptr));
        terrainSmoothLable->setText(QApplication::translate("bcBlackCatEditorClass", "Smooth:", nullptr));
        terrainSmoothValue->setText(QApplication::translate("bcBlackCatEditorClass", "1", nullptr));
        terrainMaterialLable->setText(QApplication::translate("bcBlackCatEditorClass", "Material: ", nullptr));
        terrainMaterialCombo->setItemText(0, QApplication::translate("bcBlackCatEditorClass", "Material 1", nullptr));
        terrainMaterialCombo->setItemText(1, QApplication::translate("bcBlackCatEditorClass", "Material 2", nullptr));
        terrainMaterialCombo->setItemText(2, QApplication::translate("bcBlackCatEditorClass", "Material 3", nullptr));
        terrainMaterialCombo->setItemText(3, QApplication::translate("bcBlackCatEditorClass", "Material 4", nullptr));
        terrainMaterialCombo->setItemText(4, QApplication::translate("bcBlackCatEditorClass", "Material 5", nullptr));
        terrainMaterialCombo->setItemText(5, QApplication::translate("bcBlackCatEditorClass", "Material 6", nullptr));
        terrainMaterialCombo->setItemText(6, QApplication::translate("bcBlackCatEditorClass", "Material 7", nullptr));
        terrainMaterialCombo->setItemText(7, QApplication::translate("bcBlackCatEditorClass", "Material 8", nullptr));

        terrainRadiusLable->setText(QApplication::translate("bcBlackCatEditorClass", "Radius: ", nullptr));
        terrainRadiusValue->setText(QApplication::translate("bcBlackCatEditorClass", "25", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxTerrain), QApplication::translate("bcBlackCatEditorClass", "Terrain", nullptr));
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxDecalInsert), QApplication::translate("bcBlackCatEditorClass", "Decal Paint", nullptr));
        decalUpdateButton->setText(QApplication::translate("bcBlackCatEditorClass", "Update", nullptr));
        decalDeleteButton->setText(QApplication::translate("bcBlackCatEditorClass", "Delete", nullptr));
        decalNameLabel->setText(QApplication::translate("bcBlackCatEditorClass", "Decal Name:", nullptr));
        decalRotationLabel->setText(QApplication::translate("bcBlackCatEditorClass", "Decal Rotation:", nullptr));
        decalNameLabelValue->setText(QString());
        rightToolBox->setItemText(rightToolBox->indexOf(rightToolBoxDecalSelect), QApplication::translate("bcBlackCatEditorClass", "Decal Select", nullptr));
        fileMenu->setTitle(QApplication::translate("bcBlackCatEditorClass", "File", nullptr));
#ifndef QT_NO_TOOLTIP
        objectSelectButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Object Select", nullptr));
#endif // QT_NO_TOOLTIP
        objectSelectButton->setText(QString());
        objectSelectButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "handpointero", nullptr)));
#ifndef QT_NO_TOOLTIP
        objectInsertButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Object Insert", nullptr));
#endif // QT_NO_TOOLTIP
        objectInsertButton->setText(QString());
        objectInsertButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "cubes", nullptr)));
#ifndef QT_NO_TOOLTIP
        terrainHeightButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Height", nullptr));
#endif // QT_NO_TOOLTIP
        terrainHeightButton->setText(QString());
        terrainHeightButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "areachart", nullptr)));
#ifndef QT_NO_TOOLTIP
        terrainSmoothButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Smooth", nullptr));
#endif // QT_NO_TOOLTIP
        terrainSmoothButton->setText(QString());
        terrainSmoothButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "bullseye", nullptr)));
#ifndef QT_NO_TOOLTIP
        terrainMaterialButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Material", nullptr));
#endif // QT_NO_TOOLTIP
        terrainMaterialButton->setText(QString());
        terrainMaterialButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "envira", nullptr)));
#ifndef QT_NO_TOOLTIP
        terrainMaterialSmoothButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Terrain Material Smooth", nullptr));
#endif // QT_NO_TOOLTIP
        terrainMaterialSmoothButton->setText(QString());
        terrainMaterialSmoothButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "bullseye", nullptr)));
#ifndef QT_NO_TOOLTIP
        decalPaintButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Decal Paint", nullptr));
#endif // QT_NO_TOOLTIP
        decalPaintButton->setText(QString());
        decalPaintButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "pencil", nullptr)));
#ifndef QT_NO_TOOLTIP
        decalSelectButton->setToolTip(QApplication::translate("bcBlackCatEditorClass", "Decal Select", nullptr));
#endif // QT_NO_TOOLTIP
        decalSelectButton->setText(QString());
        decalSelectButton->setProperty("fontAwesome", QVariant(QApplication::translate("bcBlackCatEditorClass", "pencilsquareo", nullptr)));
    } // retranslateUi

};

namespace Ui {
    class bcBlackCatEditorClass: public Ui_bcBlackCatEditorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BCBLACKCATEDITOR_H
