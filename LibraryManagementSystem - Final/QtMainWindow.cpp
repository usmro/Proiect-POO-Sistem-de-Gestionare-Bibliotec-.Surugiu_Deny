#include "QtMainWindow.h"
#include "QtDialogs.h"
#include "Cititor.h"
#include <QFrame>
#include <QGridLayout>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QCheckBox>
#include <QComboBox>
#include <QHeaderView>
#include <QTableWidget>
#include <set>

QtMainWindow::QtMainWindow(Biblioteca& bib, QWidget *parent)
    : QMainWindow(parent), m_bib(bib), m_currentUser(nullptr), m_appWidget(nullptr)
{
    this->resize(1280, 800);
    this->showMaximized();
    
    this->setStyleSheet(R"(
        QMainWindow { background-color: #fdfbf7; color: #1c1917; font-family: 'Segoe UI', sans-serif; }
        QLineEdit { padding: 8px 16px; border: 2px solid #e5e5e5; border-radius: 20px; background: #fafaf9; }
        QLineEdit:focus { border: 2px solid #b45309; background: white; }
        QScrollBar:vertical { border: none; background: transparent; width: 8px; }
        QScrollBar::handle:vertical { background: #d6d3d1; border-radius: 4px; }
        QScrollBar::handle:vertical:hover { background: #a8a29e; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }
    )");
    
    m_rootStackedWidget = new QStackedWidget(this);
    setCentralWidget(m_rootStackedWidget);

    setupLoginView();
    m_rootStackedWidget->addWidget(m_loginWidget);
    m_rootStackedWidget->setCurrentWidget(m_loginWidget);
}

void QtMainWindow::setupLoginView() {
    m_loginWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(m_loginWidget);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame* loginFrame = new QFrame(m_loginWidget);
    loginFrame->setObjectName("LoginFrame");
    loginFrame->setStyleSheet("QFrame#LoginFrame { background-color: white; border: 1px solid #e7e5e4; border-radius: 12px; }");
    loginFrame->setFixedSize(400, 360);
    
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20); shadow->setColor(QColor(0,0,0,30)); shadow->setOffset(0, 10);
    loginFrame->setGraphicsEffect(shadow);

    QVBoxLayout* frameLayout = new QVBoxLayout(loginFrame);
    frameLayout->setContentsMargins(40, 40, 40, 40);
    
    QLabel* titleLabel = new QLabel("Athenaeum", loginFrame);
    titleLabel->setStyleSheet("font-family: 'Georgia', serif; font-size: 28px; font-weight: bold; color: #231a16;");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QLabel* subLabel = new QLabel("Vă rugăm să vă identificați", loginFrame);
    subLabel->setStyleSheet("color: #78716c; margin-bottom: 20px;");
    subLabel->setAlignment(Qt::AlignCenter);
    
    m_idInput = new QLineEdit(loginFrame);
    m_idInput->setPlaceholderText("ID sau Email...");
    
    m_passInput = new QLineEdit(loginFrame);
    m_passInput->setEchoMode(QLineEdit::Password);
    m_passInput->setPlaceholderText("Parolă...");

    QPushButton* loginBtn = new QPushButton("Autentificare", loginFrame);
    loginBtn->setStyleSheet("QPushButton { background-color: #2c4c3b; color: white; border-radius: 20px; padding: 12px; font-weight: bold; margin-top:10px; }"
                            "QPushButton:hover { background-color: #1a3622; }");
    connect(loginBtn, &QPushButton::clicked, this, &QtMainWindow::onLoginClicked);

    frameLayout->addWidget(titleLabel);
    frameLayout->addWidget(subLabel);
    frameLayout->addWidget(m_idInput);
    frameLayout->addWidget(m_passInput);
    frameLayout->addWidget(loginBtn);
    frameLayout->addStretch();

    mainLayout->addWidget(loginFrame);
}

void QtMainWindow::buildDynamicAppView() {
    if (m_appWidget) {
        m_rootStackedWidget->removeWidget(m_appWidget);
        delete m_appWidget;
        m_navButtons.clear();
    }

    m_appWidget = new QWidget(this);
    QHBoxLayout* mainLayout = new QHBoxLayout(m_appWidget);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->setSpacing(0);

    QFrame* sidebar = new QFrame(m_appWidget);
    sidebar->setFixedWidth(260);
    sidebar->setStyleSheet(R"(
        QFrame { background-color: #231a16; border-right: 1px solid #3d2e26; color: #d6d3d1; }
        QPushButton { text-align: left; background: transparent; padding: 12px 16px; border-radius: 8px; color: #a8a29e; font-size: 14px; margin: 2px 12px; border: 1px solid transparent; }
        QPushButton:hover { background-color: #1a130f; color: #e7e5e4; }
        QPushButton:checked { background-color: #3d2e26; color: #fbbf24; border: 1px solid #523e33; }
        QLabel#Brand { font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #fef3c7; border:none; }
        QLabel#SubBrand { font-size: 10px; font-weight: bold; color: #b45309; letter-spacing: 2px; border:none; }
        QLabel#Section { font-size: 11px; font-weight: bold; color: #8c7462; letter-spacing: 1px; padding-left: 16px; padding-top: 16px; padding-bottom: 8px; border:none; }
        QFrame#UserCard { background-color: #1a130f; border-top: 1px solid #3d2e26; border-right: none; border-bottom: none; border-left: none; }
    )");

    QVBoxLayout* sLayout = new QVBoxLayout(sidebar);
    sLayout->setContentsMargins(0,0,0,0);
    sLayout->setSpacing(0);

    QFrame* brandFrame = new QFrame(sidebar);
    brandFrame->setStyleSheet("background-color: #1a130f; border-bottom: 1px solid #3d2e26; border-top:none; border-left:none; border-right:none;");
    brandFrame->setFixedHeight(84);
    QHBoxLayout* brandLayout = new QHBoxLayout(brandFrame);
    QLabel* brandIcon = new QLabel("|||", brandFrame);
    brandIcon->setStyleSheet("font-size: 24px; font-weight: bold; color: #b45309; border:none; background:transparent;");
    QVBoxLayout* brandText = new QVBoxLayout();
    brandText->setSpacing(0);
    QLabel* lBrand = new QLabel("Athenaeum", brandFrame);
    lBrand->setObjectName("Brand");
    QLabel* lSubBrand = new QLabel("BIBLIOTECĂ CENTRALĂ", brandFrame);
    lSubBrand->setObjectName("SubBrand");
    brandText->addWidget(lBrand);
    brandText->addWidget(lSubBrand);
    brandLayout->addWidget(brandIcon);
    brandLayout->addLayout(brandText);
    sLayout->addWidget(brandFrame);
    sLayout->addSpacing(16);

    QWidget* contentWidget = new QWidget(m_appWidget);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0,0,0,0);
    contentLayout->setSpacing(0);

    QFrame* header = new QFrame();
    header->setFixedHeight(84);
    header->setStyleSheet("QFrame { background-color: rgba(255,255,255,0.9); border-bottom: 1px solid #e5e5e5; }");
    QHBoxLayout* hLayout = new QHBoxLayout(header);
    hLayout->setContentsMargins(40, 0, 40, 0);
    QLineEdit* searchBar = new QLineEdit(header);
    searchBar->setPlaceholderText(" Căutați manuscrise, titluri, autori... (Enter)");
    searchBar->setMinimumWidth(300);
    searchBar->setMaximumWidth(600);
    searchBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(searchBar, &QLineEdit::returnPressed, [this, searchBar](){
        QtFunc::cautaCarte(this, m_bib, m_currentUser, searchBar->text());
    });
    
    hLayout->addWidget(searchBar);
    hLayout->addStretch();
    
    std::string rol = m_currentUser->getTip();
    
    if(rol == "DIRECTOR" || rol == "BIBLIOTECAR") {
        QPushButton* btnAddLoan = new QPushButton("[+] Împrumut Nou", header);
        btnAddLoan->setStyleSheet("QPushButton { background-color: #2c4c3b; color: white; border-radius: 6px; padding: 10px 20px; font-weight: bold; border-bottom: 3px solid #132618; font-family: 'Georgia', serif; } QPushButton:hover { background-color: #1a3622; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnAddLoan, &QPushButton::clicked, [this](){ QtFunc::imprumutaCarte(this, m_bib); });
        hLayout->addWidget(btnAddLoan);
    }
    contentLayout->addWidget(header);

    m_appPagesWidget = new QStackedWidget(contentWidget);
    contentLayout->addWidget(m_appPagesWidget);

    auto addSection = [&](const QString& text) {
        QLabel* sec = new QLabel(text, sidebar);
        sec->setObjectName("Section");
        sLayout->addWidget(sec);
    };

    int pageIndex = 0;
    auto addNavBtn = [&](const QString& icon, const QString& text, QWidget* page) {
        QPushButton* btn = new QPushButton(icon + "   " + text, sidebar);
        btn->setCheckable(true);
        int idx = pageIndex++;
        m_appPagesWidget->addWidget(page);
        connect(btn, &QPushButton::clicked, [this, idx, btn](){
            switchPage(idx);
            for(auto b : m_navButtons) b->setChecked(b == btn);
        });
        sLayout->addWidget(btn);
        m_navButtons.append(btn);
    };

    if (rol == "DIRECTOR" || rol == "BIBLIOTECAR") {
        addSection("SĂLI DE LECTURĂ");
        addNavBtn(">", "Biroul Principal", createDashboardPage());
        addNavBtn(">", "Catalogul Cărților", createCatalogPage());
        addNavBtn(">", "Registrul Membrilor", createMembriPage());
        addNavBtn(">", "Fișe de Împrumut", createImprumuturiPage());
        
        if (rol == "DIRECTOR") {
            addSection("ARHIVĂ & ADMIN");
            addNavBtn(">", "Finanțe & Achiziții", createFinantePage());
            addNavBtn(">", "Cronică / Rapoarte", createRaportPage());
        }
    } else if (rol == "ÎNGRIJITOR" || rol == "INGRIJITOR") {
        addSection("MENTENANȚĂ");
        addNavBtn(">", "Catalogul Cărților", createCatalogPage());
        addNavBtn(">", "Sarcini Îngrijitor", createSarciniIngrijitorPage());
    } else if (rol == "CITITOR") {
        addSection("LECTURĂ");
        addNavBtn(">", "Catalogul Cărților", createCatalogPage());
        addNavBtn(">", "Activitatea Mea", createProfilCititorPage());
    }

    sLayout->addStretch();

    QFrame* userCard = new QFrame(sidebar);
    userCard->setObjectName("UserCard");
    userCard->setFixedHeight(80);
    QHBoxLayout* userLayout = new QHBoxLayout(userCard);
    QLabel* uAvatar = new QLabel("US", userCard);
    uAvatar->setAlignment(Qt::AlignCenter);
    uAvatar->setStyleSheet("font-weight: bold; color: #fef3c7; font-size: 16px; border:none; background: #3d2e26; border-radius: 18px; padding: 4px;");
    uAvatar->setFixedSize(40,40);
    QVBoxLayout* uInfo = new QVBoxLayout();
    QLabel* uName = new QLabel(QString::fromStdString(m_currentUser->getNumeComplet()), userCard);
    uName->setStyleSheet("font-family: 'Georgia', serif; font-weight: bold; color: #fef3c7; border:none; background:transparent;");
    QLabel* uRole = new QLabel(QString::fromStdString(m_currentUser->getTip()), userCard);
    uRole->setStyleSheet("font-size: 11px; color: #8c7462; border:none; background:transparent;");
    uInfo->addWidget(uName);
    uInfo->addWidget(uRole);
    QPushButton* btnLogout = new QPushButton("Ieșire", userCard);
    btnLogout->setStyleSheet("background:transparent; color: #ef4444; font-size:12px; font-weight:bold; border:none; padding:0; margin:0;");
    connect(btnLogout, &QPushButton::clicked, this, &QtMainWindow::onLogoutClicked);

    userLayout->addWidget(uAvatar);
    userLayout->addLayout(uInfo, 1);
    userLayout->addWidget(btnLogout);
    sLayout->addWidget(userCard);

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(contentWidget, 1);

    m_rootStackedWidget->addWidget(m_appWidget);
    
    if (!m_navButtons.empty()) {
        m_navButtons[0]->setChecked(true);
        switchPage(0);
    }
}

// -------------------------------------------------------------
// METODE CREEARE PAGINI (UTILS)
// -------------------------------------------------------------
QWidget* QtMainWindow::createRaportPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(32, 32, 32, 32);
    layout->setSpacing(24);
    
    QLabel* title = new QLabel("Jurnal Audit Sistem", page);
    title->setFont(QFont("Segoe UI", 24, QFont::Bold));
    title->setStyleSheet("color: #1c1917;");
    layout->addWidget(title);
    
    QLabel* sub = new QLabel("Ultimele 100 de acțiuni înregistrate în sistem", page);
    sub->setStyleSheet("color: #57534e; font-size: 14px;");
    layout->addWidget(sub);
    
    QTableWidget* table = new QTableWidget(page);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Dată și Oră", "ID Utilizator", "Acțiune"});
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->setVisible(false);
    
    auto logs = m_bib.getJurnalAudit(100);
    table->setRowCount(logs.size());
    for(size_t i=0; i<logs.size(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(logs[i].data_ora)));
        table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(logs[i].id_utilizator)));
        table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(logs[i].actiune)));
    }
    
    layout->addWidget(table);
    return page;
}

QWidget* QtMainWindow::createPlaceholderPage(const QString& title, const QString& subtitle) {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setAlignment(Qt::AlignCenter);
    
    QLabel* icon = new QLabel("[ ]", page);
    icon->setStyleSheet("font-size: 32px; color: #a8a29e;");
    icon->setAlignment(Qt::AlignCenter);
    
    QLabel* lTitle = new QLabel(title, page);
    lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #78716c;");
    lTitle->setAlignment(Qt::AlignCenter);
    
    QLabel* lSub = new QLabel(subtitle, page);
    lSub->setStyleSheet("font-family: 'Georgia', serif; font-size: 18px; color: #a8a29e;");
    lSub->setAlignment(Qt::AlignCenter);
    
    layout->addWidget(icon);
    layout->addWidget(lTitle);
    layout->addWidget(lSub);
    return page;
}

#include <functional>
static void addGridCard(QGridLayout* grid, const QString& title, const QString& icon, std::function<void()> func, int r, int c) {
    QPushButton* btn = new QPushButton();
    btn->setFixedSize(180, 130);
    btn->setStyleSheet(
        "QPushButton { "
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #ffffff, stop:1 #f5f5f4); "
        "  border: 1px solid #d6d3d1; "
        "  border-radius: 12px; "
        "  border-bottom: 3px solid #b45309; "
        "} "
        "QPushButton:hover { "
        "  background: white; "
        "  border: 1px solid #b45309; "
        "  border-bottom: 4px solid #92400e; "
        "}"
    );
    
    QVBoxLayout* l = new QVBoxLayout(btn);
    l->setContentsMargins(15, 20, 15, 20);
    l->setSpacing(10);
    
    QLabel* ic = new QLabel(icon); 
    ic->setStyleSheet("font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #78350f; background: #fef3c7; border: 2px solid #fde68a; border-radius: 25px;");
    ic->setFixedSize(50, 50);
    ic->setAlignment(Qt::AlignCenter);
    
    QLabel* t = new QLabel(title); 
    t->setStyleSheet("font-weight: bold; font-size: 14px; color: #292524; border: none; background: transparent;");
    t->setAlignment(Qt::AlignCenter);
    t->setWordWrap(true);
    
    l->addWidget(ic); 
    l->addWidget(t);
    
    QObject::connect(btn, &QPushButton::clicked, func);
    grid->addWidget(btn, r, c);
}

QWidget* QtMainWindow::createMembriPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40,20,40,20); layout->setSpacing(20);
    QLabel* tMain = new QLabel("Gestiune Utilizatori", page);
    tMain->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524;");
    layout->addWidget(tMain);

    QGridLayout* grid = new QGridLayout();
    grid->setSpacing(20);
    addGridCard(grid, "Adaugă", "A", [this](){ QtFunc::adaugaUtilizator(this, m_bib); }, 0, 0);
    addGridCard(grid, "Afișează Membri", "M", [this](){ QtFunc::afiseazaUtilizatori(this, m_bib); }, 0, 1);
    addGridCard(grid, "Șterge Utilizator", "X", [this](){ QtFunc::stergeUtilizator(this, m_bib, m_currentUser->getId()); }, 0, 2);
    addGridCard(grid, "Modifică Utilizator", "E", [this](){ QtFunc::modificaUtilizator(this, m_bib, m_currentUser->getId()); }, 1, 0);
    addGridCard(grid, "Restaurează", "R", [this](){ QtFunc::restaureazaUtilizator(this, m_bib, m_currentUser->getId()); }, 1, 1);
    
    layout->addLayout(grid); layout->addStretch(); return page;
}

