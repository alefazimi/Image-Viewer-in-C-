#include "MainWindow.h"
#include "ImageUtils.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QSettings>
#include <QToolBar>
#include <QStyle>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), undoIndex(-1), fitToWindow(true),
      currentScale(1.0), isGrayscale(false), cropMode(false)
{
    // Create the main graphics view for image display
    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setBackgroundBrush(QBrush(QColor(30, 30, 30)));
    view->setFrameShape(QFrame::NoFrame);
    view->viewport()->installEventFilter(this);
    setCentralWidget(view);

    setupEditorPanel();
    setupToolbar();
    createMenuBar();

    rubberBand = new QRubberBand(QRubberBand::Rectangle, view->viewport());

    statusLabel = new QLabel("Ready");
    statusBar()->addWidget(statusLabel);

    loadSettings();
}

MainWindow::~MainWindow() {}

void MainWindow::createMenuBar()
{
    // File menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    fileMenu->addAction(QIcon::fromTheme("document-open"), "&Open...",
                        this, &MainWindow::openFile, QKeySequence::Open);

    fileMenu->addAction(QIcon::fromTheme("document-save"), "&Save",
                        this, &MainWindow::saveFile, QKeySequence::Save);
    fileMenu->addAction(QIcon::fromTheme("document-save-as"), "Save &As...",
                        this, &MainWindow::saveAsFile, QKeySequence::SaveAs);
    fileMenu->addSeparator();

    fileMenu->addAction("E&xit", qApp, &QApplication::quit, QKeySequence::Quit);

    // Edit menu
    QMenu *editMenu = menuBar()->addMenu("&Edit");

    editMenu->addAction(QIcon::fromTheme("edit-undo"), "&Undo",
                        this, &MainWindow::performUndo, QKeySequence::Undo);
    editMenu->addAction(QIcon::fromTheme("edit-redo"), "&Redo",
                        this, &MainWindow::performRedo, QKeySequence::Redo);

    editMenu->addSeparator();

    editMenu->addAction("Crop Mode", this, &MainWindow::toggleCropMode, Qt::Key_C);

    // View menu
    QMenu *viewMenu = menuBar()->addMenu("&View");

    viewMenu->addAction("Zoom &In", this, &MainWindow::zoomIn, QKeySequence::ZoomIn);

    viewMenu->addAction("Zoom &Out", this, &MainWindow::zoomOut, QKeySequence::ZoomOut);

    QAction *fitAction = viewMenu->addAction("&Fit to Window",
                                              this, &MainWindow::toggleFitToWindow, Qt::Key_F);

    fitAction->setCheckable(true);
    fitAction->setChecked(fitToWindow);

    //nav menu
    QMenu *navMenu = menuBar()->addMenu("&Navigation");

    navMenu->addAction("&Next Image", this, &MainWindow::nextImage, Qt::Key_Right);

    navMenu->addAction("&Previous Image", this, &MainWindow::prevImage, Qt::Key_Left);
}

