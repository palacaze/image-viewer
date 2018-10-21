#include <QApplication>
#include <QMainWindow>
#include <QStyle>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QImageReader>
#include "image-viewer.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        auto viewer = new pal::ImageViewer(this);
        viewer->setText("A test viewer");

        QList<QByteArray> formats = QImageReader::supportedImageFormats();
        QStringList list;
        for (auto &fmt : formats)
            list.append("*." + QString(fmt));
        auto filter = "Images (" + list.join(" ") + ")";

        auto open_action = new QAction(tr("Open an image file..."), this);
        connect(open_action, &QAction::triggered, [=] {
            QString path = QFileDialog::getOpenFileName(nullptr, "Pick an image file",
                                                        nullptr, filter);
            if (path.isEmpty())
                return;
            viewer->setImage(QImage(path));
        });

        auto file_menu = menuBar()->addMenu(tr("&File"));
        file_menu->addAction(open_action);

        setCentralWidget(viewer);
    }

    ~MainWindow() = default;
};


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

#include "main.moc"
