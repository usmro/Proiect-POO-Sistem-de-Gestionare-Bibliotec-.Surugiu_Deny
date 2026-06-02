#include "QtDialogs.h"
#include "CarteFizica.h"
#include "Cititor.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QInputDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDateTime>
#include <QCheckBox>
#include <QPushButton>
#include <QScrollArea>
#include <QListWidget>

#include <random>
#include <ctime>
#include <QDateTime>

// Generatoare identice cu cele din consola
static std::string generateISBN_Qt() {
    static std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dis(1000000, 9999999);
    return "978-973-" + std::to_string(dis(gen));
}

static std::string generateUserId_Qt(const std::string& prefix) {
    static std::mt19937 gen(time(nullptr));
    std::uniform_int_distribution<> dis(100, 999);
    return prefix + std::to_string(dis(gen));
}

// ---------------------------------------------------------
// Dialog Adăugare Carte Fizică
// ---------------------------------------------------------
AdaugaCarteFizicaDialog::AdaugaCarteFizicaDialog(Biblioteca& bib, QWidget* parent) 
    : QDialog(parent), m_bib(bib) 
{
    setWindowTitle("Adaugă Carte Fizică (Rapid)");
    setMinimumWidth(450);

    QFormLayout* formLayout = new QFormLayout();

    m_titluEdit = new QLineEdit(this);
    m_autoriEdit = new QLineEdit(this);
    m_autoriEdit->setPlaceholderText("Autori despărțiți prin virgulă");
    m_edituraEdit = new QLineEdit(this);
    
    int an_curent = QDateTime::currentDateTime().date().year();
    m_anEdit = new QSpinBox(this);
    m_anEdit->setRange(1000, an_curent);
    m_anEdit->setValue(an_curent);

    m_paginiEdit = new QSpinBox(this);
    m_paginiEdit->setRange(1, 10000);
    m_paginiEdit->setValue(100);

    m_pretEdit = new QDoubleSpinBox(this);
    m_pretEdit->setRange(0.01, 10000.0);
    m_pretEdit->setValue(50.0);

    m_stocEdit = new QSpinBox(this);
    m_stocEdit->setRange(0, 1000);
    m_stocEdit->setValue(1);

    m_categorieCombo = new QComboBox(this);
    for (const auto& c : m_bib.getCategoriiDistincte()) {
        m_categorieCombo->addItem(QString::fromStdString(c));
    }

    m_stareCombo = new QComboBox(this);
    m_stareCombo->addItems({"BUNA", "DEFECTA"});

    m_copertaCombo = new QComboBox(this);
    m_copertaCombo->addItems({"Hardcover", "Softcover"});

    m_raftCombo = new QComboBox(this);
    for (const auto& r : m_bib.getToateRafturile()) {
        Locatie temp_loc = {r.cladire, r.camera, r.culoar, r.nume_raft};
        int curent = m_bib.obtineNumarCartiFiziceRaft(temp_loc);
        QString label = QString::fromStdString(r.id) + " (Ocupat: " + QString::number(curent) + "/" + QString::number(r.capacitate_maxima) + ")";
        m_raftCombo->addItem(label, QVariant::fromValue(static_cast<void*>(const_cast<Raft*>(&r))));
    }

    formLayout->addRow("Titlu:", m_titluEdit);
    formLayout->addRow("Autori (,):", m_autoriEdit);
    formLayout->addRow("Editură:", m_edituraEdit);
    formLayout->addRow("An Publicare:", m_anEdit);
    formLayout->addRow("Număr Pagini:", m_paginiEdit);
    formLayout->addRow("Preț (RON):", m_pretEdit);
    formLayout->addRow("Stoc Inițial:", m_stocEdit);
    formLayout->addRow("Categorie:", m_categorieCombo);
    formLayout->addRow("Stare:", m_stareCombo);
    formLayout->addRow("Copertă:", m_copertaCombo);
    formLayout->addRow("Selectează Raft:", m_raftCombo);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("Salvează", this);
    QPushButton* cancelBtn = new QPushButton("Anulează", this);
    
    connect(saveBtn, &QPushButton::clicked, this, &AdaugaCarteFizicaDialog::onSave);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
}

void AdaugaCarteFizicaDialog::onSave() {
    std::string titlu = m_titluEdit->text().toStdString();
    std::string autori_str = m_autoriEdit->text().toStdString();
    std::string editura = m_edituraEdit->text().toStdString();
    int an = m_anEdit->value();
    int pagini = m_paginiEdit->value();
    double pret = m_pretEdit->value();
    int stoc = m_stocEdit->value();
    std::string categorie = m_categorieCombo->currentText().toStdString();
    bool defecta = (m_stareCombo->currentIndex() == 1);
    std::string coperta = m_copertaCombo->currentText().toStdString();

    if (titlu.empty() || autori_str.empty() || editura.empty()) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile text sunt obligatorii!");
        return;
    }

    if (m_raftCombo->count() == 0) {
        QMessageBox::critical(this, "Eroare", "Nu exista rafturi definite in sistem!");
        return;
    }

    Raft* r = static_cast<Raft*>(m_raftCombo->currentData().value<void*>());
    Locatie loc = {r->cladire, r->camera, r->culoar, r->nume_raft};

    if(m_bib.obtineNumarCartiFiziceRaft(loc) + stoc > r->capacitate_maxima) {
        QMessageBox::critical(this, "Eroare", "Raftul selectat nu are suficient spatiu!");
        return;
    }

    std::string isbn = generateISBN_Qt();
    std::string data_add = m_bib.getDataCurentaStr();

    bool ok = m_bib.adaugaCarteFizica(isbn, titlu, Carte::stringToAutori(autori_str), editura, pret, categorie, an, pagini, data_add, 0, stoc, loc, coperta, defecta);
    
    if (ok) {
        m_bib.salveazaDate();
        QMessageBox::information(this, "Succes", QString("Cartea fizică a fost adăugată cu succes!\nISBN: %1").arg(QString::fromStdString(isbn)));
        accept();
    } else {
        QMessageBox::critical(this, "Eroare", "S-a depășit limita de 50 cărți pe raft!");
    }
}

// ---------------------------------------------------------
// Dialog Adăugare Carte Digitală
// ---------------------------------------------------------
AdaugaCarteDigitalaDialog::AdaugaCarteDigitalaDialog(Biblioteca& bib, QWidget* parent) : QDialog(parent), m_bib(bib) {
    setWindowTitle("Adaugă Carte Digitală");
    setMinimumWidth(400);

    QFormLayout* form = new QFormLayout(this);

    m_titluEdit = new QLineEdit(this);
    m_autoriEdit = new QLineEdit(this); m_autoriEdit->setPlaceholderText("Nume Autor 1, Nume Autor 2");
    m_edituraEdit = new QLineEdit(this);

    m_anEdit = new QSpinBox(this); m_anEdit->setRange(1000, 2100); m_anEdit->setValue(2023);
    m_paginiEdit = new QSpinBox(this); m_paginiEdit->setRange(1, 10000);
    m_pretEdit = new QDoubleSpinBox(this); m_pretEdit->setRange(0.0, 10000.0);

    m_categorieCombo = new QComboBox(this);
    for (const auto& c : bib.getCategoriiDistincte()) m_categorieCombo->addItem(QString::fromStdString(c));
    m_categorieCombo->setEditable(true);

    m_formatEdit = new QLineEdit(this); m_formatEdit->setPlaceholderText("ex: PDF, EPUB, MOBI");
    m_linkEdit = new QLineEdit(this); m_linkEdit->setPlaceholderText("https://...");

    form->addRow("Titlu:", m_titluEdit);
    form->addRow("Autori (cu virgulă):", m_autoriEdit);
    form->addRow("Editura:", m_edituraEdit);
    form->addRow("An Apariție:", m_anEdit);
    form->addRow("Nr. Pagini:", m_paginiEdit);
    form->addRow("Preț:", m_pretEdit);
    form->addRow("Categorie:", m_categorieCombo);
    form->addRow("Format Fișier:", m_formatEdit);
    form->addRow("Link Acces:", m_linkEdit);

    QPushButton* saveBtn = new QPushButton("Salvează", this);
    form->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &AdaugaCarteDigitalaDialog::onSave);
}

void AdaugaCarteDigitalaDialog::onSave() {
    std::string titlu = m_titluEdit->text().toStdString();
    std::string autori_str = m_autoriEdit->text().toStdString();
    std::string editura = m_edituraEdit->text().toStdString();
    
    if (titlu.empty() || autori_str.empty() || editura.empty()) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile text sunt obligatorii!");
        return;
    }

    std::vector<std::string> autori = Carte::stringToAutori(autori_str);

    time_t now = time(0);
    tm* ltm = localtime(&now);
    char date_buf[20];
    strftime(date_buf, sizeof(date_buf), "%d/%m/%Y", ltm);
    std::string data_adaugarii = date_buf;

    std::string isbn = generateISBN_Qt();
    
    QString formatStr = m_formatEdit->text().toUpper();
    double sizePerPg = 0.05;
    if(formatStr == "EPUB") sizePerPg = 0.02;
    else if(formatStr == "MOBI") sizePerPg = 0.03;
    double calcDimensiune = m_paginiEdit->value() * sizePerPg;

    m_bib.adaugaCarteDigitala(
        isbn, titlu, autori, editura, m_pretEdit->value(),
        m_categorieCombo->currentText().toStdString(),
        m_anEdit->value(), m_paginiEdit->value(),
        data_adaugarii, 0,
        m_formatEdit->text().toStdString(), calcDimensiune, m_linkEdit->text().toStdString()
    );
    m_bib.salveazaDate();

    QMessageBox::information(this, "Succes", QString("Cartea digitală a fost adăugată!\nISBN: %1\nDimensiune calculată: %2 MB").arg(QString::fromStdString(isbn)).arg(calcDimensiune));
    accept();
}

// ---------------------------------------------------------
// Dialog Adăugare Carte Audio
// ---------------------------------------------------------
AdaugaCarteAudioDialog::AdaugaCarteAudioDialog(Biblioteca& bib, QWidget* parent) : QDialog(parent), m_bib(bib) {
    setWindowTitle("Adaugă Audiobook");
    setMinimumWidth(400);

    QFormLayout* form = new QFormLayout(this);

    m_titluEdit = new QLineEdit(this);
    m_autoriEdit = new QLineEdit(this); m_autoriEdit->setPlaceholderText("Nume Autor 1, Nume Autor 2");
    m_edituraEdit = new QLineEdit(this);

    m_anEdit = new QSpinBox(this); m_anEdit->setRange(1000, 2100); m_anEdit->setValue(2023);
    m_pretEdit = new QDoubleSpinBox(this); m_pretEdit->setRange(0.0, 10000.0);

    m_categorieCombo = new QComboBox(this);
    for (const auto& c : bib.getCategoriiDistincte()) m_categorieCombo->addItem(QString::fromStdString(c));
    m_categorieCombo->setEditable(true);

    m_naratorEdit = new QLineEdit(this);
    m_minuteEdit = new QSpinBox(this); m_minuteEdit->setRange(1, 10000); m_minuteEdit->setSuffix(" minute");
    m_linkEdit = new QLineEdit(this); m_linkEdit->setPlaceholderText("https://...");

    form->addRow("Titlu:", m_titluEdit);
    form->addRow("Autori (cu virgulă):", m_autoriEdit);
    form->addRow("Editura:", m_edituraEdit);
    form->addRow("An Apariție:", m_anEdit);
    form->addRow("Preț:", m_pretEdit);
    form->addRow("Categorie:", m_categorieCombo);
    form->addRow("Narator:", m_naratorEdit);
    form->addRow("Durată (minute):", m_minuteEdit);
    form->addRow("Link Acces:", m_linkEdit);

    QPushButton* saveBtn = new QPushButton("Salvează", this);
    form->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &AdaugaCarteAudioDialog::onSave);
}

void AdaugaCarteAudioDialog::onSave() {
    std::string titlu = m_titluEdit->text().toStdString();
    std::string autori_str = m_autoriEdit->text().toStdString();
    std::string editura = m_edituraEdit->text().toStdString();
    std::string narator = m_naratorEdit->text().toStdString();
    
    if (titlu.empty() || autori_str.empty() || editura.empty() || narator.empty()) {
        QMessageBox::warning(this, "Eroare", "Toate câmpurile text sunt obligatorii!");
        return;
    }

    std::vector<std::string> autori = Carte::stringToAutori(autori_str);
    std::string isbn = generateISBN_Qt();

    m_bib.adaugaCarteAudio(
        isbn, titlu, autori, editura, m_pretEdit->value(),
        m_categorieCombo->currentText().toStdString(),
        m_anEdit->value(), 0, "01/01/2023", 0, 
        m_minuteEdit->value(), narator, m_linkEdit->text().toStdString()
    );
    m_bib.salveazaDate();

    QMessageBox::information(this, "Succes", "Audiobook-ul a fost adăugat!\nISBN: " + QString::fromStdString(isbn));
    accept();
}

// ---------------------------------------------------------
// Dialog Ștergere Carte
// ---------------------------------------------------------
StergeCarteDialog::StergeCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Șterge Carte");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Filtru Tip Carte:"));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Toate", "Fizice", "Digitale", "Audiobook"});
    topLayout->addWidget(m_typeCombo);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Nr", "Titlu", "ISBN"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnSterge = new QPushButton("Șterge", this);
    m_btnSterge->setStyleSheet("background-color: #ef4444; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnSterge->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnSterge);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_typeCombo, &QComboBox::currentIndexChanged, this, &StergeCarteDialog::onTypeChanged);
    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnSterge->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnSterge, &QPushButton::clicked, this, &StergeCarteDialog::onSterge);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void StergeCarteDialog::refreshTable() {
    int tip_opt = m_typeCombo->currentIndex();
    auto toate = m_bib.getToateCartile();
    m_cartiCurente.clear();

    for(const auto& c : toate) {
        if(tip_opt == 0) m_cartiCurente.push_back(c);
        else if(tip_opt == 1 && c->getTipFormat() == 0) m_cartiCurente.push_back(c);
        else if(tip_opt == 2 && c->getTipFormat() == 1) m_cartiCurente.push_back(c);
        else if(tip_opt == 3 && c->getTipFormat() == 2) m_cartiCurente.push_back(c);
    }

    m_table->setRowCount(m_cartiCurente.size());
    for(size_t i=0; i<m_cartiCurente.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getIsbn())));
    }
}

