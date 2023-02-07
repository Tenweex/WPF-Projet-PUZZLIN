#include "CLibImageProcessing.h"

CLibImageProcessing::CLibImageProcessing() { }


// Puzzle
void CLibImageProcessing::PuzzleImageProcessing(byte* data, int stride, int nbLig, int nbCol) {
	// Extraction de l'image
	this->_imgPuzzleInPtr = new CImageCouleur(nbLig, nbCol);

	byte* pixPtr = (byte*)data;

	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			this->_imgPuzzleInPtr->operator()(y, x)[0] = pixPtr[3 * x + 2];
			this->_imgPuzzleInPtr->operator()(y, x)[1] = pixPtr[3 * x + 1];
			this->_imgPuzzleInPtr->operator()(y, x)[2] = pixPtr[3 * x];
		}
		pixPtr += stride;
	}

	CImageCouleur imgPuzzle(*_imgPuzzleInPtr);

	// Traitement
	CImageNdg puzzleR = this->_imgPuzzleInPtr->plan(0);
	CImageNdg puzzleG = this->_imgPuzzleInPtr->plan(1);
	CImageNdg puzzleB = this->_imgPuzzleInPtr->plan(2);
	CImageNdg puzzleL = this->_imgPuzzleInPtr->plan(3);

	CImageNdg puzzleInvertG = puzzleG.inverseImg(puzzleG);
	CImageNdg puzzleFiltreG = puzzleInvertG.filtrage("moyennage");

	CImageNdg puzzleSeuilR = puzzleR.seuillage("otsu");
	CImageNdg puzzleSeuilG = puzzleFiltreG.seuillage("otsu");
	CImageNdg puzzleSeuilB = puzzleB.seuillage("otsu");
	CImageNdg puzzleSeuilL = puzzleL.seuillage("otsu");


	CImageNdg puzzle(puzzleB.lireHauteur(), puzzleB.lireLargeur(), 0); // somme des plan RGB du puzzle pour labeliser l'image
	for (int i = 0; i < puzzleB.lireHauteur() * puzzleB.lireLargeur(); i++) {
		if (puzzleSeuilL(i) != 0 || puzzleSeuilR(i) != 0 || puzzleSeuilG(i) != 0 || puzzleSeuilB(i) != 0) {
			puzzle(i) = 255;
		}
	}

	CImageNdg imgPuBin = puzzle;
	imgPuBin = imgPuBin.morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8");

	CImageClasse imglabPuz(imgPuBin, "V8");

	std::vector<SIGNATURE_Forme> SigPuzzle = imglabPuz.sigComposantesConnexes();

	int surfaceMax = 0, Labelpuzzle = 0;
	for (int i = 1; i < SigPuzzle.size(); i++)
	{
		if (SigPuzzle[i].surface > surfaceMax) {
			surfaceMax = SigPuzzle[i].surface;
			Labelpuzzle = i;
		}
	}

	CImageCouleur imgPuzzleSuppFond(imglabPuz.lireHauteur(), imglabPuz.lireLargeur(), 0, 0, 0);
	for (int i = 0; i < imglabPuz.lireHauteur() * imglabPuz.lireLargeur(); i++) {
		if (imglabPuz(i) == Labelpuzzle) {
			imgPuzzleSuppFond(i)[0] = imgPuzzle(i)[0];
			imgPuzzleSuppFond(i)[1] = imgPuzzle(i)[1];
			imgPuzzleSuppFond(i)[2] = imgPuzzle(i)[2];
			imgPuzzleSuppFond(i)[3] = imgPuzzle(i)[3];
		}
	}


	// Taille du crop
	int sBas = 1, sHaut = 255;
	CImageNdg imgPuSF_seuil = imgPuzzleSuppFond.plan(3).seuillage("manuel", sBas, sHaut);

	CImageClasse imgPuSF_lab(imgPuSF_seuil, "V8");
	std::vector<SIGNATURE_Forme> SigPuSF = imgPuSF_lab.sigComposantesConnexes();

	_PuzzleCropHauteur = (SigPuSF[1].rectEnglob_Bi - SigPuSF[1].rectEnglob_Hi) + 20;
	_PuzzleCropLargeur = (SigPuSF[1].rectEnglob_Bj - SigPuSF[1].rectEnglob_Hj) + 20;

	_imgPuzzleSuppFond = imgPuzzleSuppFond;
	CImageCouleur imgOut = imgPuzzleSuppFond;

	// Retour de l'image
	pixPtr = (byte*)data;
	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			pixPtr[3 * x + 2] = imgOut(y, x)[0];
			pixPtr[3 * x + 1] = imgOut(y, x)[1];
			pixPtr[3 * x] = imgOut(y, x)[2];
		}
		pixPtr += stride;
	}
}

