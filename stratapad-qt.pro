#-------------------------------------------------
#
# Project created by QtCreator 2017-03-29T15:35:45
#
#-------------------------------------------------

QT += core gui widgets
TARGET = stratapad-qt
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#QMAKE_CXXFLAGS += /W4

include("model_test\modeltest.pro")

SOURCES +=\
    main.cpp \
    sp_tool_button.cpp \
    sp_node_editor.cpp \
    sp_textedit.cpp \
    sp_richtext_editor.cpp \
    sp_flow_layout.cpp \
    sp_tag_dialog.cpp \
    sp_reader_xml.cpp \
    sp_reader_mem.cpp \
    sp_main_window.cpp \
    sp_segmented_mode_control.cpp \
    sp_search_dialog.cpp \
    sp_log_view.cpp \
    sp_log_model.cpp \
    sp_log_editor.cpp \
    sp_link_text_object.cpp \
    sp_create_node_dialog.cpp \
    sp_claim_view.cpp \
    sp_claim_select_dialog.cpp \
    sp_claim_dialog.cpp \
    sp_db_manager.cpp \
    sp_doc_parser.cpp \
    sp_writer_mem.cpp \
    sp_writer_xml.cpp \
    sp_nodeset_delegate.cpp \
    sp_constraints_model.cpp \
    sp_nodes_model.cpp

HEADERS  += \
    sp_parser.h \
    sp_proxy_style.h \
    sp_tool_button.h \
    sp_types.h \
    sp_node_editor.h \
    sp_textedit.h \
    sp_richtext_editor.h \
    sp_nodeset_item.h \
    sp_tags_model.h \
    sp_flow_layout.h \
    sp_tag_dialog.h \
    sp_claim_dialog.h \
    sp_claim_select_dialog.h \
    sp_claim_view.h \
    sp_create_node_dialog.h \
    sp_link_text_object.h \
    sp_log_editor.h \
    sp_log_model.h \
    sp_log_view.h \
    sp_main_window.h \
    sp_parser_types.h \
    sp_reader.h \
    sp_search_dialog.h \
    sp_segmented_mode_control.h \
    sp_switchboard.h \
    sp_util.h \
    sp_writer.h \
    sp_db_manager.h \
    sp_db_signaler.h \
    sp_nodeset_delegate.h \
    sp_constraints_model.h \
    sp_nodes_model.h

FORMS += mainwindow.ui

RESOURCES += \
    css/main_window.css \
    css/rich_text_editor.css \
    resources/icons.qrc

DISTFILES += \
    css/rich_text_editor.css \
    css/main_window.css \
    resources/stratapad-icon.ico

win32: LIBS += -L$$PWD/../stratapad-library/build/ -lwin32_dll

INCLUDEPATH += $$PWD/../stratapad-shared