void StergeCarteDialog::onTypeChanged(int) {
    refreshTable();
}

void StergeCarteDialog::onSterge() {
    int row = m_table->currentRow();
    if(row < 0) return;

    QString titlu = QString::fromStdString(m_cartiCurente[row]->getTitlu());
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmare", "Ești sigur că vrei să ștergi cartea '" + titlu + "'?", QMessageBox::Yes | QMessageBox::No);
    
    if(reply == QMessageBox::Yes) {
        if(m_bib.stergeCarte(m_cartiCurente[row]->getIsbn(), m_id_actor)) {
            QMessageBox::information(this, "Succes", "Cartea a fost mutată la coșul de reciclare.");
            refreshTable();
        } else {
            QMessageBox::critical(this, "Eroare", "Nu s-a putut șterge cartea!");
        }
    }
}

// ---------------------------------------------------------
// Dialog Restaurare Carte
// ---------------------------------------------------------
RestaureazaCarteDialog::RestaureazaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Restaurează Carte");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Nr", "Titlu", "ISBN"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnRestaureaza = new QPushButton("Restaurează", this);
    m_btnRestaureaza->setStyleSheet("background-color: #64748b; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnRestaureaza->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnRestaureaza);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnRestaureaza->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnRestaureaza, &QPushButton::clicked, this, &RestaureazaCarteDialog::onRestaureaza);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void RestaureazaCarteDialog::refreshTable() {
    auto sterse = m_bib.getCartiSterse();
    m_cartiCurente = sterse;

    m_table->setRowCount(m_cartiCurente.size());
    for(size_t i=0; i<m_cartiCurente.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getIsbn())));
    }
}

void RestaureazaCarteDialog::onRestaureaza() {
    int row = m_table->currentRow();
    if(row < 0) return;

    QString titlu = QString::fromStdString(m_cartiCurente[row]->getTitlu());
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmare", "Restaurezi cartea '" + titlu + "'?", QMessageBox::Yes | QMessageBox::No);
    
    if(reply == QMessageBox::Yes) {
        if(m_bib.restaureazaCarte(m_cartiCurente[row]->getIsbn(), m_id_actor)) {
            QMessageBox::information(this, "Succes", "Cartea a fost restaurată cu succes!");
            refreshTable();
        } else {
            QMessageBox::critical(this, "Eroare", "Eroare la restaurare!");
        }
    }
}

// ---------------------------------------------------------
// Dialog Selecție pentru Modificare Carte
// ---------------------------------------------------------
SelectieModificareCarteDialog::SelectieModificareCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Alege Carte pentru Modificare");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Filtru Tip Carte:"));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Toate", "Fizice", "Digitale", "Audiobook"});
    topLayout->addWidget(m_typeCombo);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Nr", "Titlu", "ISBN"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnModifica = new QPushButton("Editează...", this);
    m_btnModifica->setStyleSheet("background-color: #fbbf24; color: #78350f; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnModifica->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnModifica);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_typeCombo, &QComboBox::currentIndexChanged, this, &SelectieModificareCarteDialog::onTypeChanged);
    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnModifica->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnModifica, &QPushButton::clicked, this, &SelectieModificareCarteDialog::onModifica);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void SelectieModificareCarteDialog::refreshTable() {
    int tip_opt = m_typeCombo->currentIndex();
    auto toate = m_bib.getToateCartile();
    m_cartiCurente.clear();

    for(const auto& c : toate) {
        if(tip_opt == 0) m_cartiCurente.push_back(c);
        else if(tip_opt == 1 && c->getTipFormat() == 0) m_cartiCurente.push_back(c);
        else if(tip_opt == 2 && c->getTipFormat() == 1) m_cartiCurente.push_back(c);
        else if(tip_opt == 3 && c->getTipFormat() == 2) m_cartiCurente.push_back(c);
    }

    m_table->setRowCount(m_cartiCurente.size());
    for(size_t i=0; i<m_cartiCurente.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_cartiCurente[i]->getIsbn())));
    }
}

void SelectieModificareCarteDialog::onTypeChanged(int) { refreshTable(); }

void SelectieModificareCarteDialog::onModifica() {
    int row = m_table->currentRow();
    if(row < 0) return;

    EditeazaCarteDialog dlg(m_bib, m_cartiCurente[row], m_id_actor, this);
    if(dlg.exec() == QDialog::Accepted) {
        refreshTable();
    }
}

// ---------------------------------------------------------
// Dialog Editare Carte (Proprietăți detaliate)
// ---------------------------------------------------------
EditeazaCarteDialog::EditeazaCarteDialog(Biblioteca& bib, std::shared_ptr<Carte> carte, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_carte(carte), m_id_actor(id_actor) {
    setWindowTitle("Editează Carte: " + QString::fromStdString(carte->getTitlu()));
    setMinimumWidth(450);

    QFormLayout* form = new QFormLayout(this);

    m_titluEdit = new QLineEdit(QString::fromStdString(carte->getTitlu()), this);
    
    std::string autori_str = "";
    auto autori = carte->getAutori();
    for(size_t i=0; i<autori.size(); ++i) {
        autori_str += autori[i];
        if(i < autori.size() - 1) autori_str += ", ";
    }
    m_autoriEdit = new QLineEdit(QString::fromStdString(autori_str), this);
    m_edituraEdit = new QLineEdit(QString::fromStdString(carte->getEditura()), this);
    m_anEdit = new QSpinBox(this); m_anEdit->setRange(1000, 2100); m_anEdit->setValue(carte->getAnAparitie());
    m_pretEdit = new QDoubleSpinBox(this); m_pretEdit->setRange(0.0, 10000.0); m_pretEdit->setValue(carte->getPretIntrare());
    m_categorieCombo = new QComboBox(this);
    for(const auto& c : bib.getCategoriiDistincte()) m_categorieCombo->addItem(QString::fromStdString(c));
    m_categorieCombo->setEditable(true);
    m_categorieCombo->setCurrentText(QString::fromStdString(carte->getCategorie()));

    form->addRow("Titlu:", m_titluEdit);
    form->addRow("Autori:", m_autoriEdit);
    form->addRow("Editură:", m_edituraEdit);
    form->addRow("An:", m_anEdit);
    form->addRow("Preț:", m_pretEdit);
    form->addRow("Categorie:", m_categorieCombo);

    m_paginiEdit = nullptr;
    m_stocEdit = nullptr;
    m_defecteEdit = nullptr;
    m_locatieEdit = nullptr;
    m_copertaCombo = nullptr;
    m_formatEdit = nullptr;
    m_linkEdit = nullptr;
    m_naratorEdit = nullptr;
    m_minuteEdit = nullptr;

    if (auto cf = std::dynamic_pointer_cast<CarteFizica>(carte)) {
        m_paginiEdit = new QSpinBox(this); m_paginiEdit->setRange(1, 10000); m_paginiEdit->setValue(cf->getNrPagini());
        m_stocEdit = new QSpinBox(this); m_stocEdit->setRange(0, 1000); m_stocEdit->setValue(cf->getStocDisponibil());
        m_locatieEdit = new QLineEdit(QString::fromStdString(cf->getLocatieScurta()), this);
        m_defecteEdit = new QCheckBox("Are defecte fizice", this); m_defecteEdit->setChecked(cf->getStareDefecta());
        m_copertaCombo = new QComboBox(this); m_copertaCombo->addItems({"Softcover", "Hardcover"});
        m_copertaCombo->setCurrentText(QString::fromStdString(cf->getTipCoperta()));

        form->addRow("Nr. Pagini:", m_paginiEdit);
        form->addRow("Stoc Disponibil:", m_stocEdit);
        form->addRow("Locație (Raft):", m_locatieEdit);
        form->addRow("Copertă:", m_copertaCombo);
        form->addRow("Stare:", m_defecteEdit);
    } 
    else if (auto cd = std::dynamic_pointer_cast<CarteDigitala>(carte)) {
        m_paginiEdit = new QSpinBox(this); m_paginiEdit->setRange(1, 10000); m_paginiEdit->setValue(cd->getNrPagini());
        m_formatEdit = new QLineEdit(QString::fromStdString(cd->getFormatFisier()), this);
        m_linkEdit = new QLineEdit(QString::fromStdString(cd->getLinkAcces()), this);

        form->addRow("Nr. Pagini:", m_paginiEdit);
        form->addRow("Format (ex. PDF):", m_formatEdit);
        form->addRow("Link Acces:", m_linkEdit);
    }
    else if (auto ca = std::dynamic_pointer_cast<CarteAudio>(carte)) {
        m_minuteEdit = new QSpinBox(this); m_minuteEdit->setRange(1, 10000); m_minuteEdit->setValue(ca->getDurataMinute());
        m_naratorEdit = new QLineEdit(QString::fromStdString(ca->getNarator()), this);
        m_linkEdit = new QLineEdit(QString::fromStdString(ca->getLinkAcces()), this);

        form->addRow("Durată (minute):", m_minuteEdit);
        form->addRow("Narator:", m_naratorEdit);
        form->addRow("Link Acces:", m_linkEdit);
    }

    QPushButton* saveBtn = new QPushButton("Salvează Modificările", this);
    saveBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 6px;");
    form->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &EditeazaCarteDialog::onSave);
}

void EditeazaCarteDialog::onSave() {
    m_carte->setTitlu(m_titluEdit->text().toStdString());
    m_carte->setAutori(Carte::stringToAutori(m_autoriEdit->text().toStdString()));
    m_carte->setEditura(m_edituraEdit->text().toStdString());
    m_carte->setAnAparitie(m_anEdit->value());
    m_carte->setPretIntrare(m_pretEdit->value());
    m_carte->setCategorie(m_categorieCombo->currentText().toStdString());

    if (auto cf = std::dynamic_pointer_cast<CarteFizica>(m_carte)) {
        cf->setNrPagini(m_paginiEdit->value());
        cf->setStocDisponibil(m_stocEdit->value());
        
        Locatie nloc;
        nloc.cladire = "Main";
        nloc.camera = "1";
        nloc.culoar = "A";
        nloc.raft = m_locatieEdit->text().toStdString();
        cf->setLocatieRaft(nloc);

        cf->setStareDefecta(m_defecteEdit->isChecked());
        cf->setTipCoperta(m_copertaCombo->currentText().toStdString());
    } 
    else if (auto cd = std::dynamic_pointer_cast<CarteDigitala>(m_carte)) {
        cd->setNrPagini(m_paginiEdit->value());
        cd->setFormatFisier(m_formatEdit->text().toStdString());
        cd->setLinkAcces(m_linkEdit->text().toStdString());
        QString formatStr = m_formatEdit->text().toUpper();
        double sizePerPg = 0.05;
        if(formatStr == "EPUB") sizePerPg = 0.02;
        else if(formatStr == "MOBI") sizePerPg = 0.03;
        cd->setDimensiuneMB(m_paginiEdit->value() * sizePerPg);
    }
    else if (auto ca = std::dynamic_pointer_cast<CarteAudio>(m_carte)) {
        ca->setDurataMinute(m_minuteEdit->value());
        ca->setNarator(m_naratorEdit->text().toStdString());
        ca->setLinkAcces(m_linkEdit->text().toStdString());
    }

    m_bib.salveazaDate();
    QMessageBox::information(this, "Succes", "Modificările au fost salvate!");
    accept();
}

// ---------------------------------------------------------
// Dialog Adăugare Utilizator
// ---------------------------------------------------------
AdaugaUtilizatorDialog::AdaugaUtilizatorDialog(Biblioteca& bib, QWidget* parent) 
    : QDialog(parent), m_bib(bib) 
{
    setWindowTitle("Date Utilizator");
    setMinimumWidth(350);

    QFormLayout* formLayout = new QFormLayout();

    m_tipCombo = new QComboBox(this);
    m_tipCombo->addItems({"Director", "Bibliotecar", "Îngrijitor", "Cititor"});

    m_numeEdit = new QLineEdit(this);
    m_prenumeEdit = new QLineEdit(this);
    m_emailEdit = new QLineEdit(this);
    m_telefonEdit = new QLineEdit(this);
    m_parolaEdit = new QLineEdit(this);
    m_parolaEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Tip Utilizator:", m_tipCombo);
    formLayout->addRow("Nume:", m_numeEdit);
    formLayout->addRow("Prenume:", m_prenumeEdit);
    formLayout->addRow("Email:", m_emailEdit);
    formLayout->addRow("Telefon:", m_telefonEdit);
    formLayout->addRow("Parolă dorită:", m_parolaEdit);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* saveBtn = new QPushButton("Generează & Salvează", this);
    QPushButton* cancelBtn = new QPushButton("Anulează", this);
    
    connect(saveBtn, &QPushButton::clicked, this, &AdaugaUtilizatorDialog::onSave);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(btnLayout);
}

