#-------------------------------------------------
#
# Project created by QtCreator 2016-05-24T17:06:01
#
#-------------------------------------------------


QT       += core gui concurrent sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

## Comment out if you need OpenGL support
DEFINES += QV_WITHOUT_OPENGL

contains(DEFINES, QV_WITHOUT_OPENGL) {
    message(QuickViewer without OpenGL Support)
} else {
    message(QuickViewer with OpenGL Support)
    QT += opengl opengl-private
}

VERSION = 1.0.4

TARGET = QuickViewer
TEMPLATE = app
CONFIG += plugin

QMAKE_TARGET_COMPANY = KATO Kanryu(k.kanryu@gmail.com)
QMAKE_TARGET_PRODUCT = QuickViewer
QMAKE_TARGET_DESCRIPTION = QuickViewer for folders/archives
QMAKE_TARGET_COPYRIGHT = (C) 2017 KATO Kanryu

DEFINES += \
  APP_VERSION=\\\"$$VERSION\\\" \
  APP_NAME=\\\"$$QMAKE_TARGET_PRODUCT\\\" \

CODECFORSRC = UTF-8


greaterThan(QT_MAJOR_VERSION, 4) {
    TARGET_ARCH=$${QT_ARCH}
} else {
    TARGET_ARCH=$${QMAKE_HOST.arch}
}
win32 {
    contains(TARGET_ARCH, x86_64) {
        TARGET_ARCH = x64
    } else {
        TARGET_ARCH = x86
    }
}

#linux:equals(TARGET_ARCH, x64) {
#} else {
#}

DESTDIR = ../bin

INCLUDEPATH += ../ResizeHalf/ResizeHalf
INCLUDEPATH += ../easyexif/easyexif
INCLUDEPATH += ../fileloader
INCLUDEPATH += ../zimg
INCLUDEPATH += ./src ./src/catalog ./src/widgets ./src/models ./src/folderview
INCLUDEPATH += ./src/qfullscreenframe ./src/qlanguageselector ./src/qnamedpipe ./src/qactionmanager


LIBDIR = ../lib

#LIBS += -L$${LIBDIR}  -leasyexif -lresizehalf -lfileloader -lQt7z -lunrar -lzimg -lzlib -lquazip
LIBS += -L$${LIBDIR}  -leasyexif -lresizehalf -lfileloader -lQt7z -lunrar -lzimg


win32 {
    win32-msvc* {
        QMAKE_CXXFLAGS += /wd4819
        # more heap area for x86
        equals(TARGET_ARCH, x86) {
            QMAKE_LFLAGS += /LARGEADDRESSAWARE
        }
    }
    LIBS += -luser32 -ladvapi32 -lShlwapi -loleaut32 -lole32 -luuid
}
unix {
    DEFINES += _UNIX
    QMAKE_LFLAGS += -Wl,-rpath,../lib
    GCC_MAJOR = 6
}


SOURCES += \
    src/catalog/catalogwindow.cpp \
    src/catalog/databasesettingdialog.cpp \
    src/catalog/managedatabasedialog.cpp \
    src/catalog/volumeitemmodel.cpp \
    src/exifdialog.cpp \
    src/folderview/folderitemdelegate.cpp \
    src/folderview/folderitemmodel.cpp \
    src/folderview/foldertreeview.cpp \
    src/folderview/folderwindow.cpp \
    src/imageview.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/models/bookprogressmanager.cpp \
    src/models/pagecontent.cpp \
    src/models/pagemanager.cpp \
    src/models/qvapplication.cpp \
    src/models/shadermanager.cpp \
    src/models/thumbnailmanager.cpp \
    src/models/timeorderdcache.cpp \
    src/optionsdialog.cpp \
    src/renamedialog.cpp \
    src/widgets/flowlayout.cpp \
    src/widgets/pageslider.cpp \
    src/qlanguageselector/qlanguageselector.cpp \
    src/qnamedpipe/qnamedpipe.cpp \
    src/qfullscreenframe/qinnerframe.cpp \
    src/models/qvmovie.cpp \
    src/models/volumemanager.cpp \
    src/models/volumemanagerbuilder.cpp \
    src/qactionmanager/keyconfigdialog.cpp \
    src/qactionmanager/mouseconfigdialog.cpp \
    src/qactionmanager/qactionmanager.cpp \
    src/qactionmanager/qmousesequence.cpp \
    src/qactionmanager/shortcutbutton.cpp \
    src/models/imagestring.cpp


