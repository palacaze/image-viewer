#ifndef PAL_IMAGE_VIEWER_H
#define PAL_IMAGE_VIEWER_H

#include <QFrame>
#include <QGraphicsPixmapItem>
#include <pal/image-viewer-export.h>

QT_BEGIN_NAMESPACE
class QGraphicsView;
class QLabel;
QT_END_NAMESPACE

namespace pal {

class PixmapItem;
class GraphicsView;

// 5 -> 6 transition
#if QT_VERSION_MAJOR > 5
using EnterEvent = QEnterEvent;
#else
using EnterEvent = QEvent;
#endif


/**
 * @brief ImageViewer displays images and allows basic interaction with it
 */
class PAL_IMAGE_VIEWER_EXPORT ImageViewer : public QFrame {
    Q_OBJECT

    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation NOTIFY rotationChanged)

public:
    /**
     * ToolBar visibility
     */
    enum class ToolBarMode {
        Visible,
        Hidden,
        AutoHidden
    };

public:
    explicit ImageViewer(QWidget *parent = nullptr);

    /// Text displayed on the left side of the toolbar
    QString text() const;

    /// The currently displayed image
    const QImage& image() const;

    /// Access to the pixmap so that other tools can add things to it
    const PixmapItem* pixmapItem() const;
    PixmapItem* pixmapItem();

    /// Add a tool to the toolbar
    void addTool(QWidget *tool);

    /// Toolbar visibility
    ToolBarMode toolBarMode() const;
    void setToolBarMode(ToolBarMode mode);

    /// Anti-aliasing
    bool isAntialiasingEnabled() const;
    void enableAntialiasing(bool on = true);

    /// QGraphicsView control
    QGraphicsView* view() const;

    /// Get view rotation angle
    qreal rotation() const;

    /// Get aspect ratio mode
    Qt::AspectRatioMode aspectRatioMode() const;

public slots:
    void setText(const QString &txt);
    void setImage(const QImage &);
    void setRotation(qreal angle);
    /*
     * Set aspect ratio mode.
     * Currently, only Qt::KeepAspectRatio and Qt::KeepAspectRatioByExpanding are supported
     */
    void setAspectRatioMode(Qt::AspectRatioMode aspect_ratio_mode);

    void zoomFit();
    void zoomOriginal();
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);
    void mouseAt(int x, int y);

private slots:
    void updateSceneRect(int w, int h);

signals:
    void imageChanged();
    void rotationChanged(double rotation);
    void zoomChanged(double scale);

protected:
    void enterEvent(EnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    qreal rotationRadians() const;
    qreal scale() const;
    void setMatrix();
    void makeToolbar();

private:
    int m_zoom_level;
    QLabel *m_text_label;
    QLabel *m_pixel_value;
    GraphicsView *m_view;
    PixmapItem *m_pixmap;
    QWidget *m_toolbar;
    bool m_fit;
    ToolBarMode m_bar_mode;
    Qt::AspectRatioMode m_aspect_ratio_mode;
};


class PAL_IMAGE_VIEWER_EXPORT PixmapItem : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    PixmapItem(QGraphicsItem *parent = nullptr);
    const QImage & image() const { return m_image; }

public slots:
    void setImage(QImage im);

signals:
    void doubleClicked(int x, int y);
    void imageChanged(const QImage &);
    void sizeChanged(int w, int h);
    void mouseMoved(int x, int y);

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

private:
    QImage m_image;
};

} // namespace pal

#endif // PAL_IMAGE_VIEWER_H