void AdaugaUtilizatorDialog::onSave() {
    int opt = m_tipCombo->currentIndex();
    std::string n = m_numeEdit->text().toStdString();
    std::string pr = m_prenumeEdit->text().toStdString();
    std::string e = m_emailEdit->text().toStdString();
    std::string t = m_telefonEdit->text().toStdString();
    std::string p = m_parolaEdit->text().toStdString();

    if (n.empty() || pr.empty() || p.empty()) {
        QMessageBox::warning(this, "Eroare", "Numele, prenumele și parola sunt obligatorii!");
        return;
    }

    std::string prefix = (opt == 0) ? "DIR" : (opt == 1) ? "BIB" : (opt == 2) ? "ING" : "CIT";
    std::string id = generateUserId_Qt(prefix);

    if(opt == 3) {
        m_bib.adaugaCititor(id, p, n, pr, "0000000", e, t, "Adresa", "standard", "01/01/2026", "01/01/2027", 3);
    } else if(opt == 1) {
        m_bib.adaugaBibliotecar(id, p, n, pr, "000000", e, t, "Adresa", "Generala", 3500, "01/01/2024", "08-16", 100);
    } else if (opt == 2) {
        m_bib.adaugaIngrijitor(id, p, n, pr, "000000", e, t, "Adresa", "Curte", 3000, "01/01/2024", "08-16", "Mătura");
    } else {
        m_bib.adaugaDirector(id, p, n, pr, "000000", e, t, "Adresa", "Management", 8500, "01/01/2024", "Birou 1", 5);
    }

    m_bib.salveazaDate();
    QMessageBox::information(this, "Succes", QString("Utilizator generat cu succes!\nID: %1\nParolă: %2").arg(QString::fromStdString(id), QString::fromStdString(p)));
    accept();
}

// ---------------------------------------------------------
// Dialog Vizualizare Tabelară
// ---------------------------------------------------------
VizualizareTableDialog::VizualizareTableDialog(const QString& title, QWidget* parent) 
    : QDialog(parent) 
{
    setWindowTitle(title);
    resize(800, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel* lTitle = new QLabel(title, this);
    lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #292524;");
    layout->addWidget(lTitle);

    m_table = new QTableWidget(this);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Style the table
    m_table->setStyleSheet("QTableWidget { background-color: white; border: 1px solid #e5e5e5; border-radius: 6px; } "
                           "QHeaderView::section { background-color: #f5f5f4; border: none; border-bottom: 1px solid #d6d3d1; padding: 8px; font-weight: bold; color: #57534e; text-transform: uppercase; }");
    
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    QPushButton* closeBtn = new QPushButton("Închide", this);
    closeBtn->setStyleSheet("background-color: #3b82f6; color: white; font-weight: bold; padding: 8px 16px; border-radius: 4px; border:none;");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);
}

void VizualizareTableDialog::setHeaders(const QStringList& headers) {
    m_table->setColumnCount(headers.size());
    m_table->setHorizontalHeaderLabels(headers);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    if (headers.size() >= 3) {
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        for(int i = 1; i < headers.size(); i++) {
            m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }
}

void VizualizareTableDialog::addRow(const QStringList& rowData) {
    int row = m_table->rowCount();
    m_table->insertRow(row);
    for (int i = 0; i < rowData.size(); ++i) {
        m_table->setItem(row, i, new QTableWidgetItem(rowData[i]));
    }
}

void VizualizareTableDialog::adjustColumns() {
    m_table->resizeColumnsToContents();
}

// ---------------------------------------------------------
// Implementare Funcții Helper
// ---------------------------------------------------------
namespace QtFunc {
    void adaugaCarteFizica(QWidget* parent, Biblioteca& bib) {
        AdaugaCarteFizicaDialog dlg(bib, parent);
        dlg.exec();
    }

    void adaugaUtilizator(QWidget* parent, Biblioteca& bib) {
        AdaugaUtilizatorDialog dlg(bib, parent);
        dlg.exec();
    }

    void vizualizareCatalog(QWidget* parent, Biblioteca& bib) {
        VizualizareTableDialog dlg("Catalog Cărți", parent);
        dlg.setHeaders({"Tip", "Titlu", "Autor", "An", "Status", "Alte Detalii"});

        for (const auto& c : bib.getToateCartile()) {
            QString tip = QString::fromStdString(c->getTip());
            QString titlu = QString::fromStdString(c->getTitlu());
            
            QString autor;
            if (!c->getAutori().empty()) autor = QString::fromStdString(c->getAutori()[0]);
            else autor = "Necunoscut";
            
            QString an = QString::number(c->getAnAparitie());
            QString status = (c->getStocDisponibil() > 0) ? "Disponibil" : "Împrumutat/Indisponibil";
            
            // Un mic string pentru detalii (nr. pagini, format, etc)
            QString detalii = "N/A";
            if (auto cf = std::dynamic_pointer_cast<CarteFizica>(c)) {
                detalii = QString("Raft: %1").arg(QString::fromStdString(cf->getLocatieScurta()));
            }
            
            dlg.addRow({tip, titlu, autor, an, status, detalii});
        }
        dlg.adjustColumns();
        dlg.exec();
    }

    void afiseazaUtilizatori(QWidget* parent, Biblioteca& bib) {
        VizualizareTableDialog dlg("Listă Utilizatori", parent);
        dlg.setHeaders({"ID", "Nume", "Rol", "Penalizări"});

        for (const auto& u : bib.getTotiUtilizatorii()) {
            QString id = QString::fromStdString(u->getId());
            QString nume = QString::fromStdString(u->getNumeComplet());
            QString rol = QString::fromStdString(u->getTip());
            QString penalizari = "0"; // TODO if cititor
            
            dlg.addRow({id, nume, rol, penalizari});
        }
        dlg.adjustColumns();
        dlg.exec();
    }
} // end namespace QtFunc

// ---------------------------------------------------------
// Dialog Ștergere Utilizator
// ---------------------------------------------------------
StergeUtilizatorDialog::StergeUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Șterge Utilizator");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Filtru Tip Utilizator:"));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Toti", "Director", "Bibliotecar", "Ingrijitor", "Cititor"});
    topLayout->addWidget(m_typeCombo);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ID", "Nume", "Rol"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnSterge = new QPushButton("Șterge", this);
    m_btnSterge->setStyleSheet("background-color: #ef4444; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnSterge->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnSterge);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_typeCombo, &QComboBox::currentIndexChanged, this, &StergeUtilizatorDialog::onTypeChanged);
    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnSterge->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnSterge, &QPushButton::clicked, this, &StergeUtilizatorDialog::onSterge);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void StergeUtilizatorDialog::refreshTable() {
    int tip_opt = m_typeCombo->currentIndex();
    std::string tip_dorit = (tip_opt == 1) ? "DIRECTOR" : (tip_opt == 2) ? "BIBLIOTECAR" : (tip_opt == 3) ? "INGRIJITOR" : (tip_opt == 4) ? "CITITOR" : "TOTI";

    auto toti = m_bib.getTotiUtilizatorii();
    m_utilsCurenti.clear();

    for(const auto& u : toti) {
        if(tip_opt == 0 || u->getTip() == tip_dorit) m_utilsCurenti.push_back(u);
    }

    m_table->setRowCount(m_utilsCurenti.size());
    for(size_t i=0; i<m_utilsCurenti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getId())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getNumeComplet())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getTip())));
    }
}

void StergeUtilizatorDialog::onTypeChanged(int) { refreshTable(); }

void StergeUtilizatorDialog::onSterge() {
    int row = m_table->currentRow();
    if(row < 0) return;

    QString nume = QString::fromStdString(m_utilsCurenti[row]->getNumeComplet());
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmare", "Ești sigur că vrei să ștergi utilizatorul '" + nume + "'?", QMessageBox::Yes | QMessageBox::No);
    
    if(reply == QMessageBox::Yes) {
        if(m_bib.stergeUtilizator(m_utilsCurenti[row]->getId(), m_id_actor)) {
            QMessageBox::information(this, "Succes", "Utilizator șters cu succes!");
            refreshTable();
        } else {
            QMessageBox::critical(this, "Eroare", "Nu s-a putut șterge utilizatorul!");
        }
    }
}

// ---------------------------------------------------------
// Dialog Restaurare Utilizator
// ---------------------------------------------------------
RestaureazaUtilizatorDialog::RestaureazaUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Restaurează Utilizator");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ID", "Nume", "Rol"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnRestaureaza = new QPushButton("Restaurează", this);
    m_btnRestaureaza->setStyleSheet("background-color: #64748b; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnRestaureaza->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnRestaureaza);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnRestaureaza->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnRestaureaza, &QPushButton::clicked, this, &RestaureazaUtilizatorDialog::onRestaureaza);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void RestaureazaUtilizatorDialog::refreshTable() {
    m_utilsCurenti = m_bib.getUtilizatoriStersi();
    m_table->setRowCount(m_utilsCurenti.size());
    for(size_t i=0; i<m_utilsCurenti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getId())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getNumeComplet())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getTip())));
    }
}

void RestaureazaUtilizatorDialog::onRestaureaza() {
    int row = m_table->currentRow();
    if(row < 0) return;

    QString nume = QString::fromStdString(m_utilsCurenti[row]->getNumeComplet());
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmare", "Restaurezi utilizatorul '" + nume + "'?", QMessageBox::Yes | QMessageBox::No);
    
    if(reply == QMessageBox::Yes) {
        if(m_bib.restaureazaUtilizator(m_utilsCurenti[row]->getId(), m_id_actor)) {
            QMessageBox::information(this, "Succes", "Utilizator restaurat cu succes!");
            refreshTable();
        } else {
            QMessageBox::critical(this, "Eroare", "Eroare la restaurare!");
        }
    }
}

// ---------------------------------------------------------
// Dialog Selecție pentru Modificare Utilizator
// ---------------------------------------------------------
SelectieModificareUtilizatorDialog::SelectieModificareUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Alege Utilizator pentru Modificare");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Filtru Tip Utilizator:"));
    m_typeCombo = new QComboBox(this);
    m_typeCombo->addItems({"Toti", "Director", "Bibliotecar", "Ingrijitor", "Cititor"});
    topLayout->addWidget(m_typeCombo);
    topLayout->addStretch();
    layout->addLayout(topLayout);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ID", "Nume", "Rol"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnModifica = new QPushButton("Editează...", this);
    m_btnModifica->setStyleSheet("background-color: #fbbf24; color: #78350f; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnModifica->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnModifica);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_typeCombo, &QComboBox::currentIndexChanged, this, &SelectieModificareUtilizatorDialog::onTypeChanged);
    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnModifica->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnModifica, &QPushButton::clicked, this, &SelectieModificareUtilizatorDialog::onModifica);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void SelectieModificareUtilizatorDialog::refreshTable() {
    int tip_opt = m_typeCombo->currentIndex();
    std::string tip_dorit = (tip_opt == 1) ? "DIRECTOR" : (tip_opt == 2) ? "BIBLIOTECAR" : (tip_opt == 3) ? "INGRIJITOR" : (tip_opt == 4) ? "CITITOR" : "TOTI";

    auto toti = m_bib.getTotiUtilizatorii();
    m_utilsCurenti.clear();

    for(const auto& u : toti) {
        if(tip_opt == 0 || u->getTip() == tip_dorit) m_utilsCurenti.push_back(u);
    }

    m_table->setRowCount(m_utilsCurenti.size());
    for(size_t i=0; i<m_utilsCurenti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getId())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getNumeComplet())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_utilsCurenti[i]->getTip())));
    }
}

void SelectieModificareUtilizatorDialog::onTypeChanged(int) { refreshTable(); }

void SelectieModificareUtilizatorDialog::onModifica() {
    int row = m_table->currentRow();
    if(row < 0) return;

    EditeazaUtilizatorDialog dlg(m_bib, m_utilsCurenti[row], m_id_actor, this);
    if(dlg.exec() == QDialog::Accepted) {
        refreshTable();
    }
}

// ---------------------------------------------------------
// Dialog Editare Utilizator
// ---------------------------------------------------------
EditeazaUtilizatorDialog::EditeazaUtilizatorDialog(Biblioteca& bib, std::shared_ptr<Utilizator> util, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_util(util), m_id_actor(id_actor) {
    setWindowTitle("Editează Utilizator: " + QString::fromStdString(util->getNumeComplet()));
    setMinimumWidth(400);

    QFormLayout* form = new QFormLayout(this);

    m_numeEdit = new QLineEdit(QString::fromStdString(util->getNume()), this);
    m_prenumeEdit = new QLineEdit(QString::fromStdString(util->getPrenume()), this);
    m_emailEdit = new QLineEdit(QString::fromStdString(util->getEmail()), this);
    m_telefonEdit = new QLineEdit(QString::fromStdString(util->getTelefon()), this);
    m_adresaEdit = new QLineEdit(QString::fromStdString(util->getAdresa()), this);
    m_parolaEdit = new QLineEdit(this);
    m_parolaEdit->setEchoMode(QLineEdit::Password);
    m_parolaEdit->setPlaceholderText("Lăsați gol pentru a nu schimba parola");
    
    form->addRow("Nume:", m_numeEdit);
    form->addRow("Prenume:", m_prenumeEdit);
    form->addRow("Email:", m_emailEdit);
    form->addRow("Telefon:", m_telefonEdit);
    form->addRow("Adresa:", m_adresaEdit);
    form->addRow("Parola:", m_parolaEdit);

    QPushButton* saveBtn = new QPushButton("Salvează Modificările", this);
    saveBtn->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 6px;");
    form->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, this, &EditeazaUtilizatorDialog::onSave);
}

void EditeazaUtilizatorDialog::onSave() {
    m_util->setNume(m_numeEdit->text().toStdString());
    m_util->setPrenume(m_prenumeEdit->text().toStdString());
    m_util->setEmail(m_emailEdit->text().toStdString());
    m_util->setTelefon(m_telefonEdit->text().toStdString());
    m_util->setAdresa(m_adresaEdit->text().toStdString());
    
    if(!m_parolaEdit->text().isEmpty()) {
        m_util->setParola(m_parolaEdit->text().toStdString());
    }
    
    m_bib.salveazaDate();
    QMessageBox::information(this, "Succes", "Informațiile au fost actualizate!");
    accept();
}

