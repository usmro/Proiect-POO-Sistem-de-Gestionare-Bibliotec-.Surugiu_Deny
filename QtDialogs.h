#ifndef QTDIALOGS_H
#define QTDIALOGS_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableWidget>
#include <QMessageBox>
#include <QLabel>
#include <QDateEdit>
#include <memory>
#include "Biblioteca.h"
#include "Utilizator.h"

// ---------------------------------------------------------
// Dialog Adăugare Carte Fizică
// ---------------------------------------------------------
// ---------------------------------------------------------
// Dialog Adăugare Carte Fizică
// ---------------------------------------------------------
class AdaugaCarteFizicaDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdaugaCarteFizicaDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    QLineEdit* m_titluEdit;
    QLineEdit* m_autoriEdit;
    QLineEdit* m_edituraEdit;
    QSpinBox* m_anEdit;
    QSpinBox* m_paginiEdit;
    QDoubleSpinBox* m_pretEdit;
    QSpinBox* m_stocEdit;
    QComboBox* m_categorieCombo;
    QComboBox* m_stareCombo;
    QComboBox* m_copertaCombo;
    QComboBox* m_raftCombo;
};

// ---------------------------------------------------------
// Dialog Adăugare Utilizator
// ---------------------------------------------------------
class AdaugaCarteDigitalaDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdaugaCarteDigitalaDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    QLineEdit* m_titluEdit;
    QLineEdit* m_autoriEdit;
    QLineEdit* m_edituraEdit;
    QSpinBox* m_anEdit;
    QSpinBox* m_paginiEdit;
    QDoubleSpinBox* m_pretEdit;
    QComboBox* m_categorieCombo;
    QLineEdit* m_formatEdit;
    QLineEdit* m_linkEdit;
};

class AdaugaCarteAudioDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdaugaCarteAudioDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    QLineEdit* m_titluEdit;
    QLineEdit* m_autoriEdit;
    QLineEdit* m_edituraEdit;
    QSpinBox* m_anEdit;
    QDoubleSpinBox* m_pretEdit;
    QComboBox* m_categorieCombo;
    QLineEdit* m_naratorEdit;
    QSpinBox* m_minuteEdit;
    QLineEdit* m_linkEdit;
};

class StergeCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit StergeCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onTypeChanged(int index);
    void onSterge();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QComboBox* m_typeCombo;
    QTableWidget* m_table;
    QPushButton* m_btnSterge;
    std::vector<std::shared_ptr<Carte>> m_cartiCurente;
};

class RestaureazaCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit RestaureazaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onRestaureaza();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnRestaureaza;
    std::vector<std::shared_ptr<Carte>> m_cartiCurente;
};

class SelectieModificareCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit SelectieModificareCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onTypeChanged(int index);
    void onModifica();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QComboBox* m_typeCombo;
    QTableWidget* m_table;
    QPushButton* m_btnModifica;
    std::vector<std::shared_ptr<Carte>> m_cartiCurente;
};

class EditeazaCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditeazaCarteDialog(Biblioteca& bib, std::shared_ptr<Carte> carte, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    std::shared_ptr<Carte> m_carte;
    std::string m_id_actor;

    QLineEdit* m_titluEdit;
    QLineEdit* m_autoriEdit;
    QLineEdit* m_edituraEdit;
    QSpinBox* m_anEdit;
    QDoubleSpinBox* m_pretEdit;
    QComboBox* m_categorieCombo;
    
    // Specifice
    QSpinBox* m_paginiEdit;
    QSpinBox* m_stocEdit;
    QCheckBox* m_defecteEdit;
    QLineEdit* m_locatieEdit;
    QComboBox* m_copertaCombo;
    
    QLineEdit* m_formatEdit;
    QLineEdit* m_linkEdit;
    
    QLineEdit* m_naratorEdit;
    QSpinBox* m_minuteEdit;
};

class AdaugaUtilizatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit AdaugaUtilizatorDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    QComboBox* m_tipCombo;
    QLineEdit* m_numeEdit;
    QLineEdit* m_prenumeEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_telefonEdit;
    QLineEdit* m_parolaEdit;
};

class StergeUtilizatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit StergeUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onTypeChanged(int index);
    void onSterge();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QComboBox* m_typeCombo;
    QTableWidget* m_table;
    QPushButton* m_btnSterge;
    std::vector<std::shared_ptr<Utilizator>> m_utilsCurenti;
};

class RestaureazaUtilizatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit RestaureazaUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onRestaureaza();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnRestaureaza;
    std::vector<std::shared_ptr<Utilizator>> m_utilsCurenti;
};

class SelectieModificareUtilizatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit SelectieModificareUtilizatorDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onTypeChanged(int index);
    void onModifica();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QComboBox* m_typeCombo;
    QTableWidget* m_table;
    QPushButton* m_btnModifica;
    std::vector<std::shared_ptr<Utilizator>> m_utilsCurenti;
};

class EditeazaUtilizatorDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditeazaUtilizatorDialog(Biblioteca& bib, std::shared_ptr<Utilizator> util, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onSave();
private:
    Biblioteca& m_bib;
    std::shared_ptr<Utilizator> m_util;
    std::string m_id_actor;

    QLineEdit* m_numeEdit;
    QLineEdit* m_prenumeEdit;
    QLineEdit* m_emailEdit;
    QLineEdit* m_telefonEdit;
    QLineEdit* m_adresaEdit;
    QLineEdit* m_parolaEdit;
};

// ---------------------------------------------------------
// Dialog Vizualizare Catalog / Utilizatori (Tabel)
// ---------------------------------------------------------
class VizualizareTableDialog : public QDialog {
    Q_OBJECT
public:
    explicit VizualizareTableDialog(const QString& title, QWidget* parent = nullptr);
    void setHeaders(const QStringList& headers);
    void addRow(const QStringList& rowData);
    void adjustColumns();
private:
    QTableWidget* m_table;
};

// ---------------------------------------------------------
// Dialog Generic de Selecție din Tabel (Echivalent showPaginatedTable)
// ---------------------------------------------------------
class ListSelectionDialog : public QDialog {
    Q_OBJECT
public:
    ListSelectionDialog(const QString& title, const QStringList& headers, const QVector<QStringList>& data, QWidget* parent = nullptr);
    int getSelectedIndex() const { return m_selectedIndex; }
private slots:
    void onSelectionChanged();
    void onAccept();
private:
    QTableWidget* m_table;
    QPushButton* m_okBtn;
    int m_selectedIndex = -1;
};

class CatalogGridDialog : public QDialog {
    Q_OBJECT
public:
    CatalogGridDialog(const QString& title, const std::vector<std::shared_ptr<Carte>>& carti, Biblioteca& bib, std::shared_ptr<Utilizator> user, QWidget* parent = nullptr);
};

class DetaliiCarteDialog : public QDialog {
    Q_OBJECT
public:
    DetaliiCarteDialog(const std::shared_ptr<Carte>& carte, Biblioteca& bib, std::shared_ptr<Utilizator> user, QWidget* parent = nullptr);
};

// ---------------------------------------------------------
// DIALOGURI ÎMPRUMUTURI ȘI RETURURI
// ---------------------------------------------------------

class ImprumutaCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit ImprumutaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onImprumuta();
private:
    void refreshCarti();
    void refreshUtilizatori();

    Biblioteca& m_bib;
    std::string m_id_actor;
    
    QTableWidget* m_tableCarti;
    QTableWidget* m_tableCititori;
    QPushButton* m_btnImprumuta;

    std::vector<std::shared_ptr<Carte>> m_carti;
    std::vector<std::shared_ptr<Utilizator>> m_cititori;
};

class ReturneazaCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReturneazaCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onReturneaza();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnReturneaza;
    QPushButton* m_btnReturneazaDefecta;
    std::vector<Imprumut> m_imprumuturi;
};

class ConfirmaReturnariDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConfirmaReturnariDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onConfirma();
    void onRefuza();
private:
    void refreshTable();

    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnConfirma;
    QPushButton* m_btnRefuza;
};

class ConfirmaPlatiDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConfirmaPlatiDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onConfirma();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnConfirma;
};

class ReconditioneazaCartiDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReconditioneazaCartiDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void peReparata();
    void peCasata();
private:
    void refreshTable();
    Biblioteca& m_bib;
    QTableWidget* m_table;
    QPushButton* m_btnReparata;
    QPushButton* m_btnCasata;
};

class CasareCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit CasareCarteDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onCaseaza();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnCaseaza;
};

class AchizitiiDialog : public QDialog {
    Q_OBJECT
public:
    explicit AchizitiiDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onAchizitie();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QSpinBox* m_spinCantitate;
    QPushButton* m_btnAchizitie;
};

class ConfirmaRezervariDialog : public QDialog {
    Q_OBJECT
public:
    explicit ConfirmaRezervariDialog(Biblioteca& bib, const std::string& id_actor, QWidget* parent = nullptr);
private slots:
    void onConfirma();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_actor;
    QTableWidget* m_table;
    QPushButton* m_btnConfirma;
    std::vector<Rezervare> m_toate_rezervarile;
    std::vector<size_t> m_real_indices;
};

class RezervaCarteDialog : public QDialog {
    Q_OBJECT
public:
    explicit RezervaCarteDialog(Biblioteca& bib, const std::string& id_cititor, QWidget* parent = nullptr);
private slots:
    void onRezerva();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_cititor;
    QTableWidget* m_table;
    QPushButton* m_btnRezerva;
    std::vector<std::shared_ptr<Carte>> m_carti;
};

