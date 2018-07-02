#include <QApplication>
#include <QTextStream>
#include <QDebug>

#include "sp_switchboard.h"
#include "sp_main_window.h"
#include "Windows.h"
#include "sp_proxy_style.h"

#include <iostream>

void
stratapadMessageOut(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString formattedMessage = qFormatLogMessage(type, context, msg);
    OutputDebugString((const wchar_t *) (formattedMessage.utf16()));
}

int
main(int argc, char* argv[])
{
    qSetMessagePattern("%{time [hh:mm:ss.zzz]} [%{threadid}] [%{function} %{line}] %{message}");
    qInstallMessageHandler(stratapadMessageOut);

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);

    QFile stylesheet(":/css/main_window.css");
    stylesheet.open(QFile::ReadOnly);
    QString styles = QString(stylesheet.readAll());

    app.setStyleSheet(styles);
    app.setStyle(new StratapadProxyStyle);
    app.setApplicationName("Stratapad");
    app.setOrganizationName("EWA");
    app.setOrganizationDomain("sp.com");

    InitBackend();

    MainWindow w;
    w.MainWindowCenterAndResize();
    w.show();

    return app.exec();
}
