
#include "playground.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

QString DEFAULT_VS =
    "attribute highp vec4 vertices;"
    "varying highp vec2 position;"
    "void main() {"
    "    gl_Position = vertices;"
    "    position = vertices.xy;"
    "}";
QString DEFAULT_FS =
    "uniform lowp float iGlobalTime;"
    "varying highp vec2 position;"
    "void main() {"
    "    float t = iGlobalTime;"
    "    lowp float i = 1. - (pow(abs(position.x), 4.) + pow(abs(position.y), 4.));"
    "    i = smoothstep(sin(t) - 0.8, sin(t) + 0.8, i);"
    "    i = floor(i * 20.) / 20.;"
    "    gl_FragColor = vec4(position * .5 + .5, i, i);"
    "}";

Playground::Playground()
    : m_t(0)
    , m_renderer(0)
{
    connect(this, &QQuickItem::windowChanged, this, &Playground::handleWindowChanged);
}

void Playground::setT(qreal t)
{
    m_t += 0.016;  // 60 fps
    emit tChanged();
    if (window())
        window()->update();
}

void Playground::setSource(QString source)
{
    if(source.isEmpty())
        return;

    m_source = source;
    emit sourceChanged();

    if(!m_renderer)
        return;

    m_renderer->updateProgram("", m_source);
}

void Playground::setCompilelog(QString log)
{
    m_compilelog = log;
    emit compilelogChanged();
}

void Playground::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Playground::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Playground::cleanup, Qt::DirectConnection);

        // If we allow QML to do the clearing, they would clear what we paint
        // and nothing would show.

        win->setClearBeforeRendering(false);
    }
}

void Playground::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = 0;
    }
}

void Playground::sync()
{
    if (!m_renderer) {
        m_renderer = new SquircleRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &SquircleRenderer::paint, Qt::DirectConnection);
    }
   // m_renderer->setViewportSize(QSize(parentItem()->width() * window()->devicePixelRatio(), parentItem()->height() * window()->devicePixelRatio()));
    qreal ratio = window()->devicePixelRatio();
    m_renderer->setViewportRect(QRect(parentItem()->x() * ratio,
                                      parentItem()->y() * ratio,
                                      parentItem()->width() * ratio,
                                      parentItem()->height() * ratio
                                      ));
    m_renderer->setT(m_t);
    m_renderer->setWindow(window());
}

SquircleRenderer::SquircleRenderer()
    : m_t(0)
    , m_program(0)
    , m_vertexSource(DEFAULT_VS)
    , m_fragmentSource(DEFAULT_FS)
{

}

SquircleRenderer::~SquircleRenderer()
{
    delete m_program;
}

void SquircleRenderer::paint()
{
    if (!m_program) {
        initializeOpenGLFunctions();

        m_program = new QOpenGLShaderProgram();
//        m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, m_program);
//        m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, m_program);

        linkProgram();
    }

    m_program->bind();

    m_program->enableAttributeArray(0);

    float values[] = {
        -1, -1,
        1, -1,
        -1, 1,
        1, 1
    };

    m_program->setAttributeArray(0, GL_FLOAT, values, 2);
    m_program->setUniformValue("iGlobalTime", (float) m_t);
    m_program->setUniformValue("iResolution", QVector3D(m_viewportRect.width(), m_viewportRect.height(), 1.0));

    glViewport(m_viewportRect.x(), m_viewportRect.y(), m_viewportRect.width(), m_viewportRect.height());

    glDisable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(0);
    m_program->release();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
    m_window->resetOpenGLState();
}

void SquircleRenderer::updateProgram(QString vs, QString fs)
{
    m_vertexSource = vs.isEmpty() ? DEFAULT_VS : vs;
    m_fragmentSource = fs.isEmpty() ? DEFAULT_FS : fs;

    if(m_program){
        delete m_program;
        m_program = 0;
    }

}

void SquircleRenderer::linkProgram()
{
//    if(!m_vertexShader->compileSourceCode(m_vertexSource))
//        qDebug() << m_vertexShader->log() << endl;
//    else
//        m_vertexShader->compileSourceCode(DEFAULT_VS);
//    m_program->addShader(m_vertexShader);

//    if(!m_fragmentShader->compileSourceCode(m_fragmentSource))
//        qDebug() << m_fragmentShader->log() << endl;
//    else
//        m_fragmentShader->compileSourceCode(DEFAULT_FS);
//    m_program->addShader(m_fragmentShader);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, m_vertexSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, m_fragmentSource);

//    m_window->findChild("playground").setCompilelog(m_program->log());
//    qDebug() << "hehe" << m_program->log();

    m_program->bindAttributeLocation("vertices", 0);
    m_program->link();
}
