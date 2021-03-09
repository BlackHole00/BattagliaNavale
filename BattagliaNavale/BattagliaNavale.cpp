#include <iostream>
#include <Windows.h>
//#include "gfx.h"
using namespace std;

#define LUNG_NAVE(_idNave) (_idNave + 1)
#define CHAR_TO_INT(_character) (toupper(_character) - 'A')

/*  Definizioni di costanti
*/
const int DIM_C = 20;
const int DIM_R = 20;
const int MIN_C = 5;
const int MIN_R = 5;
const int NUMERO_CASELLE = DIM_C * DIM_R;

//  Rateo max di caselle di navi che ci possono essere in relazione con il numero di caselle.
const float RATEO_CAPACITA_MAX = 0.30;

//  Costanti di definizione dei tipi di caselle.
const int ID_ACQUA = 0;
const int ID_NAVE = 1;
const int ID_ACQUA_COLPITA = 2;
const int ID_NAVE_COLPITA = 3;
const int ID_NAVE_AFFONDATA = 4;

//  Costanti per la definzione dell'id delle navi. Poco o niente utilizzate...
const int NUM_NAVI = 5;
const int ID_MINA = 0;
const int ID_CACCIATOR = 1;
const int ID_INCROCIATORE = 2;
const int ID_CORAZZATA = 3;
const int ID_PORTAERERI = 4;

//  Costanti per i caratteri da rappresentare.
//  Utilizziamo la code page 850 (default per applicazioni testuali, anche chiamata DOS-Latin1). 
//  Riferimento caratteri: https://en.wikipedia.org/wiki/Code_page_850
const char CHAR_ACQUA = 0xB0;
const char CHAR_NAVE = 0xCE;
const char CHAR_ACQUA_COLPITA = 0xFE;
const char CHAR_NAVE_COLPITA = 0xC5;
const char CHAR_NAVE_AFFONDATA = 0xB2;

//  Costanti per aiutarci con l'identificazione del verso (che viene spesso impresso sotto forma di valore booleano)
const int VERSO_ORIZZONTALE = true;
const int VERSO_VERTICALE = false;

//  Costanti per il nome delle navi. Ogni elemento ha l'indice che corrisponde all'ID delle navi. (es. VECT_NOMI_NAVI[ID_MINA] => "Mina")
const string VECT_NOMI_NAVI[NUM_NAVI] = {
    "Mina",                     // 0
    "Cacciator Pediniere",      // 1
    "Incrociatore",             // 2
    "Corazzata",                // 3
    "Portaerei"                 // 4
};

//  Dichiarazioni prototipi funzioni
int LeggiNumeriNavi(int[], int&, int, int);
bool ControllaSeMaxRateo(int, int, int);
bool ChiediVerso();
void InserimentoNavi(int[][DIM_C], int[], int, int, HANDLE);
bool ControllaPosizionamento(int[][DIM_C], int, int, bool, int, int, int);
void PosizionaNave(int[][DIM_C], int, int, bool, int);
void DisegnaCampo(int[][DIM_C], int, int, HANDLE);
void DisegnaBersagli(int[][DIM_C], int, int, HANDLE);
void ResetMatrice(int[][DIM_C], int);
void ChiediGrandezzaCampo(int&, int&);

int NaveOrizzontale(int[][DIM_C], int, int, int);
int NaveVerticale(int[][DIM_C], int, int, int);
int NaviColpite(int[][DIM_C], int, int, int, int);

void ChiediCoordinate(int&, int&, int, int);
int Gioco(int[][DIM_C], int[][DIM_C], HANDLE, int, int, int);

