
#include "playground.h"

#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLContext>

QString DEFAULT_VS =
    "attribute highp vec4 vertices;\n"
    "attribute mediump vec2 texc;\n"
    "varying highp vec2 position;\n"
    "varying mediump vec2 texcoord;\n"
    "void main() {"
    "    gl_Position = vertices;"
    "    position = vertices.xy;"
    "    texcoord = texc;"
    "}";
QString DEFAULT_FS_HEAD =
        "uniform sampler2D iChannel0;\n"
        "uniform lowp float iGlobalTime;\n"
        "varying highp vec2 position;\n"
        "varying mediump vec2 texcoord;\n";

QString DEFAULT_FS = DEFAULT_FS_HEAD +
    "void main() {"
    "    float t = iGlobalTime;"
    "    lowp float i = 1. - (pow(abs(position.x), 4.) + pow(abs(position.y), 4.));"
    "    i = smoothstep(sin(t) - 0.8, sin(t) + 0.8, i);"
    "    i = floor(i * 20.) / 20.;"
    "    gl_FragColor = vec4(position * .5 + .5, i, i);"
    "}";

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

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
    memset(textures, 0, sizeof(textures));
    textures[0] = new QOpenGLTexture(QImage(QString(":/logo.jpg")));
}

SquircleRenderer::~SquircleRenderer()
{
    for (int i = 0; i < 4; ++i)
        delete textures[i];
    delete m_program;
}

void SquircleRenderer::initializeGL()
{
    initializeOpenGLFunctions();

    m_program = new QOpenGLShaderProgram();
//        m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, m_program);
//        m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, m_program);

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

    m_program->bindAttributeLocation("vertices", PROGRAM_VERTEX_ATTRIBUTE);
    m_program->bindAttributeLocation("texc", PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->link();

    m_program->bind();
    m_program->setUniformValue("iChannel0", 0);
    m_program->setUniformValue("iChannel1", 1);
    m_program->setUniformValue("iChannel2", 2);
    m_program->setUniformValue("iChannel3", 3);


}

void SquircleRenderer::paint()
{
    if (!m_program) {
        initializeGL();
    }
	m_program->bind();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(m_viewportRect.x(), m_viewportRect.y(), m_viewportRect.width(), m_viewportRect.height());

    makeObject();

    m_program->setUniformValue("iGlobalTime", (float) m_t);
    m_program->setUniformValue("iResolution", QVector3D(m_viewportRect.width(), m_viewportRect.height(), 1.0));
    m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    //    for(int i = 0; i < 4; ++i){
        textures[0]->bind();

//    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    m_program->disableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_program->disableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->release();
    vbo.destroy();

    // Not strictly needed for this example, but generally useful for when
    // mixing with raw OpenGL.
     m_window->resetOpenGLState();
}

void SquircleRenderer::updateProgram(QString vs, QString fs)
{
    m_vertexSource = vs.isEmpty() ? DEFAULT_VS : vs;
    if(fs.isEmpty()){
        m_fragmentSource = DEFAULT_FS;
    }else{
        m_fragmentSource = DEFAULT_FS_HEAD + fs;
    }

    if(m_program){
        delete m_program;
        m_program = 0;
    }

}

void SquircleRenderer::makeObject()
{
    // cube
//    static const int cubecoords[6][4][3] = {
//        { { +1, -1, -1 }, { -1, -1, -1 }, { -1, +1, -1 }, { +1, +1, -1 } },
//        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, +1, +1 }, { +1, +1, +1 } },
//        { { +1, -1, +1 }, { +1, -1, -1 }, { +1, +1, -1 }, { +1, +1, +1 } },
//        { { -1, -1, -1 }, { -1, -1, +1 }, { -1, +1, +1 }, { -1, +1, -1 } },
//        { { +1, -1, +1 }, { -1, -1, +1 }, { -1, -1, -1 }, { +1, -1, -1 } },
//        { { -1, -1, +1 }, { +1, -1, +1 }, { +1, +1, +1 }, { -1, +1, +1 } }
//    };
//    QVector<GLfloat> vertData;
//    int facenum = 1;
//    int vertexnum = 4;
//    for (int i = 0; i < facenum; ++i) {  // face
//        for (int j = 0; j < vertexnum; ++j) {  // vertex
//            // vertex position
//            vertData.append(coords[i][j][0]);
//            vertData.append(coords[i][j][1]);
//            vertData.append(coords[i][j][2]);
//            // texture coordinate
//            vertData.append(j == 0 || j == 3);
//            vertData.append(j == 0 || j == 1);
//        }
//    }

    static const int planecoords[4][5] = {
        -1, -1, -1, 0, 1,
        1, -1, -1, 1, 1,
        -1, 1, -1, 0, 0,
        1, 1, -1, 1, 0
    };

    QVector<GLfloat> vertData;
    for (int j = 0; j < 4; ++j) {  // vertex
        // vertex position
        vertData.append(planecoords[j][0]);
        vertData.append(planecoords[j][1]);
        vertData.append(planecoords[j][2]);
        // texture coordinate
        vertData.append(planecoords[j][3]);
        vertData.append(planecoords[j][4]);
    }


    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
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

    m_program->bindAttributeLocation("vertices", PROGRAM_VERTEX_ATTRIBUTE);
    m_program->bindAttributeLocation("texc", PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->link();
}
