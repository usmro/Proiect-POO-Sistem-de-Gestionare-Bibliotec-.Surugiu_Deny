#ifndef QTMAINWINDOW_H
#define QTMAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QScrollArea>
#include <memory>
#include "Biblioteca.h"
#include "Utilizator.h"

class QtMainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit QtMainWindow(Biblioteca& bib, QWidget *parent = nullptr);
    ~QtMainWindow() = default;

private slots:
    void onLoginClicked();
    void onLogoutClicked();
    void switchPage(int index);

private:
    Biblioteca& m_bib;
    std::shared_ptr<Utilizator> m_currentUser;

    // View Setup
    void setupLoginView();
    void buildDynamicAppView(); // Recrează interfața în funcție de rol
    
    // Pages
    QWidget* createDashboardPage();
    QWidget* createCatalogPage();
    QWidget* createMembriPage();
    QWidget* createImprumuturiPage();
    QWidget* createFinantePage();
    QWidget* createProfilCititorPage();
    QWidget* createSarciniIngrijitorPage();
    QWidget* createRaportPage();
    QWidget* createPlaceholderPage(const QString& title, const QString& subtitle);

    QStackedWidget* m_rootStackedWidget; // 0: Login, 1: AppView
    
    // Login
    QWidget* m_loginWidget;
    QLineEdit* m_idInput;
    QLineEdit* m_passInput;

    // App Main
    QWidget* m_appWidget; // Va fi șters și recreat la fiecare login
    QStackedWidget* m_appPagesWidget;
    
    // Sidebar
    QList<QPushButton*> m_navButtons;
    
    // Catalog State
    int m_catalogPage = 0;
    std::vector<std::shared_ptr<Carte>> m_currentCatalogData;
    QWidget* m_catalogGridContainer = nullptr;
    QGridLayout* m_catalogGridLayout = nullptr;
    QLabel* m_pageLabel = nullptr;
    QPushButton* m_btnPrev = nullptr;
    QPushButton* m_btnNext = nullptr;
    void refreshCatalogGrid();
    void applyCatalogFilters(const QString& category, bool inStock, const QString& searchTxt);
};

#endif // QTMAINWINDOW_H