int main()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    //  Dichiarazione matrici
    int matGiocatore1[DIM_R][DIM_C];
    int matGiocatore2[DIM_R][DIM_C];
    int lungR, lungC;

    //  Reset matrici 
    ResetMatrice(matGiocatore1, ID_ACQUA);
    ResetMatrice(matGiocatore2, ID_ACQUA);

    ChiediGrandezzaCampo(lungC, lungR);

    //  Vettore nel quale vengono salvati il numero di navi per ogni tipo
    int vectNavi[NUM_NAVI];
    int naviTot;

    //  Richiesta e lettura numero navi per ogni tipo
    LeggiNumeriNavi(vectNavi, naviTot, lungC, lungR);

    //  Inserimento navi per ogni giocatore
    system("cls");
    DisegnaCampo(matGiocatore1, lungC, lungR, hConsole);
    InserimentoNavi(matGiocatore1, vectNavi, lungC, lungR, hConsole);
    system("pause");
    system("cls");
    DisegnaCampo(matGiocatore2, lungC, lungR, hConsole);
    InserimentoNavi(matGiocatore2, vectNavi, lungC, lungR, hConsole);
    system("pause");


    int res = Gioco(matGiocatore1, matGiocatore2, hConsole, naviTot, lungC, lungR);
    system("cls");
    switch (res)
    {
    case 1:
    case 2:
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
        cout << endl << "<<<Ha vinto il giocatore " << res << "!!!>>>" << endl << endl;
        break;
    }
    default:
    {
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
        cout << endl << "<<<Pareggio>>>" << endl << endl;
        break;
    }
    }
    
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
    system("pause");
}

/*  Procedura LeggiNumeriNavi
*
*   Chiede all'utente il numero di navi per ogni tipo e salva in vectNavi[].
*   Se le navi posizionate sono 0, allora prenderà i numeri da una tabella di default.
*   Se il numero complessivo di caselle occupate dalle navi fratto il numero di caselle
*   e' maggiore di RATEO_CAPACITA_MAX, allora il programma non permette di immettere più
*   navi.
*   Vedi come funziona vectNavi nelle ipotesi.
*/
int LeggiNumeriNavi(int vectNavi[], int& numNavi, int lungC, int lungR)
{
    //  Calcolo caselle utilizzate.
    int numCaselleUtilizzate = 0;
    numNavi = 0;

    for (int i = 0; i < NUM_NAVI; i++)
    {
        int temp;
        bool ok;
        do
        {
            ok = true;
            //  Utilizziamo temp per non modificare il contenuto di numCaselleUtilizzate se l'utente inserisce qualcosa di non valido
            temp = numCaselleUtilizzate;

            //  Richiesta all'utente
            cout << "Quante navi di tipo " << VECT_NOMI_NAVI[i] << " (lunghezza: " << LUNG_NAVE(i) << ") vuoi? ";
            cin >> vectNavi[i];

            temp += vectNavi[i] * LUNG_NAVE(i);

            if (ControllaSeMaxRateo(temp, lungC, lungR))  //  Se il rateo massimo è sorpassato, allora non permettiamo all'utente di inserire la nave
            {
                cout << "Numero max di caselle per le navi raggiunto!!!" << endl;
                ok = false;
            }
            else if (vectNavi[i] < 0)       //  Anche se il numero di navi è invalido non permettiamo all'utente di continuare
            {
                cout << "Numero non valido!!!" << endl;
                ok = false;
            }
        } while (!ok);

        //  Aggiorniamo numCaselleUtilizzate
        numCaselleUtilizzate = temp;
        numNavi += vectNavi[i];
    }

    if (numCaselleUtilizzate <= 0)  //  Se numCaselleUtilizzate è <= 0, allora carichiamo una configurazione di default
    {
        cout << "Nessuna nave in campo!!! Utilizzo Tabella di default!!!" << endl;
        vectNavi[ID_MINA] = 2;
        vectNavi[ID_CACCIATOR] = 3;
        vectNavi[ID_INCROCIATORE] = 3;
        vectNavi[ID_CORAZZATA] = 1;
        vectNavi[ID_PORTAERERI] = 0;

        numNavi = 9;
    }

    return numNavi;
}


/*  Funzione ControllaSeMaxRateo
*
*   Controlla se il RATEO_CAPACITA_MAX è stato sorpassato.
*/
bool ControllaSeMaxRateo(int numCaselleUtilizzate, int lungC, int lungR)
{
    if (((float)numCaselleUtilizzate / (float)(lungC * lungR)) > RATEO_CAPACITA_MAX)
        return true;
    return false;
}


