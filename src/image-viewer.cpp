#define _USE_MATH_DEFINES
#include <cmath>
#include <mutex>
#include <QApplication>
#include <QEnterEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "pal/image-viewer.h"

static void init_image_viewer_resource() {
    // This must be done outside of any namespace
    Q_INIT_RESOURCE(image_viewer);
}

namespace pal {

// Graphics View with better mouse events handling
class GraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit GraphicsView(ImageViewer *viewer)
        : QGraphicsView()
        , m_viewer(viewer)
    {
        static std::once_flag inititialized;
        std::call_once(inititialized, init_image_viewer_resource);

        // no antialiasing or filtering, we want to see the exact image content
        setRenderHint(QPainter::Antialiasing, false);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setOptimizationFlags(QGraphicsView::DontSavePainterState);
        setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse); // zoom at cursor position
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setInteractive(true);
        setMouseTracking(true);
    }

protected:
    void wheelEvent(QWheelEvent *event) override {
        const auto d = event->angleDelta();

        if (event->modifiers() == Qt::NoModifier) {
            auto dm = abs(d.x()) > abs(d.y()) ? d.x() : d.y();
            if (dm > 0)
                m_viewer->zoomIn(3);
            else if (dm < 0)
                m_viewer->zoomOut(3);
            event->accept();
        }
        else
            QGraphicsView::wheelEvent(event);
    }

    void enterEvent(EnterEvent *event) override {
        QGraphicsView::enterEvent(event);
        viewport()->setCursor(Qt::CrossCursor);
    }

    void mousePressEvent(QMouseEvent *event) override {
        QGraphicsView::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        QGraphicsView::mouseReleaseEvent(event);
        viewport()->setCursor(Qt::CrossCursor);
    }

private:
    ImageViewer *m_viewer;
};


ImageViewer::ImageViewer(QWidget *parent)
    : QFrame(parent)
    , m_zoom_level(0)
    , m_fit(true)
    , m_bar_mode(ToolBarMode::Visible)
    , m_aspect_ratio_mode(Qt::KeepAspectRatio)
{
    auto scene = new QGraphicsScene(this);
    m_view = new GraphicsView(this);
    m_view->setScene(scene);

    // graphic object holding the image buffer
    m_pixmap = new PixmapItem;
    scene->addItem(m_pixmap);
    connect(m_pixmap, &PixmapItem::mouseMoved, this, &ImageViewer::mouseAt);
    connect(m_pixmap, &PixmapItem::sizeChanged, this, &ImageViewer::updateSceneRect);

    makeToolbar();

    auto box = new QVBoxLayout;
    box->setContentsMargins(5,0,5,0);
    box->addWidget(m_toolbar);
    box->addWidget(m_view, 1);
    setLayout(box);
}

// toolbar with a few quick actions and display information
void ImageViewer::makeToolbar() {
    // text and value at pixel
    m_text_label = new QLabel(this);
    m_text_label->setStyleSheet(QStringLiteral("QLabel { font-weight: bold; }"));
    m_pixel_value = new QLabel(this);

    auto fit = new QToolButton(this);
    fit->setToolTip(tr("Fit image to window"));
    fit->setIcon(QIcon(":zoom-fit"));
    connect(fit, &QToolButton::clicked, this, &ImageViewer::zoomFit);

    auto orig = new QToolButton(this);
    orig->setToolTip(tr("Resize image to its original size"));
    orig->setIcon(QIcon(":zoom-1"));
    connect(orig, &QToolButton::clicked, this, &ImageViewer::zoomOriginal);

    m_toolbar = new QWidget;
    auto box = new QHBoxLayout(m_toolbar);
    m_toolbar->setContentsMargins(0,0,0,0);
    box->setContentsMargins(0,0,0,0);
    box->addWidget(m_text_label);
    box->addStretch(1);
    box->addWidget(m_pixel_value);
    box->addWidget(fit);
    box->addWidget(orig);
}

QString ImageViewer::text() const {
    return m_text_label->text();
}

void ImageViewer::setText(const QString &txt) {
    m_text_label->setText(txt);
}

const QImage &ImageViewer::image() const {
    return m_pixmap->image();
}

void ImageViewer::setImage(const QImage &im) {
    m_pixmap->setImage(im);

    if (m_fit)
        zoomFit();

    emit imageChanged();
}

void ImageViewer::setAspectRatioMode(Qt::AspectRatioMode aspect_ratio_mode) {
    m_aspect_ratio_mode = aspect_ratio_mode;
    if (m_fit)
        zoomFit();
}

const PixmapItem *ImageViewer::pixmapItem() const {
    return m_pixmap;
}

PixmapItem *ImageViewer::pixmapItem() {
    return m_pixmap;
}

ImageViewer::ToolBarMode ImageViewer::toolBarMode() const {
    return m_bar_mode;
}

void ImageViewer::setToolBarMode(ToolBarMode mode) {
    m_bar_mode = mode;
    if (mode == ToolBarMode::Hidden)
        m_toolbar->hide();
    else if (mode == ToolBarMode::Visible)
        m_toolbar->show();
    else
        m_toolbar->setVisible(underMouse());
}