void MainWindow::setupToolbar()
{
    QToolBar *toolbar = addToolBar("Main Toolbar");
    toolbar->setMovable(false);

    toolbar->addAction(QIcon::fromTheme("document-open"), "Open",
                       this, &MainWindow::openFile);
    toolbar->addAction(QIcon::fromTheme("document-save"), "Save",
                       this, &MainWindow::saveFile);
    toolbar->addSeparator();

    auto prevIcon = QIcon::fromTheme("go-previous", style()->standardIcon(QStyle::SP_ArrowLeft));
    auto nextIcon = QIcon::fromTheme("go-next", style()->standardIcon(QStyle::SP_ArrowRight));

    toolbar->addAction(prevIcon, "Prev", this, &MainWindow::prevImage);

    toolbar->addAction(nextIcon, "Next", this, &MainWindow::nextImage);
    toolbar->addSeparator();

    auto rotateIcon = QIcon::fromTheme("object-rotate-right", style()->standardIcon(QStyle::SP_BrowserReload));
    auto cropIcon = QIcon::fromTheme("transform-crop-and-resize", style()->standardIcon(QStyle::SP_FileIcon));
    toolbar->addAction(rotateIcon, "Rotate", this, &MainWindow::rotateRight);

    toolbar->addAction(cropIcon, "Crop", this, &MainWindow::toggleCropMode);
    toolbar->addSeparator();

    auto undoIcon = QIcon::fromTheme("edit-undo", style()->standardIcon(QStyle::SP_ArrowBack));
    auto redoIcon = QIcon::fromTheme("edit-redo", style()->standardIcon(QStyle::SP_ArrowForward));
    toolbar->addAction(undoIcon, "Undo", this, &MainWindow::performUndo);

    toolbar->addAction(redoIcon, "Redo", this, &MainWindow::performRedo);
}
void MainWindow::setupEditorPanel()
{
    editorDock = new QDockWidget("Editor", this);
    editorDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);

    QWidget *dockWidget = new QWidget();
    QVBoxLayout *dockLayout = new QVBoxLayout(dockWidget);

    // brightness slider

    dockLayout->addWidget(new QLabel("Brightness"));

    brightnessSlider = new QSlider(Qt::Horizontal);
    brightnessSlider->setRange(-100, 100);
    brightnessSlider->setValue(0);
    connect(brightnessSlider, &QSlider::valueChanged, this, &MainWindow::onSliderChanged);
    connect(brightnessSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    dockLayout->addWidget(brightnessSlider);

    //Contrast slider
    dockLayout->addWidget(new QLabel("Contrast"));
    contrastSlider = new QSlider(Qt::Horizontal);
    contrastSlider->setRange(0, 30);
    contrastSlider->setValue(10);
    connect(contrastSlider, &QSlider::valueChanged, this, &MainWindow::onSliderChanged);
    connect(contrastSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    dockLayout->addWidget(contrastSlider);

    // saturation slider

    dockLayout->addWidget(new QLabel("Saturation"));
    saturationSlider = new QSlider(Qt::Horizontal);
    saturationSlider->setRange(-100, 100);
    saturationSlider->setValue(0);

    connect(saturationSlider, &QSlider::valueChanged, this, &MainWindow::onSliderChanged);
    connect(saturationSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    dockLayout->addWidget(saturationSlider);


    //  Blur slider
    dockLayout->addWidget(new QLabel("Blur"));
    blurSlider = new QSlider(Qt::Horizontal);
    blurSlider->setRange(0, 10);
    blurSlider->setValue(0);
    connect(blurSlider, &QSlider::valueChanged, this, &MainWindow::onSliderChanged);
    connect(blurSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    dockLayout->addWidget(blurSlider);

    // tools
    QGroupBox *toolGroup = new QGroupBox("Tools");
    QVBoxLayout *toolLayout = new QVBoxLayout(toolGroup);

    QPushButton *grayscaleBtn = new QPushButton("Toggle Grayscale");
    connect(grayscaleBtn, &QPushButton::clicked, this, &MainWindow::toggleGrayscale);
    toolLayout->addWidget(grayscaleBtn);

    QPushButton *sharpenBtn = new QPushButton("Sharpen");
    connect(sharpenBtn, &QPushButton::clicked, this, &MainWindow::applySharpen);
    toolLayout->addWidget(sharpenBtn);

    QPushButton *resetBtn = new QPushButton("Reset All");
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::resetEdits);
    toolLayout->addWidget(resetBtn);

    dockLayout->addWidget(toolGroup);
    dockLayout->addStretch();

    editorDock->setWidget(dockWidget);
    addDockWidget(Qt::RightDockWidgetArea, editorDock);
}

//History logic

void MainWindow::pushUndoState()
{

    // cear the redo stack when a new action is taken
    if (undoIndex < (int)undoHistory.size() - 1)
        undoHistory.resize(undoIndex + 1);

    //Store the current display state
    undoHistory.push_back(displayMat.clone());
    undoIndex++;

    // Update the base working copy and reset to neutral positions
    originalMat = displayMat.clone();
    resetSlidersToDefaults();
}


void MainWindow::setSliderValueBlocking(QSlider *slider, int value)
{

    if (!slider) return;
    bool wasBlocked = slider->blockSignals(true);
    slider->setValue(value);
    slider->blockSignals(wasBlocked);
}

void MainWindow::resetSlidersToDefaults()
{
    setSliderValueBlocking(brightnessSlider, 0);
    setSliderValueBlocking(contrastSlider, 10);
    setSliderValueBlocking(saturationSlider, 0);
    setSliderValueBlocking(blurSlider, 0);
    isGrayscale = false;
}


void MainWindow::performUndo()
{
    if (undoIndex > 0) {
        undoIndex--;
        originalMat = undoHistory[undoIndex].clone();
        displayMat = originalMat.clone();
        resetSlidersToDefaults();
        onSliderChanged();
    }

}

void MainWindow::performRedo()


{
    if (undoIndex < (int)undoHistory.size() - 1) {
        undoIndex++;
        originalMat = undoHistory[undoIndex].clone();
        displayMat = originalMat.clone();
        resetSlidersToDefaults();
        onSliderChanged();
    }
}

//Loading & saving

void MainWindow::loadImage(const std::filesystem::path &path)
{
    cleanMat = cv::imread(path.string());

    if (cleanMat.empty())
    {
        statusLabel->setText("Error loading image.");
        return;
    }
    // Reset History
    undoHistory.clear();
    originalMat = cleanMat.clone();
    undoHistory.push_back(originalMat.clone());
    undoIndex = 0;
    resetEdits();
}

void MainWindow::saveFile()
{
    if (displayMat.empty())
        return;
    std::string currentPath = scanner.current().string();
    if (currentPath.empty())
        return;

    if (cv::imwrite(currentPath, displayMat))
    {
        statusLabel->setText("Saved: " + QString::fromStdString(currentPath));
        cleanMat = displayMat.clone();
    }
    else
    {
        QMessageBox::critical(this, "Error", "Failed to save file.");
    }
}


void MainWindow::saveAsFile()
{
    if (displayMat.empty())
        return;
    QString filter = "Images (*.jpg *.png *.bmp *.tif)";
    QString filename = QFileDialog::getSaveFileName(this, "Save Image As", "", filter);

    if (!filename.isEmpty())
    {
        if (cv::imwrite(filename.toStdString(), displayMat))
        {
            statusLabel->setText("Saved As: " + filename);
        }
        else
        {
            QMessageBox::critical(this, "Error", "Failed to save file.");
        }
    }
}

//editing

void MainWindow::onSliderChanged()
{
    if (originalMat.empty())
        return;

    cv::Mat result = originalMat.clone();

    if (saturationSlider->value() != 0)
        result = ImageUtils::adjustSaturation(result, saturationSlider->value());


    double alpha = contrastSlider->value() / 10.0;
    int beta = brightnessSlider->value();
    if (alpha != 1.0 || beta != 0)
        result = ImageUtils::adjustBrightnessContrast(result, alpha, beta);

    if (blurSlider->value() > 0)
        result = ImageUtils::applyBlur(result, blurSlider->value());
    if (isGrayscale)
        result = ImageUtils::toGrayscale(result);

    displayMat = result;

    QImage img = ImageUtils::matToQImage(displayMat);
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(img));
    scene->setSceneRect(img.rect());

    updateView();
}

void MainWindow::onSliderReleased()
{
    pushUndoState();
}

void MainWindow::rotateRight()
{
    if (originalMat.empty())
        return;

    pushUndoState();
    originalMat = ImageUtils::rotate90(originalMat);
    displayMat = originalMat.clone();
    onSliderChanged();
}

void MainWindow::toggleGrayscale()
{
    isGrayscale = !isGrayscale;
    onSliderChanged();

    if (isGrayscale)
        pushUndoState();
}

void MainWindow::applySharpen()
{
    if (originalMat.empty())
        return;

    pushUndoState();
    originalMat = ImageUtils::sharpen(originalMat);
    displayMat = originalMat.clone();
    onSliderChanged();
}

void MainWindow::resetEdits()
{
    if (cleanMat.empty())
        return;

    undoHistory.clear();
    originalMat = cleanMat.clone();
    undoHistory.push_back(originalMat.clone());
    undoIndex = 0;
    resetSlidersToDefaults();
    onSliderChanged();
}

// crop tool

void MainWindow::toggleCropMode()
{
    cropMode = !cropMode;
    if (cropMode) {
        view->setDragMode(QGraphicsView::NoDrag);
        view->setCursor(Qt::CrossCursor);
        statusLabel->setText("Crop Mode: Drag to select area.");
    } else {
        view->setDragMode(QGraphicsView::ScrollHandDrag);
        view->setCursor(Qt::ArrowCursor);
        rubberBand->hide();
        statusLabel->setText("Ready");
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (!cropMode || obj != view->viewport())
        return QMainWindow::eventFilter(obj, event);

    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

    if (event->type() == QEvent::MouseButtonPress) {
        originCrop = mouseEvent->pos();
        rubberBand->setGeometry(QRect(originCrop, QSize()));
        rubberBand->show();
        return true;
    }
    else if (event->type() == QEvent::MouseMove) {
        rubberBand->setGeometry(QRect(originCrop, mouseEvent->pos()).normalized());
        return true;
    }
    else if (event->type() == QEvent::MouseButtonRelease) {
        rubberBand->hide();
        QRect rect = QRect(originCrop, mouseEvent->pos()).normalized();

        QPointF topLeft = view->mapToScene(rect.topLeft());
        QPointF bottomRight = view->mapToScene(rect.bottomRight());

        cv::Rect cropRect(topLeft.x(), topLeft.y(),
                          bottomRight.x() - topLeft.x(),
                          bottomRight.y() - topLeft.y());

        if (cropRect.width > 10 && cropRect.height > 10) {
            pushUndoState();
            originalMat = ImageUtils::crop(originalMat, cropRect);
            displayMat = originalMat.clone();
            onSliderChanged();
        }

        toggleCropMode();

        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}



void MainWindow::openFile()
{
    QSettings settings("MySoft", "ProImageViewer");
    QString lastDir = settings.value("lastDir", "").toString();
    QString fileName = QFileDialog::getOpenFileName(this, "Open Image", lastDir, "Images (*.png *.jpg *.jpeg *.bmp *.tif)");
    if (!fileName.isEmpty())
    {
        if (scanner.openDirectory(std::filesystem::path(fileName.toStdString())))
        {
            loadImage(scanner.current());

        }

    }
}

void MainWindow::nextImage()
{
    std::filesystem::path p = scanner.next();
    if (!p.empty())
        loadImage(p);
}


void MainWindow::prevImage()
{
    std::filesystem::path p = scanner.previous();
    if (!p.empty())
        loadImage(p);
}

void MainWindow::zoomIn()
{
    fitToWindow = false;
    currentScale *= 1.25;
    updateView();
    updateStatusBar();
}

void MainWindow::zoomOut()
{
    fitToWindow = false;
    currentScale /= 1.25;
    updateView();
    updateStatusBar();
}

void MainWindow::toggleFitToWindow()
{
    fitToWindow = !fitToWindow;
    updateView();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (fitToWindow)
        updateView();
}

void MainWindow::updateView()
{
    if (displayMat.empty())
        return;
    if (fitToWindow)
    {
        view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        currentScale = view->transform().m11();
    }
    else
    {
        view->resetTransform();
        view->scale(currentScale, currentScale);
    }
    updateStatusBar();
}

void MainWindow::updateStatusBar()
{
    if (originalMat.empty())
        return;
    QString info = QString("File: %1 | Res: %2x%3 | Zoom: %4%")
                       .arg(QString::fromStdString(scanner.current().filename().string()))
                       .arg(originalMat.cols)
                       .arg(originalMat.rows)
                       .arg(static_cast<int>(currentScale * 100));
    statusLabel->setText(info);
}

void MainWindow::loadSettings()
{
    QSettings settings("MySoft", "ProImageViewer");
    restoreGeometry(settings.value("geometry").toByteArray());

    restoreState(settings.value("windowState").toByteArray());

    fitToWindow = settings.value("fitToWindow", true).toBool();
}

void MainWindow::saveSettings()
{
    QSettings settings("MySoft", "ProImageViewer");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("fitToWindow", fitToWindow);

    if (!scanner.current().empty())
        settings.setValue("lastDir", QString::fromStdString(scanner.current().parent_path().string()));
}

void MainWindow::closeEvent(QCloseEvent *event)

{
    saveSettings();

    QMainWindow::closeEvent(event);

}