/*  Funzione ChiediVerso
*
*   Chiede il verso della nave, facendo in modo che l'input sia valido.
*/
bool ChiediVerso()
{
    bool verso, ok;

    do
    {
        char temp;

        cout << "Vuoi inserire la nave [O]rizzontalmente o [V]erticalmente? [O/V] ";
        cin >> temp;

        temp = toupper(temp);

        if (temp != 'O' && temp != 'V') //  Controlliamo che la scelta sia valida
        {
            cout << "Input non valido!!!" << endl;
            ok = false;
        }
        else
        {
            verso = ((temp == 'O') ? VERSO_ORIZZONTALE : VERSO_VERTICALE);  //  Aggiorna verso
            ok = true;
        }
    } while (!ok);

    return verso;
}


/*  Procedura InserimentoNavi
*
*   Chiede all'utente per ogni nave la posizione ed il verso. Controlla se la posizione è valida (quindi non va fuori dal campo e
*   non tocca un'altra nave) e posiziona la nave.
*/
void InserimentoNavi(int matCampo[][DIM_C], int vectNavi[], int lungC, int lungR, HANDLE hConsole)
{
    for (int i = (NUM_NAVI - 1); i >= 0; i--)
    {
        if (vectNavi[i] != 0)   // Immettiamo il messaggio solo se ci sono navi da inserire
            cout << "Inserisci le navi di tipo " << VECT_NOMI_NAVI[i] << " (lunghezza: " << LUNG_NAVE(i) << "):" << endl;

        for (int j = 0; j < vectNavi[i]; j++)
        {
            int coordC, coordR;
            bool verso = VERSO_ORIZZONTALE;
            bool ok;

            do
            {
                cout << "Nave numero " << j + 1 << ": " << endl;
                ChiediCoordinate(coordC, coordR, lungC, lungR);    //  Chiediamo la posizione della nave
                if (i != ID_MINA)   //  Se la nave non è una mina, chiediamo il verso
                    verso = ChiediVerso();
                if (!ControllaPosizionamento(matCampo, coordC, coordR, verso, i, lungC, lungR))   //  Controlliamo se la nave ci sta
                {
                    cout << "La nave non ci sta!!!" << endl;
                    ok = false;
                }
                else {
                    PosizionaNave(matCampo, coordC, coordR, verso, i);  //  Posizioniamo la nave e disegnamo il campo
                    ok = true;

                    system("cls");
                    DisegnaCampo(matCampo, lungC, lungR, hConsole);
                }
            } while (!ok);
        }
    }
}

