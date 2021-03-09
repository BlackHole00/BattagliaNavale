/*	File:		gfx.cpp
*	Autore:		Vicix
*
*	Descrizione:
*		Questo file contiene le implementazioni delle funzioni che servono per
*		Disegnare caratteri, stringhe, scatole e altro sulla console.
*/
#include "gfx.h"
#include <fstream>
#include <string>
#include <sstream>

/*	Funzione DrawChar
* 
*	Serve per disegnare un carattere nella console.
*/
inline void DrawChar(HANDLE hConsole, char character)
{
	/*	Di per se non esiste una funzione per disegnare caratteri.
	*	Utilizziamo WriteConsoleA, che disegna tot caratteri all'
	*		interno di un array di caratteri.
	*	Siamo obbligati a castare l'indirizzo del carattere come
	*		array di caratteri.
	*/
	WriteConsoleA(hConsole, (char*)&character, 1, NULL, NULL);
}

/*	Funzione DrawCharAtPos
* 
*	Semplicemente disegna un carattere ad una determinata posizione.
*/
inline void DrawCharAtPos(HANDLE hConsole, char character, COORD pos)
{
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsoleA(hConsole, (char*)&character, 1, NULL, NULL);
}


/*	Funzione DrawString
* 
*	Serve per disegnare una stringa
*/
inline void DrawString(HANDLE hConsole, string text)
{
	WriteConsoleA(hConsole, text.c_str(), text.length(), NULL, NULL);
}


/*	Funzione DrawStringAtPos
* 
*	Disegna una stringa ad una determinata posizione
*/
inline void DrawStringAtPos(HANDLE hConsole, string text, COORD pos)
{
	SetConsoleCursorPosition(hConsole, pos);
	WriteConsoleA(hConsole, text.c_str(), text.length(), NULL, NULL);
}


/*	Funzione DrawStringCentered
* 
*	Simile a DrawStringAtPos().
*	Al posto di prendere le coordinate del punto in alto a sinistra
*		della parola prende le coordinate del centro della parola.
*		Utile per disegnare le stringhe in modo centrato.
*/
void DrawStringCentered(HANDLE hConsole, string text, COORD pos)
{
	SetConsoleCursorPosition(hConsole, { pos.X - (short int)text.length() / 2, pos.Y });
	WriteConsoleA(hConsole, text.c_str(), text.length(), NULL, NULL);
}


/*	Funzione DrawBox
* 
*	Disegna una scatola/rettangolo.
*	Nota:
*		-StartPos indica l'angolo in alto a sinistra, mentre endPos
*			indica l'angolo in basso a destra.
*/
void DrawBox(HANDLE hConsole, COORD startPos, COORD endPos)
{
	//	Angolo alto sinistra
	DrawCharAtPos(hConsole, 0xC9, startPos);
	//	Lato alto
	for (int i = startPos.X + 1; i < endPos.X; i++)
		DrawChar(hConsole, 0xCD);
	//	Angolo alto destra
	DrawChar(hConsole, 0xBB);

	//	Lato destro e sinistro
	for (short int i = 1; i < endPos.Y - startPos.Y; i++)
	{
		DrawCharAtPos(hConsole, 0xBA, { startPos.X, startPos.Y + i });
		DrawCharAtPos(hConsole, 0xBA, { endPos.X, startPos.Y + i });
	}

	//  Angolo basso sinistra
	DrawCharAtPos(hConsole, 0xC8, { startPos.X, endPos.Y });
	//	Lato basso
	for (int i = startPos.X + 1; i < endPos.X; i++)
		DrawChar(hConsole, 0xCD);
	//	Angolo basso destro
	DrawChar(hConsole, 0xBC);
}


/*	Funzione DrawBoxCentered
* 
*	Serve per disegnare una scatola centrata. Simile a DrawStringCentered.
*/
//	Non Usare: BUGGATO!!!
void DrawBoxCentered(HANDLE hConsole, COORD centerPos, COORD BoxSize)
{
	DrawBox(hConsole, { centerPos.X - BoxSize.X / 2, centerPos.Y - BoxSize.Y / 2 }, { centerPos.X + BoxSize.X / 2, centerPos.Y + BoxSize.Y / 2 });
}