HEADERS  += \
    src/catalog/catalogwindow.h \
    src/catalog/databasesettingdialog.h \
    src/catalog/managedatabasedialog.h \
    src/catalog/qc_init.h \
    src/catalog/volumeitemmodel.h \
    src/exifdialog.h \
    src/folderview/folderitemdelegate.h \
    src/folderview/folderitemmodel.h \
    src/folderview/foldertreeview.h \
    src/folderview/folderwindow.h \
    src/imageview.h \
    src/mainwindow.h \
    src/models/bookprogressmanager.h \
    src/models/pagecontent.h \
    src/models/pagemanager.h \
    src/models/qvapplication.h \
    src/models/shadermanager.h \
    src/models/thumbnailmanager.h \
    src/models/timeorderdcache.h \
    src/optionsdialog.h \
    src/qv_init.h \
    src/renamedialog.h \
    src/stdafx.h \
    src/widgets/flowlayout.h \
    src/widgets/pageslider.h \
    src/qlanguageselector/qlanguageselector.h \
    src/qnamedpipe/qnamedpipe.h \
    src/qfullscreenframe/qinnerframe.h \
    src/models/qvmovie.h \
    src/models/volumemanager.h \
    src/models/volumemanagerbuilder.h \
    src/qactionmanager/keyconfigdialog.h \
    src/qactionmanager/mouseconfigdialog.h \
    src/qactionmanager/qactionmanager.h \
    src/qactionmanager/qmousesequence.h \
    src/qactionmanager/shortcutbutton.h \
    src/models/imagestring.h

win32 {
    INCLUDEPATH += ../AssociateFilesWithQuickViewer
    SOURCES += src/mainwindowforwindows.cpp ../AssociateFilesWithQuickViewer/fileassocdialog.cpp
    HEADERS += src/mainwindowforwindows.h ../AssociateFilesWithQuickViewer/fileassocdialog.h
    *g++*: DEFINES += NTDDI_VERSION=NTDDI_VISTA

}


PRECOMPILED_HEADER += src/stdafx.h
precompile_header:!isEmpty(PRECOMPILED_HEADER) {
    DEFINES += USING_PCH
}

FORMS    += \
    src/mainwindow.ui \
    src/exifdialog.ui \
    src/qactionmanager/keyconfigdialog.ui \
    src/catalog/cataloglist.ui \
    src/catalog/catalogwindow.ui \
    src/catalog/createdb.ui \
    src/folderview/folderwindow.ui \
    src/optionsdialog.ui \
    src/renamedialog.ui \
    ../AssociateFilesWithQuickViewer/fileassocdialog.ui

RESOURCES += toolbar.qrc

!CONFIG(debug, debug|release) {
    win32 {
        RESOURCES += qtconf-win.qrc
    }
    unix {
        RESOURCES += qtconf-unix.qrc
    }
}

RC_ICONS = icons/appicon.ico


DISTFILES += \
    translations/quickviewer_ja.qm \
    translations/quickviewer_es.qm \
    translations/quickviewer_zh.qm \
    translations/quickviewer_el.qm \

# Shaders will be installed into DIST_DIR/shaders
SHADERS += \
    shaders/bicubic.frag \
    shaders/lanczos.frag \

DBS += \
    database/schema.sql \

OTHER_FILES  += qt.conf $$DBS

DBBIN += \
    database/thumbnail.sqlite3.db \

DBDIR += database/

!greaterThan(QT_MAJOR_VERSION, 4) {
    defineReplace(shell_path) {
        path = $$1
        return($$replace(path, "/", "\\"))
    }
}


