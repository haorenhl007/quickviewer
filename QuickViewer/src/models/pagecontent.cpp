#include <QtConcurrent>

#include "pagecontent.h"
#include "qvapplication.h"
#include "qzimg.h"

void ImageContent::initialize()
{
    if(!Movie.isNull() && !Movie.data()) {
        Movie.load();
        QMovie* qm = Movie.data();
        qm->jumpToFrame(0);
        QPixmap firstFrame = qm->currentPixmap();
        Image = firstFrame.toImage();
        BaseSize = ImportSize = firstFrame.size();
    }
}

PageContent::PageContent(QObject* parent)
    : QObject(parent)
    , Ic()
    , GrItem(nullptr)
    , Rotate(0)
    , m_resizeGeneratingState(0)
    , initialized(false)
    , GText(nullptr)
    , GTextSurface(nullptr)
    , Separation(NoSeparated)
{

}

PageContent::PageContent(QObject* parent, QGraphicsScene *s, ImageContent ic)
    : QObject(parent)
    , Scene(s)
    , Ic(ic)
    , GrItem(nullptr)
    , Rotate(0)
    , m_resizeGeneratingState(0)
    , initialized(false)
    , GText(nullptr)
    , GTextSurface(nullptr)
    , Separation(Ic.wideImage() && qApp->SeparatePagesWhenWideImage() ? FirstSeparated : NoSeparated)
{
//    qDebug() << Ic.wideImage() << qApp->SeparatePagesWhenWideImage();
    if(!Ic.ImportSize.width()) {
        QGraphicsTextItem* gtext = s->addText(tr("NOT IMAGE FILE", "Error messages to be displayed on screen when image loading fails"));
        gtext->setDefaultTextColor(Qt::white);
        //qDebug() << gtext;
        GrItem = gtext;
        return;
    }
    if(ic.Info.ImageWidth > 0 && ic.Info.Orientation != 1) {
        switch(ic.Info.Orientation) {
        case 6: // left 90 digree turned
            Rotate = 90;
            break;
        case 8: // right 90 digree turned
            Rotate = 270;
            break;
        }
    }
    initializePage();
}

PageContent::PageContent(const PageContent &rhs)
    : QObject(rhs.parent())
    , Scene(rhs.Scene)
    , Ic(rhs.Ic)
//    , ResizedPage(rhs.ResizedPage)
    , GrItem(rhs.GrItem)
    , Rotate(rhs.Rotate)
    , m_resizeGeneratingState(0)
    , initialized(false)
    , GText(rhs.GText)
    , GTextSurface(rhs.GTextSurface)
    , DrawScale(rhs.DrawScale)
    , Separation(rhs.Separation)
{

}

PageContent &PageContent::operator=(const PageContent &rhs) {
    Scene = rhs.Scene;
    Ic = rhs.Ic;
//    ResizedPage = rhs.ResizedPage;
    GrItem = rhs.GrItem;
    Rotate = rhs.Rotate;
    GText  = rhs.GText;
    GTextSurface  = rhs.GTextSurface;
    m_resizeGeneratingState = 0;
    DrawScale = rhs.DrawScale;
    Separation = rhs.Separation;
    return *this;
}

QPoint PageContent::Offset(int rotateOffset) {
    int rot = (Rotate+rotateOffset) % 360;
    switch(rot) {
    case 90:  return QPoint(Ic.Image.height(), 0);
    case 180: return QPoint(Ic.Image.width(), Ic.Image.height());
    case 270: return QPoint(0, Ic.Image.width());
    default:  return QPoint();
    }
}

QSize PageContent::CurrentSize(int rotateOffset) {
    int rot = (Rotate+rotateOffset) % 360;
    return rot==90 || rot==270 ? QSize(Ic.Image.height(), Ic.Image.width()) : Ic.Image.size();
}