class RezervaSalaDialog : public QDialog {
    Q_OBJECT
public:
    explicit RezervaSalaDialog(Biblioteca& bib, const std::string& id_cititor, const std::string& nume_cititor, QWidget* parent = nullptr);
private slots:
    void onRezerva();
private:
    Biblioteca& m_bib;
    std::string m_id_cititor;
    std::string m_nume_cititor;
    QComboBox* m_cbSala;
    QDateEdit* m_deData;
    QComboBox* m_cbInterval;
    QPushButton* m_btnRezerva;
};

class ScrieRecenzieDialog : public QDialog {
    Q_OBJECT
public:
    explicit ScrieRecenzieDialog(Biblioteca& bib, const std::string& id_cititor, const std::string& nume_cititor, QWidget* parent = nullptr);
private slots:
    void onSalveaza();
private:
    void refreshTable();
    Biblioteca& m_bib;
    std::string m_id_cititor;
    std::string m_nume_cititor;
    QTableWidget* m_table;
    QComboBox* m_cbNota;
    QLineEdit* m_leComentariu;
    QPushButton* m_btnSalveaza;
    std::vector<std::shared_ptr<Carte>> m_carti;
};

class GestiuneRafturiDialog : public QDialog {
    Q_OBJECT
public:
    explicit GestiuneRafturiDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onAdauga();
    void onSterge();
    void onVeziCarti();
private:
    void refreshTable();
    Biblioteca& m_bib;
    QTableWidget* m_table;
    QPushButton* m_btnAdauga;
    QPushButton* m_btnVeziCarti;
    QPushButton* m_btnSterge;
    std::vector<Raft> m_rafturi;
};

class MutaCarteFizicaDialog : public QDialog {
    Q_OBJECT
public:
    explicit MutaCarteFizicaDialog(Biblioteca& bib, QWidget* parent = nullptr);
private slots:
    void onMuta();
private:
    void refreshCarti();
    void refreshRafturi();
    Biblioteca& m_bib;
    QTableWidget* m_tableCarti;
    QTableWidget* m_tableRafturi;
    QPushButton* m_btnMuta;
    std::vector<std::shared_ptr<CarteFizica>> m_carti;
    std::vector<Raft> m_rafturi;
};

// ---------------------------------------------------------
// Namespace pentru toate funcțiile Qt (Echivalent ui_functii)
// ---------------------------------------------------------
namespace QtFunc {
    // Cărți
    void adaugaCarteFizica(QWidget* parent, Biblioteca& bib);
    void adaugaCarteDigitala(QWidget* parent, Biblioteca& bib);
    void adaugaCarteAudio(QWidget* parent, Biblioteca& bib);
    void stergeCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void restaureazaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void modificaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void vizualizareCatalog(QWidget* parent, Biblioteca& bib);
    void cautaCarte(QWidget* parent, Biblioteca& bib, std::shared_ptr<Utilizator> user, const QString& defaultKeyword = "");
    void mutaCarteFizica(QWidget* parent, Biblioteca& bib);
    void gestiuneRafturi(QWidget* parent, Biblioteca& bib);
    void reconditioneazaCarti(QWidget* parent, Biblioteca& bib);
    void scrieRecenzie(QWidget* parent, Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor);

    void simuleazaTimp(QWidget* parent, Biblioteca& bib);
    void platesteSalarii(QWidget* parent, Biblioteca& bib);

    // Utilizatori
    void adaugaUtilizator(QWidget* parent, Biblioteca& bib);
    void afiseazaUtilizatori(QWidget* parent, Biblioteca& bib);
    void stergeUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void restaureazaUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void modificaUtilizator(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void modificaProfil(QWidget* parent, Biblioteca& bib, const std::string& id_actor);

    // Imprumuturi & Retururi
    void imprumutaCarte(QWidget* parent, Biblioteca& bib);
    void returneazaCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void toateImprumuturile(QWidget* parent, Biblioteca& bib);
    void confirmaReturnari(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void confirmaRidicarRezervare(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void casareCarte(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    
    void imprumutaCarteCititor(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void returneazaCarteCititor(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void rezervaCarte(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void veziImprumuturileMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void veziIstoricLectura(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void veziRecenziileMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    void veziRezervariCartiMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
    
    // Finante & Sali
    void achizitii(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    void solicitaPlata(QWidget* parent, Biblioteca& bib, const std::shared_ptr<Utilizator>& u);
    void confirmaPlati(QWidget* parent, Biblioteca& bib, const std::string& id_actor);
    
    void rezervaSala(QWidget* parent, Biblioteca& bib, const std::string& idCititor, const std::string& numeCititor);
    void veziRezervariSaliMele(QWidget* parent, Biblioteca& bib, const std::string& idCititor);
}

#endif // QTDIALOGS_H