/*  Funzione ControllaPosizionamento
*
*   Data la posizione della nave, il verso e la lunghezza, controlla tutte le caselle che circondano la nave.
*   Se una o più di queste caselle è una nave (ID_NAVE), allora la posizione è invalida e ritorniamo false.
*
*   NOTE:
*       - LungNave non è la lunghezza reale: sarebbe la lunghezza - 1
*       - Utilizziamo i return all'interno dei cicli per evitare di utilizzare condizioni extra. Utilizziamo sempre
*           cicli do-while, quindi indeterminati.
*       - Le coordinate in input non possono essere minori di 0 o maggiori della dimensione dellla matrice. Questo è
*           assicurato da ChiediPosizioneNave().
*/
bool ControllaPosizionamento(int matCampo[][DIM_C], int coordC, int coordR, bool verso, int lungNave, int lungC, int lungR)
{
    if (verso == VERSO_ORIZZONTALE) //  Per ogni verso dobbiamo fare qualcosa di differente. La nave in questo caso è messa in orizzontale
    {
        if (coordC + lungNave >= lungC) //  Se la nave va fuori dalla matrice ritorniamo subito.
            return false;

        //  Calcolo delle coordinate iniziali all'esterno del ciclo per evitare di fare lo stesso calcolo più volte e per motivi di leggibilità.
        int startR = coordR - ((coordR <= 0) ? 0 : 1);  //  Se siamo nella prima riga della matrice non possiamo controllare più in alto
        int endR = coordR + ((coordR >= (lungR - 1)) ? 0 : 1); //  Se siamo nella ultima riga della matrice non possiamo controllare più in basso
        int startC = coordC - ((coordC <= 0) ? 0 : 1); //  Se siamo nella prima colonna della matrice non possiamo controllare più a sinistra.
        int endC = coordC + lungNave + ((coordC >= (lungC - 1)) ? 0 : 1);   //  Se la nave arriva ad occupare l'ultima colonna della matrice non possiamo controllare più a destra

        //  Controlliamo ogni singola casella compresa tra (startX, startY) e tra (endX, endY) compresi.
        int r = startR;
        while (r <= endR)
        {
            int c = startC;
            while (c <= endC)
            {
                if (matCampo[r][c] == ID_NAVE)  //  Se troviamo una nave usciamo
                    return false;
                c++;
            }
            r++;
        }
    }
    else    //La nave  è messa in verticale
    {
        if (coordR + lungNave >= lungR)  //  Se la nave va fuori dalla matrice ritorniamo subito.
            return false;

        //  Calcolo delle coordinate iniziali all'esterno del ciclo per evitare di fare lo stesso calcolo più volte e per motivi di leggibilità.
        int startR = coordR - ((coordR <= 0) ? 0 : 1);  //  Se siamo nella prima riga della matrice non possiamo controllare più in alto
        int endR = coordR + lungNave + ((coordR >= (lungR - 1)) ? 0 : 1);   //  Se la nave arriva ad occupare l'ultima riga della matrice non possiamo controllare più in basso
        int startC = coordC - ((coordC <= 0) ? 0 : 1);  //  Se siamo nella prima colonna della matrice non possiamo controllare più a sinistra.
        int endC = coordC + ((coordC >= (lungC - 1)) ? 0 : 1);  //  Se siamo nell'ultima colonna della matrice non possiamo controllare più a destra.

        //  Controlliamo ogni singola casella compresa tra (startX, startY) e tra (endX, endY) compresi.
        int r = startR;
        while (r <= endR)
        {
            int c = startC;
            while (c <= endC)
            {
                if (matCampo[r][c] == ID_NAVE)  //  Se troviamo una nave usciamo
                    return false;
                c++;
            }
            r++;
        }
    }

    return true;
}


/*  Procedura PosizionaNave
*
*   Posiziona La nave nella matrice, senza fare controlli.
*/
void PosizionaNave(int matCampo[][DIM_C], int coordC, int coordR, bool verso, int lungNave)
{
    if (verso == VERSO_ORIZZONTALE)
    {
        for (int c = coordC; c <= coordC + lungNave; c++)
            matCampo[coordR][c] = ID_NAVE;
    }
    else    //  Nave verticale
    {
        for (int r = coordR; r <= coordR + lungNave; r++)
            matCampo[r][coordC] = ID_NAVE;
    }
}


/*  Procedura DisegnaCampo
*
*   Disegna il proprio campo, segnalando le proprie navi, colpite e non colpite.
*   Dobbiamo solo disegnare la navi non colpite, le navi colpite e le navi affondate.
*/
void DisegnaCampo(int matCampo[][DIM_C], int lungC, int lungR, HANDLE hConsole)
{
    cout << "    ";
    for (int c = 0; c < lungC; c++) //cout lettere separate da tab
        cout << c + 1 << (c < 9 ? "  " : " ");
    cout << endl;


    for (int r = 0; r < lungR; r++)
    {
        cout << (char)(r + 65) << "   ";
        for (int c = 0; c < lungC; c++)
        {
            if (matCampo[r][c] == ID_ACQUA)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                cout << CHAR_ACQUA;
            }
            else if (matCampo[r][c] == ID_ACQUA_COLPITA)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
                cout << CHAR_ACQUA_COLPITA;
            }
            else if (matCampo[r][c] == ID_NAVE)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
                cout << CHAR_NAVE;
            }
            else if (matCampo[r][c] == ID_NAVE_COLPITA)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN);
                cout << CHAR_NAVE_COLPITA;
            }
            else if (matCampo[r][c] == ID_NAVE_AFFONDATA)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
                cout << CHAR_NAVE_AFFONDATA;
            }
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
            cout << "  ";
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
        cout << endl;
    }
    SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << endl;
}

