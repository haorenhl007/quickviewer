#include <QtWidgets>

#include "ui_folderwindow.h"
#include "ui_mainwindow.h"

#include "folderwindow.h"
#include "models/filevolume.h"
#include "models/qvapplication.h"

FolderWindow::FolderWindow(QWidget *parent, Ui::MainWindow *uiMain)
    : QWidget(parent)
    , ui(new Ui::FolderWindow)
    , m_itemModel(this)
    , m_sortModeMenu(nullptr)
    , m_itemContextMenu(nullptr)
{
    ui->setupUi(this);

    ui->folderView->installEventFilter(this);

    // folderView
//    m_itemModel.setHeaderData(0, Qt::Horizontal, tr("Name"));
//    m_itemModel.setHeaderData(1, Qt::Horizontal, tr("ModifiedTime"));
    ui->folderView->setModel(&m_itemModel);

    resetSortMode();

    // menus
    ui->menuBar->removeAction(ui->menuSort->menuAction());
    m_sortModeMenu = ui->menuSort;
    ui->menuBar->removeAction(ui->menuItemContext->menuAction());
    m_itemContextMenu = ui->menuItemContext;
}

FolderWindow::~FolderWindow()
{
    if(m_sortModeMenu)
        delete m_sortModeMenu;
    if(m_itemContextMenu)
        delete m_itemContextMenu;
    delete ui;
}

void FolderWindow::setAsToplevelWindow()
{
    ui->menuBar->setVisible(true);
    ui->folderView->header()->setVisible(true);
    QRect rect = geometry();
    ui->folderView->setColumnWidth(0, rect.width()-150);
    ui->folderView->setColumnWidth(1, 150);
    m_itemModel.setColumns(2);
}

void FolderWindow::setAsInnerWidget()
{
    ui->menuBar->setVisible(false);
    ui->folderView->header()->setVisible(false);
    m_itemModel.setColumns(1);
}

static QModelIndex selectedIdx;

bool FolderWindow::eventFilter(QObject *obj, QEvent *event)
{
    qDebug() << obj << event << event->type();
    QMouseEvent *mouseEvent = NULL;
    QContextMenuEvent *contextEvent = nullptr;
    switch (event->type()) {
    case QEvent::ContextMenu:
        contextEvent = dynamic_cast<QContextMenuEvent*>(event);
        QPoint inner = ui->folderView->mapFromGlobal(QCursor::pos());
        selectedIdx = ui->folderView->indexAt(inner);
        m_itemContextMenu->exec(QCursor::pos());
        return true;
    }
    return QObject::eventFilter(obj, event);
}


void FolderWindow::on_setHome_triggered()
{
    int row = selectedIdx.row();
    if(row < 0 || row >= m_volumes.size())
        return;
    const FolderItem& item = m_volumes[row];
    QDir dir(m_currentPath);
    qApp->setHomeFolderPath(dir.filePath(item.name));
}


void FolderWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if(e->mimeData()->hasFormat("text/uri-list"))
    {
        e->acceptProposedAction();
    }
}

void FolderWindow::dropEvent(QDropEvent *e)
{
    if(!e->mimeData()->hasUrls())
        return;
    QList<QUrl> urlList = e->mimeData()->urls();
    for (int i = 0; i < urlList.size(); i++) {
        QUrl url = urlList[i];
        QFileInfo info(url.toLocalFile());
        if(info.isDir() || info.isFile()) {
            setFolderPath(info.absoluteFilePath(), false);
            break;
        }
    }
}

void FolderWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    QFontMetrics fontMetrics(ui->pathLabel->font());
    QString pathLabelTxt = fontMetrics.elidedText(m_currentPath, Qt::ElideMiddle, event->size().width()-10);
    ui->pathLabel->setText(pathLabelTxt);
}

static bool filenameLessThan(const FolderItem& lhs, const FolderItem& rhs)
{
    if(lhs.type != rhs.type)
        return lhs.type < rhs.type;
    return IFileLoader::caseInsensitiveLessThan(lhs.name, rhs.name);
}

static bool updatedAtLessThan(const FolderItem& lhs, const FolderItem& rhs)
{
    if(lhs.type != rhs.type)
        return lhs.type < rhs.type;
    return lhs.updated_at < rhs.updated_at;
}

//IFileLoader::caseInsensitiveLessThan