QRect PageContent::setPageLayoutFitting(QRect viewport, PageContent::Fitting fitting, qreal loupe, int rotateOffset) {
    if(!Ic.ImportSize.width()) {
        applyResize(1.0, 0, viewport.topLeft(), QSize(100,100));
        return QRect(viewport.topLeft(), QSize(100, 100));
    }
    QSize currentSize = CurrentSize(rotateOffset);
    bool goSeparate = viewport.height() > viewport.width() && Separation != NoSeparated;
    if(goSeparate)
        currentSize = QSize(currentSize.width()/2, currentSize.height());
    QSize newsize = currentSize.scaled(viewport.size(), Qt::KeepAspectRatio);
    qreal scale = DrawScale = 1.0*newsize.width()/currentSize.width();
    if(loupe > 1.0) {
        return setPageLayoutManual(viewport, fitting, scale*loupe, rotateOffset, true);
    }
    if(scale > 1.0 && qApp->DontEnlargeSmallImagesOnFitting())
        return setPageLayoutManual(viewport, fitting, 1.0, rotateOffset);

    QPoint of = Offset(rotateOffset);
    of *= scale;

    QRect drawRect;
    if(newsize.height() == viewport.height()) { // fitting on upper and bottom
        int ofsinviewport = fitting==FitRight ? 0 : fitting==FitCenter ? (viewport.width()-newsize.width())/2 : viewport.width()-newsize.width();
        drawRect = QRect(QPoint(of.x() + viewport.x() + ofsinviewport, of.y()), newsize);
    } else { // fitting on left and right
        drawRect = QRect(QPoint(of.x() + viewport.x(), of.y() + (viewport.height()-newsize.height())/2), newsize);
    }

    QPoint imagePos = drawRect.topLeft();
    if(goSeparate
        && (   (Separation == FirstSeparated && qApp->RightSideBook())
            || (Separation == SecondSeparated && !qApp->RightSideBook()))) {
        // Display only the right side of the image
        imagePos = QPoint(imagePos.x()-newsize.width(), imagePos.y());
    }
    applyResize(scale, rotateOffset, imagePos, newsize);
    return drawRect;
}

QRect PageContent::setPageLayoutManual(QRect viewport, PageContent::Fitting fitting, qreal scale, int rotateOffset, bool loupe) {
    if(!Ic.ImportSize.width()) {
        applyResize(1.0, 0, viewport.topLeft(), QSize(100,100));
        return QRect(viewport.topLeft(), QSize(100, 100));
    }
    QSize currentSize = CurrentSize(rotateOffset);
    bool goSeparate = viewport.height() > viewport.width() && Separation != NoSeparated;
    if(goSeparate)
        currentSize = QSize(currentSize.width()/2, currentSize.height());
    QSize newsize = currentSize * scale;
    DrawScale = scale;

    QPoint of = Offset(rotateOffset);
    of *= scale;

    int ofsinviewport = fitting==FitRight ? 0 : fitting==FitCenter ? qMax(0, (viewport.width()-newsize.width())/2) : viewport.width()-newsize.width();
    int offsetY = qMax(0, (viewport.height()-newsize.height())/2);
    QRect drawRect(QPoint(of.x() + viewport.x() + ofsinviewport, of.y() + offsetY), newsize);

    QPoint imagePos = drawRect.topLeft();
    if(goSeparate
        && (   (Separation == FirstSeparated && qApp->RightSideBook())
            || (Separation == SecondSeparated && !qApp->RightSideBook()))) {
        // Display only the right side of the image
        imagePos = QPoint(imagePos.x()-newsize.width(), imagePos.y());
    }
    applyResize(scale, rotateOffset, imagePos, newsize, loupe);
    return drawRect;
}

void PageContent::applyResize(qreal scale, int rotateOffset, QPoint pos, QSize newsize, bool loupe)
{
    checkInitialize();
//    QSize newsize2 = Ic.Info.Orientation==6 || Ic.Info.Orientation==8 ? QSize(newsize.height(), newsize.width()) : newsize;
    QSize newsize2 = (Rotate+rotateOffset) % 180 ? QSize(newsize.height(), newsize.width()) : newsize;
    qvEnums::ShaderEffect effect = Ic.Movie.isNull() ? qApp->Effect() : qvEnums::Bilinear;
    // only CPU resizing
    if(effect < qvEnums::UsingFixedShader) {
        if(loupe && !Ic.ResizedImage.isNull()) {
            Ic.ResizedImage = QImage();
            initializePage(true);
        }
        else if(Ic.ResizedImage.isNull()
        || (!Ic.ResizedImage.isNull() && Ic.ResizedImage.size() != newsize2)) {
            QImage resized = QZimg::scaled(Ic.Image, newsize2, Qt::IgnoreAspectRatio, QZimg::ResizeBicubic);
            Ic.ResizedImage = resized;
            Scene->removeItem(GrItem);
            delete GrItem;
            GrItem = Scene->addPixmap(QPixmap::fromImage(Ic.ResizedImage));
            GrItem->setRotation(Rotate);
        }
        GrItem->setScale(Ic.ResizedImage.isNull() ? scale : 1.0);
    }
    // CPU resizing after GPU preview
    if(effect > qvEnums::UsingCpuResizer && qApp->Effect() < qvEnums::UsingSomeShader) {
        if(!Ic.ResizedImage.isNull() && Ic.ResizedImage.size() != newsize) {
            initializePage(true);
        }
        if(Ic.ResizedImage.isNull() && m_resizeGeneratingState==0) {
            m_resizeGeneratingState = 1;
            QFuture<QImage> future = QtConcurrent::run(QZimg::scaled, Ic.Image, newsize2, Qt::IgnoreAspectRatio, QZimg::ResizeBicubic);
            connect(&generateWatcher, SIGNAL(finished()), this, SLOT(on_resizeFinished_trigger()));
            generateWatcher.setFuture(future);
        }
        if(!Ic.ResizedImage.isNull() && m_resizeGeneratingState==2) {
            Scene->removeItem(GrItem);
            delete GrItem;
            GrItem = Scene->addPixmap(QPixmap::fromImage(Ic.ResizedImage));
            GrItem->setRotation(Rotate);
        }
        GrItem->setScale(Ic.ResizedImage.isNull() ? scale : 1.0);
    }
    // only GPU resizing
    if((effect > qvEnums::UsingFixedShader && effect < qvEnums::UsingCpuResizer) || effect > qvEnums::UsingSomeShader) {
        if(!Ic.ResizedImage.isNull())
            initializePage(true);
        GrItem->setScale(scale);
    }
    GrItem->setRotation(Rotate+rotateOffset);
    GrItem->setPos(pos);
}

