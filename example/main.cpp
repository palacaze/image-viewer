#include <QApplication>
#include <QMainWindow>
#include <QStyle>
#include <QMenuBar>
#include <QToolButton>
#include <QAction>
#include <QFileDialog>
#include <QImageReader>
#include <QGraphicsView>
#include "image-viewer.h"
#include "rect-selection.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        // viewer
        auto viewer = new pal::ImageViewer(this);
        viewer->setText("A test viewer");
        viewer->setToolBarMode(pal::ImageViewer::ToolBarMode::AutoHidden);

        // selection tool
        auto selecter = new pal::SelectionItem(viewer->pixmapItem());
        selecter->setVisible(false);

        auto sel = new QToolButton(this);
        sel->setToolTip(tr("Selects a rectangle area in the image"));
        sel->setIcon(QIcon(":/icons/image-selection.png"));
        sel->setCheckable(true);

        // cropping only allowed when an image is visible
        auto updater = [=] {
            const bool ok = !viewer->image().isNull();
            if (ok)
                selecter->resetSelection();
            else
                sel->setChecked(false);
            sel->setEnabled(ok);
        };

        connect(sel, &QToolButton::toggled, selecter, &pal::SelectionItem::setVisible);
        connect(viewer, &pal::ImageViewer::imageChanged, selecter, updater);
        updater();
        viewer->addTool(sel);

        // file open
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

        auto scrollbar_actions = new QActionGroup(this);
        scrollbar_actions->setExclusive(true);

        auto scrollbar_menu = menuBar()->addMenu(tr("&Scroll bars"));
        using ScrollBarPolicyMenuItem = std::pair<QString, Qt::ScrollBarPolicy>;
        for (auto item : {
                 ScrollBarPolicyMenuItem{tr("As needed"), Qt::ScrollBarAsNeeded},
                 ScrollBarPolicyMenuItem{tr("Always off"), Qt::ScrollBarAlwaysOff},
                 ScrollBarPolicyMenuItem{tr("Always on"), Qt::ScrollBarAlwaysOn},
            }) {
            auto scrollbar_action = scrollbar_menu->addAction(item.first);
            scrollbar_action->setCheckable(true);
            scrollbar_action->setChecked(viewer->view()->horizontalScrollBarPolicy() == item.second);
            connect(scrollbar_action, &QAction::triggered, [=] {
                viewer->view()->setHorizontalScrollBarPolicy(item.second);
                viewer->view()->setVerticalScrollBarPolicy(item.second);
                scrollbar_action->setChecked(true);
            });
            scrollbar_actions->addAction(scrollbar_action);
        }

        setCentralWidget(viewer);
        resize(800, 600);
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
