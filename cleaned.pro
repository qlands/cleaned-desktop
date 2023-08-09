QT += core widgets sql webenginewidgets
requires(qtConfig(filedialog))

system("git submodule update --init")

QXLSX_PARENTPATH=$$PWD/
QXLSX_HEADERPATH=./external/QXlsx/QXlsx/header/
QXLSX_SOURCEPATH=./external/QXlsx/QXlsx/source/
include(./external/QXlsx/QXlsx/QXlsx.pri)

HEADERS       += mainwindow.h \
                aboutcleaned.h \
                addfeed.h \
                addfertilizer.h \
                addlivestock.h \
                addseason.h \
                basketmodel.h \
                cleanedstudy.h \
                clonedatabase.h \
                combodelegate.h \
                comparemodelsdialog.h \
                comparemodelsresultdialog.h \
                cropinputsmodel.h \
                databasemodel.h \
                delegatordialog.h \
                feedsmodel.h \
                fertilizermodel.h \
                importdatabase.h \
                livestockmodel.h \
                managedatabases.h \
                runmodelsdialog.h \
                seasonmodel.h \
                selectdatabase.h \
                settings.h \
                technicalmanualcleand.h \
                versiondialog.h

SOURCES       += main.cpp \
                aboutcleaned.cpp \
                addfeed.cpp \
                addfertilizer.cpp \
                addlivestock.cpp \
                addseason.cpp \
                basketmodel.cpp \
                cleanedstudy.cpp \
                clonedatabase.cpp \
                combodelegate.cpp \
                comparemodelsdialog.cpp \
                comparemodelsresultdialog.cpp \
                cropinputsmodel.cpp \
                databasemodel.cpp \
                delegatordialog.cpp \
                feedsmodel.cpp \
                fertilizermodel.cpp \
                importdatabase.cpp \
                livestockmodel.cpp \
                mainwindow.cpp \
                managedatabases.cpp \
                runmodelsdialog.cpp \
                seasonmodel.cpp \
                selectdatabase.cpp \
                settings.cpp \
                technicalmanualcleand.cpp \
                versiondialog.cpp
RESOURCES     = cleaned.qrc

TARGET = cleaned

QMAKE_LFLAGS += -fuse-ld=gold

TEMPLATE = app

FORMS += \
    aboutcleaned.ui \
    addfeed.ui \
    addfertilizer.ui \
    addlivestock.ui \
    addseason.ui \
    cleanedstudy.ui \
    clonedatabase.ui \
    comparemodelsdialog.ui \
    comparemodelsresultdialog.ui \
    delegatordialog.ui \
    importdatabase.ui \
    managedatabases.ui \
    runmodelsdialog.ui \
    selectdatabase.ui \
    settings.ui \
    technicalmanualcleand.ui \
    versiondialog.ui
