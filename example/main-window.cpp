#include <QStyle>
#include <QMenuBar>
#include <QAction>
#include <QFileDialog>
#include <QImageReader>
#include "main-window.h"
#include "image-viewer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    auto viewer = new pal::ImageViewer("A test viewer", this);

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

MainWindow::~MainWindow() {}