int CLibImageProcessing::GetHauteurPuzzleCrop()
{
	return _PuzzleCropHauteur;
}

int CLibImageProcessing::GetLargeurPuzzleCrop()
{
	return _PuzzleCropLargeur;
}

void CLibImageProcessing::PuzzleCrop(byte* data, int stride, int nbLig, int nbCol)
{
	// Traitement
	int sBas = 1, sHaut = 255;
	CImageNdg imgPuSF_seuil = _imgPuzzleSuppFond.plan(3).seuillage("manuel", sBas, sHaut);

	CImageClasse imgPuSF_lab(imgPuSF_seuil, "V8");
	std::vector<SIGNATURE_Forme> SigPuSF = imgPuSF_lab.sigComposantesConnexes();

	CImageCouleur imgPuSF_crop((SigPuSF[1].rectEnglob_Bi - SigPuSF[1].rectEnglob_Hi) + 20, (SigPuSF[1].rectEnglob_Bj - SigPuSF[1].rectEnglob_Hj) + 20, 0, 0, 0);

	int crop_i = 10, crop_j = 10;
	// Création image du puzzle croppé
	for (int i = SigPuSF[1].rectEnglob_Hi; i < SigPuSF[1].rectEnglob_Bi; i++)
	{
		crop_j = 10;
		for (int j = SigPuSF[1].rectEnglob_Hj; j < SigPuSF[1].rectEnglob_Bj; j++)
		{
			imgPuSF_crop(crop_i, crop_j)[0] = _imgPuzzleSuppFond(i, j)[0];
			imgPuSF_crop(crop_i, crop_j)[1] = _imgPuzzleSuppFond(i, j)[1];
			imgPuSF_crop(crop_i, crop_j)[2] = _imgPuzzleSuppFond(i, j)[2];
			crop_j++;
		}
		crop_i++;
	}
	
	CImageCouleur imgOut = imgPuSF_crop;

	// Retour de l'image
	byte* pixPtr = (byte*)data;
	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			pixPtr[3 * x + 2] = imgOut(y, x)[0];
			pixPtr[3 * x + 1] = imgOut(y, x)[1];
			pixPtr[3 * x] = imgOut(y, x)[2];
		}
		pixPtr += stride;
	}
}


// Piece
void CLibImageProcessing::PieceImageProcessing(byte* data, int stride, int nbLig, int nbCol) {
	// Extraction de l'image
	this->_imgPieceInPtr = new CImageCouleur(nbLig, nbCol);

	byte* pixPtr = (byte*)data;

	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			this->_imgPieceInPtr->operator()(y, x)[0] = pixPtr[3 * x + 2];
			this->_imgPieceInPtr->operator()(y, x)[1] = pixPtr[3 * x + 1];
			this->_imgPieceInPtr->operator()(y, x)[2] = pixPtr[3 * x];
		}
		pixPtr += stride;
	}

	CImageCouleur imgPiece(*_imgPieceInPtr);

	// Traitement
	CImageNdg pieceR = this->_imgPieceInPtr->plan(0);
	CImageNdg pieceG = this->_imgPieceInPtr->plan(1);
	CImageNdg pieceB = this->_imgPieceInPtr->plan(2);
	CImageNdg pieceL = this->_imgPieceInPtr->plan(3);

	CImageNdg pieceInvertG = pieceG.inverseImg(pieceG);

	CImageNdg pieceSeuilR = pieceR.seuillage("otsu");
	CImageNdg pieceSeuilG = pieceInvertG.seuillage("otsu");
	CImageNdg pieceSeuilB = pieceB.seuillage("otsu");
	CImageNdg pieceSeuilL = pieceL.seuillage("otsu");


	CImageNdg piece(pieceB.lireHauteur(), pieceB.lireLargeur(), 0); // somme des plan RGB de la pièce
	for (int i = 0; i < pieceB.lireHauteur() * pieceB.lireLargeur(); i++) {
		if (pieceSeuilL(i) != 0 || pieceSeuilR(i) != 0 || pieceSeuilG(i) != 0 || pieceSeuilB(i) != 0) {
			piece(i) = 255;
		}
	}

	CImageNdg imgPiBin = piece;

	imgPiBin = imgPiBin.morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("dilatation", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8").morphologie("erosion", "V8");

	CImageClasse imglab(imgPiBin, "V8");

	std::vector<SIGNATURE_Forme> SigPiece = imglab.sigComposantesConnexes();

	int LabelPiece = 0;

	for (int i = 0; i < SigPiece.size(); i++)
	{
		if ((SigPiece[i].rectEnglob_Hi != 0) && (SigPiece[i].rectEnglob_Hj != 0) && (SigPiece[i].rectEnglob_Bi != imglab.lireHauteur()) && (SigPiece[i].rectEnglob_Bj != imglab.lireLargeur()) && (SigPiece[i].surface > 1000)) {

			LabelPiece = i;
		}
	}

	CImageCouleur imgPieceSuppFond(imglab.lireHauteur(), imglab.lireLargeur(), 0, 0, 0);
	for (int i = 0; i < imglab.lireHauteur() * imglab.lireLargeur(); i++) {
		if (imglab(i) == LabelPiece) {
			imgPieceSuppFond(i)[0] = imgPiece(i)[0];
			imgPieceSuppFond(i)[1] = imgPiece(i)[1];
			imgPieceSuppFond(i)[2] = imgPiece(i)[2];
			imgPieceSuppFond(i)[3] = imgPiece(i)[3];
		}
	}

	// crop piece
	int sBas = 1, sHaut = 255;
	CImageNdg imgPiSF_seuil = imgPieceSuppFond.plan(3).seuillage("manuel", sBas, sHaut);

	imgPiSF_seuil.sauvegarde("imgPiSF_seuil.bmp");

	CImageClasse imgPiSF_lab(imgPiSF_seuil, "V8");
	std::vector<SIGNATURE_Forme> SigPiSF = imgPiSF_lab.sigComposantesConnexes();

	_PieceCropHauteur = (SigPiSF[1].rectEnglob_Bi - SigPiSF[1].rectEnglob_Hi);
	_PieceCropLargeur = (SigPiSF[1].rectEnglob_Bj - SigPiSF[1].rectEnglob_Hj);

	_imgPieceSuppFond = imgPieceSuppFond;
	CImageCouleur imgOut = imgPieceSuppFond;

	// Retour de l'image
	pixPtr = (byte*)data;
	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			pixPtr[3 * x + 2] = imgOut(y, x)[0];
			pixPtr[3 * x + 1] = imgOut(y, x)[1];
			pixPtr[3 * x] = imgOut(y, x)[2];
		}
		pixPtr += stride;
	}
}