# win32 depoying, please add 'jom install' into build setting on qt-creator
win32 : !CONFIG(debug, debug|release) {
    mingw {
        MY_DEFAULT_INSTALL = ../../QuickViewer-$${VERSION}-mingw-$${TARGET_ARCH}

        install_target.files = $${DESTDIR}/QuickViewer.exe $${DESTDIR}/AssociateFilesWithQuickViewer.exe $${LIBDIR}/fileloader.dll

        INSTALLS += install_target install_deploy_files install_translations install_assoc_icons
    } else {
        MY_DEFAULT_INSTALL = ../../QuickViewer-$${VERSION}-$${TARGET_ARCH}

        install_target.files = $${DESTDIR}/QuickViewer.exe $${DESTDIR}/AssociateFilesWithQuickViewer.exe

        install_qrawspeed.path = $${MY_DEFAULT_INSTALL}/imageformats
        install_qrawspeed.files = \
            ../../../qrawspeed/imageformats-$${TARGET_ARCH}/qrawspeed0.dll \
            ../../../qrawspeed/imageformats-$${TARGET_ARCH}/qapng2.dll \

#            ../../../qrawspeed/imageformats-$${TARGET_ARCH}/qlodepng0.dll \

        # dlls instead of vcredist_xxx.exe
        install_msvcrt.PATH = C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/redist/$${TARGET_ARCH}/Microsoft.VC140.CRT
        install_msvcrt.path = $${MY_DEFAULT_INSTALL}
        install_msvcrt.removefiles = $$shell_path($${MY_DEFAULT_INSTALL}/vcredist_$${TARGET_ARCH}.exe)
        install_msvcrt.commands = -$(DEL_FILE) "$${install_msvcrt.removefiles}"
        install_msvcrt.depends = install_install_deploy_files
        install_msvcrt.files = \
            "$${install_msvcrt.PATH}/concrt140.dll" \
            "$${install_msvcrt.PATH}/msvcp140.dll" \
            "$${install_msvcrt.PATH}/vccorlib140.dll" \
            "$${install_msvcrt.PATH}/vcruntime140.dll"

        INSTALLS += install_target install_deploy_files install_translations install_qrawspeed install_msvcrt install_assoc_icons
    }
    install_target.path = $${MY_DEFAULT_INSTALL}
#   install_target.files += $${DESTDIR}/QuickViewer.exe $${DESTDIR}/AssociateFilesWithQuickViewer.exe $${LIBDIR}/fileloader.dll
    install_deploy_files.path = $${MY_DEFAULT_INSTALL}
    install_deploy_files.files = $${PWD}/../README.md $${PWD}/../LICENSE
    install_deploy_files.commands = $$shell_path($$[QT_INSTALL_BINS]/windeployqt) --release --compiler-runtime $$shell_path($${MY_DEFAULT_INSTALL}/QuickViewer.exe)
    install_translations.path = $${MY_DEFAULT_INSTALL}/translations
    install_translations.files = \
        $${PWD}/translations/languages.ini \
        $${PWD}/translations/quickviewer_ja.qm \
        $${PWD}/translations/quickviewer_es.qm \
        $${PWD}/translations/quickviewer_zh.qm \
        $${PWD}/translations/quickviewer_el.qm \
        $${PWD}/translations/qt_el.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_zh_CN.qm \

    install_assoc_icons.path = $${MY_DEFAULT_INSTALL}/iconengines
    install_assoc_icons.files = \
        ../AssociateFilesWithQuickViewer/icons/qv_apng.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_bmp.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_dds.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_gif.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_icon.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_jpeg.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_png.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_raw.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_tga.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_tiff.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_webp.ico \

    install_shaders.path = $${MY_DEFAULT_INSTALL}/shaders
    install_shaders.files = $$SHADERS
    install_db.path = $${MY_DEFAULT_INSTALL}/database
    install_db.files = $$DBS $$DBBIN

    INSTALLS += install_db
    !contains(DEFINES, QV_WITHOUT_OPENGL) {
        INSTALLS += install_shaders
    }
    greaterThan(QT_MAJOR_VERSION, 4):greaterThan(QT_MINOR_VERSION, 8):win32 {
        install_direct2d.path = $${MY_DEFAULT_INSTALL}/platforms
        install_direct2d.files = $$[QT_INSTALL_PLUGINS]/platforms/qdirect2d.dll
        INSTALLS += install_direct2d
    }

}

