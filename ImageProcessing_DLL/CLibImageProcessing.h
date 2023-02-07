#pragma once

#include "ImageClasse.h"
#include "ImageNdg.h"
#include "ImageCouleur.h"
#include "ImageDouble.h"

#include <Windows.h>

// Class C++

class CLibImageProcessing {

private:
	CImageCouleur* _imgPuzzleInPtr;
	CImageCouleur _imgPuzzleSuppFond;
	int _PuzzleCropHauteur;
	int _PuzzleCropLargeur;

	CImageCouleur* _imgPieceInPtr;
	CImageCouleur _imgPieceSuppFond;
	int _PieceCropHauteur;
	int _PieceCropLargeur;

	CImageCouleur* _imgPuzzleCropPtr;
	CImageCouleur* _imgPieceCropPtr;
public:

	_declspec(dllexport) CLibImageProcessing();

	// Puzzle
	_declspec(dllexport) void PuzzleImageProcessing(byte* data, int stride, int nbLig, int nbCol);

	_declspec(dllexport) int GetHauteurPuzzleCrop();
	_declspec(dllexport) int GetLargeurPuzzleCrop();

	_declspec(dllexport) void PuzzleCrop(byte* data, int stride, int nbLig, int nbCol);

	// Piece
	_declspec(dllexport) void PieceImageProcessing(byte* data, int stride, int nbLig, int nbCol);

	_declspec(dllexport) int GetHauteurPieceCrop();
	_declspec(dllexport) int GetLargeurPieceCrop();

	_declspec(dllexport) void PieceCrop(byte* data, int stride, int nbLig, int nbCol);

	// Position Piece
	_declspec(dllexport) void SearchPiece(byte* dataPu, byte* dataPi, int stridePu, int stridePi, int nbLigPu, int nbLigPi, int nbColPu, int nbColPi);
};



// Wrapper

extern "C" _declspec(dllexport) CLibImageProcessing * Build_CLibImageProcessing()
{
	CLibImageProcessing* libImageProcessing = new CLibImageProcessing();
	return libImageProcessing;
}


// Puzzle ImageProcessing
extern "C" _declspec(dllexport) void PuzzleImageProcessing(CLibImageProcessing * libImageProcessing, byte * data, int stride, int nbLig, int nbCol)
{
	libImageProcessing->PuzzleImageProcessing(data, stride, nbLig, nbCol);
}
extern "C" _declspec(dllexport) int GetHauteurPuzzleCrop(CLibImageProcessing * libImageProcessing)
{
	return libImageProcessing->GetHauteurPuzzleCrop();
}
extern "C" _declspec(dllexport) int GetLargeurPuzzleCrop(CLibImageProcessing * libImageProcessing)
{
	return libImageProcessing->GetLargeurPuzzleCrop();
}
extern "C" _declspec(dllexport) void PuzzleCrop(CLibImageProcessing * libImageProcessing, byte * data, int stride, int nbLig, int nbCol)
{
	libImageProcessing->PuzzleCrop(data, stride, nbLig, nbCol);
}


// Piece ImageProcessing
extern "C" _declspec(dllexport) void PieceImageProcessing(CLibImageProcessing * libImageProcessing, byte * data, int stride, int nbLig, int nbCol)
{
	libImageProcessing->PieceImageProcessing(data, stride, nbLig, nbCol);
}
extern "C" _declspec(dllexport) int GetHauteurPieceCrop(CLibImageProcessing * libImageProcessing)
{
	return libImageProcessing->GetHauteurPieceCrop();
}
extern "C" _declspec(dllexport) int GetLargeurPieceCrop(CLibImageProcessing * libImageProcessing)
{
	return libImageProcessing->GetLargeurPieceCrop();
}
extern "C" _declspec(dllexport) void PieceCrop(CLibImageProcessing * libImageProcessing, byte * data, int stride, int nbLig, int nbCol)
{
	libImageProcessing->PieceCrop(data, stride, nbLig, nbCol);
}


// Position Piece
extern "C" _declspec(dllexport) void SearchPiece(CLibImageProcessing * libImageProcessing, byte * dataPu, byte * dataPi, int stridePu, int stridePi, int nbLigPu, int nbLigPi, int nbColPu, int nbColPi) {
	libImageProcessing->SearchPiece(dataPu, dataPi, stridePu, stridePi, nbLigPu, nbLigPi, nbColPu, nbColPi);
}