int CLibImageProcessing::GetHauteurPieceCrop() {
	return _PieceCropHauteur;
}

int CLibImageProcessing::GetLargeurPieceCrop() {
	return _PieceCropLargeur;
}

void CLibImageProcessing::PieceCrop(byte* data, int stride, int nbLig, int nbCol) {

	// Traitement
	int sBas = 1, sHaut = 255;
	CImageNdg imgPiSF_seuil = _imgPieceSuppFond.plan(3).seuillage("manuel", sBas, sHaut);

	CImageClasse imgPiSF_lab(imgPiSF_seuil, "V8");
	std::vector<SIGNATURE_Forme> SigPiSF = imgPiSF_lab.sigComposantesConnexes();

	CImageCouleur imgPiSF_crop((SigPiSF[1].rectEnglob_Bi - SigPiSF[1].rectEnglob_Hi), (SigPiSF[1].rectEnglob_Bj - SigPiSF[1].rectEnglob_Hj), 0, 0, 0);

	int crop_i = 0, crop_j = 0;
	for (int i = SigPiSF[1].rectEnglob_Hi; i < SigPiSF[1].rectEnglob_Bi; i++)
	{
		crop_j = 0;
		for (int j = SigPiSF[1].rectEnglob_Hj; j < SigPiSF[1].rectEnglob_Bj; j++)
		{
			imgPiSF_crop(crop_i, crop_j)[0] = _imgPieceSuppFond(i, j)[0];
			imgPiSF_crop(crop_i, crop_j)[1] = _imgPieceSuppFond(i, j)[1];
			imgPiSF_crop(crop_i, crop_j)[2] = _imgPieceSuppFond(i, j)[2];
			crop_j++;
		}
		crop_i++;
	}

	CImageCouleur imgOut = imgPiSF_crop;

	// Retour de l'image
	byte* pixPtr = (byte*)data;
	for (int y = 0; y < nbLig; y++)
	{
		for (int x = 0; x < nbCol; x++)
		{
			pixPtr[3 * x + 2] = imgOut(y, x)[0];
			pixPtr[3 * x + 1] = imgOut(y, x)[1];
			pixPtr[3 * x] = imgOut(y, x)[2];
		}
		pixPtr += stride;
	}
}


