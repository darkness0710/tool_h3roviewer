QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++23

#QMAKE_CXXFLAGS += -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
win32-msvc* {
    MSVC_LIB=$$(MSVC_LIB_DIR)
    isEmpty(MSVC_LIB){
      error("The environment variable \"MSVC_LIB_DIR\" is not set. This needs to have the path to windows kit which contains msvc libraries (requires shell32.lib). Set it as an environment variable in the project. Remember to add quotation marks around the path.")
    }
    LIBS += -L$(MSVC_LIB_DIR) -ladvapi32
} else:win32 {
    QMAKE_CXXFLAGS += -pedantic -Wall -Wextra -Wcast-qual -Wcast-align -Wstrict-null-sentinel -Werror -Wno-unused -Wold-style-cast
}


RC_ICONS = resources/images/main/icon.ico

VERSION = 1.3.0.0
QMAKE_TARGET_DESCRIPTION = Heroes 3 HotA Hero Viewewr

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/data_providers/battlelossprovider.cpp \
    src/data_providers/buffprovider.cpp \
    src/data_providers/creaturecountprovider.cpp \
    src/data_providers/creaturetypeprovider.cpp \
    src/data_providers/dataprovider.cpp \
    src/data_providers/deltastatsprovider.cpp \
    src/data_providers/equipmentprovider.cpp \
    src/data_providers/heronameprovider.cpp \
    src/data_providers/levelprogressprovider.cpp \
    src/data_providers/luckmoraleprovider.cpp \
    src/data_providers/manaprovider.cpp \
    src/data_providers/movementProvider.cpp \
    src/data_providers/playercolorprovider.cpp \
    src/data_providers/primaryskillprovider.cpp \
    src/data_providers/skillprovider.cpp \
    src/data_providers/specialtyprovider.cpp \
    src/data_providers/statusprovider.cpp \
    src/edituiwindow.cpp \
    src/imageframe.cpp \
    src/imageframeborder.cpp \
    src/imageframeedit.cpp \
    src/outlinelabel.cpp \
    src/aboutwindow.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/memoryscanner.cpp \
    src/offsetscanner.cpp \
    src/providercollection.cpp \
    src/settingswindow.cpp

HEADERS += \
    src/data_providers/battlelossprovider.h \
    src/data_providers/buffprovider.h \
    src/data_providers/creaturecountprovider.h \
    src/data_providers/creaturetypeprovider.h \
    src/data_providers/dataprovider.h \
    src/data_providers/deltastatsprovider.h \
    src/data_providers/equipmentprovider.h \
    src/data_providers/heronameprovider.h \
    src/data_providers/levelprogressprovider.h \
    src/data_providers/luckmoraleprovider.h \
    src/data_providers/manaprovider.h \
    src/data_providers/movementProvider.h \
    src/data_providers/playercolorprovider.h \
    src/data_providers/primaryskillprovider.h \
    src/data_providers/skillprovider.h \
    src/data_providers/specialtyprovider.h \
    src/data_providers/statusprovider.h \
    src/edituiwindow.h \
    src/gamestructs.h \
    src/imageframe.h \
    src/imageframeborder.h \
    src/imageframeedit.h \
    src/outlinelabel.h \
    src/aboutwindow.h \
    src/imagesMap.h \
    src/mainwindow.h \
    src/memoryscanner.h \
    src/offsetscanner.h \
    src/providercollection.h \
    src/settingswindow.h

FORMS += \
    forms/imageframeedit.ui \
    forms/mainwindow.ui \
    forms/aboutwindow.ui \
    forms/settingswindow.ui \
    forms/edituiwindow.ui

#TRANSLATIONS += \
#    H3roViewer_en_150.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc


copyReadme.commands = $(COPY_FILE) $$shell_path($$PWD\README.md) $$shell_path($$OUT_PWD/release/)
copyLicense.commands = $(COPY_FILE) $$shell_path($$PWD\LICENSE.md) $$shell_path($$OUT_PWD/release/)
first.depends = $(first) copyReadme copyLicense
export(first.depends)
export(copyReadme.commands)
export(copyLicense.commands)

QMAKE_EXTRA_TARGETS += first copyReadme copyLicense

DISTFILES += \
    resources/images/items/Artifact_Cannon.png
