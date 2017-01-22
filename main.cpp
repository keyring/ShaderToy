#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

#include "playground.h"
#include "codeEditor.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<Playground>("ShaderToyQML", 1, 0, "Playground") ;
    qmlRegisterType<CodeEditor>("CodeEditorQML", 1, 0, "CodeEditor");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QObject *topobject = engine.rootObjects().value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topobject);
    window->show();

    return app.exec();
}