// ---------------------------------------------------------
// Dialog Împrumut Carte (Manual - Bibliotecar/Director)
// ---------------------------------------------------------
ImprumutaCarteDialog::ImprumutaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Împrumut Manual Carte");
    setMinimumSize(800, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* splitLayout = new QHBoxLayout();

    // Stanga: Cărți
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("1. Selectează Cartea:"));
    m_tableCarti = new QTableWidget(0, 3, this);
    m_tableCarti->setHorizontalHeaderLabels({"Nr", "Titlu", "Stoc"});
    m_tableCarti->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCarti->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableCarti->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCarti->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    leftLayout->addWidget(m_tableCarti);
    splitLayout->addLayout(leftLayout);

    // Dreapta: Cititori
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("2. Selectează Cititorul:"));
    m_tableCititori = new QTableWidget(0, 3, this);
    m_tableCititori->setHorizontalHeaderLabels({"ID", "Nume", "Rol"});
    m_tableCititori->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCititori->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableCititori->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCititori->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    rightLayout->addWidget(m_tableCititori);
    splitLayout->addLayout(rightLayout);

    layout->addLayout(splitLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnImprumuta = new QPushButton("Împrumută", this);
    m_btnImprumuta->setStyleSheet("background-color: #0284c7; color: white; font-weight: bold; padding: 8px 16px; border-radius: 4px;");
    m_btnImprumuta->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnImprumuta);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    auto checkSelection = [this]() {
        bool hasCarte = m_tableCarti->currentRow() >= 0;
        bool hasCititor = m_tableCititori->currentRow() >= 0;
        m_btnImprumuta->setEnabled(hasCarte && hasCititor);
    };

    connect(m_tableCarti, &QTableWidget::itemSelectionChanged, checkSelection);
    connect(m_tableCititori, &QTableWidget::itemSelectionChanged, checkSelection);
    connect(m_btnImprumuta, &QPushButton::clicked, this, &ImprumutaCarteDialog::onImprumuta);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshCarti();
    refreshUtilizatori();
}

void ImprumutaCarteDialog::refreshCarti() {
    m_carti = m_bib.getToateCartile();
    m_tableCarti->setRowCount(m_carti.size());
    for(size_t i=0; i<m_carti.size(); ++i) {
        m_tableCarti->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
        m_tableCarti->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getTitlu())));
        m_tableCarti->setItem(i, 2, new QTableWidgetItem(QString::number(m_carti[i]->getStocDisponibil())));
    }
}

void ImprumutaCarteDialog::refreshUtilizatori() {
    auto utils = m_bib.getTotiUtilizatorii();
    m_cititori.clear();
    for(const auto& u : utils) {
        if(u->getTip() == "CITITOR") m_cititori.push_back(u);
    }
    m_tableCititori->setRowCount(m_cititori.size());
    for(size_t i=0; i<m_cititori.size(); ++i) {
        m_tableCititori->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_cititori[i]->getId())));
        m_tableCititori->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_cititori[i]->getNumeComplet())));
        m_tableCititori->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_cititori[i]->getTip())));
    }
}

void ImprumutaCarteDialog::onImprumuta() {
    int rC = m_tableCarti->currentRow();
    int rU = m_tableCititori->currentRow();
    if(rC < 0 || rU < 0) return;

    if (m_bib.calculeazaPenalizariTotale(m_cititori[rU]->getId()) > 0) {
        QMessageBox::critical(this, "Atenție", "Acest cititor are penalizări neachitate și nu poate împrumuta cărți!");
        return;
    }

    std::string imp = m_bib.getDataCurentaStr();
    std::string ret = m_bib.getDataCurentaPlusStr(14);

    if(m_bib.adaugaImprumut(m_carti[rC]->getIsbn(), m_cititori[rU]->getId(), imp, ret, "Manual")) {
        QMessageBox::information(this, "Succes", "Împrumut realizat cu succes!");
        refreshCarti();
        m_btnImprumuta->setEnabled(false);
    } else {
        QMessageBox::critical(this, "Eroare", "Stoc insuficient sau limită atinsă pentru cititor!");
    }
}

// ---------------------------------------------------------
// Dialog Returnare Carte (Manual - Bibliotecar/Director)
// ---------------------------------------------------------
ReturneazaCarteDialog::ReturneazaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Returnare Manuală Carte");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Titlu", "Cititor", "Termen Limită"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnReturneaza = new QPushButton("Returnează Selectată", this);
    m_btnReturneaza->setStyleSheet("background-color: #10b981; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnReturneaza->setEnabled(false);
    m_btnReturneazaDefecta = new QPushButton("Returnează (Defectă)", this);
    m_btnReturneazaDefecta->setStyleSheet("background-color: #ef4444; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnReturneazaDefecta->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnReturneaza);
    btnLayout->addWidget(m_btnReturneazaDefecta);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        bool hasSelection = m_table->currentRow() >= 0;
        m_btnReturneaza->setEnabled(hasSelection);
        m_btnReturneazaDefecta->setEnabled(hasSelection);
    });
    connect(m_btnReturneaza, &QPushButton::clicked, this, &ReturneazaCarteDialog::onReturneaza);
    
    connect(m_btnReturneazaDefecta, &QPushButton::clicked, [this]() {
        int row = m_table->currentRow();
        if(row < 0) return;
        
        m_bib.solicitaReturnare(m_imprumuturi[row].getIdCarte(), m_imprumuturi[row].getIdCititor());
        size_t lastIdx = m_bib.getReturnariInAsteptare().size() - 1;
        if(m_bib.refuzaReturnareDefecta(lastIdx, m_id_actor)) {
             QMessageBox::information(this, "Raportat", "Cartea a fost returnată și marcată ca defectă. Cititorul a fost penalizat.");
             refreshTable();
        } else {
             QMessageBox::warning(this, "Eroare", "A apărut o eroare la returnare.");
        }
    });

    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ReturneazaCarteDialog::refreshTable() {
    m_imprumuturi = m_bib.getToateImprumuturile();
    m_table->setRowCount(m_imprumuturi.size());
    for(size_t i=0; i<m_imprumuturi.size(); ++i) {
        auto c = m_bib.gasesteCarte(m_imprumuturi[i].getIdCarte());
        auto u = m_bib.gasesteUtilizator(m_imprumuturi[i].getIdCititor());
        QString titlu = c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(m_imprumuturi[i].getIdCarte());
        QString nume = u ? QString::fromStdString(u->getNumeComplet()) : QString::fromStdString(m_imprumuturi[i].getIdCititor());
        
        m_table->setItem(i, 0, new QTableWidgetItem(titlu));
        m_table->setItem(i, 1, new QTableWidgetItem(nume));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(m_imprumuturi[i].getTermenLimita())));
    }
}

void ReturneazaCarteDialog::onReturneaza() {
    int row = m_table->currentRow();
    if(row < 0) return;

    if (m_bib.returneazaCarte(m_imprumuturi[row].getIdCarte(), m_imprumuturi[row].getIdCititor())) {
        QMessageBox::information(this, "Succes", "Cartea a fost returnată cu succes!");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Eroare", "A apărut o problemă la returnare.");
    }
}

// ---------------------------------------------------------
// Dialog Confirmă Returnări (Tonomat)
// ---------------------------------------------------------
ConfirmaReturnariDialog::ConfirmaReturnariDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Confirmare Returnări Tonomat");
    setMinimumSize(700, 450);
    setStyleSheet("QDialog { background-color: #fdfbf7; }");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    m_table = new QTableWidget(0, 4, this);
    m_table->setHorizontalHeaderLabels({"Titlu Carte", "Cititor (Nume Complet)", "Data Retur", "Amendă"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setStyleSheet("QTableWidget { background-color: white; border: 1px solid #e5e5e5; border-radius: 4px; } "
                           "QHeaderView::section { background-color: #f5f5f4; border: none; border-bottom: 1px solid #d6d3d1; padding: 6px; font-weight: bold; }");
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnConfirma = new QPushButton("Confirmă (Stare Bună)", this);
    m_btnConfirma->setStyleSheet("QPushButton { background-color: #10b981; color: white; font-weight: bold; padding: 8px 16px; border-radius: 4px; border-bottom: 3px solid #059669; } QPushButton:hover { background-color: #059669; } QPushButton:disabled { background-color: #a7f3d0; border-bottom: none; }");
    m_btnConfirma->setEnabled(false);
    
    m_btnRefuza = new QPushButton("Raportează Defectă", this);
    m_btnRefuza->setStyleSheet("QPushButton { background-color: #ef4444; color: white; font-weight: bold; padding: 8px 16px; border-radius: 4px; border-bottom: 3px solid #b91c1c; } QPushButton:hover { background-color: #dc2626; } QPushButton:disabled { background-color: #fca5a5; border-bottom: none; }");
    m_btnRefuza->setEnabled(false);

    QPushButton* btnClose = new QPushButton("Închide", this);
    btnClose->setStyleSheet("QPushButton { background-color: #e5e5e5; color: #292524; font-weight: bold; padding: 8px 16px; border-radius: 4px; border-bottom: 3px solid #d4d4d4; } QPushButton:hover { background-color: #d4d4d4; }");
    
    btnLayout->addWidget(m_btnConfirma);
    btnLayout->addWidget(m_btnRefuza);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        bool hasSelection = m_table->currentRow() >= 0;
        m_btnConfirma->setEnabled(hasSelection);
        m_btnRefuza->setEnabled(hasSelection);
    });
    connect(m_btnConfirma, &QPushButton::clicked, this, &ConfirmaReturnariDialog::onConfirma);
    connect(m_btnRefuza, &QPushButton::clicked, this, &ConfirmaReturnariDialog::onRefuza);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ConfirmaReturnariDialog::refreshTable() {
    auto ret = m_bib.getReturnariInAsteptare();
    m_table->setRowCount(ret.size());
    for(size_t i=0; i<ret.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(ret[i].titlu_carte)));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(ret[i].nume_cititor)));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ret[i].data_returnare)));
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(ret[i].amenda_intarziere)));
    }
}

void ConfirmaReturnariDialog::onConfirma() {
    int row = m_table->currentRow();
    if(row < 0) return;
    m_bib.confirmaReturnare(row, m_id_actor);
    QMessageBox::information(this, "Succes", "Returnare confirmată!");
    refreshTable();
}

void ConfirmaReturnariDialog::onRefuza() {
    int row = m_table->currentRow();
    if(row < 0) return;
    m_bib.refuzaReturnareDefecta(row, m_id_actor);
    QMessageBox::information(this, "Atenție", "Cartea a fost mutată la defecte și amenda aplicată.");
    refreshTable();
}

// ---------------------------------------------------------
// Dialog Confirmă Plăți
// ---------------------------------------------------------
ConfirmaPlatiDialog::ConfirmaPlatiDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Confirmare Plăți (Amenzi)");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);

    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Cititor", "Motiv", "Suma (RON)"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnConfirma = new QPushButton("Confirmă Plata", this);
    m_btnConfirma->setStyleSheet("background-color: #10b981; color: white; font-weight: bold; padding: 6px 12px; border-radius: 4px;");
    m_btnConfirma->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnConfirma);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnConfirma->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnConfirma, &QPushButton::clicked, this, &ConfirmaPlatiDialog::onConfirma);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ConfirmaPlatiDialog::refreshTable() {
    auto plati = m_bib.getPlatiInAsteptare();
    m_table->setRowCount(plati.size());
    for(size_t i=0; i<plati.size(); ++i) {
        auto u = m_bib.gasesteUtilizator(plati[i].id_cititor);
        QString nume = u ? QString::fromStdString(u->getNumeComplet()) : QString::fromStdString(plati[i].id_cititor);
        m_table->setItem(i, 0, new QTableWidgetItem(nume));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(plati[i].data_solicitare)));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(plati[i].suma)));
    }
}

void ConfirmaPlatiDialog::onConfirma() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    m_bib.confirmaPlata(row, m_id_actor);
    QMessageBox::information(this, "Succes", "Plată confirmată și amenda ștearsă!");
    refreshTable();
}

// ---------------------------------------------------------
// Dialog Recondiționează Cărți
// ---------------------------------------------------------
ReconditioneazaCartiDialog::ReconditioneazaCartiDialog(Biblioteca& bib, QWidget* parent) 
    : QDialog(parent), m_bib(bib) {
    setWindowTitle("Recondiționare Cărți Defecte");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ISBN", "Titlu", "Locație"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    
    m_btnReparata = new QPushButton("Reparată (Mută la raft)", this);
    m_btnReparata->setStyleSheet("background-color: #0ea5e9; color: white; padding: 6px; border-radius: 4px;");
    m_btnReparata->setEnabled(false);
    
    m_btnCasata = new QPushButton("Inutilizabilă (Casează)", this);
    m_btnCasata->setStyleSheet("background-color: #ef4444; color: white; padding: 6px; border-radius: 4px;");
    m_btnCasata->setEnabled(false);

    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnReparata);
    btnLayout->addWidget(m_btnCasata);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        bool s = m_table->currentRow() >= 0;
        m_btnReparata->setEnabled(s);
        m_btnCasata->setEnabled(s);
    });
    connect(m_btnReparata, &QPushButton::clicked, this, &ReconditioneazaCartiDialog::peReparata);
    connect(m_btnCasata, &QPushButton::clicked, this, &ReconditioneazaCartiDialog::peCasata);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ReconditioneazaCartiDialog::refreshTable() {
    const auto& def = m_bib.getStocDefect();
    m_table->setRowCount(def.size());
    int i = 0;
    for(const auto& p : def) {
        auto c = m_bib.gasesteCarte(p.first);
        QString titlu = c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(p.first);
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(p.first)));
        m_table->setItem(i, 1, new QTableWidgetItem(titlu));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(p.second)));
        i++;
    }
}

void ReconditioneazaCartiDialog::peReparata() {
    int row = m_table->currentRow();
    if(row < 0) return;
    std::string isbn = m_table->item(row, 0)->text().toStdString();
    m_bib.reparaCarte(isbn);
    QMessageBox::information(this, "Succes", "Exemplarul a fost reparat și repus pe stoc.");
    refreshTable();
}

