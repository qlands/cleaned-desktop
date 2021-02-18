QT += core widgets sql 
requires(qtConfig(filedialog))

HEADERS       = mainwindow.h \
                addfeed.h \
                addfertilizer.h \
                addlivestock.h \
                addseason.h \
                basketmodel.h \
                cleanedstudy.h \
                combodelegate.h \
                cropinputsmodel.h \
                feedsmodel.h \
                fertilizermodel.h \
                livestockmodel.h \
                seasonmodel.h \
                settings.h
SOURCES       = main.cpp \
                addfeed.cpp \
                addfertilizer.cpp \
                addlivestock.cpp \
                addseason.cpp \
                basketmodel.cpp \
                cleanedstudy.cpp \
                combodelegate.cpp \
                cropinputsmodel.cpp \
                feedsmodel.cpp \
                fertilizermodel.cpp \
                livestockmodel.cpp \
                mainwindow.cpp \
                seasonmodel.cpp \
                settings.cpp
RESOURCES     = cleaned.qrc

TARGET = cleaned

TEMPLATE = app

FORMS += \
    addfeed.ui \
    addfertilizer.ui \
    addlivestock.ui \
    addseason.ui \
    cleanedstudy.ui \
    settings.ui
