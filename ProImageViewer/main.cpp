#include "MainWindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setOrganizationName("MySoft");
    app.setApplicationName("ImageViewer");
    app.setStyle("Fusion");

    MainWindow window;
    window.show();

    return app.exec();
}