void ReconditioneazaCartiDialog::peCasata() {
    int row = m_table->currentRow();
    if(row < 0) return;
    std::string isbn = m_table->item(row, 0)->text().toStdString();
    m_bib.caseazaCarte(isbn);
    QMessageBox::information(this, "Casare", "Exemplarul defect a fost casat și șters definitiv.");
    refreshTable();
}

// ---------------------------------------------------------
// Dialog Casare Carte
// ---------------------------------------------------------
CasareCarteDialog::CasareCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Casare / Pierdere Carte (Stoc Curent)");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ISBN", "Titlu", "Stoc Disponibil"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    
    m_btnCaseaza = new QPushButton("Casează / Declară Pierdută", this);
    m_btnCaseaza->setStyleSheet("background-color: #ef4444; color: white; padding: 6px; border-radius: 4px;");
    m_btnCaseaza->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnCaseaza);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnCaseaza->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnCaseaza, &QPushButton::clicked, this, &CasareCarteDialog::onCaseaza);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void CasareCarteDialog::refreshTable() {
    auto carti = m_bib.getToateCartile();
    m_table->setRowCount(carti.size());
    for(size_t i=0; i<carti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(carti[i]->getIsbn())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(carti[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(carti[i]->getStocDisponibil())));
    }
}

void CasareCarteDialog::onCaseaza() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    auto carti = m_bib.getToateCartile();
    auto isbn = carti[row]->getIsbn();
    if(carti[row]->getStocDisponibil() == 0) {
        QMessageBox::warning(this, "Eroare", "Stoc zero pentru a putea casa din depozit. Caută în returnări dacă a fost pierdută de un cititor.");
        return;
    }

    auto util = m_bib.getTotiUtilizatorii();
    QStringList h = {"ID", "Nume"};
    QVector<QStringList> r;
    QVector<std::string> ids;
    r.append(QStringList{"Nimeni", "Casare din Stoc Intern (Fără Amendă)"});
    ids.append("");
    for(const auto& u : util) {
        if(u->getTip() == "CITITOR") {
            r.append(QStringList{QString::fromStdString(u->getId()), QString::fromStdString(u->getNumeComplet())});
            ids.append(u->getId());
        }
    }
    
    ListSelectionDialog dlg("Cine a pierdut cartea?", h, r, this);
    if(dlg.exec() == QDialog::Accepted) {
        int sel = dlg.getSelectedIndex();
        if(!ids[sel].empty()) {
            m_bib.caseazaCarte(isbn, ids[sel]);
            m_bib.solicitaPlata(ids[sel]); // Generează plata standard.
            QMessageBox::information(this, "Casare", "Cartea a fost ștearsă și cititorul a primit o amendă de despăgubire.");
        } else {
            m_bib.caseazaCarte(isbn);
            QMessageBox::information(this, "Casare", "Cartea a fost casată din stocul intern.");
        }
        m_bib.salveazaDate();
        refreshTable();
    }
}

// ---------------------------------------------------------
// Dialog Achiziții (Suplimentare Stoc)
// ---------------------------------------------------------
AchizitiiDialog::AchizitiiDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Achiziții / Suplimentare Stoc Curent");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ISBN", "Titlu", "Stoc Actual"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(new QLabel("Cantitate exemplare:"));
    m_spinCantitate = new QSpinBox(this);
    m_spinCantitate->setRange(1, 100);
    m_spinCantitate->setValue(1);
    btnLayout->addWidget(m_spinCantitate);

    btnLayout->addStretch();
    m_btnAchizitie = new QPushButton("Confirmă Achiziția", this);
    m_btnAchizitie->setStyleSheet("background-color: #0284c7; color: white; padding: 6px; border-radius: 4px;");
    m_btnAchizitie->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnAchizitie);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnAchizitie->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnAchizitie, &QPushButton::clicked, this, &AchizitiiDialog::onAchizitie);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void AchizitiiDialog::refreshTable() {
    auto carti = m_bib.getToateCartile();
    m_table->setRowCount(carti.size());
    for(size_t i=0; i<carti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(carti[i]->getIsbn())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(carti[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(carti[i]->getStocDisponibil())));
    }
}

void AchizitiiDialog::onAchizitie() {
    int row = m_table->currentRow();
    if(row < 0) return;
    auto carti = m_bib.getToateCartile();
    auto isbn = carti[row]->getIsbn();
    int cant = m_spinCantitate->value();
    
    if (m_bib.achizitioneazaExemplare(isbn, cant, m_id_actor)) {
        QMessageBox::information(this, "Succes", "Exemplarele au fost adăugate pe stoc!");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Eroare", "A apărut o problemă la achiziție.");
    }
}

// ---------------------------------------------------------
// Dialog Confirmă Rezervări (Ridicare Fizică)
// ---------------------------------------------------------
ConfirmaRezervariDialog::ConfirmaRezervariDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_actor(id_actor) {
    setWindowTitle("Confirmare Ridicare Rezervări");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"Cititor", "Titlu Carte", "Expiră la"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnConfirma = new QPushButton("Confirmă Ridicare Carte", this);
    m_btnConfirma->setStyleSheet("background-color: #10b981; color: white; padding: 6px; border-radius: 4px;");
    m_btnConfirma->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnConfirma);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnConfirma->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnConfirma, &QPushButton::clicked, this, &ConfirmaRezervariDialog::onConfirma);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ConfirmaRezervariDialog::refreshTable() {
    m_toate_rezervarile = m_bib.getRezervari();
    m_real_indices.clear();
    
    // Filtrare (doar rezervări care așteaptă ridicarea de către cititor)
    // Presupunem că le afișăm pe toate.
    m_table->setRowCount(0);
    for(size_t i=0; i<m_toate_rezervarile.size(); ++i) {
        auto c = m_bib.gasesteCarte(m_toate_rezervarile[i].isbn);
        auto u = m_bib.gasesteUtilizator(m_toate_rezervarile[i].id_cititor);
        QString titlu = c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(m_toate_rezervarile[i].isbn);
        QString nume = u ? QString::fromStdString(u->getNumeComplet()) : QString::fromStdString(m_toate_rezervarile[i].id_cititor);
        
        int row = m_table->rowCount();
        m_table->insertRow(row);
        m_table->setItem(row, 0, new QTableWidgetItem(nume));
        m_table->setItem(row, 1, new QTableWidgetItem(titlu));
        m_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(m_toate_rezervarile[i].data_expirare)));
        m_real_indices.push_back(i);
    }
}

void ConfirmaRezervariDialog::onConfirma() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    size_t real_idx = m_real_indices[row];
    m_bib.confirmaRidicare(real_idx, m_id_actor);
    QMessageBox::information(this, "Succes", "Rezervare ridicată! Status schimbat în împrumut.");
    refreshTable();
}

// ---------------------------------------------------------
// Dialog Rezervare Carte (Pentru Cititori)
// ---------------------------------------------------------
RezervaCarteDialog::RezervaCarteDialog(Biblioteca& bib, const std::string& id_cititor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_cititor(id_cititor) {
    setWindowTitle("Rezervă o Carte");
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 3, this);
    m_table->setHorizontalHeaderLabels({"ISBN", "Titlu", "Stoc Actual"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnRezerva = new QPushButton("Rezervă Selectată", this);
    m_btnRezerva->setStyleSheet("background-color: #0284c7; color: white; padding: 6px; border-radius: 4px;");
    m_btnRezerva->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnRezerva);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnRezerva->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnRezerva, &QPushButton::clicked, this, &RezervaCarteDialog::onRezerva);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void RezervaCarteDialog::refreshTable() {
    m_carti.clear();
    auto toate = m_bib.getToateCartile();
    for(const auto& c : toate) {
        if(c->getStocDisponibil() == 0) m_carti.push_back(c);
    }
    
    m_table->setRowCount(m_carti.size());
    for(size_t i=0; i<m_carti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getIsbn())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getTitlu())));
        m_table->setItem(i, 2, new QTableWidgetItem("0"));
    }
}

void RezervaCarteDialog::onRezerva() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    auto isbn = m_carti[row]->getIsbn();
    
    if (m_bib.adaugaRezervare(isbn, m_id_cititor)) {
        QMessageBox::information(this, "Succes", "Cartea a fost rezervată! Ai 3 zile să o ridici.");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Eroare", "Ai atins limita de rezervări sau cartea e indisponibilă.");
    }
}

// ---------------------------------------------------------
// Dialog Rezervare Sală Lectură (Pentru Cititori)
// ---------------------------------------------------------
RezervaSalaDialog::RezervaSalaDialog(Biblioteca& bib, const std::string& id_cititor, const std::string& nume_cititor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_cititor(id_cititor), m_nume_cititor(nume_cititor) {
    setWindowTitle("Rezervare Sală Lectură / Eveniment");
    setMinimumSize(400, 250);

    QFormLayout* form = new QFormLayout(this);
    
    m_cbSala = new QComboBox(this);
    m_cbSala->addItems({"Sala A", "Sala B", "Sala C", "Auditorium"});
    
    m_deData = new QDateEdit(QDate::currentDate(), this);
    m_deData->setCalendarPopup(true);
    m_deData->setMinimumDate(QDate::currentDate());

    m_cbInterval = new QComboBox(this);
    m_cbInterval->addItems({"08:00-10:00", "10:00-12:00", "12:00-14:00", "14:00-16:00", "16:00-18:00"});

    form->addRow("Selectează Sala:", m_cbSala);
    form->addRow("Data Dorită:", m_deData);
    form->addRow("Interval Orar:", m_cbInterval);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnRezerva = new QPushButton("Confirmă Rezervarea", this);
    m_btnRezerva->setStyleSheet("background-color: #0284c7; color: white; padding: 6px; border-radius: 4px;");
    QPushButton* btnClose = new QPushButton("Anulează", this);
    btnLayout->addWidget(m_btnRezerva);
    btnLayout->addWidget(btnClose);
    form->addRow(btnLayout);

    connect(m_btnRezerva, &QPushButton::clicked, this, &RezervaSalaDialog::onRezerva);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);
}

void RezervaSalaDialog::onRezerva() {
    QString sala = m_cbSala->currentText();
    QString data = m_deData->date().toString("dd/MM/yyyy");
    QString interval = m_cbInterval->currentText();

    if (m_bib.rezervaSala(m_id_cititor, m_nume_cititor, sala.toStdString(), data.toStdString(), interval.toStdString())) {
        m_bib.salveazaDate();
        QMessageBox::information(this, "Succes", "Sala a fost rezervată cu succes!");
        accept();
    } else {
        QMessageBox::warning(this, "Eroare", "Sala este deja ocupată în acel interval, sau ai atins limita.");
    }
}

// ---------------------------------------------------------
// Dialog Scrie Recenzie
// ---------------------------------------------------------
ScrieRecenzieDialog::ScrieRecenzieDialog(Biblioteca& bib, const std::string& id_cititor, const std::string& nume_cititor, QWidget* parent) 
    : QDialog(parent), m_bib(bib), m_id_cititor(id_cititor), m_nume_cititor(nume_cititor) {
    setWindowTitle("Scrie o Recenzie");
    setMinimumSize(600, 450);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 2, this);
    m_table->setHorizontalHeaderLabels({"Titlu", "Autori"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QFormLayout* form = new QFormLayout();
    m_cbNota = new QComboBox(this);
    m_cbNota->addItems({"1 - Foarte slabă", "2 - Slabă", "3 - Medie", "4 - Bună", "5 - Excelentă"});
    m_cbNota->setCurrentIndex(4); // Default 5 stele
    
    m_leComentariu = new QLineEdit(this);
    m_leComentariu->setPlaceholderText("Scrie părerea ta...");

    form->addRow("Acordă o notă:", m_cbNota);
    form->addRow("Comentariu:", m_leComentariu);
    layout->addLayout(form);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnSalveaza = new QPushButton("Salvează Recenzia", this);
    m_btnSalveaza->setStyleSheet("background-color: #10b981; color: white; padding: 6px; border-radius: 4px;");
    m_btnSalveaza->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnSalveaza);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        m_btnSalveaza->setEnabled(m_table->currentRow() >= 0);
    });
    connect(m_btnSalveaza, &QPushButton::clicked, this, &ScrieRecenzieDialog::onSalveaza);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void ScrieRecenzieDialog::refreshTable() {
    m_carti = m_bib.getToateCartile();
    m_table->setRowCount(m_carti.size());
    for(size_t i=0; i<m_carti.size(); ++i) {
        m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getTitlu())));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_carti[i]->autoriToString())));
    }
}

void ScrieRecenzieDialog::onSalveaza() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    auto isbn = m_carti[row]->getIsbn();
    int nota = m_cbNota->currentIndex() + 1;
    std::string text = m_leComentariu->text().toStdString();
    
    if (m_bib.adaugaRecenzie(isbn, m_id_cititor, m_nume_cititor, nota, text)) {
        QMessageBox::information(this, "Succes", "Recenzia a fost adăugată cu succes!");
        accept();
    } else {
        QMessageBox::warning(this, "Eroare", "A apărut o eroare.");
    }
}