/*	Funzione DrawLine
* 
*	Disegna una linea del carattere specificato in input.
*	A seconda delt tipo di linea la disegna o orizzontale o verticale
*/
void DrawLine(HANDLE hConsole, COORD coord, int offset, char carattere, TipoLinea tipo)
{
	SetConsoleCursorPosition(hConsole, coord);

	if (tipo == Orizzontale)
		for (int i = 0; i < offset; i++)
			DrawChar(hConsole, carattere);
	else
		for (short int i = 0; i < offset; i++)
		{
			DrawChar(hConsole, carattere);
			SetConsoleCursorPosition(hConsole, { coord.X, coord.Y + i + 1 });
		}
}


/*	Funzione DrawBorders
* 
*	Dato in input la grandezza della finestra disegna una scatola come contorno
*		della finestra.
*/
void DrawBorders(HANDLE hConsole, COORD windowSize)
{
	DrawBox(hConsole, { 0, 0 }, windowSize);
}


/*	Funzione DrawStringInBox
* 
*	Disegna una scatola con una stringa all'interno.
*/
void DrawStringInBox(HANDLE hConsole, COORD coord, string parola, WORD attributiParola, WORD attributiBox)
{
	//	Cambio colore per la scatola
	SetConsoleTextAttribute(hConsole, attributiBox);
	//	Disegno scatola
	DrawBox(hConsole, coord, { coord.X + (short int)parola.length() + 1, coord.Y + 2 });
	//	Cambio colore per il testo
	SetConsoleTextAttribute(hConsole, attributiParola);
	//	Disegno la stringa
	DrawStringAtPos(hConsole, parola, { coord.X + 1, coord.Y + 1 });
	// Resetto il colore.
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
}

/*	Funzione DrawStringInBoxCentered
* 
*	Simile a DrawStringCentered.
*/
void DrawStringInBoxCentered(HANDLE hConsole, COORD coord, string parola, WORD attributiParola, WORD attributiBox)
{
	short int length = parola.length();

	DrawStringInBox(hConsole, { coord.X - length / 2 - 1, coord.Y - 1 }, parola, attributiParola, attributiBox);
}


void ClearArea(HANDLE hConsole, COORD startPos, COORD endPos)
{
	for (short int x = startPos.X; x < endPos.X; x++)
		for (short int y = startPos.Y; y < endPos.Y; y++)
			DrawCharAtPos(hConsole, ' ', { x, y });
}


/*	Funzione HideCursor
* 
*	Rende il cursore invisibile.
*/
void HideCursor(HANDLE hConsole)
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(hConsole, &info);
}

/*	Funzione ShowCursor
*
*	Rende il cursore visibile.
*/
void ShowCursor(HANDLE hConsole)
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = TRUE;
	SetConsoleCursorInfo(hConsole, &info);
}



/*	Funzione GetAnimatedFramesFromFiles
*
*	Prende i frame da disegnare da file multipli.
*	Il codice cerca nei file fileRoot + numero frame.
*	Se non trova uno o più file ritorna NULL.
*	Se non ci sono problemi ritorna FrameData (puntatore
*		ad array di strnghe).
*/
FrameData GetAnimatedFramesFromFiles(string fileRoot, int spriteNumber)
{
	//	Alloco FrameData
	FrameData frames = new string[spriteNumber];

	fstream file;
	string filePath, temp;

	for (int i = 0; i < spriteNumber; i++)
	{
		//	ottengo il filePath
		filePath = fileRoot + to_string(i) + ".txt";
		file.open(filePath);

		/*	Se il file non è aperto ritorno NULL
		*	E' spartano uscire dal ciclo così, ma serve per non
		*		transformare il for in un while e per non avere
		*		controlli extra nel ciclo
		*/
		if (!file.is_open())
			return NULL;

		//	inizializzo frames[i]
		frames[i] = "";

		while (getline(file, temp))
			frames[i] += temp + "\n";

		file.close();
	}

	return frames;
}


/*	Funzione DrawFrame
*
*	Prende FrameData e disegna un frame ad una determinata coordinata.
*	Utilizziamo una stringstram in modo da poter dividere la stringa in
*	linee.
*/
void DrawFrame(HANDLE hConsole, FrameData frames, int frame, COORD pos)
{
	stringstream ss;
	string temp;

	ss << frames[frame];

	short int i = 0;
	while (getline(ss, temp))
	{
		DrawStringAtPos(hConsole, temp, { pos.X, pos.Y + i });
		i++;
	}
}