/*  Procedura Disegna Campo.
*
*   Disegna dove abbiamo colpito nel campo dell'avversario.
*   Dobbiamo disegnare l'acqua colpita, le navi colpite e navi affondate.
*   Le navi non colpite vengono ignorate.
*/
void DisegnaBersagli(int matCampo[][DIM_C], int lungC, int lungR, HANDLE hConsole)
{
    cout << "    ";
    for (int c = 0; c < lungC; c++) //cout numeri separate da spazi
        cout << c + 1 << (c < 9 ? "  " : " ");
    cout << endl;

    for (int r = 0; r < lungR; r++)
    {
        cout << (char)(r + 65) << "   ";
        for (int c = 0; c < lungC; c++)
        {
            if (matCampo[r][c] == ID_ACQUA_COLPITA)
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE);
                cout << CHAR_ACQUA_COLPITA;
            }
            else if (matCampo[r][c] == ID_NAVE_COLPITA)
            {
                SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                cout << CHAR_NAVE_COLPITA;
            }
            else if (matCampo[r][c] == ID_NAVE_AFFONDATA)
            {
                SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
                cout << CHAR_NAVE_AFFONDATA;
            }
            else
            {
                SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                cout << CHAR_ACQUA;
            }
            SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
            cout << "  ";
        }
        SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN);
        cout << endl;
    }

}


/*  Procedura ResetMatrice
*
*   Semplice funzione per resettare tutta una matrice ad un valore.
*/
void ResetMatrice(int matCampo[][DIM_C], int valore)
{
    /*for (int y = 0; y < DIM_R; y++)
        for (int x = 0; x < DIM_C; x++)
            matCampo[x][y] = valore;*/

    memset(matCampo, 0, DIM_C * DIM_R * sizeof(int));
}


/*  Procedura ChiediCoordinate
*
*   Richiede e controlla le coordinate sulle quali si vuole colpire.
*/
void ChiediCoordinate(int& coordC, int& coordR, int lungC, int lungR)
{
    bool ok;

    do
    {
        ok = true;

        cout << "Inserisci la colonna [1-" << lungC << "]: ";
        cin >> coordC;

        coordC--;

        if (coordC < 0 || coordC >= lungC)
        {
            cout << "colonna non valida!!!" << endl;
            ok = false;
        }
    } while (!ok);

    do
    {
        ok = true;
        char temp;

        cout << "Inserisci la riga [A-" << (char)(lungR + 64) << "]: ";
        cin >> temp;
        temp = CHAR_TO_INT(temp);

        //  Visto che char è praticamente un intero possiamo fare sto cosa...
        if (temp < 0 || temp >= lungR)
        {
            cout << "Input Non valido!!!" << endl;
            ok = false;
        }
        coordR = temp;
    } while (!ok);
}