// ---------------------------------------------------------
// Dialog Gestiune Rafturi
// ---------------------------------------------------------
GestiuneRafturiDialog::GestiuneRafturiDialog(Biblioteca& bib, QWidget* parent) 
    : QDialog(parent), m_bib(bib) {
    setWindowTitle("Gestiune Rafturi și Spații");
    setMinimumSize(700, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    
    m_table = new QTableWidget(0, 4, this);
    m_table->setHorizontalHeaderLabels({"Locație (Clădire-Cameră-Culoar-Raft)", "ID Raft", "Stoc / Capacitate Max", "Status"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    layout->addWidget(m_table);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    m_btnAdauga = new QPushButton("Adaugă Raft Nou", this);
    m_btnAdauga->setStyleSheet("background-color: #0ea5e9; color: white; padding: 6px; border-radius: 4px;");
    
    btnLayout->addStretch();
    m_btnSterge = new QPushButton("Șterge Raft Selectat", this);
    m_btnSterge->setStyleSheet("background-color: #ef4444; color: white; padding: 6px; border-radius: 4px;");
    m_btnSterge->setEnabled(false);
    
    m_btnVeziCarti = new QPushButton("Vezi Cărți", this);
    m_btnVeziCarti->setStyleSheet("background-color: #f59e0b; color: white; padding: 6px; border-radius: 4px;");
    m_btnVeziCarti->setEnabled(false);
    
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnAdauga);
    btnLayout->addWidget(m_btnVeziCarti);
    btnLayout->addWidget(m_btnSterge);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, [this](){
        bool hasSelection = m_table->currentRow() >= 0;
        m_btnSterge->setEnabled(hasSelection);
        m_btnVeziCarti->setEnabled(hasSelection);
    });
    connect(m_btnAdauga, &QPushButton::clicked, this, &GestiuneRafturiDialog::onAdauga);
    connect(m_btnSterge, &QPushButton::clicked, this, &GestiuneRafturiDialog::onSterge);
    connect(m_btnVeziCarti, &QPushButton::clicked, this, &GestiuneRafturiDialog::onVeziCarti);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshTable();
}

void GestiuneRafturiDialog::refreshTable() {
    m_rafturi = m_bib.getToateRafturile();
    m_table->setRowCount(m_rafturi.size());
    for(size_t i=0; i<m_rafturi.size(); ++i) {
        Locatie loc = {m_rafturi[i].cladire, m_rafturi[i].camera, m_rafturi[i].culoar, m_rafturi[i].nume_raft};
        int carti = m_bib.obtineNumarCartiFiziceRaft(loc);
        
        QString locationStr = QString::fromStdString(loc.cladire + "-" + loc.camera + "-" + loc.culoar + "-" + loc.raft);
        QString status = (carti >= m_rafturi[i].capacitate_maxima) ? "PLIN" : "LIBER";
        
        m_table->setItem(i, 0, new QTableWidgetItem(locationStr));
        m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_rafturi[i].id)));
        m_table->setItem(i, 2, new QTableWidgetItem(QString::number(carti) + " / " + QString::number(m_rafturi[i].capacitate_maxima)));
        
        auto* stItem = new QTableWidgetItem(status);
        if (status == "PLIN") stItem->setForeground(QBrush(Qt::red));
        else stItem->setForeground(QBrush(Qt::darkGreen));
        
        m_table->setItem(i, 3, stItem);
    }
}

void GestiuneRafturiDialog::onAdauga() {
    QString id = QInputDialog::getText(this, "Adaugă Raft", "ID Unic Raft (ex. RAFT-001):");
    if(id.isEmpty()) return;
    
    QString cladire = QInputDialog::getText(this, "Adaugă Raft", "Clădire:");
    QString camera = QInputDialog::getText(this, "Adaugă Raft", "Camera/Sala:");
    QString culoar = QInputDialog::getText(this, "Adaugă Raft", "Culoar:");
    QString nume = QInputDialog::getText(this, "Adaugă Raft", "Nume Raft (ex. R1):");
    int capacitate = QInputDialog::getInt(this, "Adaugă Raft", "Capacitate Maximă (cărți):", 50, 1, 1000);
    
    if(cladire.isEmpty() || camera.isEmpty() || culoar.isEmpty() || nume.isEmpty()) return;
    
    if(m_bib.adaugaRaft(cladire.toStdString(), camera.toStdString(), culoar.toStdString(), nume.toStdString(), capacitate)) {
        m_bib.salveazaDate();
        refreshTable();
    } else {
        QMessageBox::critical(this, "Eroare", "Există deja un raft cu acest ID.");
    }
}

void GestiuneRafturiDialog::onSterge() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    Locatie loc = {m_rafturi[row].cladire, m_rafturi[row].camera, m_rafturi[row].culoar, m_rafturi[row].nume_raft};
    if (m_bib.obtineNumarCartiFiziceRaft(loc) > 0) {
        QMessageBox::warning(this, "Eroare", "Raftul nu este gol! Mută cărțile pe un alt raft mai întâi.");
        return;
    }
    
    auto reply = QMessageBox::question(this, "Confirmare", "Sigur dorești să ștergi acest raft?");
    if(reply == QMessageBox::Yes) {
        m_bib.stergeRaft(m_rafturi[row].id);
        m_bib.salveazaDate();
        refreshTable();
    }
}

void GestiuneRafturiDialog::onVeziCarti() {
    int row = m_table->currentRow();
    if(row < 0) return;
    
    Locatie loc = {m_rafturi[row].cladire, m_rafturi[row].camera, m_rafturi[row].culoar, m_rafturi[row].nume_raft};
    auto carti = m_bib.getToateCartile();
    QStringList cartiRaft;
    for(const auto& c : carti) {
        if(c->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if(cf) {
                Locatie l = cf->getLocatieRaft();
                if(l.cladire == loc.cladire && l.camera == loc.camera && l.culoar == loc.culoar && l.raft == loc.raft) {
                    cartiRaft << QString::fromStdString("- " + c->getTitlu() + " (Stoc: " + std::to_string(c->getStocDisponibil()) + ")");
                }
            }
        }
    }
    
    QDialog dlg(this);
    dlg.setWindowTitle("Cărți pe raftul selectat");
    dlg.setMinimumSize(400, 300);
    QVBoxLayout* l = new QVBoxLayout(&dlg);
    QListWidget* list = new QListWidget(&dlg);
    if(cartiRaft.isEmpty()) {
        list->addItem("Raftul este gol.");
    } else {
        list->addItems(cartiRaft);
    }
    l->addWidget(list);
    QPushButton* btn = new QPushButton("Închide", &dlg);
    connect(btn, &QPushButton::clicked, &dlg, &QDialog::accept);
    l->addWidget(btn);
    dlg.exec();
}

// ---------------------------------------------------------
// Dialog Mutare Carte Fizică
// ---------------------------------------------------------
MutaCarteFizicaDialog::MutaCarteFizicaDialog(Biblioteca& bib, QWidget* parent) 
    : QDialog(parent), m_bib(bib) {
    setWindowTitle("Mutare Cărți Fizice între Rafturi");
    setMinimumSize(800, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* splitLayout = new QHBoxLayout();

    // Stanga: Cărți Fizice
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addWidget(new QLabel("1. Selectează Cartea Fizică:"));
    m_tableCarti = new QTableWidget(0, 3, this);
    m_tableCarti->setHorizontalHeaderLabels({"ISBN", "Titlu", "Locație Curentă"});
    m_tableCarti->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCarti->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableCarti->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCarti->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    leftLayout->addWidget(m_tableCarti);
    splitLayout->addLayout(leftLayout);

    // Dreapta: Rafturi
    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(new QLabel("2. Selectează Noul Raft:"));
    m_tableRafturi = new QTableWidget(0, 3, this);
    m_tableRafturi->setHorizontalHeaderLabels({"ID Raft", "Locație", "Disponibilitate"});
    m_tableRafturi->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableRafturi->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableRafturi->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableRafturi->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    rightLayout->addWidget(m_tableRafturi);
    splitLayout->addLayout(rightLayout);

    layout->addLayout(splitLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnMuta = new QPushButton("Mută Cartea", this);
    m_btnMuta->setStyleSheet("background-color: #0284c7; color: white; padding: 8px 16px; border-radius: 4px; font-weight: bold;");
    m_btnMuta->setEnabled(false);
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnLayout->addWidget(m_btnMuta);
    btnLayout->addWidget(btnClose);
    layout->addLayout(btnLayout);

    auto checkSelection = [this]() {
        bool hasCarte = m_tableCarti->currentRow() >= 0;
        bool hasRaft = m_tableRafturi->currentRow() >= 0;
        m_btnMuta->setEnabled(hasCarte && hasRaft);
    };

    connect(m_tableCarti, &QTableWidget::itemSelectionChanged, checkSelection);
    connect(m_tableRafturi, &QTableWidget::itemSelectionChanged, checkSelection);
    connect(m_btnMuta, &QPushButton::clicked, this, &MutaCarteFizicaDialog::onMuta);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::reject);

    refreshCarti();
    refreshRafturi();
}

void MutaCarteFizicaDialog::refreshCarti() {
    auto all = m_bib.getToateCartile();
    m_carti.clear();
    for(const auto& c : all) {
        if(c->getTipFormat() == 0) {
            auto cf = std::dynamic_pointer_cast<CarteFizica>(c);
            if(cf) m_carti.push_back(cf);
        }
    }
    m_tableCarti->setRowCount(m_carti.size());
    for(size_t i=0; i<m_carti.size(); ++i) {
        Locatie l = m_carti[i]->getLocatieRaft();
        QString lStr = QString::fromStdString(l.cladire + "-" + l.camera + "-" + l.culoar + "-" + l.raft);
        m_tableCarti->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getIsbn())));
        m_tableCarti->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_carti[i]->getTitlu())));
        m_tableCarti->setItem(i, 2, new QTableWidgetItem(lStr));
    }
}

void MutaCarteFizicaDialog::refreshRafturi() {
    m_rafturi = m_bib.getToateRafturile();
    m_tableRafturi->setRowCount(m_rafturi.size());
    for(size_t i=0; i<m_rafturi.size(); ++i) {
        Locatie loc = {m_rafturi[i].cladire, m_rafturi[i].camera, m_rafturi[i].culoar, m_rafturi[i].nume_raft};
        int carti = m_bib.obtineNumarCartiFiziceRaft(loc);
        QString lStr = QString::fromStdString(loc.cladire + "-" + loc.camera + "-" + loc.culoar + "-" + loc.raft);
        QString disp = QString::number(carti) + " / " + QString::number(m_rafturi[i].capacitate_maxima);
        
        m_tableRafturi->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_rafturi[i].id)));
        m_tableRafturi->setItem(i, 1, new QTableWidgetItem(lStr));
        m_tableRafturi->setItem(i, 2, new QTableWidgetItem(disp));
    }
}

void MutaCarteFizicaDialog::onMuta() {
    int rC = m_tableCarti->currentRow();
    int rR = m_tableRafturi->currentRow();
    if(rC < 0 || rR < 0) return;

    Locatie newLoc = {m_rafturi[rR].cladire, m_rafturi[rR].camera, m_rafturi[rR].culoar, m_rafturi[rR].nume_raft};
    
    // Verificăm dacă depășim capacitatea
    int nrCurent = m_bib.obtineNumarCartiFiziceRaft(newLoc);
    if(nrCurent >= m_rafturi[rR].capacitate_maxima) {
        QMessageBox::critical(this, "Eroare", "Raftul selectat este plin!");
        return;
    }

    m_carti[rC]->setLocatieRaft(newLoc);
    m_bib.salveazaDate();
    
    QMessageBox::information(this, "Succes", "Cartea a fost mutată pe noul raft cu succes.");
    refreshCarti();
    refreshRafturi();
}

