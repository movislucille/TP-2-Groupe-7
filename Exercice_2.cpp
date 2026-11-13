// ============================================================
// Exercice 2 : Gestion des etudiants et des resultats academiques
//
// QUESTIONS POSEES :
//  1. Identifier les classes Etudiant, Filiere, UE, Matiere et Note.
//  2. Definir les relations entre ces classes.
//  3. Realiser un diagramme de classes UML.
//  4. Implementer les classes en C++.
//  5. Calculer la moyenne d'un etudiant.
//  6. Determiner si un etudiant valide une UE.
//  7. Afficher le releve de notes d'un etudiant.
//
// NOTIONS : encapsulation, composition et agregation,
//           relations entre objets, collections d'objets,
//           methodes de calcul, multiplicite UML.
// ============================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstddef>
using namespace std;

// ------------------------------------------------------------
// QUESTION 1 : identification des classes
//   Matiere, UE, Filiere, Note, Etudiant
//
// QUESTION 2 : relations entre les classes
//   Filiere  <>--(1 .. 1..*)-- UE        : composition
//   UE       <>--(1 .. 1..*)-- Matiere   : composition
//   Etudiant <>--(1 .. 0..*)-- Note      : composition
//   Etudiant o--(0..* .. 1)-- Filiere    : agregation
//   Note     -->(0..* .. 1)-- Matiere    : association
//
// QUESTION 3 : diagramme UML
//   Il se dessine a partir des classes et relations ci-dessus
//   (attributs prives, methodes publiques, multiplicites).
//
// QUESTION 4 : implementation en C++ (tout ce qui suit)
//   Encapsulation : attributs prives + accesseurs
// ------------------------------------------------------------

// ---------------- Classe Matiere ----------------
class Matiere {
    string code, intitule;
    double coefficient;
public:
    Matiere(const string& c, const string& i, double coef)
        : code(c), intitule(i), coefficient(coef) {
        if (coef <= 0) throw invalid_argument("Coefficient invalide");
    }
    string getCode() const { return code; }
    string getIntitule() const { return intitule; }
    double getCoefficient() const { return coefficient; }
};

// ---------------- Classe UE ----------------
// Composition : une UE contient 1 ou plusieurs Matiere
class UE {
    string code, intitule;
    int credits;
    vector<Matiere> matieres;     // collection d'objets
public:
    UE(const string& c, const string& i, int cr)
        : code(c), intitule(i), credits(cr) {}
    void ajouterMatiere(const Matiere& m) { matieres.push_back(m); }
    string getCode() const { return code; }
    string getIntitule() const { return intitule; }
    int getCredits() const { return credits; }
    const vector<Matiere>& getMatieres() const { return matieres; }
    bool contientMatiere(const string& codeMat) const {
        for (size_t i = 0; i < matieres.size(); i++)
            if (matieres[i].getCode() == codeMat) return true;
        return false;
    }
};

// ---------------- Classe Filiere ----------------
// Composition : une Filiere contient plusieurs UE
class Filiere {
    string nom;
    vector<UE> ues;               // collection d'objets
public:
    Filiere(const string& n) : nom(n) {}
    void ajouterUE(const UE& ue) { ues.push_back(ue); }
    string getNom() const { return nom; }
    const vector<UE>& getUEs() const { return ues; }
    int totalCredits() const {
        int t = 0;
        for (size_t i = 0; i < ues.size(); i++) t += ues[i].getCredits();
        return t;
    }
    bool contientMatiere(const string& codeMat) const {
        for (size_t i = 0; i < ues.size(); i++)
            if (ues[i].contientMatiere(codeMat)) return true;
        return false;
    }
};

// ---------------- Classe Note ----------------
// Association : une Note concerne une Matiere (par son code)
class Note {
    string codeMatiere;
    double valeur;
public:
    Note(const string& cm, double v) : codeMatiere(cm), valeur(v) {
        if (v < 0.0 || v > 20.0) throw invalid_argument("Note hors de [0;20]");
    }
    string getCodeMatiere() const { return codeMatiere; }
    double getValeur() const { return valeur; }
    void setValeur(double v) {
        if (v < 0.0 || v > 20.0) throw invalid_argument("Note hors de [0;20]");
        valeur = v;
    }
};

// ---------------- Classe Etudiant ----------------
// Un etudiant appartient a une filiere et a un niveau (L1, L2 ou L3)
class Etudiant {
    string matricule, nom, prenom, niveau;   // niveau : "L1", "L2" ou "L3"
    const Filiere* filiere;                  // agregation
    vector<Note> notes;                      // composition
public:
    Etudiant(const string& mat, const string& n, const string& p,
             const string& niv, const Filiere& f)
        : matricule(mat), nom(n), prenom(p), niveau(niv), filiere(&f) {
        if (niv != "L1" && niv != "L2" && niv != "L3")
            throw invalid_argument("Niveau invalide (L1, L2 ou L3)");
    }

    // Les etudiants obtiennent des notes dans les differentes matieres
    void ajouterNote(const string& codeMatiere, double valeur) {
        if (!filiere->contientMatiere(codeMatiere))
            throw invalid_argument("Matiere inconnue : " + codeMatiere);
        for (size_t i = 0; i < notes.size(); i++) {
            if (notes[i].getCodeMatiere() == codeMatiere) {
                notes[i].setValeur(valeur);
                return;
            }
        }
        notes.push_back(Note(codeMatiere, valeur));
    }

