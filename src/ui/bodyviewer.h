#ifndef BODYVIEWER_H
#define BODYVIEWER_H

#include <QWidget>
#include <QMimeType>

class QLabel;
class QAction;
class Settings;
class QStackedWidget;
class BodyViewer : public QWidget
{
    Q_OBJECT
public:
    explicit BodyViewer(QWidget *parent = 0);
    ~BodyViewer();
public slots:
    void setData(const QByteArray &data, const QString &contentType = {});
    void setLabel(const QString &label);
    void save();
private:
    class TextBrowser;
    TextBrowser *textBrowser;
    QStackedWidget *contentStack;
    QLabel *label;
    QLabel *imageView;
    QAction * actionText;
    QAction *actionHex;
    QAction *actionImage;
    QAction *actionSave;
    QByteArray data;
    QMimeType mimetype;
    enum Mode { Text, Hex, Image };
    Mode mode;
    void setMode(Mode mode);
    void updateBody();
    bool contentIsImage() const;
};

#endif // BODYVIEWER_H
