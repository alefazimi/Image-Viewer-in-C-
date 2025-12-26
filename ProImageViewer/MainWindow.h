#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QSlider>
#include <QDockWidget>
#include <QEvent>
#include <QRubberBand>
#include <opencv2/opencv.hpp>
#include "DirectoryScanner.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void openFile();
    void saveFile();
    void saveAsFile();

    void nextImage();
    void prevImage();

    void zoomIn();
    void zoomOut();

    void toggleFitToWindow();

    void onSliderReleased();
    void onSliderChanged();
    void rotateRight();

    void toggleGrayscale();

    void applySharpen();
    void toggleCropMode();

    void performUndo();
    void performRedo();

    void resetEdits();
    void resetSlidersToDefaults();

    void setSliderValueBlocking(QSlider* slider, int value);

private:
    void loadImage(const std::filesystem::path& path);
    void updateView();
    void updateStatusBar();
    void setupEditorPanel();
    void setupToolbar();
    void createMenuBar();
    void loadSettings();
    void saveSettings();
    void pushUndoState();

    // UI components
    QGraphicsView* view;
    QGraphicsScene* scene;
    QLabel* statusLabel;
    QDockWidget* editorDock;
    QRubberBand* rubberBand;

    // sliders
    QSlider* brightnessSlider;
    QSlider* contrastSlider;
    QSlider* saturationSlider;
    QSlider* blurSlider;

    DirectoryScanner scanner;
    std::vector<cv::Mat> undoHistory;
    int undoIndex;

    cv::Mat cleanMat;       // file from disk
    cv::Mat originalMat;    // working copy
    cv::Mat displayMat;     // rendered copy

    bool fitToWindow;
    double currentScale;
    bool isGrayscale;
    bool cropMode;
    QPoint originCrop;
};

#endif