// Position Piece
void CLibImageProcessing::SearchPiece(byte* dataPu, byte* dataPi, int stridePu, int stridePi, int nbLigPu, int nbLigPi, int nbColPu, int nbColPi) {
	// Extraction de l'image Puzzle
	this->_imgPuzzleCropPtr = new CImageCouleur(nbLigPu, nbColPu);

	byte* PupixPtr = (byte*)dataPu;

	for (int y = 0; y < nbLigPu; y++)
	{
		for (int x = 0; x < nbColPu; x++)
		{
			this->_imgPuzzleCropPtr->operator()(y, x)[0] = PupixPtr[3 * x + 2];
			this->_imgPuzzleCropPtr->operator()(y, x)[1] = PupixPtr[3 * x + 1];
			this->_imgPuzzleCropPtr->operator()(y, x)[2] = PupixPtr[3 * x];
		}
		PupixPtr += stridePu;
	}

	// Extraction de l'image Piece
	this->_imgPieceCropPtr = new CImageCouleur(nbLigPi, nbColPi);

	byte* PipixPtr = (byte*)dataPi;

	for (int y = 0; y < nbLigPi; y++)
	{
		for (int x = 0; x < nbColPi; x++)
		{
			this->_imgPieceCropPtr->operator()(y, x)[0] = PipixPtr[3 * x + 2];
			this->_imgPieceCropPtr->operator()(y, x)[1] = PipixPtr[3 * x + 1];
			this->_imgPieceCropPtr->operator()(y, x)[2] = PipixPtr[3 * x];
		}
		PipixPtr += stridePi;
	}

	CImageCouleur imgPuSF_crop(*_imgPuzzleCropPtr);
	CImageCouleur imgPiSF_crop(*_imgPieceCropPtr);

	// Traitement 

	// Test algo position piece : 
	const int PAS = 25;
	CImageDouble IMGDoublePosPiece(imgPuSF_crop.lireHauteur() - imgPiSF_crop.lireHauteur(), imgPuSF_crop.lireLargeur() - imgPiSF_crop.lireLargeur()); // Creation image double pour somme pix puzzle -> piece

	for (int i = 0; i < (imgPuSF_crop.lireHauteur() - imgPiSF_crop.lireHauteur()); i++)
		for (int j = 0; j < imgPuSF_crop.lireLargeur() - imgPiSF_crop.lireLargeur(); j++)
			for (int m = 0; m < imgPiSF_crop.lireHauteur(); m += PAS)
				for (int n = 0; n < imgPiSF_crop.lireLargeur(); n += PAS)
					IMGDoublePosPiece(i, j) += (abs(imgPuSF_crop(i + m, j + n)[0] - imgPiSF_crop(m, n)[0]) + abs(imgPuSF_crop(i + m, j + n)[1] - imgPiSF_crop(m, n)[1]) + abs(imgPuSF_crop(i + m, j + n)[2] - imgPiSF_crop(m, n)[2])) / 3;


	double min = 10000000000;
	int ipose = 0, jpose = 0;

	for (int i = 0; i < IMGDoublePosPiece.lireHauteur(); i++)
	{
		for (int j = 0; j < IMGDoublePosPiece.lireLargeur(); j++)
		{
			if ((IMGDoublePosPiece(i, j) < min) && (IMGDoublePosPiece(i, j) != 0)) {

				min = IMGDoublePosPiece(i, j);
				ipose = i;
				jpose = j;
			}
		}
	}

	CImageCouleur PosePiece(imgPuSF_crop);

	int jloc = 0, iloc = 0;

	for (int i = ipose; i < ipose + imgPiSF_crop.lireHauteur(); i++)
	{
		jloc = 0;
		for (int j = jpose; j < jpose + imgPiSF_crop.lireLargeur(); j++)
		{
			if ((imgPiSF_crop(iloc, jloc)[0] != 0) && (imgPiSF_crop(iloc, jloc)[1] != 0) && (imgPiSF_crop(iloc, jloc)[2] != 0))
			{
				PosePiece(i, j)[0] = imgPiSF_crop(iloc, jloc)[0];
				PosePiece(i, j)[1] = imgPiSF_crop(iloc, jloc)[1];
				PosePiece(i, j)[2] = imgPiSF_crop(iloc, jloc)[2];
			}

			if ((iloc < 4) || (iloc > imgPiSF_crop.lireHauteur() - 5) || (jloc < 4) || (jloc > imgPiSF_crop.lireLargeur() - 5))
			{
				PosePiece(i, j)[0] = 255;
				PosePiece(i, j)[1] = 0;
				PosePiece(i, j)[2] = 0;
			}
			jloc++;
		}
		iloc++;
	}

	CImageCouleur PuimgOut = PosePiece;

	// Retour de l'image Puzzle
	PupixPtr = (byte*)dataPu;
	for (int y = 0; y < nbLigPu; y++)
	{
		for (int x = 0; x < nbColPu; x++)
		{
			PupixPtr[3 * x + 2] = PuimgOut(y, x)[0];
			PupixPtr[3 * x + 1] = PuimgOut(y, x)[1];
			PupixPtr[3 * x] = PuimgOut(y, x)[2];
		}
		PupixPtr += stridePu;
	}
}