int Gioco(int matGiocatore1[][DIM_C], int matGiocatore2[][DIM_C], HANDLE hConsole, int naviTot, int lungC, int lungR)
{
    system("cls");

    int coordC, coordR, lungNave, res;
    int naviG1 = naviTot, naviG2 = naviTot;
    bool ok;

    while (naviG1 != 0 && naviG2 != 0)
    {
        do
        {
            system("cls");
            cout << "\t   GIOCATORE 1" << endl;
            DisegnaCampo(matGiocatore1, lungC, lungR, hConsole);
            cout << endl;
            DisegnaBersagli(matGiocatore2, lungC, lungR, hConsole);

            cout << "Inserisci le coordinate dove vuoi colpire: " << endl;
            ChiediCoordinate(coordC, coordR, lungC, lungR);

            res = NaviColpite(matGiocatore2, coordC, coordR, lungC, lungR);

            if (res == -2) {
                cout << "Hai gia' colpito in quel punto. " << endl;
                ok = false;
            }
            if (res == -1)
            {
                cout << "Nave colpita!!" << endl;
                ok = false;
            }
            else if (res == 0)
            {
                cout << "Acqua colpita!!" << endl;
                ok = true;
            }
            else
            {
                naviG2--;
                cout << "Hai colpito un " << VECT_NOMI_NAVI[res - 1] << "!!!" << endl;
                ok = false;
            }

            system("pause");
        } while (!ok && naviG2 > 0);        



        do
        {
            system("cls");

            cout << "\t   GIOCATORE 2" << endl;
            DisegnaCampo(matGiocatore2, lungC, lungR, hConsole);
            cout << endl;
            DisegnaBersagli(matGiocatore1, lungC, lungR, hConsole);

            ok = true;
            cout << "Inserisci le coordinate dove vuoi colpire: " << endl;
            ChiediCoordinate(coordC, coordR, lungC, lungR);

            res = NaviColpite(matGiocatore1, coordC, coordR, lungC, lungR);

            if (res == -2) {
                cout << "Hai gia' colpito in quel punto. " << endl;
                ok = false;
            }
            if (res == -1)
            {
                cout << "Nave colpita!!" << endl;
                ok = false;
            }
            else if (res == 0)
            {
                cout << "Acqua colpita!!" << endl;
                ok = true;
            }
            else
            {
                naviG1--;
                cout << "Hai colpito un " << VECT_NOMI_NAVI[res - 1] << "!!!" << endl;
                ok = false;
            }

            system("pause");
        } while (!ok && naviG1 > 0);
    }
    //cout vincitore
    if (naviG1 != 0 && naviG2 == 0)
        return 1;
    else if (naviG1 == 0 && naviG2 != 0)
        return 2;
    else
        return 3; //voglio dare la possibilità anche al giocatore due di fare la sua mossa altrimenti G1 sarà sempre avvantaggiato
}

/*
        -2 = coordinate già usate
        -1 = nave colpita (non affondata)
         0 = acqua colpita
        num >= 1 = lunghezza nave affondata
*/
int NaviColpite(int mat[][DIM_C], int coordC, int coordR, int lungC, int lungR)
{
    bool ok = false;
    int returnValue = -2;

    if ((mat[coordR][coordC] != ID_ACQUA_COLPITA) && (mat[coordR][coordC] != ID_NAVE_COLPITA) && (mat[coordR][coordC] != ID_NAVE_AFFONDATA)) //non è stato colpito
    {
        ok = true;
        if (mat[coordR][coordC] != ID_ACQUA)
        {
            if (mat[coordR][coordC] == ID_NAVE)
            { //da capire se la nave è stata affondata o solo colpita

                //  Calcolo delle coordinate iniziali all'esterno del ciclo per evitare di fare lo stesso calcolo più volte e per motivi di leggibilità.
                int startR = coordR - ((coordR <= 0) ? 0 : 1);  //  Se siamo nella prima riga della matrice non possiamo controllare più in alto
                int endR = coordR + ((coordR >= (lungR - 1)) ? 0 : 1);   //  Se la nave arriva ad occupare l'ultima riga della matrice non possiamo controllare più in basso
                int startC = coordC - ((coordC <= 0) ? 0 : 1);  //  Se siamo nella prima colonna della matrice non possiamo controllare più a sinistra.
                int endC = coordC + ((coordC >= (lungC - 1)) ? 0 : 1);  //  Se siamo nell'ultima colonna della matrice non possiamo controllare più a destra.

                bool mina = true;
                bool verso;

                //  Settiamo temporaneamente la nave come acqua. Un controllo in meno nel ciclo...
                mat[coordR][coordC] = ID_ACQUA;

                int r = startR;
                while (r <= endR)
                {
                    int c = startC;
                    while (c <= endC)
                    {
                        if (mat[r][c] == ID_NAVE || mat[r][c] == ID_NAVE_COLPITA)
                        {
                            if (c < coordC || c > coordC)
                            {
                                mina = false;
                                verso = VERSO_ORIZZONTALE;

                                break;
                            }
                            else if (r < coordR || r > coordR)
                            {
                                mina = false;
                                verso = VERSO_VERTICALE;

                                break;
                            }
                        }
                        c++;
                    }
                    r++;
                }

                mat[coordR][coordC] = ID_NAVE_COLPITA;

                if (mina)  //  Solo la mina
                {
                    returnValue = 1;
                    mat[coordR][coordC] = ID_NAVE_AFFONDATA;
                    return ok;
                    //cout << "Hai colpito una " << VECT_NOMI_NAVI[0] << "!!!" << endl;
                }
                else {
                    if (verso == VERSO_ORIZZONTALE)
                        returnValue = NaveOrizzontale(mat, coordC, coordR, lungC);
                    else
                        returnValue = NaveVerticale(mat, coordC, coordR, lungR);
                }
            }
        }
        else
        {
            mat[coordR][coordC] = ID_ACQUA_COLPITA; //cambio con acqua colpita
            returnValue = 0;
        }
    }
    else
        returnValue = -2;

    return returnValue;
}