QWidget* QtMainWindow::createImprumuturiPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40,20,40,20); layout->setSpacing(20);
    QLabel* tMain = new QLabel("Fișe de Împrumut și Retur", page);
    tMain->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524;");
    layout->addWidget(tMain);

    QGridLayout* grid = new QGridLayout(); grid->setSpacing(20);
    addGridCard(grid, "Împrumută Carte", "I", [this](){ QtFunc::imprumutaCarte(this, m_bib); }, 0, 0);
    addGridCard(grid, "Returnează Carte", "R", [this](){ QtFunc::returneazaCarte(this, m_bib, m_currentUser->getId()); }, 0, 1);
    addGridCard(grid, "Istoric General", "H", [this](){ QtFunc::toateImprumuturile(this, m_bib); }, 0, 2);
    addGridCard(grid, "Confirmă Returnări", "V", [this](){ QtFunc::confirmaReturnari(this, m_bib, m_currentUser->getId()); }, 1, 0);
    addGridCard(grid, "Confirmă Rezervări", "V", [this](){ QtFunc::confirmaRidicarRezervare(this, m_bib, m_currentUser->getId()); }, 1, 1);
    
    layout->addLayout(grid); layout->addStretch(); return page;
}

QWidget* QtMainWindow::createFinantePage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40,20,40,20); layout->setSpacing(20);
    QLabel* tMain = new QLabel("Administrație și Finanțe", page);
    tMain->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524;");
    layout->addWidget(tMain);

    QGridLayout* grid = new QGridLayout(); grid->setSpacing(20);
    addGridCard(grid, "Stare Financiară", "F", [this](){ QMessageBox::information(this, "Buget", QString("Buget: %1 RON\nSalarii: %2 RON").arg(m_bib.getBuget()).arg(m_bib.calculeazaSalariiTotale())); }, 0, 0);
    addGridCard(grid, "Achiziții Noi", "C", [this](){ QtFunc::achizitii(this, m_bib, m_currentUser->getId()); }, 0, 1);
    addGridCard(grid, "Confirmă Plăți", "P", [this](){ QtFunc::confirmaPlati(this, m_bib, m_currentUser->getId()); }, 0, 2);
    addGridCard(grid, "Casare Cărți", "D", [this](){ QtFunc::casareCarte(this, m_bib, m_currentUser->getId()); }, 1, 0);
    addGridCard(grid, "Gestiune Rafturi", "R", [this](){ QtFunc::gestiuneRafturi(this, m_bib); }, 1, 1);
    addGridCard(grid, "Simulare Timp", "T", [this](){ QtFunc::simuleazaTimp(this, m_bib); }, 1, 2);
    addGridCard(grid, "Plată Salarii", "S", [this](){ QtFunc::platesteSalarii(this, m_bib); }, 2, 0);
    
    layout->addLayout(grid); layout->addStretch(); return page;
}