bool ImageViewer::isAntialiasingEnabled() const {
    return m_view->renderHints() & QPainter::Antialiasing;
}

void ImageViewer::enableAntialiasing(bool on) {
    m_view->setRenderHint(QPainter::Antialiasing, on);
}

QGraphicsView *ImageViewer::view() const {
    return m_view;
}

void ImageViewer::addTool(QWidget *tool) {
    m_toolbar->layout()->addWidget(tool);
}

Qt::AspectRatioMode ImageViewer::aspectRatioMode() const {
    return m_aspect_ratio_mode;
}

qreal ImageViewer::rotation() const {
    return 180. * rotationRadians() / M_PI;
}

qreal ImageViewer::rotationRadians() const {
    auto p10 = m_view->transform().map(QPointF(1., 0.));
    return std::atan2(p10.y(), p10.x());
}

void ImageViewer::setRotation(qreal angle) {
    m_view->rotate(angle - rotation());
    if (m_fit)
        zoomFit();
}

qreal ImageViewer::scale() const {
    auto square = [](qreal value) { return value * value; };
    return std::sqrt(square(m_view->transform().m11()) + square(m_view->transform().m12()));
}

void ImageViewer::setMatrix() {
    qreal newScale = std::pow(2.0, m_zoom_level / 10.0);

    QTransform mat;
    mat.scale(newScale, newScale);
    mat.rotateRadians(rotationRadians());

    m_view->setTransform(mat);
    emit zoomChanged(scale());
}

void ImageViewer::zoomFit() {
    /* Fit in view by KeepAspectRatioByExpanding does not keep the position
     * find out the current viewport center move back to that position after
     * fitting. It is done here instead of inside the resize event handler
     * because fitInView may be triggered from a number of events, not just
     * the resize event.
     */
    auto cr = QRect(m_view->viewport()->rect().center(), QSize(2, 2));
    auto cen = m_view->mapToScene(cr).boundingRect().center();

    m_view->fitInView(m_pixmap, m_aspect_ratio_mode);
    m_zoom_level = int(10.0 * std::log2(scale()));
    m_fit = true;

    if (m_aspect_ratio_mode == Qt::KeepAspectRatioByExpanding)
        m_view->centerOn(cen);

    emit zoomChanged(scale());
}

void ImageViewer::zoomOriginal() {
    m_zoom_level = 0;
    m_fit = false;
    setMatrix();
}

void ImageViewer::zoomIn(int level) {
    m_zoom_level += level;
    m_fit = false;
    setMatrix();
}

void ImageViewer::zoomOut(int level) {
    m_zoom_level -= level;
    m_fit = false;
    setMatrix();
}

void ImageViewer::mouseAt(int x, int y) {
    if (m_pixmap->image().valid(x,y)) {
        QRgb rgb = m_pixmap->image().pixel(x, y);
        auto s = QStringLiteral("[%1, %2] (%3, %4, %5)")
                    .arg(x)
                    .arg(y)
                    .arg(qRed(rgb))
                    .arg(qGreen(rgb))
                    .arg(qBlue(rgb));
        m_pixel_value->setText(s);
    }
    else
        m_pixel_value->setText(QString());
}

void ImageViewer::updateSceneRect(int w, int h) {
    Q_UNUSED(w)
    Q_UNUSED(h)
    m_view->scene()->setSceneRect(m_pixmap->boundingRect());
}

void ImageViewer::enterEvent(EnterEvent *event) {
    QFrame::enterEvent(event);
    if (m_bar_mode == ToolBarMode::AutoHidden) {
        m_toolbar->show();
        if (m_fit)
            zoomFit();
    }
}

void ImageViewer::leaveEvent(QEvent *event) {
    QFrame::leaveEvent(event);
    if (m_bar_mode == ToolBarMode::AutoHidden) {
        m_toolbar->hide();
        if (m_fit)
            zoomFit();
    }
}

void ImageViewer::resizeEvent(QResizeEvent *event) {
    QFrame::resizeEvent(event);
    if (m_fit)
        zoomFit();
}

void ImageViewer::showEvent(QShowEvent *event) {
    QFrame::showEvent(event);
    if (m_fit)
        zoomFit();
}


PixmapItem::PixmapItem(QGraphicsItem *parent) :
    QObject(), QGraphicsPixmapItem(parent)
{
    setAcceptHoverEvents(true);
}

void PixmapItem::setImage(QImage im) {
    if (im.isNull()) {
        m_image.fill(Qt::white);
        im = m_image.copy();
    }
    std::swap(m_image, im);

    setPixmap(QPixmap::fromImage(m_image));

    if (m_image.size() != im.size())
        emit sizeChanged(m_image.width(), m_image.height());

    emit imageChanged(m_image);
}

void PixmapItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    auto pos = event->pos();
    emit doubleClicked(int(pos.x()), int(pos.y()));
    QGraphicsItem::mouseDoubleClickEvent(event);
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mousePressEvent(event);
}

void PixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseReleaseEvent(event);
}

void PixmapItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event) {
    auto pos = event->pos();
    emit mouseMoved(int(pos.x()), int(pos.y()));
    QGraphicsItem::hoverMoveEvent(event);
}

} // namespace pal

#include "image-viewer.moc"
