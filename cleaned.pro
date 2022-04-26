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
                clonedatabase.h \
                combodelegate.h \
                comparemodelsdialog.h \
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
                versiondialog.h
SOURCES       = main.cpp \
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
    delegatordialog.ui \
    importdatabase.ui \
    managedatabases.ui \
    runmodelsdialog.ui \
    selectdatabase.ui \
    settings.ui \
    versiondialog.ui