QWidget* QtMainWindow::createProfilCititorPage() {
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; } QWidget#CititorContainer { background-color: transparent; }");

    QWidget* container = new QWidget();
    container->setObjectName("CititorContainer");
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(40, 20, 40, 40);
    layout->setSpacing(20);

    QFrame* welcomeFrame = new QFrame(container);
    welcomeFrame->setStyleSheet("border-bottom: 2px solid #e5e5e5;");
    QHBoxLayout* wLayout = new QHBoxLayout(welcomeFrame);
    wLayout->setContentsMargins(0,0,0,16);
    
    QVBoxLayout* texts = new QVBoxLayout();
    QLabel* wTitle = new QLabel("Bine ai revenit, " + QString::fromStdString(m_currentUser->getNumeComplet()) + "!", welcomeFrame);
    wTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #231a16; border:none;");
    QLabel* wSub = new QLabel("\"O carte bună este un prieten bun.\" — Să vedem ce ai citit.", welcomeFrame);
    wSub->setStyleSheet("font-family: 'Georgia', serif; font-size: 16px; font-style: italic; color: #78716c; border:none;");
    texts->addWidget(wTitle);
    texts->addWidget(wSub);
    
    QLabel* wDate = new QLabel("Data: " + QString::fromStdString(m_bib.getDataCurentaStr()), welcomeFrame);
    wDate->setStyleSheet("background-color: #e0f2fe; color: #0369a1; padding: 10px 20px; border-radius: 6px; border: 1px solid #bae6fd; font-family: 'Georgia', serif; font-weight: bold;");
    
    wLayout->addLayout(texts);
    wLayout->addStretch();
    wLayout->addWidget(wDate, 0, Qt::AlignBottom);
    layout->addWidget(welcomeFrame);

    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(20);
    
    auto createStatCard = [](const QString& title, const QString& val, const QString& icon, const QString& bgCol, const QString& textCol, const QString& borderCol) {
        QFrame* card = new QFrame();
        card->setStyleSheet(QString("QFrame { background-color: white; border: 1px solid #e5e5e5; border-top: 4px solid %1; border-radius: 6px; }").arg(borderCol));
        QVBoxLayout* l = new QVBoxLayout(card);
        l->setContentsMargins(24, 24, 24, 24);
        
        QHBoxLayout* hl = new QHBoxLayout();
        QLabel* ic = new QLabel(icon);
        ic->setStyleSheet(QString("font-size: 18px; font-weight:bold; background-color: %1; color: %2; border-radius: 20px; border:none; padding: 8px;").arg(bgCol).arg(textCol));
        ic->setFixedSize(40, 40);
        ic->setAlignment(Qt::AlignCenter);
        
        QLabel* badge = new QLabel("SITUAȚIA TA");
        badge->setStyleSheet("font-size: 10px; font-weight: bold; color: #a8a29e; background-color: #f5f5f4; border:none; padding: 4px 8px; border-radius: 4px;");
        hl->addWidget(ic);
        hl->addStretch();
        hl->addWidget(badge);
        
        QLabel* vLabel = new QLabel(val);
        vLabel->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524; border:none; margin-top: 10px;");
        
        QLabel* tLabel = new QLabel(title);
        tLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #78716c; border:none; text-transform: uppercase; letter-spacing: 1px;");
        
        l->addLayout(hl);
        l->addWidget(vLabel);
        l->addWidget(tLabel);
        return card;
    };

    size_t active = 0;
    size_t history = 0;
    size_t recenzii = 0;
    double penalizari = m_bib.calculeazaPenalizariTotale(m_currentUser->getId());
    
    for(const auto& i : m_bib.getToateImprumuturile()) if(i.getIdCititor() == m_currentUser->getId()) active++;
    for(const auto& i : m_bib.getToateIstoricLectura()) if(i.id_cititor == m_currentUser->getId()) history++;
    for(const auto& r : m_bib.getToateRecenziile()) if(r.id_cititor == m_currentUser->getId()) recenzii++;

    statsGrid->addWidget(createStatCard("Împrumuturi Active", QString::number(active), "A", "#e0f2fe", "#0369a1", "#bae6fd"), 0, 0);
    statsGrid->addWidget(createStatCard("Cărți Citite", QString::number(history), "C", "#d1fae5", "#047857", "#a7f3d0"), 0, 1);
    statsGrid->addWidget(createStatCard("Recenzii Scrise", QString::number(recenzii), "R", "#fef3c7", "#b45309", "#fde68a"), 0, 2);
    statsGrid->addWidget(createStatCard("Penalizări (RON)", QString::number(penalizari), "!", "#ffe4e6", "#be123c", "#fecdd3"), 0, 3);
    layout->addLayout(statsGrid);

    QLabel* actsTitle = new QLabel("Acțiuni Rapide", container);
    actsTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #292524; margin-top: 20px;");
    layout->addWidget(actsTitle);

    QGridLayout* grid = new QGridLayout(); grid->setSpacing(20);
    addGridCard(grid, "Împrumut Nou", "I", [this](){ QtFunc::imprumutaCarteCititor(this, m_bib, m_currentUser->getId()); }, 0, 0);
    addGridCard(grid, "Returnează", "R", [this](){ QtFunc::returneazaCarteCititor(this, m_bib, m_currentUser->getId()); }, 0, 1);
    addGridCard(grid, "Cărțile Mele", "C", [this](){ QtFunc::veziImprumuturileMele(this, m_bib, m_currentUser->getId()); }, 0, 2);
    addGridCard(grid, "Istoric Lectură", "H", [this](){ QtFunc::veziIstoricLectura(this, m_bib, m_currentUser->getId()); }, 0, 3);
    
    addGridCard(grid, "Rezervă Carte", "Z", [this](){ QtFunc::rezervaCarte(this, m_bib, m_currentUser->getId()); }, 1, 0);
    addGridCard(grid, "Rezervări Cărți", "V", [this](){ QtFunc::veziRezervariCartiMele(this, m_bib, m_currentUser->getId()); }, 1, 1);
    addGridCard(grid, "Rezervă Sală", "S", [this](){ QtFunc::rezervaSala(this, m_bib, m_currentUser->getId(), m_currentUser->getNumeComplet()); }, 1, 2);
    addGridCard(grid, "Sălile Mele", "L", [this](){ QtFunc::veziRezervariSaliMele(this, m_bib, m_currentUser->getId()); }, 1, 3);
    
    addGridCard(grid, "Scrie Recenzie", "O", [this](){ QtFunc::scrieRecenzie(this, m_bib, m_currentUser->getId(), m_currentUser->getNumeComplet()); }, 2, 0);
    addGridCard(grid, "Recenziile Mele", "M", [this](){ QtFunc::veziRecenziileMele(this, m_bib, m_currentUser->getId()); }, 2, 1);
    addGridCard(grid, "Plătește Amenzi", "P", [this](){ QtFunc::solicitaPlata(this, m_bib, m_currentUser); }, 2, 2);
    addGridCard(grid, "Modifică Profil", "U", [this](){ QtFunc::modificaProfil(this, m_bib, m_currentUser->getId()); }, 2, 3);
    
    layout->addLayout(grid);
    layout->addStretch();
    scroll->setWidget(container);
    return scroll;
}