// ---------------------------------------------------------
// Implementare ListSelectionDialog
// ---------------------------------------------------------
ListSelectionDialog::ListSelectionDialog(const QString& title, const QStringList& headers, const QVector<QStringList>& data, QWidget* parent) 
    : QDialog(parent)
{
    setWindowTitle(title);
    setMinimumSize(600, 400);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(data.size(), headers.size(), this);
    m_table->setHorizontalHeaderLabels(headers);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    
    // Stretch columns intelligently based on typical data content
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    if (headers.size() >= 3) {
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        for(int i = 1; i < headers.size(); i++) {
            m_table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }
    
    // Make table look elegant like the rest of the app
    m_table->setStyleSheet("QTableWidget { background-color: white; border: 1px solid #e5e5e5; border-radius: 4px; } "
                           "QHeaderView::section { background-color: #f5f5f4; border: none; border-bottom: 1px solid #d6d3d1; padding: 8px; font-weight: bold; }");
    layout->setContentsMargins(20, 20, 20, 20);

    for(int r = 0; r < data.size(); ++r) {
        for(int c = 0; c < headers.size(); ++c) {
            m_table->setItem(r, c, new QTableWidgetItem(data[r][c]));
        }
    }

    m_okBtn = new QPushButton("Selectează", this);
    m_okBtn->setEnabled(false);
    QPushButton* cancelBtn = new QPushButton("Anulează", this);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_okBtn);
    btnLayout->addWidget(cancelBtn);

    layout->addWidget(m_table);
    layout->addLayout(btnLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &ListSelectionDialog::onSelectionChanged);
    connect(m_okBtn, &QPushButton::clicked, this, &ListSelectionDialog::onAccept);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void ListSelectionDialog::onSelectionChanged() {
    m_okBtn->setEnabled(m_table->currentRow() >= 0);
}

void ListSelectionDialog::onAccept() {
    m_selectedIndex = m_table->currentRow();
    accept();
}

CatalogGridDialog::CatalogGridDialog(const QString& title, const std::vector<std::shared_ptr<Carte>>& carti, Biblioteca& bib, std::shared_ptr<Utilizator> user, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(title);
    setMinimumSize(900, 600);
    setStyleSheet("QDialog { background-color: #fdfbf7; }");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QLabel* lTitle = new QLabel(title, this);
    lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 24px; font-weight: bold; color: #292524;");
    mainLayout->addWidget(lTitle);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setStyleSheet("QScrollArea { border: none; background: transparent; }");

    QWidget* container = new QWidget();
    QGridLayout* grid = new QGridLayout(container);
    grid->setSpacing(20);

    int row = 0, col = 0;
    QStringList colors = {"#292524", "#0c4a6e", "#78350f", "#064e3b", "#881337", "#312e81"};

    for (size_t i = 0; i < carti.size(); ++i) {
        auto c = carti[i];
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
        
        std::string isbn = c->getIsbn();
        std::string titlu = c->getTitlu();
        int stoc = c->getStocDisponibil();
        std::string rec_count = std::to_string(bib.getNumarRecenzii(isbn));
        
        connect(btnDet, &QPushButton::clicked, [this, c, &bib, user](){
            DetaliiCarteDialog dlg(c, bib, user, this);
            dlg.exec();
        });

        cl->addWidget(lAuth); 
        cl->addWidget(lTitle); 
        cl->addStretch(); 
        cl->addWidget(lId, 0, Qt::AlignBottom | Qt::AlignLeft);
        cl->addWidget(btnDet);

        grid->addWidget(cCard, row, col);
        col++; if (col >= 4) { col = 0; row++; }
    }
    
    scroll->setWidget(container);
    mainLayout->addWidget(scroll);
    
    QPushButton* btnClose = new QPushButton("Închide", this);
    btnClose->setStyleSheet("padding: 8px; background-color: #e5e5e5; border-radius: 4px; font-weight:bold;");
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout* bl = new QHBoxLayout();
    bl->addStretch();
    bl->addWidget(btnClose);
    mainLayout->addLayout(bl);
}

DetaliiCarteDialog::DetaliiCarteDialog(const std::shared_ptr<Carte>& carte, Biblioteca& bib, std::shared_ptr<Utilizator> user, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Detalii Carte");
    setMinimumSize(700, 500);
    setStyleSheet("QDialog { background-color: #fdfbf7; }");

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Coloana Stânga: Copertă 3D & Acțiuni
    QFrame* leftCol = new QFrame(this);
    leftCol->setFixedWidth(260);
    leftCol->setStyleSheet("background-color: #292524;");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftCol);
    leftLayout->setContentsMargins(30, 40, 30, 40);
    
    QFrame* cover3D = new QFrame(leftCol);
    cover3D->setFixedSize(160, 240);
    cover3D->setStyleSheet("background-color: #b45309; border-radius: 4px; border-left: 8px solid rgba(0,0,0,0.4);");
    leftLayout->addWidget(cover3D, 0, Qt::AlignHCenter);
    
    leftLayout->addSpacing(30);
    
    QPushButton* btnImprumut = new QPushButton("Împrumută", leftCol);
    btnImprumut->setStyleSheet("QPushButton { background-color: #10b981; color: white; border-radius: 4px; padding: 10px; font-weight: bold; border-bottom: 3px solid #059669; } QPushButton:hover { background-color: #059669; }");
    QPushButton* btnRezerva = new QPushButton("Rezervă", leftCol);
    btnRezerva->setStyleSheet("QPushButton { background-color: #3b82f6; color: white; border-radius: 4px; padding: 10px; font-weight: bold; border-bottom: 3px solid #2563eb; } QPushButton:hover { background-color: #2563eb; }");
    QPushButton* btnRecenzie = new QPushButton("Scrie Recenzie", leftCol);
    btnRecenzie->setStyleSheet("QPushButton { background-color: #eab308; color: #422006; border-radius: 4px; padding: 10px; font-weight: bold; border-bottom: 3px solid #ca8a04; } QPushButton:hover { background-color: #ca8a04; }");
    QPushButton* btnVeziRecenzii = new QPushButton("Vezi Recenzii", leftCol);
    btnVeziRecenzii->setStyleSheet("QPushButton { background-color: #f3f4f6; color: #374151; border-radius: 4px; padding: 10px; font-weight: bold; border-bottom: 3px solid #d1d5db; } QPushButton:hover { background-color: #d1d5db; }");
    
    leftLayout->addWidget(btnImprumut);
    leftLayout->addWidget(btnRezerva);
    leftLayout->addWidget(btnRecenzie);
    leftLayout->addWidget(btnVeziRecenzii);
    leftLayout->addStretch();
    
    connect(btnVeziRecenzii, &QPushButton::clicked, [this, &bib, carte]() {
        QDialog dlg(this);
        dlg.setWindowTitle("Recenzii: " + QString::fromStdString(carte->getTitlu()));
        dlg.setMinimumSize(500, 400);
        QVBoxLayout* l = new QVBoxLayout(&dlg);
        QTableWidget* tw = new QTableWidget(0, 3, &dlg);
        tw->setHorizontalHeaderLabels({"Cititor", "Notă", "Comentariu"});
        tw->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        tw->setSelectionMode(QAbstractItemView::NoSelection);
        tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
        
        auto recenzii = bib.getToateRecenziile();
        for (const auto& r : recenzii) {
            if (r.isbn == carte->getIsbn()) {
                int row = tw->rowCount();
                tw->insertRow(row);
                tw->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(r.nume_cititor)));
                
                QString stars = "";
                for(int i=0; i<r.nota; i++) stars += "★";
                for(int i=r.nota; i<5; i++) stars += "☆";
                tw->setItem(row, 1, new QTableWidgetItem(stars));
                
                tw->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(r.text)));
            }
        }
        l->addWidget(tw);
        if (tw->rowCount() == 0) {
            QMessageBox::information(this, "Info", "Nu există nicio recenzie pentru această carte.");
            return;
        }
        dlg.exec();
    });
    
    connect(btnImprumut, &QPushButton::clicked, [this, &bib, carte, user](){
        std::string targetId = "";
        if (user && user->getTip() == "CITITOR") {
            targetId = user->getId();
        } else {
            bool ok;
            QString input = QInputDialog::getText(this, "Împrumută", "Introduceți ID Cititor:", QLineEdit::Normal, "", &ok);
            if (!ok || input.trimmed().isEmpty()) return;
            targetId = input.trimmed().toStdString();
        }
        if (targetId.empty()) return;
        
        if (bib.calculeazaPenalizariTotale(targetId) > 0) {
            QMessageBox::critical(this, "Atenție", "Acest cititor are penalizări neachitate și nu poate împrumuta cărți!");
            return;
        }
        
        std::string imp = bib.getDataCurentaStr();
        std::string ret = bib.getDataCurentaPlusStr(14);
        
        if (bib.adaugaImprumut(carte->getIsbn(), targetId, imp, ret, user && user->getTip() == "CITITOR" ? "Tonomat" : "Manual")) {
            QMessageBox::information(this, "Succes", "Cartea a fost împrumutată cu succes!");
            this->accept();
        } else {
            QMessageBox::critical(this, "Eroare", "Stoc insuficient sau limită atinsă!");
        }
    });

    connect(btnRezerva, &QPushButton::clicked, [this, &bib, carte, user](){
        std::string targetId = "";
        if (user && user->getTip() == "CITITOR") {
            targetId = user->getId();
            auto cit = std::dynamic_pointer_cast<Cititor>(user);
            if (cit && cit->getPenalizari() > 0) {
                QMessageBox::warning(this, "Eroare", "Nu poți rezerva! Ai amenzi neplătite.");
                return;
            }
        } else {
            bool ok;
            QString input = QInputDialog::getText(this, "Rezervă", "Introduceți ID Cititor:", QLineEdit::Normal, "", &ok);
            if (!ok || input.trimmed().isEmpty()) return;
            targetId = input.trimmed().toStdString();
        }
        
        if(bib.adaugaRezervare(carte->getIsbn(), targetId)) {
            QMessageBox::information(this, "Succes", "Cartea a fost rezervată timp de 3 zile.");
        } else {
            QMessageBox::warning(this, "Eroare", "A apărut o eroare la rezervare.");
        }
    });

    connect(btnRecenzie, &QPushButton::clicked, [this, &bib, carte, user](){
        std::string targetId = user ? user->getId() : "Necunoscut";
        std::string targetNume = user ? user->getNumeComplet() : "Anonim";
        
        if (user && user->getTip() != "CITITOR") {
             bool ok;
             QString input = QInputDialog::getText(this, "ID Cititor", "Introduceți ID Cititor pentru recenzie:", QLineEdit::Normal, "", &ok);
             if (!ok || input.trimmed().isEmpty()) return;
             targetId = input.trimmed().toStdString();
             auto gasit = bib.gasesteUtilizator(targetId);
             targetNume = gasit ? gasit->getNumeComplet() : "Cititor";
        }
        
        bool okNota;
        int nota = QInputDialog::getInt(this, "Recenzie", "Nota (1-5):", 5, 1, 5, 1, &okNota);
        if (!okNota) return;
        
        bool okText;
        QString text = QInputDialog::getText(this, "Recenzie", "Comentariu:", QLineEdit::Normal, "", &okText);
        if (!okText) return;
        
        if (bib.adaugaRecenzie(carte->getIsbn(), targetId, targetNume, nota, text.toStdString())) {
            QMessageBox::information(this, "Succes", "Recenzia a fost adăugată!");
        } else {
            QMessageBox::warning(this, "Eroare", "A apărut o eroare la adăugarea recenziei.");
        }
    });

    // Coloana Dreapta: Informații Textuale
    QWidget* rightCol = new QWidget(this);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightCol);
    rightLayout->setContentsMargins(40, 40, 40, 40);
    rightLayout->setSpacing(10);
    
    QLabel* lCateg = new QLabel(QString::fromStdString(carte->getCategorie()).toUpper(), rightCol);
    lCateg->setStyleSheet("font-family: 'Inter', sans-serif; font-size: 12px; font-weight: bold; color: #b45309; letter-spacing: 2px;");
    
    QLabel* lTitle = new QLabel(QString::fromStdString(carte->getTitlu()), rightCol);
    lTitle->setStyleSheet("font-family: 'Georgia', serif; font-size: 32px; font-weight: bold; color: #1c1917;");
    lTitle->setWordWrap(true);
    
    QLabel* lAuth = new QLabel("de " + QString::fromStdString(carte->autoriToString()), rightCol);
    lAuth->setStyleSheet("font-family: 'Georgia', serif; font-size: 18px; font-style: italic; color: #57534e;");
    
    int stoc = carte->getStocDisponibil();
    QLabel* lStoc = new QLabel(stoc > 0 ? QString("Disponibil (%1 ex.)").arg(stoc) : "Stoc Epuizat", rightCol);
    lStoc->setStyleSheet(QString("font-weight: bold; padding: 4px 8px; border-radius: 4px; color: white; background-color: %1;").arg(stoc > 0 ? "#10b981" : "#ef4444"));
    lStoc->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    rightLayout->addWidget(lCateg);
    rightLayout->addWidget(lTitle);
    rightLayout->addWidget(lAuth);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(lStoc);
    
    rightLayout->addSpacing(20);
    
    QFrame* infoBox = new QFrame(rightCol);
    infoBox->setStyleSheet("background-color: white; border-radius: 8px; border: 1px solid #e5e5e5;");
    QFormLayout* form = new QFormLayout(infoBox);
    form->setContentsMargins(20, 20, 20, 20);
    form->setSpacing(15);
    
    auto addRow = [&](const QString& lbl, const QString& val) {
        QLabel* label = new QLabel(lbl, infoBox); label->setStyleSheet("font-weight: bold; color: #78716c;");
        QLabel* value = new QLabel(val, infoBox); value->setStyleSheet("font-weight: bold; color: #292524;");
        form->addRow(label, value);
    };
    
    addRow("ISBN", QString::fromStdString(carte->getIsbn()));
    addRow("Anul Apariției", QString::number(carte->getAnAparitie()));
    addRow("Tip Categorie", QString::fromStdString(carte->getCategorie()));
    
    int rat = bib.calculeazaRatingMediu(carte->getIsbn());
    QString stars = "";
    for(int i=0; i<5; i++) stars += (i < rat) ? "★ " : "☆ ";
    addRow("Rating", stars + QString(" (%1 recenzii)").arg(bib.getNumarRecenzii(carte->getIsbn())));
    
    rightLayout->addWidget(infoBox);
    rightLayout->addStretch();
    
    QPushButton* btnClose = new QPushButton("Închide Detalii", rightCol);
    btnClose->setStyleSheet("QPushButton { background-color: transparent; border: 2px solid #d6d3d1; color: #57534e; font-weight: bold; border-radius: 4px; padding: 8px 16px; } QPushButton:hover { background-color: #f5f5f4; color: #292524; }");
    btnClose->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout* bl = new QHBoxLayout();
    bl->addStretch();
    bl->addWidget(btnClose);
    rightLayout->addLayout(bl);
    
    mainLayout->addWidget(leftCol);
    mainLayout->addWidget(rightCol);
}

namespace QtFunc {
    void adaugaCarteDigitala(QWidget* parent, Biblioteca& bib) {
        AdaugaCarteDigitalaDialog dlg(bib, parent);
        dlg.exec();
    }

    void adaugaCarteAudio(QWidget* parent, Biblioteca& bib) {
        AdaugaCarteAudioDialog dlg(bib, parent);
        dlg.exec();
    }

