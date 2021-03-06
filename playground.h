
#ifndef __PLAYGROUND_H__
#define __PLAYGROUND_H__

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLTexture>

class SquircleRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    SquircleRenderer();
    ~SquircleRenderer();

    void setT(qreal t) { m_t = t; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setViewportRect(const QRect &rect) { m_viewportRect = rect; }
    void setWindow(QQuickWindow *window) { m_window = window; }

    void updateProgram(QString vs, QString fs);
public slots:
    void paint();

private:
    QRect m_viewportRect;
    QSize m_viewportSize;
    qreal m_t;
    QOpenGLBuffer vbo;
    QOpenGLTexture *textures[4];
    QOpenGLShaderProgram *m_program;
    QOpenGLShader *m_vertexShader;
    QOpenGLShader *m_fragmentShader;
    QString m_vertexSource;
    QString m_fragmentSource;
    QQuickWindow *m_window;

    void initializeGL();
    void makeObject();
    void linkProgram();
};


class Playground : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QString compilelog READ compilelog  NOTIFY compilelogChanged)

public:
    Playground();

    qreal t() const { return m_t; }
    void setT(qreal t);

    QString source() const { return m_source; }
    void setSource(QString source);

    QString compilelog() const { return m_compilelog; }
    void setCompilelog(QString log);

signals:
    void tChanged();
    void sourceChanged();
    void compilelogChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow *win);

private:
    qreal m_t;
    QString m_source;
    QString m_compilelog;
    SquircleRenderer *m_renderer;

};


#endif // __PLAYGROUND_H__
