
#include "codeEditor.h"
#include <QtGui/QTextDocument>
#include <QtGui/QTextCursor>
#include <QtCore/QFileInfo>
#include <QFileDialog>

CodeEditor::CodeEditor()
    : m_target(0)
    , m_doc(0)
    , m_cursorPosition(-1)
    , m_selectionStart(0)
    , m_selectionEnd(0)
    , m_isUntitled(true)
{
}

void CodeEditor::setTarget(QQuickItem *target)
{
    m_doc = 0;
    m_target = target;
    if (!m_target)
        return;

    QVariant doc = m_target->property("textDocument");
    if (doc.canConvert<QQuickTextDocument*>()) {
        QQuickTextDocument *qqdoc = doc.value<QQuickTextDocument*>();
        if (qqdoc)
            m_doc = qqdoc->textDocument();
    }
    emit targetChanged();
}

void CodeEditor::setNewFileUrl(const QUrl &arg)
{
    loadFile(arg);
    setFileUrl(arg);
}

void CodeEditor::setFileUrl(const QUrl &arg)
{
    if (m_fileUrl != arg) {
        m_fileUrl = arg;
        m_isUntitled = false;
        emit fileUrlChanged();
        emit untitledChanged();
    }
}

QString CodeEditor::documentTitle() const
{
    return m_documentTitle;
}

void CodeEditor::setDocumentTitle(QString arg)
{
    if (m_documentTitle != arg) {
        m_documentTitle = arg;
        emit documentTitleChanged();
    }
}

void CodeEditor::setText(const QString &arg)
{
    if (m_text != arg) {
        m_text = arg;
        emit textChanged();
    }
}

void CodeEditor::save()
{
    if(!m_doc->isModified())
        return;

    if(!m_isUntitled)
        saveFile(m_fileUrl);
}

void CodeEditor::saveAs(const QUrl &arg)
{
    saveFile(arg);
}

QUrl CodeEditor::fileUrl() const
{
    return m_fileUrl;
}

QString CodeEditor::text() const
{
    return m_text;
}

bool CodeEditor::isUntitled() const
{
    return m_isUntitled;
}

void CodeEditor::setCursorPosition(int position)
{
    if (position == m_cursorPosition)
        return;

    m_cursorPosition = position;

}

void CodeEditor::setSelectionStart(int position)
{
    m_selectionStart = position;
}

void CodeEditor::setSelectionEnd(int position)
{
    m_selectionEnd = position;
}


QTextCursor CodeEditor::textCursor() const
{
    QTextCursor cursor = QTextCursor(m_doc);
    if (m_selectionStart != m_selectionEnd) {
        cursor.setPosition(m_selectionStart);
        cursor.setPosition(m_selectionEnd, QTextCursor::KeepAnchor);
    } else {
        cursor.setPosition(m_cursorPosition);
    }
    return cursor;
}

void CodeEditor::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);
    cursor.mergeCharFormat(format);
}

void CodeEditor::saveFile(const QUrl &arg)
{
    QString localPath = arg.toLocalFile();
    bool isHtml = localPath.endsWith(QLatin1String("html")) || localPath.endsWith(QLatin1String("htm"));

    QFile f(localPath);
    if (!f.open(QFile::WriteOnly | QFile::Truncate | (isHtml ? QFile::NotOpen : QFile::Text))) {
        emit error(tr("Cannot save: ") + f.errorString());
        return;
    }
    QByteArray source = (isHtml ? m_doc->toHtml() : m_doc->toPlainText()).toUtf8();
    f.write(source);
    f.close();
    setFileUrl(QUrl::fromLocalFile(localPath));
    setText(m_doc->toPlainText());
}

void CodeEditor::loadFile(const QUrl &arg)
{
    QString fileName = QQmlFile::urlToLocalFileOrQrc(arg);
    if (QFile::exists(fileName)) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly)) {
            QByteArray data = file.readAll();
            QTextCodec *utf8 = QTextCodec::codecForName("utf8");
            setText(utf8->toUnicode(data));
            if (m_doc)
                m_doc->setModified(false);
            if (fileName.isEmpty())
                m_documentTitle = QStringLiteral("untitled.txt");
            else
                m_documentTitle = QFileInfo(fileName).fileName();

//            emit textChanged();
            emit documentTitleChanged();

        }
    }
}