QWidget* QtMainWindow::createSarciniIngrijitorPage() {
    QWidget* page = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(page);
    layout->setContentsMargins(40,20,40,20); layout->setSpacing(20);
    QLabel* tMain = new QLabel("Sarcini Mentenanță", page);
    tMain->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524;");
    layout->addWidget(tMain);

    QGridLayout* grid = new QGridLayout(); grid->setSpacing(20);
    addGridCard(grid, "Recondiționare", "R", [this](){ QtFunc::reconditioneazaCarti(this, m_bib); }, 0, 0);
    addGridCard(grid, "Mutare Cărți", "M", [this](){ QtFunc::mutaCarteFizica(this, m_bib); }, 0, 1);
    addGridCard(grid, "Profilul Meu", "U", [this](){ QtFunc::modificaProfil(this, m_bib, m_currentUser->getId()); }, 0, 2);
    
    layout->addLayout(grid); layout->addStretch(); return page;
}

// -------------------------------------------------------------
// PAGINILE COMPLEXE (Dashboard si Catalog)
// -------------------------------------------------------------
QWidget* QtMainWindow::createDashboardPage() {
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; } QWidget#DashContainer { background-color: transparent; }");

    QWidget* container = new QWidget();
    container->setObjectName("DashContainer");
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(40, 20, 40, 40);
    layout->setSpacing(20);

    QFrame* welcomeFrame = new QFrame(container);
    welcomeFrame->setStyleSheet("border-bottom: 2px solid #e5e5e5;");
    QHBoxLayout* wLayout = new QHBoxLayout(welcomeFrame);
    wLayout->setContentsMargins(0,0,0,16);
    
    QVBoxLayout* texts = new QVBoxLayout();
    QLabel* wTitle = new QLabel("Bine ați revenit în sala de lectură.", welcomeFrame);
    wTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #231a16; border:none;");
    QLabel* wSub = new QLabel("\"O bibliotecă este o cameră cu prieteni.\" — Iată starea lor astăzi.", welcomeFrame);
    wSub->setStyleSheet("font-family: 'Georgia', serif; font-size: 16px; font-style: italic; color: #78716c; border:none;");
    texts->addWidget(wTitle);
    texts->addWidget(wSub);
    
    std::string vDateStr = m_bib.getDataCurentaStr();
    QDate qDate = QDate::fromString(QString::fromStdString(vDateStr), "dd/MM/yyyy");
    QString displayDate = qDate.isValid() ? qDate.toString("dddd, dd MMMM yyyy") : QString::fromStdString(vDateStr);
    
    QLabel* wDate = new QLabel(displayDate, welcomeFrame);
    wDate->setStyleSheet("background-color: #fef3c7; color: #78350f; padding: 10px 20px; border-radius: 6px; border: 1px solid #fde68a; font-family: 'Georgia', serif; font-weight: bold;");
    
    wLayout->addLayout(texts);
    wLayout->addStretch();
    wLayout->addWidget(wDate, 0, Qt::AlignBottom);
    layout->addWidget(welcomeFrame);

    QGridLayout* statsGrid = new QGridLayout();
    statsGrid->setSpacing(20);
    
    auto createStatCard = [](const QString& title, const QString& val, const QString& icon, const QString& bgCol, const QString& textCol, const QString& borderCol) {
        QFrame* card = new QFrame();
        card->setStyleSheet(QString("QFrame { background-color: white; border: 1px solid #e5e5e5; border-top: 4px solid %1; border-radius: 6px; }").arg(borderCol));
        QVBoxLayout* l = new QVBoxLayout(card);
        l->setContentsMargins(24, 24, 24, 24);
        
        QHBoxLayout* hl = new QHBoxLayout();
        QLabel* ic = new QLabel(icon);
        ic->setStyleSheet(QString("font-size: 18px; font-weight:bold; background-color: %1; color: %2; border-radius: 20px; border:none; padding: 8px;").arg(bgCol).arg(textCol));
        ic->setFixedSize(40, 40);
        ic->setAlignment(Qt::AlignCenter);
        
        QLabel* badge = new QLabel("SITUAȚIE LA ZI");
        badge->setStyleSheet("font-size: 10px; font-weight: bold; color: #a8a29e; background-color: #f5f5f4; border:none; padding: 4px 8px; border-radius: 4px;");
        hl->addWidget(ic);
        hl->addStretch();
        hl->addWidget(badge);
        
        QLabel* vLabel = new QLabel(val);
        vLabel->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524; border:none; margin-top: 10px;");
        
        QLabel* tLabel = new QLabel(title);
        tLabel->setStyleSheet("font-size: 12px; font-weight: bold; color: #78716c; border:none; text-transform: uppercase; letter-spacing: 1px;");
        
        l->addLayout(hl);
        l->addWidget(vLabel);
        l->addWidget(tLabel);
        return card;
    };

    size_t nrCarti = m_bib.getToateCartile().size();
    size_t nrUseri = m_bib.getTotiUtilizatorii().size();
    size_t nrImp = m_bib.getToateImprumuturile().size();
    
    statsGrid->addWidget(createStatCard("Volume în Colecție", QString::number(nrCarti), "V", "#fef3c7", "#b45309", "#fde68a"), 0, 0);
    statsGrid->addWidget(createStatCard("Cititori Înregistrați", QString::number(nrUseri), "U", "#d1fae5", "#047857", "#a7f3d0"), 0, 1);
    statsGrid->addWidget(createStatCard("Împrumuturi Active", QString::number(nrImp), "I", "#e0f2fe", "#0369a1", "#bae6fd"), 0, 2);
    statsGrid->addWidget(createStatCard("Restanțe (Atenție)", "0", "!", "#ffe4e6", "#be123c", "#fecdd3"), 0, 3);

    layout->addLayout(statsGrid);

    QHBoxLayout* botLayout = new QHBoxLayout();
    botLayout->setSpacing(30);

    QFrame* ledger = new QFrame();
    ledger->setStyleSheet("QFrame { background-color: white; border: 1px solid #e5e5e5; border-radius: 6px; }");
    QVBoxLayout* ll = new QVBoxLayout(ledger);
    ll->setContentsMargins(30,30,30,30);
    
    QLabel* lTitle = new QLabel("Registrul Zilei", ledger);
    lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #292524; border:none; margin-bottom: 20px;");
    ll->addWidget(lTitle);
    
    auto addActivity = [ll](const QString& icon, const QString& title, const QString& desc, const QString& time) {
        QHBoxLayout* hl = new QHBoxLayout();
        QLabel* ic = new QLabel(icon); ic->setStyleSheet("font-size: 20px; font-weight:bold; color:#78716c; border:none; background:transparent;");
        QVBoxLayout* vl = new QVBoxLayout();
        QLabel* t = new QLabel(title); t->setStyleSheet("font-family: 'Georgia', serif; font-size: 16px; font-weight: bold; color: #292524; border:none;");
        QLabel* d = new QLabel(desc); d->setStyleSheet("font-size: 14px; color: #57534e; border:none;");
        QLabel* ti = new QLabel(time); ti->setStyleSheet("font-family: monospace; font-size: 12px; color: #a8a29e; background: #f5f5f4; border-radius:4px; padding:2px 4px; border:none;");
        vl->addWidget(t); vl->addWidget(d); vl->addWidget(ti);
        hl->addWidget(ic, 0, Qt::AlignTop); hl->addLayout(vl); ll->addLayout(hl); ll->addSpacing(15);
    };

    auto logs = m_bib.getJurnalAudit(5);
    if(logs.empty()) {
        addActivity("[*]", "Sistem inițializat", "Aplicația Athenaeum a fost pornită cu succes.", "Acum");
    } else {
        for(const auto& log : logs) {
            QString icon = "[*]";
            QString titlu = QString::fromStdString(log.actiune);
            if(titlu.length() > 40) titlu = titlu.left(40) + "..."; // scurtam pt titlu
            addActivity(icon, titlu, QString::fromStdString("Actor: " + log.id_utilizator), QString::fromStdString(log.data_ora));
        }
    }
    
    ll->addStretch();
    botLayout->addWidget(ledger, 2);

    QVBoxLayout* sideLayout = new QVBoxLayout();
    QFrame* quoteCard = new QFrame();
    quoteCard->setStyleSheet("QFrame { background-color: #f9f6f0; border: 1px solid #e8ddcb; border-radius: 6px; }");
    QVBoxLayout* ql = new QVBoxLayout(quoteCard);
    ql->setContentsMargins(30,30,30,30);
    
    std::vector<std::pair<QString, QString>> quotes = {
        {"\"Cărțile sunt albinele care poartă polenul însuflețitor de la o minte la alta.\"", "— James Russell Lowell"},
        {"\"O bibliotecă este o cameră cu prieteni.\"", "— Tacitus"},
        {"\"O cameră fără cărți este ca un corp fără suflet.\"", "— Cicero"},
        {"\"Citind, nu cauți o idee nouă, ci te cauți pe tine însuți.\"", "— Emil Cioran"},
        {"\"Până și cea mai mică bibliotecă conține mai mult decât un om poate ști.\"", "— Anonim"}
    };
    int qIdx = std::rand() % quotes.size();
    
    QLabel* qLabel = new QLabel(quotes[qIdx].first, quoteCard);
    qLabel->setStyleSheet("font-family: 'Georgia', serif; font-size: 18px; font-style: italic; color: #292524; border:none;");
    qLabel->setWordWrap(true);
    QLabel* qAuth = new QLabel(quotes[qIdx].second, quoteCard);
    qAuth->setStyleSheet("font-size: 14px; font-weight: bold; color: #78716c; border:none; margin-top:10px;");
    ql->addWidget(qLabel); ql->addWidget(qAuth);
    
    QFrame* alertCard = new QFrame();
    alertCard->setStyleSheet("QFrame { background-color: #fff1f2; border: 1px solid #fecdd3; border-radius: 6px; }");
    QVBoxLayout* al = new QVBoxLayout(alertCard);
    al->setContentsMargins(20,20,20,20);
    QLabel* aTitle = new QLabel("[!] Atenționare Restanțe", alertCard);
    aTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 18px; font-weight: bold; color: #881337; border:none;");
    QLabel* aDesc = new QLabel("Verificați situația returnărilor. Unele volume pot fi întârziate.", alertCard);
    aDesc->setStyleSheet("font-size: 14px; color: #be123c; border:none; margin-top:10px;");
    aDesc->setWordWrap(true);
    al->addWidget(aTitle); al->addWidget(aDesc);
    
    sideLayout->addWidget(quoteCard); sideLayout->addWidget(alertCard); sideLayout->addStretch();
    botLayout->addLayout(sideLayout, 1);
    layout->addLayout(botLayout);

    scroll->setWidget(container);
    return scroll;
}

