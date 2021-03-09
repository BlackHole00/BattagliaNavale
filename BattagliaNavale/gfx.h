#pragma once
/*	File:		gfx.h
*	Autore:		Vicix
*	
*	Descrizione:
*		Questo file contiene i prototipi delle funzioni che servono per
*		Disegnare caratteri, stringhe, scatole e altro sulla console.
*/
#include <iostream>
#include <Windows.h>
using namespace std;

//	Macro
#define _CLS system("cls")

/*	Un enum che viene utilizzato nella funzione DrawLine().
*	Serve pe indicare se disegnare una linea verticalmente ed
*		orizzontalmente.
*/
enum TipoLinea {
	Orizzontale,
	Verticale
};

/*	Definizione di un nuovo tipo, che contiene i frame da disegnare.
*	Sarebbe un array di stringhe, perciò dibbiamo ricordarci di liberare
*		la memoria dopo che abbiamo finito di usarlo (con delete[]).
*/
typedef string* FrameData;

/*	Per le funzioni più utilizzate vengono dichiarate come inline.
*	Questo permette di chiamare le funzioni più velocemente (sebbene
*		un uso troppo frequente ingrandirebbe le dimensioni del file 
*		assai).
*/
inline void DrawChar(HANDLE, char);
inline void DrawCharAtPos(HANDLE, char, COORD);
inline void DrawStringn(HANDLE, string);
inline void DrawStringAtPos(HANDLE, string, COORD);

void DrawStringCentered(HANDLE, string, COORD);
void DrawBox(HANDLE, COORD, COORD);
void DrawBoxCentered(HANDLE, COORD, COORD);
void DrawLine(HANDLE, COORD, int, char, TipoLinea);
void DrawBorders(HANDLE, COORD);
void DrawStringInBox(HANDLE, COORD, string, WORD, WORD);
void DrawStringInBoxCentered(HANDLE, COORD, string, WORD, WORD);
void ClearArea(HANDLE, COORD, COORD);

void HideCursor(HANDLE);
void HideCursor(HANDLE);

//	Funzioni per carcare e disegnare "Sprite animati"
FrameData GetAnimatedFramesFromFiles(string, int);
void DrawFrame(HANDLE, FrameData, int, COORD);