# linuxdeployqt is required.
linux : !CONFIG(debug, debug|release) {
    APPDIR = QuickViewer-$${VERSION}-$${TARGET_ARCH}.AppDir
    APPIMAGE = QuickViewer-$${VERSION}-$${TARGET_ARCH}.AppImage
    MY_DEFAULT_INSTALL = ../../$${APPDIR}

    install_target.files = $${DESTDIR}/QuickViewer
    install_target.path = $${MY_DEFAULT_INSTALL}/usr/bin

    install_libs.files = $${DESTDIR}/../lib/libfileloader.so.1 $${DESTDIR}/../lib/lib7z.so
    install_libs.path = $${MY_DEFAULT_INSTALL}/usr/lib

    install_desktop.files = $${PWD}/QuickViewer.desktop $${PWD}/../docs/quickviewer.png
    install_desktop.path = $${MY_DEFAULT_INSTALL}

    install_deploy_files.path = $${MY_DEFAULT_INSTALL}
    install_deploy_files.files = $${PWD}/../README.md $${PWD}/AppRun
    install_deploy_files.commands = linuxdeployqt $${MY_DEFAULT_INSTALL}/QuickViewer.desktop -qmake=$$[QT_INSTALL_BINS]/qmake -bundle-non-qt-libs ; rm $${MY_DEFAULT_INSTALL}/AppRun
    install_deploy_files.depends = install_install_target install_install_libs install_install_desktop

    install_apprun.path = $${MY_DEFAULT_INSTALL}
    install_apprun.files = $${PWD}/../LICENSE
    install_apprun.commands = chmod 755 $${MY_DEFAULT_INSTALL}/AppRun
    install_apprun.depends = install_install_deploy_files

    install_translations.path = $${MY_DEFAULT_INSTALL}/translations
    install_translations.commands = ldd $${MY_DEFAULT_INSTALL}/usr/bin/QuickViewer | awk \'\$$1==\"libstdc++.so.$${GCC_MAJOR}\" {print \$$3}\' | xargs cp -t $${MY_DEFAULT_INSTALL}/usr/lib
    install_translations.files = \
        $${PWD}/translations/languages.ini \
        $${PWD}/translations/quickviewer_ja.qm \
        $${PWD}/translations/quickviewer_es.qm \
        $${PWD}/translations/quickviewer_zh.qm \
        $${PWD}/translations/quickviewer_el.qm \
        $${PWD}/translations/qt_el.qm \
        $$[QT_INSTALL_TRANSLATIONS]/qt_zh_CN.qm \

    install_assoc_icons.path = $${MY_DEFAULT_INSTALL}/usr/shared/icons
    install_assoc_icons.files = \
        ../AssociateFilesWithQuickViewer/icons/qv_apng.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_bmp.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_dds.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_gif.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_icon.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_jpeg.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_png.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_raw.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_tga.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_tiff.ico \
        ../AssociateFilesWithQuickViewer/icons/qv_webp.ico \

    install_appimage.path = $${MY_DEFAULT_INSTALL}/..
    install_appimage.files = $${APPIMAGE}
    install_appimage.commands = appimagetool $${MY_DEFAULT_INSTALL} $${MY_DEFAULT_INSTALL}/../$${APPIMAGE}
    install_appimage.depends = install_install_deploy_files install_install_translations install_install_assoc_icons install_install_db

    INSTALLS += install_target install_libs install_desktop install_deploy_files install_apprun install_translations install_assoc_icons install_appimage

    install_shaders.path = $${MY_DEFAULT_INSTALL}/shared/shaders
    install_shaders.files = $$SHADERS
    install_db.path = $${MY_DEFAULT_INSTALL}/var/database
    install_db.files = $$DBS $$DBBIN

    INSTALLS += install_db
    !contains(DEFINES, QV_WITHOUT_OPENGL) {
        INSTALLS += install_shaders
    }
}

OTHER_FILES += SHADERS

