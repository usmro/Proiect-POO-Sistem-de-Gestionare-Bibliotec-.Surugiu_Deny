#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "QtMainWindow.h"
#include "Biblioteca.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Setăm fontul global pentru a da o tentă mai elegantă
    QFont font("Segoe UI", 10);
    app.setFont(font);

    // Stilizarea interfeței pentru a imita lemnul și o bibliotecă clasică
    QString styleSheet = R"(
        QWidget {
            background-color: #fafaf9; /* Stone-50 */
            color: #292524; /* Stone-800 */
            font-family: 'Segoe UI', system-ui, sans-serif;
        }
        QMainWindow, QDialog {
            background-color: #fafaf9;
        }
        QPushButton {
            background-color: white;
            border: 1px solid #e5e5e5;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
            color: #292524;
        }
        QPushButton:hover {
            background-color: #f5f5f4;
            border: 1px solid #d6d3d1;
        }
        QPushButton:pressed {
            background-color: #e7e5e4;
        }
        QLineEdit, QTableView, QListWidget, QTextEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background-color: white;
            border: 1px solid #d6d3d1;
            border-radius: 4px;
            padding: 6px;
            color: #292524;
        }
        QLineEdit:focus, QSpinBox:focus, QComboBox:focus {
            border: 1px solid #3b82f6;
        }
        QHeaderView::section {
            background-color: #f5f5f4;
            border: none;
            border-bottom: 1px solid #d6d3d1;
            padding: 8px;
            font-weight: bold;
            color: #57534e;
        }
        QLabel {
            font-size: 10pt;
            background-color: transparent;
        }
        QMessageBox {
            background-color: #fafaf9;
        }
    )";
    app.setStyleSheet(styleSheet);

    Biblioteca bib;
    QtMainWindow mainWindow(bib);
    mainWindow.resize(900, 600);
    mainWindow.setWindowTitle("Sistem de Gestiune a Bibliotecii - Qt Edition");
    mainWindow.show();

    int exitCode = app.exec();
    bib.salveazaDate(); // Salvăm datele la final
    return exitCode;
}
