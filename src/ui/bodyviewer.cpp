#include "bodyviewer.h"
#include <QTextBrowser>
#include <QPushButton>
#include <QToolBar>
#include <QLabel>
#include <QGridLayout>
#include <QFontDatabase>
#include <QMimeType>
#include <QMimeDatabase>
#include <QFileDialog>
#include <QDebug>
#include <algorithm>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QMenu>
#include <QStackedWidget>
#include <QPixmap>
#include <QImageReader>
#include <QBuffer>
#include "core/settings.h"

using namespace std;

class BodyViewer::TextBrowser : public QTextBrowser {
    Q_OBJECT
public:
    TextBrowser(QWidget *parent = 0) : QTextBrowser{parent} {}
};

BodyViewer::BodyViewer(QWidget *parent) :
    QWidget(parent)
{
    contentStack = new QStackedWidget(this);
    textBrowser = new TextBrowser(this);
    contentStack->addWidget(textBrowser);
    imageView = new QLabel(this);
    contentStack->addWidget(imageView);
    imageView->setScaledContents(true);
    auto toolbar = new QToolBar();

    actionHex = toolbar->addAction(tr("Hex"), [this](bool enabled) { if(enabled && mode != Hex) setMode(Hex); });
    actionText = toolbar->addAction(tr("Text"), [this](bool enabled) { if(enabled && mode != Text) setMode(Text); });
    actionImage = toolbar->addAction(tr("Image"), [this](bool enabled) { if(enabled && mode != Image) setMode(Image); });
    toolbar->addSeparator();
    actionSave = toolbar->addAction(tr("Save"), this, &BodyViewer::save);

    actionSave->setEnabled(false);
    actionText->setCheckable(true);
    actionHex->setCheckable(true);
    actionImage->setCheckable(true);
    actionImage->setEnabled(false);

    toolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label = new QLabel(this);
    auto layout = new QGridLayout();
    layout->addWidget(label, 0, 0);
    layout->addWidget(toolbar, 0, 1);
    layout->addWidget(contentStack, 1, 0, 1, 2);
    setLayout(layout);
    setLabel(tr("Body"));
    setMode(Text);
    textBrowser->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
}

BodyViewer::~BodyViewer()
{
}

void BodyViewer::setData(const QByteArray &data, const QString &contentType)
{
    this->data = data;
    QMimeDatabase db;
    mimetype = db.mimeTypeForData(data);
    if( (mimetype.isDefault() || ! mimetype.isValid())  && !contentType.isEmpty()) {
        auto mimetype_it = find_if(db.allMimeTypes().begin(), db.allMimeTypes().end(), [this, &contentType](const QMimeType &m) {
           return contentType == m.name() || m.aliases().contains(contentType);
        });
        if(mimetype_it != db.allMimeTypes().end())
            mimetype = *mimetype_it;
    }
    actionSave->setEnabled(data.size()>0);
    actionImage->setEnabled(contentIsImage());
    setMode(contentIsImage() ? Image : mode);
}

void BodyViewer::setLabel(const QString &label)
{
    this->label->setText(label);
}

void BodyViewer::save()
{
    QString saveDir = Settings::instance()->saveDir();
    auto filename = QFileDialog::getSaveFileName(this, tr("Save body to file"), saveDir, mimetype.filterString());
    if(filename.isEmpty())
        return;
    QFile outfile(filename);
    Settings::instance()->saveDir(QFileInfo(outfile).dir().path());
    if(!outfile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error on file save"), tr("Error opening file for writing."));
        return;
    }
    outfile.write(data);
    outfile.close();
}

void BodyViewer::setMode(BodyViewer::Mode mode)
{
    if(mode == Image && ! contentIsImage()) {
        mode = Hex;
    }
    this->mode = mode;
    actionText->setChecked(mode == Text);
    actionHex->setChecked(mode == Hex);
    actionImage->setChecked(mode == Image);

    updateBody();
}

void BodyViewer::updateBody() {
    qDebug() << "mode:" << mode << ", isImage:" << contentIsImage() << ", mimetype:" << mimetype;
    if(mode == Text) {
        contentStack->setCurrentWidget(textBrowser);
        textBrowser->setText(data);
    }
    if(mode == Hex) {
        contentStack->setCurrentWidget(textBrowser);
        auto hexData = data.toHex();
        QString hexRepresentation;
        QString stringRepresentation;
        int wrapEvery = 20;
        for(uint32_t i = 0; i < hexData.size(); i+=2) {
            uint32_t index = i/2;
            if(index % wrapEvery == 0) {
                if(index > 0) {
                    hexRepresentation += QString("    %1\n").arg(stringRepresentation);
                    stringRepresentation.clear();
                }
                hexRepresentation += QString("%1   ").arg(index, 10, 16, QChar{'0'});
            }
            auto byte_str = QString("%1%2").arg(hexData[i]).arg(hexData[i+1]);
            char byte = QByteArray::fromHex(byte_str.toLatin1())[0];
            QString byte_repr = QString("%1").arg(byte);
            if(static_cast<uint8_t>(byte) < 0x20)
                byte_repr = QString("\\x%1").arg(static_cast<uint8_t>(byte), 2, 16, QChar{'0'});
            stringRepresentation += QString("%1").arg(byte_repr);
            hexRepresentation += QString(" %1").arg(byte_str.toUpper());
        }
        textBrowser->setText(hexRepresentation);
    }
    if(mode == Image) {
        textBrowser->clear();
        contentStack->setCurrentWidget(imageView);
        QBuffer buffer(&data);
        buffer.open(QIODevice::ReadOnly);
        QImageReader imageReader(&buffer);
        QPixmap pixmap = QPixmap::fromImageReader(&imageReader);
        imageView->setPixmap(pixmap);
    }
}

bool BodyViewer::contentIsImage() const
{
    return mimetype.name().startsWith("image");
}

#include "bodyviewer.moc"
