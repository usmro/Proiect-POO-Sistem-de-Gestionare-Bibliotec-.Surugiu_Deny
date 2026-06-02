#ifndef CITITOR_H
#define CITITOR_H

#include "Utilizator.h"

class Cititor : public Utilizator {
private:
  std::string tip_abonament;
  std::string data_inregistrare;
  std::string data_expirare;
  int max_carti_permise;
  int nr_carti_imprumutate;
  double penalizari;

public:
  Cititor();
  Cititor(const std::string &id, const std::string &parola,
          const std::string &nume, const std::string &prenume,
          const std::string &cnp, const std::string &email,
          const std::string &telefon, const std::string &adresa, bool activ,
          const std::string &tip_abonament,
          const std::string &data_inregistrare,
          const std::string &data_expirare, int max_carti, int nr_imprumutate,
          double penalizari);
  ~Cititor() override;

  std::string getTipAbonament() const;
  std::string getDataInregistrare() const;
  std::string getDataExpirare() const;
  int getMaxCartiPermise() const;
  int getNrCartiImprumutate() const;
  double getPenalizari() const;

  void setTipAbonament(const std::string &t);
  void setDataInregistrare(const std::string &d);
  void setDataExpirare(const std::string &d);
  void setMaxCartiPermise(int m);
  void setNrCartiImprumutate(int n);
  void setPenalizari(double p);

  bool poateImprumuta() const;
  void incrementeazaImprumuturi();
  void decrementeazaImprumuturi();

  void afisare(std::ostream &os) const override;
  std::string formatFisier() const override;
  std::string getTip() const override;
  std::string getRol() const override;
};

#endif