void PageContent::initializePage(bool resetResized)
{
    if(GrItem) {
        Scene->removeItem(GrItem);
        delete GrItem;
    }
    if(Scene) {
        GrItem = Scene->addPixmap(QPixmap::fromImage(qApp->Effect() > qvEnums::UsingFixedShader || Ic.ResizedImage.isNull() ? Ic.Image : Ic.ResizedImage));
        GrItem->setRotation(Rotate);
    }
    if(resetResized)
        Ic.ResizedImage = QImage();
    m_resizeGeneratingState = 0;
}

void PageContent::resetSignage(QRect viewport, PageContent::Fitting fitting)
{
    if(Text.isEmpty()) {
        if(GText) {
            Scene->removeItem(GText);
            delete GText;
            GText = nullptr;
            Scene->removeItem(GTextSurface);
            delete GTextSurface;
            GTextSurface = nullptr;
        }
        return;
    }
    if(GText)
        return;
    GText = Scene->addText(Text);
    GText->setPos(fitting == PageContent::FitRight ? viewport.right()-GText->boundingRect().width() : 0, 0);
//qDebug() << GText->pos() << Text;
    GText->setDefaultTextColor(Qt::green);
    GText->setZValue(1);
    QBrush brush(QColor::fromRgb(0, 0, 0, 0x80));
    GTextSurface = Scene->addRect(GText->boundingRect(), Qt::PenStyle::NoPen, brush);
    GTextSurface->setPos(GText->pos());
}

void PageContent::dispose()
{
    if(GrItem) {
        Scene->removeItem(GrItem);
        delete GrItem;
        GrItem = nullptr;
    }
    if(GText) {
        Scene->removeItem(GText);
        delete GText;
        GText = nullptr;
        Scene->removeItem(GTextSurface);
        delete GTextSurface;
        GTextSurface = nullptr;
    }
}

void PageContent::resetScene(QGraphicsScene *)
{

}

void PageContent::on_resizeFinished_trigger()
{
    Ic.ResizedImage = generateWatcher.result();

    m_resizeGeneratingState = 2;
    disconnect(&generateWatcher, SIGNAL(finished()), this, SLOT(on_resizeFinished_trigger()));
    emit resizeFinished();
}

void PageContent::checkInitialize()
{
    if(initialized)
        return;
    if(!Ic.Movie.isNull()) {
        QMovie* movie = Ic.Movie.data();
        connect(movie, SIGNAL(finished()), SLOT(on_animateFinished_trigger()));
        connect(movie, SIGNAL(frameChanged(int)), SLOT(on_animateFrameChanged_trigger(int)));
        movie->start();
    }
    initialized = true;
}

void PageContent::on_animateFrameChanged_trigger(int frameNumber)
{
//    qDebug() << frameNumber;
    QGraphicsPixmapItem *pi = dynamic_cast<QGraphicsPixmapItem *>(GrItem);
    QMovie* movie = Ic.Movie.data();
    movie->jumpToFrame(frameNumber);
    pi->setPixmap(movie->currentPixmap());
}

void PageContent::on_animateFinished_trigger()
{
    QGraphicsPixmapItem *pi = dynamic_cast<QGraphicsPixmapItem *>(GrItem);
    QMovie* movie = Ic.Movie.data();
    movie->stop();
    movie->jumpToFrame(0);
    pi->setPixmap(movie->currentPixmap());
    movie->start();
}
