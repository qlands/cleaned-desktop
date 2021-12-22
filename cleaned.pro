QT += core widgets sql webenginewidgets
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
                aboutcleaned.h \
                addfeed.h \
                addfertilizer.h \
                addlivestock.h \
                addseason.h \
                basketmodel.h \
                cleanedstudy.h \
                combodelegate.h \
                cropinputsmodel.h \
                delegatordialog.h \
                feedsmodel.h \
                fertilizermodel.h \
                livestockmodel.h \
                runmodelsdialog.h \
                seasonmodel.h \
                settings.h \
                versiondialog.h
SOURCES       = main.cpp \
                aboutcleaned.cpp \
                addfeed.cpp \
                addfertilizer.cpp \
                addlivestock.cpp \
                addseason.cpp \
                basketmodel.cpp \
                cleanedstudy.cpp \
                combodelegate.cpp \
                cropinputsmodel.cpp \
                delegatordialog.cpp \
                feedsmodel.cpp \
                fertilizermodel.cpp \
                livestockmodel.cpp \
                mainwindow.cpp \
                runmodelsdialog.cpp \
                seasonmodel.cpp \
                settings.cpp \
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
    delegatordialog.ui \
    runmodelsdialog.ui \
    settings.ui \
    versiondialog.ui