    bool getNote(const string& codeMatiere, double& valeur) const {
        for (size_t i = 0; i < notes.size(); i++) {
            if (notes[i].getCodeMatiere() == codeMatiere) {
                valeur = notes[i].getValeur();
                return true;
            }
        }
        return false;
    }

    // --------------------------------------------------------
    // QUESTION 5 : calculer la moyenne d'un etudiant
    //   Moyenne UE      = somme(note x coef) / somme(coef)
    //   Moyenne generale = somme(moyUE x credits) / somme(credits)
    // --------------------------------------------------------
    double moyenneUE(const UE& ue, bool& complete) const {
        double somme = 0.0, coefs = 0.0;
        complete = true;
        const vector<Matiere>& ms = ue.getMatieres();
        for (size_t i = 0; i < ms.size(); i++) {
            double v = 0.0;
            if (getNote(ms[i].getCode(), v)) {
                somme += v * ms[i].getCoefficient();
                coefs += ms[i].getCoefficient();
            } else {
                complete = false;     // matiere sans note
            }
        }
        return coefs > 0.0 ? somme / coefs : 0.0;
    }

    double moyenneGenerale() const {
        double somme = 0.0;
        int credits = 0;
        const vector<UE>& ues = filiere->getUEs();
        for (size_t i = 0; i < ues.size(); i++) {
            bool c = false;
            somme += moyenneUE(ues[i], c) * ues[i].getCredits();
            credits += ues[i].getCredits();
        }
        return credits > 0 ? somme / credits : 0.0;
    }

    // --------------------------------------------------------
    // QUESTION 6 : determiner si un etudiant valide une UE
    //   Regle : moyenne de l'UE >= 10 et toutes les matieres notees
    // --------------------------------------------------------
    bool valideUE(const UE& ue) const {
        bool complete = false;
        double moy = moyenneUE(ue, complete);
        return complete && moy >= 10.0;
    }

    int creditsAcquis() const {
        int t = 0;
        const vector<UE>& ues = filiere->getUEs();
        for (size_t i = 0; i < ues.size(); i++)
            if (valideUE(ues[i])) t += ues[i].getCredits();
        return t;
    }

    // --------------------------------------------------------
    // QUESTION 7 : afficher le releve de notes d'un etudiant
    // --------------------------------------------------------
    void afficherReleve() const {
        cout << fixed << setprecision(2);
        cout << "==================================================\n";
        cout << "RELEVE DE NOTES\n";
        cout << "Etudiant : " << nom << " " << prenom << " (" << matricule << ")\n";
        cout << "Filiere  : " << filiere->getNom() << " - Niveau " << niveau << "\n";
        cout << "==================================================\n";
        const vector<UE>& ues = filiere->getUEs();
        for (size_t i = 0; i < ues.size(); i++) {
            const UE& ue = ues[i];
            cout << "\nUE " << ue.getCode() << " - " << ue.getIntitule()
                 << " (" << ue.getCredits() << " credits)\n";
            const vector<Matiere>& ms = ue.getMatieres();
            for (size_t j = 0; j < ms.size(); j++) {
                double v = 0.0;
                cout << "  " << left << setw(24) << ms[j].getIntitule()
                     << "coef " << ms[j].getCoefficient() << "  note : ";
                if (getNote(ms[j].getCode(), v)) cout << v;
                else cout << "--";
                cout << "\n";
            }
            bool complete = false;
            double moy = moyenneUE(ue, complete);
            cout << "  Moyenne UE : " << moy << " /20 -> "
                 << (valideUE(ue) ? "VALIDEE" : "NON VALIDEE") << "\n";
        }
        cout << "\n--------------------------------------------------\n";
        cout << "Moyenne generale : " << moyenneGenerale() << " /20\n";
        cout << "Credits acquis   : " << creditsAcquis()
             << " / " << filiere->totalCredits() << "\n";
        cout << "==================================================\n";
    }
};

// ---------------- Programme principal : test ----------------
int main() {
    try {
        // Creation des UE et de leurs matieres
        UE ue1("UE1", "Informatique fondamentale", 6);
        ue1.ajouterMatiere(Matiere("POO", "Programmation objet", 3));
        ue1.ajouterMatiere(Matiere("ALG", "Algorithmique", 2));

        UE ue2("UE2", "Mathematiques", 4);
        ue2.ajouterMatiere(Matiere("ANA", "Analyse", 2));
        ue2.ajouterMatiere(Matiere("ALN", "Algebre lineaire", 2));

        // Creation de la filiere
        Filiere mi("Mathematiques-Informatique");
        mi.ajouterUE(ue1);
        mi.ajouterUE(ue2);

        // Creation de l'etudiant et saisie des notes
        Etudiant e("MI2-001", "Paintos", "Etudiant", "L2", mi);
        e.ajouterNote("POO", 14.0);
        e.ajouterNote("ALG", 12.5);
        e.ajouterNote("ANA", 8.0);
        e.ajouterNote("ALN", 9.0);

        // Questions 5, 6 et 7 : moyenne, validation, releve
        e.afficherReleve();
    } catch (const exception& ex) {
        cerr << "Erreur : " << ex.what() << endl;
        return 1;
    }
    return 0;
}