void FolderWindow::setFolderPath(QString path, bool showParent)
{
    QFileInfo fileinfo(path);
    if(!fileinfo.exists())
        return;

    QDir dir;
    if(fileinfo.isDir()) {
        dir.setPath(QDir::toNativeSeparators(showParent ? fileinfo.canonicalPath() : path));
    } else {
        dir.setPath(QDir::toNativeSeparators(fileinfo.dir().path()));
    }

//    m_currentPath = fileinfo.isDir() ? path : fileinfo.canonicalPath();
    m_currentPath = dir.path();

    // Elide text(Otherwise the width of the main window will be forcibly changed)
    QFontMetrics fontMetrics(ui->pathLabel->font());
    QString pathLabelTxt = fontMetrics.elidedText(m_currentPath, Qt::ElideMiddle, width()-10);
    ui->pathLabel->setText(pathLabelTxt);

    m_volumes.clear();
    {
        QStringList subfolders = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs, QDir::Unsorted);
        foreach(const QString& sf, subfolders) {
            QFileInfo fi(dir.absoluteFilePath(sf));
            m_volumes << FolderItem(sf, FolderItem::Dir, fi.lastModified());
        }
    }

    {
        QStringList archives;
        foreach(const QString name, dir.entryList(QDir::NoDotAndDotDot | QDir::Files, QDir::Unsorted)) {
            if(IFileLoader::isArchiveFile(name))
                archives << name;
        }
        foreach(const QString& ar, archives) {
            QFileInfo fi(dir.absoluteFilePath(ar));
            m_volumes << FolderItem(ar, FolderItem::Archive, fi.lastModified());
        }
    }
    qvEnums::FolderViewSort sortmode = qApp->FolderSortMode();
    if(sortmode == qvEnums::OrderByName) {
        qSort(m_volumes.begin(), m_volumes.end(), filenameLessThan);
    } else {
        qSort(m_volumes.rbegin(), m_volumes.rend(), updatedAtLessThan);
    }

    if(m_volumes.empty()) {
        m_volumes << FolderItem(tr("No folders or archives"), FolderItem::NoItems, QDateTime());
    }
    m_itemModel.setVolumes(&m_volumes);

    if(showParent)
        on_volumeChanged_triggered(path);
}

void FolderWindow::resetSortMode()
{
    qvEnums::FolderViewSort sortMode = qApp->FolderSortMode();
    ui->actionOrderByName->setChecked(sortMode == qvEnums::OrderByName);
    ui->actionOrderByUpdatedAt->setChecked(sortMode == qvEnums::OrderByUpdatedAt);
    ui->sortModeButton->setText(sortMode == qvEnums::OrderByName
                                ? ui->actionOrderByName->text()
                                : ui->actionOrderByUpdatedAt->text());
}

void FolderWindow::on_home_triggered()
{
    if(m_historyNext.contains(m_currentPath))
        m_historyNext.removeOne(m_currentPath);
    m_historyNext << m_currentPath;
    setFolderPath(qApp->HomeFolderPath(), false);
}

void FolderWindow::on_prev_triggered()
{
    if(m_historyPrev.empty())
        return;
    if(m_historyNext.contains(m_currentPath))
        m_historyNext.removeOne(m_currentPath);
    m_historyNext << m_currentPath;
    setFolderPath(m_historyPrev.takeLast(), false);
}

void FolderWindow::on_next_triggered()
{
    if(m_historyNext.empty())
        return;
    if(m_historyPrev.contains(m_currentPath))
        m_historyPrev.removeOne(m_currentPath);
    m_historyPrev << m_currentPath;
    setFolderPath(m_historyNext.takeLast(), false);
}

void FolderWindow::on_parent_triggered()
{
    if(m_currentPath.isEmpty())
        return;
    QFileInfo info(m_currentPath);
    setFolderPath(info.canonicalPath(), false);
}

void FolderWindow::on_reload_triggered()
{
    if(m_currentPath.isEmpty())
        return;
    setFolderPath(m_currentPath, false);
}

void FolderWindow::on_volumeChanged_triggered(QString path)
{
    QFileInfo info(QDir::toNativeSeparators(path));
    if(!info.exists() || m_currentPath != info.canonicalPath())
        return;
    QString name = info.fileName();
    int row = -1;
    foreach(const FolderItem& item, m_volumes) {
        row++;
        if(name != item.name)
            continue;
        QModelIndex midx = m_itemModel.index(row, 0, QModelIndex());
        ui->folderView->setCurrentIndex(midx);

        break;
    }
}

void FolderWindow::on_itemSingleClicked(const QModelIndex &index)
{
    int row = index.row();
    if(row >= m_volumes.size())
        return;
    FolderItem& item = m_volumes[row];
    if(item.type == FolderItem::NoItems)
        return;
    QDir dir(m_currentPath);
    QString subpath = dir.absoluteFilePath(item.name);
    emit openVolume(subpath);
}

void FolderWindow::on_itemDoubleClicked(const QModelIndex &index)
{
    int row = index.row();
    if(row >= m_volumes.size())
        return;
    FolderItem& item = m_volumes[row];
    if(item.type == FolderItem::NoItems)
        return;
    QDir dir(m_currentPath);
    QString subpath = dir.absoluteFilePath(item.name);
    emit openVolume(subpath);
    setFolderPath(subpath, false);
}

void FolderWindow::on_sortMode_triggered()
{
    QWidget* widget = ui->sortModeButton;

    QPoint p = widget->mapToGlobal(QPoint(0, widget->height()));
    m_sortModeMenu->exec(p);
}

void FolderWindow::on_orderByName_triggered()
{
    qApp->setFolderSortMode(qvEnums::OrderByName);
    resetSortMode();
    on_reload_triggered();
}

void FolderWindow::on_orderByUpdatedAt_triggered()
{
    qApp->setFolderSortMode(qvEnums::OrderByUpdatedAt);
    resetSortMode();
    on_reload_triggered();
}

void FolderWindow::closeEvent(QCloseEvent *e)
{
    QWidget::closeEvent(e);
    emit closed();
}