    void stergeCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        StergeCarteDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }
    
    void restaureazaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        RestaureazaCarteDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void modificaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        SelectieModificareCarteDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void cautaCarte(QWidget* parent, Biblioteca& bib, std::shared_ptr<Utilizator> user, const QString& defaultKeyword) {
        std::vector<std::shared_ptr<Carte>> rez;
        
        if (!defaultKeyword.isEmpty()) {
            auto r1 = bib.cautaDupaNume(defaultKeyword.toStdString());
            auto r2 = bib.cautaDupaAutor(defaultKeyword.toStdString());
            for(auto& c : r1) rez.push_back(c);
            for(auto& c : r2) {
                bool found = false;
                for(auto& rc : rez) if(rc->getIsbn() == c->getIsbn()) found = true;
                if(!found) rez.push_back(c);
            }
        } else {
            QDialog dlg(parent);
            dlg.setWindowTitle("Căutare Avansată");
            dlg.setMinimumWidth(400);
            QFormLayout* form = new QFormLayout(&dlg);
            QComboBox* cb = new QComboBox(&dlg);
            cb->addItems({"După Titlu", "După Autor", "După Categorie", "După Editură"});
            QLineEdit* le = new QLineEdit(&dlg);
            form->addRow("Criteriu:", cb);
            form->addRow("Termen:", le);
            QPushButton* btn = new QPushButton("Caută", &dlg);
            form->addWidget(btn);
            QObject::connect(btn, &QPushButton::clicked, &dlg, &QDialog::accept);
            
            if(dlg.exec() != QDialog::Accepted || le->text().isEmpty()) return;
            
            QString q = le->text();
            int idx = cb->currentIndex();
            if(idx == 0) rez = bib.cautaDupaNume(q.toStdString());
            else if(idx == 1) rez = bib.cautaDupaAutor(q.toStdString());
            else if(idx == 2) rez = bib.cautaDupaCategorie(q.toStdString());
            else rez = bib.cautaDupaEditura(q.toStdString());
        }

        if (rez.empty()) {
            QMessageBox::information(parent, "Căutare", "Nu s-au găsit cărți.");
            return;
        }

        CatalogGridDialog dlgRez("Rezultate Căutare", rez, bib, user, parent);
        dlgRez.exec();
    }

    void mutaCarteFizica(QWidget* parent, Biblioteca& bib) {
        MutaCarteFizicaDialog dlg(bib, parent);
        dlg.exec();
    }

    void gestiuneRafturi(QWidget* parent, Biblioteca& bib) {
        GestiuneRafturiDialog dlg(bib, parent);
        dlg.exec();
    }
    
    void stergeUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        StergeUtilizatorDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void restaureazaUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        RestaureazaUtilizatorDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void modificaUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        SelectieModificareUtilizatorDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void imprumutaCarte(QWidget* parent, Biblioteca& bib) {
        ImprumutaCarteDialog dlg(bib, "", parent);
        dlg.exec();
    }

    void returneazaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        ReturneazaCarteDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void rezervaCarte(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto u = bib.gasesteUtilizator(idCititor);
        if(u && u->getRol() == "CITITOR") {
            auto cit = std::dynamic_pointer_cast<Cititor>(u);
            if(cit && cit->getPenalizari() > 0) {
                QMessageBox::warning(parent, "Eroare", "Nu poți rezerva! Ai amenzi neplătite.");
                return;
            }
            if(cit && !cit->poateImprumuta()) {
                QMessageBox::warning(parent, "Eroare", "Nu poți rezerva! Limita atinsă sau cont inactiv.");
                return;
            }
        }
        
        RezervaCarteDialog dlg(bib, idCititor, parent);
        dlg.exec();
    }

    void rezervaSala(QWidget* parent, Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor) {
        RezervaSalaDialog dlg(bib, idCititor, numeCititor, parent);
        dlg.exec();
    }

    void scrieRecenzie(QWidget* parent, Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor) {
        ScrieRecenzieDialog dlg(bib, idCititor, numeCititor, parent);
        dlg.exec();
    }

    void confirmaRidicarRezervare(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        ConfirmaRezervariDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void toateImprumuturile(QWidget* parent, Biblioteca& bib) {
        auto imprumuturi = bib.getToateImprumuturile();
        QStringList headers = {"Titlu", "Cititor", "Data Imp", "Termen"};
        QVector<QStringList> rows;
        for(const auto& i : imprumuturi) {
            auto c = bib.gasesteCarte(i.getIdCarte());
            auto u = bib.gasesteUtilizator(i.getIdCititor());
            QString titlu = c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(i.getIdCarte());
            QString nume = u ? QString::fromStdString(u->getNumeComplet()) : QString::fromStdString(i.getIdCititor());
            rows.append({titlu, nume, QString::fromStdString(i.getDataImprumut()), QString::fromStdString(i.getTermenLimita())});
        }
        ListSelectionDialog dlg("Toate Împrumuturile", headers, rows, parent);
        dlg.exec(); // vizualizare
    }

    void modificaProfil(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        auto u = bib.gasesteUtilizator(id_actor);
        if(!u) return;

        EditeazaUtilizatorDialog dlg(bib, u, id_actor, parent);
        dlg.exec();
    }

    void imprumutaCarteCititor(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto carti = bib.getToateCartile();
        QStringList hCarti = {"Nr", "Titlu", "Stoc"};
        QVector<QStringList> rCarti;
        for(size_t i=0; i<carti.size(); ++i) rCarti.append({QString::number(i+1), QString::fromStdString(carti[i]->getTitlu()), QString::number(carti[i]->getStocDisponibil())});
        
        ListSelectionDialog dlgCarti("Alege Cartea de Împrumutat", hCarti, rCarti, parent);
        if (dlgCarti.exec() == QDialog::Accepted) {
            if (bib.calculeazaPenalizariTotale(idCititor) > 0) {
                QMessageBox::critical(parent, "Atenție", "Ai penalizări neachitate. Achită-le înainte de a împrumuta!");
                return;
            }
            
            int sel = dlgCarti.getSelectedIndex();
            std::string imp = bib.getDataCurentaStr();
            std::string ret = bib.getDataCurentaPlusStr(14);
            if(bib.adaugaImprumut(carti[sel]->getIsbn(), idCititor, imp, ret, "Tonomat")) {
                QMessageBox::information(parent, "Succes", "Ai împrumutat cartea!");
            } else {
                QMessageBox::critical(parent, "Eroare", "Stoc epuizat sau ai atins limita de cărți!");
            }
        }
    }

    void returneazaCarteCititor(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        std::vector<Imprumut> imp;
        for (const auto& i : bib.getToateImprumuturile()) {
            if (i.getIdCititor() == idCititor) imp.push_back(i);
        }
        if(imp.empty()) { QMessageBox::information(parent, "Info", "Nu ai nicio carte împrumutată."); return; }
        
        QStringList h = {"Titlu", "Termen"};
        QVector<QStringList> r;
        for(auto& i : imp) {
            auto c = bib.gasesteCarte(i.getIdCarte());
            r.append(QStringList{c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(i.getIdCarte()), QString::fromStdString(i.getTermenLimita())});
        }
        
        ListSelectionDialog dlg("Returnare Carte", h, r, parent);
        if (dlg.exec() == QDialog::Accepted) {
            if(bib.solicitaReturnare(imp[dlg.getSelectedIndex()].getIdCarte(), idCititor)) {
                QMessageBox::information(parent, "Succes", "Ai solicitat returnarea! Te rugăm să predai cartea fizic la bibliotecă pentru confirmare.");
            } else {
                QMessageBox::warning(parent, "Eroare", "Cartea a fost deja solicitată pentru returnare sau a apărut o eroare.");
            }
        }
    }

    void veziImprumuturileMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        std::vector<Imprumut> imp;
        for (const auto& i : bib.getToateImprumuturile()) {
            if (i.getIdCititor() == idCititor) imp.push_back(i);
        }
        if(imp.empty()) { QMessageBox::information(parent, "Info", "Nu ai nicio carte împrumutată."); return; }
        QStringList h = {"Titlu", "Data", "Termen"};
        QVector<QStringList> r;
        for(auto& i : imp) {
            auto c = bib.gasesteCarte(i.getIdCarte());
            r.append(QStringList{c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(i.getIdCarte()), QString::fromStdString(i.getDataImprumut()), QString::fromStdString(i.getTermenLimita())});
        }
        ListSelectionDialog dlg("Împrumuturile Mele", h, r, parent);
        dlg.exec();
    }

    void veziIstoricLectura(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto istoric = bib.getToateIstoricLectura();
        QStringList h = {"ISBN", "Titlu", "Data Împrumut", "Data Returnare"};
        QVector<QStringList> r;
        for(const auto& x : istoric) {
            if(x.id_cititor == idCititor) {
                r.append(QStringList{QString::fromStdString(x.isbn), QString::fromStdString(x.titlu_carte), QString::fromStdString(x.data_imprumut), QString::fromStdString(x.data_returnare)});
            }
        }
        if(r.isEmpty()) { QMessageBox::information(parent, "Info", "Nu ai istoric de lectură."); return; }
        ListSelectionDialog dlg("Istoric Lectură", h, r, parent);
        dlg.exec();
    }

    void veziRecenziileMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto rec = bib.getToateRecenziile();
        QStringList h = {"ISBN", "Titlu Carte", "Nota", "Comentariu"};
        QVector<QStringList> r;
        for(const auto& x : rec) {
            if(x.id_cititor == idCititor) {
                auto c = bib.gasesteCarte(x.isbn);
                QString titlu = c ? QString::fromStdString(c->getTitlu()) : "Necunoscut";
                r.append(QStringList{QString::fromStdString(x.isbn), titlu, QString::number(x.nota) + "/5", QString::fromStdString(x.text)});
            }
        }
        if(r.isEmpty()) { QMessageBox::information(parent, "Info", "Nu ai adăugat nicio recenzie."); return; }
        ListSelectionDialog dlg("Recenziile Mele", h, r, parent);
        dlg.exec();
    }

    void veziRezervariCartiMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto rez = bib.getRezervari();
        QStringList h = {"ISBN", "Titlu", "Data Rezervare", "Expirare", "Status"};
        QVector<QStringList> r;
        for(const auto& x : rez) {
            if(x.id_cititor == idCititor) {
                auto c = bib.gasesteCarte(x.isbn);
                QString titlu = c ? QString::fromStdString(c->getTitlu()) : "Necunoscut";
                QString status = x.alocata ? "GATA DE RIDICAT" : "ÎN AȘTEPTARE";
                r.append(QStringList{QString::fromStdString(x.isbn), titlu, QString::fromStdString(x.data_rezervare), QString::fromStdString(x.data_expirare), status});
            }
        }
        if(r.isEmpty()) { QMessageBox::information(parent, "Info", "Nu ai rezervări de cărți."); return; }
        ListSelectionDialog dlg("Rezervările Mele (Cărți)", h, r, parent);
        dlg.exec();
    }

    void veziRezervariSaliMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor) {
        auto rez = bib.getToateRezervariSali();
        QStringList h = {"Sala", "Data", "Ora"};
        QVector<QStringList> r;
        for(auto& x : rez) {
            if(x.id_cititor == idCititor) {
                r.append(QStringList{QString::fromStdString(x.sala), QString::fromStdString(x.data), QString::fromStdString(x.interval_orar)});
            }
        }
        if(r.isEmpty()) { QMessageBox::information(parent, "Info", "Nu ai rezervări."); return; }
        ListSelectionDialog dlg("Rezervările Mele", h, r, parent);
        dlg.exec();
    }

    void solicitaPlata(QWidget* parent, Biblioteca& bib, const std::shared_ptr<Utilizator>& u) {
        if(!u) return;
        
        auto cit = std::dynamic_pointer_cast<Cititor>(u);
        if(cit) {
            double static_pen = cit->getPenalizari();
            double total = bib.calculeazaPenalizariTotale(u->getId());
            
            if(total > 0) {
                QString detalii = "Ai de plată suma de <b>" + QString::number(total) + " RON</b>.<br><br><b>Detalii penalizări:</b><br>";
                if (static_pen > 0) {
                    detalii += "- Daune sau cărți pierdute: " + QString::number(static_pen) + " RON<br>";
                }
                
                for (const auto& imp : bib.getToateImprumuturile()) {
                    if (imp.getIdCititor() == u->getId()) {
                        int zile = bib.calculeazaZileIntarziere(imp.getTermenLimita());
                        if (zile > 0) {
                            auto c = bib.gasesteCarte(imp.getIdCarte());
                            QString titlu = c ? QString::fromStdString(c->getTitlu()) : QString::fromStdString(imp.getIdCarte());
                            detalii += "- Întârziere carte '" + titlu + "': " + QString::number(zile) + " zile (" + QString::number(zile) + " RON)<br>";
                        }
                    }
                }
                
                detalii += "<br>Dorești să inițiezi plata acum?";
                
                QMessageBox msgBox(parent);
                msgBox.setWindowTitle("Plată Amendă");
                msgBox.setTextFormat(Qt::RichText);
                msgBox.setText(detalii);
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                
                if(msgBox.exec() == QMessageBox::Yes) {
                    if(bib.solicitaPlata(u->getId()))
                        QMessageBox::information(parent, "Succes", "Plata pentru penalizări a fost solicitată și va fi procesată de administrație.");
                }
            } else {
                QMessageBox::information(parent, "Info", "Nu ai penalizări de plată. Ești la zi!");
            }
        }
    }

    void reconditioneazaCarti(QWidget* parent, Biblioteca& bib) {
        ReconditioneazaCartiDialog dlg(bib, parent);
        dlg.exec();
    }
    
    void simuleazaTimp(QWidget* parent, Biblioteca& bib) {
        bool ok;
        int zile = QInputDialog::getInt(parent, "Simulare Timp", "Introduceți numărul de zile pentru a avansa timpul:", 30, 1, 3650, 1, &ok);
        if (ok) {
            bib.simuleazaTrecereTimp(zile);
            
            time_t virtual_time = bib.getVirtualTime();
            struct tm* ti = localtime(&virtual_time);
            char buf[80];
            strftime(buf, sizeof(buf), "%d/%m/%Y", ti);
            
            QMessageBox::information(parent, "Succes", QString("Timpul a fost avansat cu %1 zile.\nData curentă în aplicație: %2").arg(zile).arg(buf));
        }
    }

    void platesteSalarii(QWidget* parent, Biblioteca& bib) {
        double salarii = bib.calculeazaSalariiTotale();
        if (salarii <= 0) {
            QMessageBox::information(parent, "Info", "Nu există angajați sau salarii de plată.");
            return;
        }
        auto reply = QMessageBox::question(parent, "Plată Salarii", QString("Total salarii angajați: %1 RON\nDoriți să efectuați plata salariilor din bugetul curent?").arg(salarii), QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            std::string msg = bib.platesteSalarii();
            QMessageBox::information(parent, "Info", QString::fromStdString(msg));
        }
    }


    void casareCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        CasareCarteDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void achizitii(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        AchizitiiDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void confirmaPlati(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        ConfirmaPlatiDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }

    void confirmaReturnari(QWidget* parent, Biblioteca& bib, const std::string& id_actor) {
        ConfirmaReturnariDialog dlg(bib, id_actor, parent);
        dlg.exec();
    }


}
