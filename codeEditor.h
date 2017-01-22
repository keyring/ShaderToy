#ifndef __CODE_EDITOR_H__
#define __CODE_EDITOR_H__

#include <QQuickTextDocument>

#include <QtGui/QTextCharFormat>
#include <QtCore/QTextCodec>

#include <qqmlfile.h>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE


class CodeEditor: public QObject
{
    Q_OBJECT

    Q_ENUMS(HAlignment)

    Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(bool isUntitled READ isUntitled NOTIFY untitledChanged)
    Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setNewFileUrl NOTIFY fileUrlChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString documentTitle READ documentTitle WRITE setDocumentTitle NOTIFY documentTitleChanged)

public:
    CodeEditor();
    QQuickItem *target() { return m_target; }

    void setTarget(QQuickItem *target);

    void setCursorPosition(int position);
    void setSelectionStart(int position);
    void setSelectionEnd(int position);

    int cursorPosition() const { return m_cursorPosition; }
    int selectionStart() const { return m_selectionStart; }
    int selectionEnd() const { return m_selectionEnd; }

    QUrl fileUrl() const;
    QString text() const;
    bool isUntitled() const;

    QString documentTitle() const;

public Q_SLOTS:
    void setNewFileUrl(const QUrl &arg);
    void setText(const QString &arg);
    void save();
    void saveAs(const QUrl &arg);

    void setDocumentTitle(QString arg);

Q_SIGNALS:
    void targetChanged();
    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();

    void untitledChanged();
    void fileUrlChanged();

    void textChanged();
    void documentTitleChanged();
    void error(QString message);

private:
    QTextCursor textCursor() const;
    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    void setFileUrl(const QUrl &arg);
    void saveFile(const QUrl &arg);
    void loadFile(const QUrl &arg);

    QQuickItem *m_target;
    QTextDocument *m_doc;



    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;

    bool m_isUntitled;
    QUrl m_fileUrl;
    QString m_text;
    QString m_documentTitle;
};

#endif // __CODE_EDITOR_H__