int NaveOrizzontale(int mat[][DIM_C], int c, int r, int lungC)
{
    //due step
        //1- mi muovo verso sx per trovare l'acqua
    int temp = c - 1;
    int spostamentiDx = 1;
    int spostamentiSx = 1;

    while (temp >= 0 && mat[r][temp] != ID_ACQUA && mat[r][temp] != ID_ACQUA_COLPITA) {
        if (mat[r][temp] == ID_NAVE)
        {
            return -1;
        }
        temp--;
        spostamentiSx++;
    }

    temp = c + 1;
    while (temp <= (lungC - 1) && mat[r][temp] != ID_ACQUA && mat[r][temp] != ID_ACQUA_COLPITA) {
        if (mat[r][temp] == ID_NAVE)
            return -1;
        temp++;
        spostamentiDx++;
    }

    for (int i = c - spostamentiSx + 1; i <= c + spostamentiDx - 1; i++)
        mat[r][i] = ID_NAVE_AFFONDATA;

    return spostamentiDx + spostamentiSx - 1;
}


int NaveVerticale(int mat[][DIM_C], int c, int r, int lungR)
{
    //due step
    //1- mi muovo verso sx per trovare l'acqua
    int temp = r - 1;
    int spostamentiSu = 1;
    int spostamentiGiu = 1;

    while (temp >= 0 && mat[temp][c] != ID_ACQUA && mat[temp][c] != ID_ACQUA_COLPITA) {
        if (mat[temp][c] == ID_NAVE)
            return -1;
        temp--;
        spostamentiSu++;
    }

    temp = r + 1;
    while (temp <= (lungR - 1) && mat[temp][c] != ID_ACQUA && mat[temp][c] != ID_ACQUA_COLPITA) {
        if (mat[temp][c] == ID_NAVE)
            return -1;
        temp++;
        spostamentiGiu++;
    }

    for (int i = r - spostamentiSu + 1; i <= r + spostamentiGiu - 1; i++)
        mat[i][c] = ID_NAVE_AFFONDATA;

    return spostamentiSu + spostamentiGiu - 1;
}


void ChiediGrandezzaCampo(int& lungC, int& lungR) {
    bool ok = true;
    do 
    {
        ok = true;

        cout << "Quante colonne vuoi [" << MIN_C << "-" << DIM_C << "]? ";
        cin >> lungC;
        if (lungC > DIM_C || lungC < MIN_C) {
            cout << "Scelta non valida!!!" << endl;
            ok = false;
        }
    } while (!ok);

    ok = true;
    do
    {
        ok = true;

        cout << "Quante righe vuoi [" << MIN_R << "-" << DIM_R << "]? ";
        cin >> lungR;
        if (lungR > DIM_R || lungR < MIN_R) {
            cout << "Scelta non valida!!!" << endl;
            ok = false;
        }
    } while (!ok);

    cout << endl;
}