QWidget* QtMainWindow::createCatalogPage() {
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background-color: transparent; } QWidget#CatContainer { background-color: transparent; }");

    QWidget* container = new QWidget();
    container->setObjectName("CatContainer");
    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(40, 20, 40, 40);
    layout->setSpacing(20);

    QHBoxLayout* top = new QHBoxLayout();
    QVBoxLayout* titleBox = new QVBoxLayout();
    QLabel* tMain = new QLabel("Catalogul Cărților", container);
    tMain->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #292524;");
    QLabel* tSub = new QLabel("Explorează volumele din arhivă, adaugă manuscrise noi sau verifică disponibilitatea.", container);
    tSub->setStyleSheet("font-family: 'Georgia', serif; font-size: 16px; font-style: italic; color: #78716c;");
    titleBox->addWidget(tMain); titleBox->addWidget(tSub);
    
    QPushButton* btnCauta = new QPushButton("Caută Avansat", container);
    btnCauta->setStyleSheet("QPushButton { background-color: white; border: 2px solid #e5e5e5; color: #292524; border-radius: 6px; padding: 10px 20px; font-weight: bold; } QPushButton:hover { background-color: #fafaf9; border-color:#b45309; }");
    connect(btnCauta, &QPushButton::clicked, [this](){ QtFunc::cautaCarte(this, m_bib, m_currentUser); });
    
    top->addLayout(titleBox);
    top->addStretch();
    top->addWidget(btnCauta);

    if(m_currentUser->getTip() == "DIRECTOR" || m_currentUser->getTip() == "BIBLIOTECAR") {
        QPushButton* btnAddFizic = new QPushButton("[+] Carte Fizică", container);
        btnAddFizic->setStyleSheet("QPushButton { background-color: #b45309; color: white; border-radius: 6px; padding: 10px 15px; font-weight: bold; border-bottom: 3px solid #78350f; } QPushButton:hover { background-color: #92400e; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnAddFizic, &QPushButton::clicked, [this](){ QtFunc::adaugaCarteFizica(this, m_bib); });
        
        QPushButton* btnAddDigital = new QPushButton("[+] Carte Digitală", container);
        btnAddDigital->setStyleSheet("QPushButton { background-color: #0284c7; color: white; border-radius: 6px; padding: 10px 15px; font-weight: bold; border-bottom: 3px solid #0369a1; } QPushButton:hover { background-color: #0369a1; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnAddDigital, &QPushButton::clicked, [this](){ QtFunc::adaugaCarteDigitala(this, m_bib); });

        QPushButton* btnAddAudio = new QPushButton("[+] Audiobook", container);
        btnAddAudio->setStyleSheet("QPushButton { background-color: #16a34a; color: white; border-radius: 6px; padding: 10px 15px; font-weight: bold; border-bottom: 3px solid #15803d; } QPushButton:hover { background-color: #15803d; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnAddAudio, &QPushButton::clicked, [this](){ QtFunc::adaugaCarteAudio(this, m_bib); });
        
        top->addWidget(btnAddFizic);
        top->addWidget(btnAddDigital);
        top->addWidget(btnAddAudio);
    }
    
    layout->addLayout(top);

    if(m_currentUser->getTip() == "DIRECTOR" || m_currentUser->getTip() == "BIBLIOTECAR") {
        QHBoxLayout* actions2 = new QHBoxLayout();
        actions2->addStretch();
        
        QPushButton* btnModifica = new QPushButton("[~] Modifică Carte", container);
        btnModifica->setStyleSheet("QPushButton { background-color: #fbbf24; color: #78350f; border-radius: 6px; padding: 6px 12px; font-weight: bold; border-bottom: 3px solid #d97706; } QPushButton:hover { background-color: #f59e0b; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnModifica, &QPushButton::clicked, [this](){ QtFunc::modificaCarte(this, m_bib, m_currentUser->getId()); });

        QPushButton* btnSterge = new QPushButton("[x] Șterge Carte", container);
        btnSterge->setStyleSheet("QPushButton { background-color: #ef4444; color: white; border-radius: 6px; padding: 6px 12px; font-weight: bold; border-bottom: 3px solid #b91c1c; } QPushButton:hover { background-color: #dc2626; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnSterge, &QPushButton::clicked, [this](){ QtFunc::stergeCarte(this, m_bib, m_currentUser->getId()); });

        QPushButton* btnRestaureaza = new QPushButton("[o] Restaurează Carte", container);
        btnRestaureaza->setStyleSheet("QPushButton { background-color: #64748b; color: white; border-radius: 6px; padding: 6px 12px; font-weight: bold; border-bottom: 3px solid #475569; } QPushButton:hover { background-color: #475569; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnRestaureaza, &QPushButton::clicked, [this](){ QtFunc::restaureazaCarte(this, m_bib, m_currentUser->getId()); });

        QPushButton* btnRafturi = new QPushButton("[=] Rafturi", container);
        btnRafturi->setStyleSheet("QPushButton { background-color: #8b5cf6; color: white; border-radius: 6px; padding: 6px 12px; font-weight: bold; border-bottom: 3px solid #6d28d9; } QPushButton:hover { background-color: #7c3aed; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnRafturi, &QPushButton::clicked, [this](){ QtFunc::gestiuneRafturi(this, m_bib); });
        
        QPushButton* btnMutare = new QPushButton("[->] Mutare Cărți", container);
        btnMutare->setStyleSheet("QPushButton { background-color: #14b8a6; color: white; border-radius: 6px; padding: 6px 12px; font-weight: bold; border-bottom: 3px solid #0f766e; } QPushButton:hover { background-color: #0d9488; } QPushButton:pressed { border-bottom: 0px; margin-top: 3px; }");
        connect(btnMutare, &QPushButton::clicked, [this](){ QtFunc::mutaCarteFizica(this, m_bib); });

        actions2->addWidget(btnModifica);
        actions2->addWidget(btnSterge);
        actions2->addWidget(btnRestaureaza);
        actions2->addWidget(btnRafturi);
        actions2->addWidget(btnMutare);
        
        layout->addLayout(actions2);
    }

    layout->addSpacing(10);
    
    // Filtrare UI
    QHBoxLayout* filterLayout = new QHBoxLayout();
    
    QComboBox* comboCateg = new QComboBox(container);
    std::set<std::string> uniqueCategories;
    for (auto& c : m_bib.getToateCartile()) {
        uniqueCategories.insert(c->getCategorie());
    }
    comboCateg->addItem("Toate Categoriile");
    for (const auto& cat : uniqueCategories) {
        comboCateg->addItem(QString::fromStdString(cat));
    }
    comboCateg->setStyleSheet("padding: 8px; border: 1px solid #d6d3d1; border-radius: 4px;");
    
    QCheckBox* chkStoc = new QCheckBox("Doar Disponibile", container);
    chkStoc->setStyleSheet("font-weight: bold; color: #57534e;");
    
    QLineEdit* searchEdit = new QLineEdit(container);
    searchEdit->setPlaceholderText("Caută după titlu sau autor...");
    searchEdit->setStyleSheet("padding: 8px; border: 1px solid #d6d3d1; border-radius: 4px;");
    
    QPushButton* btnFiltreaza = new QPushButton("Filtrează", container);
    btnFiltreaza->setStyleSheet("QPushButton { background-color: #292524; color: white; border-radius: 4px; padding: 8px 15px; font-weight: bold; } QPushButton:hover { background-color: #44403c; }");
    
    connect(btnFiltreaza, &QPushButton::clicked, [this, comboCateg, chkStoc, searchEdit](){
        applyCatalogFilters(comboCateg->currentText(), chkStoc->isChecked(), searchEdit->text());
    });
    
    filterLayout->addWidget(comboCateg);
    filterLayout->addWidget(chkStoc);
    filterLayout->addWidget(searchEdit);
    filterLayout->addWidget(btnFiltreaza);
    
    layout->addLayout(filterLayout);
    layout->addSpacing(10);

    // Grid Container
    m_catalogGridContainer = new QWidget(container);
    m_catalogGridLayout = new QGridLayout(m_catalogGridContainer);
    m_catalogGridLayout->setSpacing(20);
    layout->addWidget(m_catalogGridContainer);
    
    // Pagination UI
    QHBoxLayout* pagLayout = new QHBoxLayout();
    pagLayout->addStretch();
    
    m_btnPrev = new QPushButton("< Pagina Anterioară", container);
    m_btnPrev->setStyleSheet("QPushButton { background-color: #e5e5e5; color: #292524; border-radius: 4px; padding: 6px 12px; font-weight: bold; } QPushButton:hover { background-color: #d4d4d4; }");
    m_btnPrev->setEnabled(false);
    
    m_pageLabel = new QLabel("Pagina 1", container);
    m_pageLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 0 15px;");
    
    m_btnNext = new QPushButton("Pagina Următoare >", container);
    m_btnNext->setStyleSheet("QPushButton { background-color: #e5e5e5; color: #292524; border-radius: 4px; padding: 6px 12px; font-weight: bold; } QPushButton:hover { background-color: #d4d4d4; }");
    
    pagLayout->addWidget(m_btnPrev);
    pagLayout->addWidget(m_pageLabel);
    pagLayout->addWidget(m_btnNext);
    pagLayout->addStretch();
    
    connect(m_btnPrev, &QPushButton::clicked, [this](){
        if(m_catalogPage > 0) { m_catalogPage--; refreshCatalogGrid(); }
    });
    connect(m_btnNext, &QPushButton::clicked, [this](){
        if((m_catalogPage + 1) * 12 < m_currentCatalogData.size()) { m_catalogPage++; refreshCatalogGrid(); }
    });
    
    layout->addLayout(pagLayout);
    layout->addStretch();

    scroll->setWidget(container);
    
    m_currentCatalogData = m_bib.getToateCartile();
    m_catalogPage = 0;
    refreshCatalogGrid();
    
    return scroll;
}

void QtMainWindow::applyCatalogFilters(const QString& category, bool inStock, const QString& searchTxt) {
    auto toate = m_bib.getToateCartile();
    m_currentCatalogData.clear();
    
    QString qSearch = searchTxt.toLower();
    
    for(auto& c : toate) {
        if(inStock && c->getStocDisponibil() <= 0) continue;
        
        if(category != "Toate Categoriile") {
            if(QString::fromStdString(c->getCategorie()) != category) continue;
        }
        
        if(!qSearch.isEmpty()) {
            QString titlu = QString::fromStdString(c->getTitlu()).toLower();
            QString autor = QString::fromStdString(c->autoriToString()).toLower();
            if(!titlu.contains(qSearch) && !autor.contains(qSearch)) continue;
        }
        
        m_currentCatalogData.push_back(c);
    }
    
    m_catalogPage = 0;
    refreshCatalogGrid();
}

void QtMainWindow::refreshCatalogGrid() {
    if(!m_catalogGridLayout) return;
    
    QLayoutItem *child;
    while ((child = m_catalogGridLayout->takeAt(0)) != nullptr) {
        if(child->widget()) delete child->widget();
        delete child;
    }
    
    int cartiPePagina = 12;
    size_t startIdx = m_catalogPage * cartiPePagina;
    size_t endIdx = std::min(startIdx + cartiPePagina, m_currentCatalogData.size());
    
    int row = 0, col = 0;
    QStringList colors = {"#292524", "#0c4a6e", "#78350f", "#064e3b", "#881337", "#312e81"};
    
    if(m_currentCatalogData.empty()) {
        QLabel* lEmpty = new QLabel("Nu s-au găsit cărți pentru filtrele selectate.");
        lEmpty->setStyleSheet("font-size: 16px; color: #78716c; font-style: italic;");
        m_catalogGridLayout->addWidget(lEmpty, 0, 0);
    } else {
        for (size_t i = startIdx; i < endIdx; ++i) { 
            auto c = m_currentCatalogData[i];
            QFrame* cCard = new QFrame();
            cCard->setFixedSize(220, 320); 
            cCard->setStyleSheet(QString("QFrame { background-color: %1; border-radius: 4px; border-left: 6px solid rgba(0,0,0,0.3); }").arg(colors[i % colors.size()]));
            
            QVBoxLayout* cl = new QVBoxLayout(cCard);
            cl->setContentsMargins(16,16,16,16);
            
            QLabel* lAuth = new QLabel(QString::fromStdString(c->autoriToString()), cCard);
            lAuth->setStyleSheet("font-family: 'Georgia', serif; font-size: 11px; font-weight: bold; color: rgba(255,255,255,0.7); text-transform: uppercase; border-bottom: 1px solid rgba(255,255,255,0.2); padding-bottom: 4px; border-left:none; background:transparent;");
            lAuth->setWordWrap(true);
            
            QLabel* lTitle = new QLabel(QString::fromStdString(c->getTitlu()), cCard);
            lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 20px; font-weight: bold; color: white; border:none; background:transparent;");
            lTitle->setWordWrap(true);
            
            QLabel* lId = new QLabel(QString::fromStdString(c->getIsbn()), cCard);
            lId->setStyleSheet("font-family: monospace; font-size: 10px; color: rgba(255,255,255,0.4); border:none; background:transparent;");
            
            QPushButton* btnDet = new QPushButton("Află Detalii", cCard);
            btnDet->setStyleSheet("QPushButton { background-color: rgba(255,255,255,0.2); color: white; border-radius: 4px; padding: 4px; font-weight:bold; border:none; } QPushButton:hover { background-color: rgba(255,255,255,0.3); }");
            
            connect(btnDet, &QPushButton::clicked, [this, c](){
                DetaliiCarteDialog dlg(c, m_bib, m_currentUser, this);
                dlg.exec();
            });

            cl->addWidget(lAuth); cl->addWidget(lTitle); cl->addStretch(); cl->addWidget(lId, 0, Qt::AlignBottom | Qt::AlignLeft);
            cl->addWidget(btnDet);
            m_catalogGridLayout->addWidget(cCard, row, col);
            
            col++; if (col >= 4) { col = 0; row++; }
        }
    }
    
    int totalPagini = (m_currentCatalogData.size() + cartiPePagina - 1) / cartiPePagina;
    if(totalPagini == 0) totalPagini = 1;
    
    m_pageLabel->setText(QString("Pagina %1 din %2").arg(m_catalogPage + 1).arg(totalPagini));
    m_btnPrev->setEnabled(m_catalogPage > 0);
    m_btnNext->setEnabled(m_catalogPage < totalPagini - 1);
}

void QtMainWindow::switchPage(int index) {
    if (index >= 0 && index < m_appPagesWidget->count()) {
        m_appPagesWidget->setCurrentIndex(index);
    }
}

void QtMainWindow::onLoginClicked() {
    std::string id = m_idInput->text().toStdString();
    std::string p = m_passInput->text().toStdString();

    m_currentUser = m_bib.autentificare(id, p);

    if (!m_currentUser) {
        QMessageBox::critical(this, "Eroare", "Autentificare eșuată! ID sau parolă incorectă.");
        return;
    }

    buildDynamicAppView();
    m_rootStackedWidget->setCurrentWidget(m_appWidget);
}

void QtMainWindow::onLogoutClicked() {
    m_currentUser = nullptr;
    m_idInput->clear();
    m_passInput->clear();
    m_rootStackedWidget->setCurrentWidget(m_loginWidget);
}
