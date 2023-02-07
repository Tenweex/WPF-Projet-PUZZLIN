#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <limits>
#include <stack>
#include <algorithm>

#include "ImageDouble.h"

#define PI 3.14159265358979323846

// constructeurs et destructeur
CImageDouble::CImageDouble() {

	this->m_iHauteur  = 0;
	this->m_iLargeur  = 0;
	this->m_sNom      = "vide";
	this->m_vMin      = 0;
	this->m_vMax      = 0;
	this->m_pucPixel  = NULL;
}

CImageDouble::CImageDouble(int hauteur, int largeur) {

	this->m_iHauteur = hauteur;
	this->m_iLargeur = largeur;
	this->m_sNom     = "inconnu";
	this->m_vMin     = 0;
	this->m_vMax     = 0;

	this->m_pucPixel = new double[hauteur*largeur];
	for (int i=0;i<hauteur*largeur;i++)
		this->m_pucPixel[i] = 0;
}

CImageDouble::CImageDouble(const CImageDouble& im) {

	this->m_iHauteur = im.lireHauteur();
	this->m_iLargeur = im.lireLargeur();
	this->m_sNom = im.lireNom();
	this->m_vMax = im.lireMax();
	this->m_vMin = im.lireMin();
	this->m_pucPixel = NULL;

	if (im.m_pucPixel != NULL) {
		this->m_pucPixel = new double[im.lireNbPixels()];
		memcpy(this->m_pucPixel, im.m_pucPixel, im.lireNbPixels()*sizeof(double));
	}
}

CImageDouble::~CImageDouble() {

	if (this->m_pucPixel) {
		delete[] this->m_pucPixel;
		this->m_pucPixel = NULL;
	}
}

CImageDouble::CImageDouble(const CImageNdg& im, const std::string& methode) {

	this->m_iHauteur = im.lireHauteur();
	this->m_iLargeur = im.lireLargeur();

	this->m_pucPixel = new double[im.lireNbPixels()];
	if (methode.compare("normalise") == 0) {
		this->m_sNom = im.lireNom() + "DN";
		this->m_vMin = 0;
		this->m_vMax = 1;
		for (int i = 0; i < this->lireNbPixels(); i++)
			this->operator()(i) = (double)im(i) / 255;
	}
	else
		if (methode.compare("cast") == 0) {
			this->m_sNom = im.lireNom() + "DC";
			this->m_vMin = 255;
			this->m_vMax = 0;
			for (int i = 0; i < this->lireNbPixels(); i++)
			{
				this->operator()(i) = (double)im(i);
				if (this->operator()(i) > this->m_vMax)
					this->m_vMax = this->operator()(i);
				if (this->operator()(i) < this->m_vMin)
					this->m_vMin = this->operator()(i);
			}
		}
		else
			if (methode.compare("integrale1") == 0) {
				this->m_sNom = im.lireNom() + "DI1";

				CImageDouble s(im.lireHauteur(), im.lireLargeur());

				// première ligne
				s(0) = (double)im(0, 0);
				this->operator()(0) = (double)im(0, 0);
				this->ecrireMin((double)im(0, 0));

				for (int j = 1; j < im.lireLargeur(); j++) {
					s(j) = s(j - 1) + (double)im(0, j);
					this->operator()(j) = this->operator()(j - 1) + (double)im(0, j);
				}

				// le reste
				for (int i = 1; i < im.lireHauteur(); i++) {
					s(i, 0) = (double)im(i, 0);
					this->operator()(i, 0) = this->operator()(i - 1, 0) + (double)im(i, 0);
					for (int j = 1; j < im.lireLargeur(); j++) {
						s(i, j) = s(i, j - 1) + (double)im(i, j);
						this->operator()(i, j) = this->operator()(i - 1, j) + s(i, j);
					}
				}

				this->ecrireMax(this->operator()(this->lireNbPixels() - 1));
			}
			else if (methode.compare("integrale2") == 0) {
				this->m_sNom = im.lireNom() + "DI2";
				CImageDouble s(im.lireHauteur(), im.lireLargeur());

				// première ligne
				s(0) = (double)im(0, 0)*(double)im(0, 0);
				this->operator()(0) = (double)im(0, 0)*(double)im(0, 0);
				this->ecrireMin(this->operator()(0));

				for (int j = 1; j < im.lireLargeur(); j++) {
					s(j) = s(j - 1) + (double)im(0, j)*(double)im(0, j);
					this->operator()(j) = this->operator()(j - 1) + (double)im(0, j)*(double)im(0, j);
				}

				// le reste
				for (int i = 1; i < im.lireHauteur(); i++) {
					s(i, 0) = (double)im(i, 0)* (double)im(i, 0);
					this->operator()(i, 0) = this->operator()(i - 1, 0) + (double)im(i, 0)* (double)im(i, 0);
					for (int j = 1; j < im.lireLargeur(); j++) {
						s(i, j) = s(i, j - 1) + (double)im(i, j)*(double)im(i, j);
						this->operator()(i, j) = this->operator()(i - 1, j) + s(i, j);
					}
				}

				this->ecrireMax(this->operator()(this->lireNbPixels() - 1));
			}
}

CImageDouble& CImageDouble::operator=(const CImageDouble& im) {

	if (&im == this)
		return *this;

	this->m_iHauteur = im.lireHauteur();
	this->m_iLargeur = im.lireLargeur();
	this->m_sNom = im.lireNom();
	this->m_vMax = im.lireMax();
	this->m_vMin = im.lireMin();

	if (this->m_pucPixel)
		delete[] this->m_pucPixel;
	this->m_pucPixel = new double[im.lireNbPixels()];

	if (im.m_pucPixel != NULL)
		memcpy(this->m_pucPixel, im.m_pucPixel, im.lireNbPixels()*sizeof(double));

	return *this;
}

// distance au fond
CImageDouble CImageDouble::distance(std::string eltStructurant, double valBord) {
	// distance au fond
	// gestion du bord : 0 ou autre valeur valant max des float

	CImageDouble out(this->lireHauteur(), this->lireLargeur());
	out.ecrireNom(this->lireNom() + "DF");
	out.ecrireMax(DBL_MIN);
	out.ecrireMin(DBL_MAX);

	CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);
	// gestion des bords ajoutés
	if (valBord != 0) {
		for (int i = 0; i<agrandie.lireHauteur(); i++) {
			agrandie(i, 0) = valBord;
			agrandie(i, agrandie.lireLargeur() - 1) = valBord;
		}
		for (int j = 0; j<agrandie.lireLargeur(); j++) {
			agrandie(0, j) = valBord;
			agrandie(agrandie.lireHauteur() - 1, j) = valBord;
		}
	}

	// coeur image
	for (int i = 0; i<this->lireHauteur(); i++)
		for (int j = 0; j<this->lireLargeur(); j++) {
			agrandie(i + 1, j + 1) = (this->operator()(i, j) > 0) ? (double)INT_MAX-1 : 0;
		}

	if (eltStructurant.compare("V4") == 0) {
		// parcours avant
		for (int i = 1; i<agrandie.lireHauteur() - 1; i++)
			for (int j = 1; j<agrandie.lireLargeur() - 1; j++) {
				double pixel = agrandie(i, j);
				pixel = std::min(pixel, agrandie(i - 1, j) + 1);
				pixel = std::min(pixel, agrandie(i, j - 1) + 1);
				agrandie(i, j) = pixel;
			}
		for (int i = agrandie.lireHauteur() - 2; i >= 1; i--)
			for (int j = agrandie.lireLargeur() - 2; j >= 1; j--) {
				double pixel = agrandie(i, j);
				pixel = std::min(pixel, agrandie(i + 1, j) + 1);
				pixel = std::min(pixel, agrandie(i, j + 1) + 1);
				agrandie(i, j) = pixel;
			}
		// conservation du centre
		for (int i = 1; i<agrandie.lireHauteur() - 1; i++)
			for (int j = 1; j<agrandie.lireLargeur() - 1; j++) {
				out(i - 1, j - 1) = agrandie(i, j);
			}
	}
	else {
		// parcours avant
		for (int i = 1; i<agrandie.lireHauteur() - 1; i++)
			for (int j = 1; j<agrandie.lireLargeur() - 1; j++) {
				double pixel = agrandie(i, j);
				pixel = std::min(pixel, agrandie(i - 1, j) + 1);
				pixel = std::min(pixel, agrandie(i, j - 1) + 1);
				pixel = std::min(pixel, agrandie(i - 1, j - 1) + 1);
				pixel = std::min(pixel, agrandie(i - 1, j + 1) + 1);
				agrandie(i, j) = pixel;
			}
		for (int i = agrandie.lireHauteur() - 2; i >= 1; i--)
			for (int j = agrandie.lireLargeur() - 2; j >= 1; j--) {
				double pixel = agrandie(i, j);
				pixel = std::min(pixel, agrandie(i + 1, j) + 1);
				pixel = std::min(pixel, agrandie(i, j + 1) + 1);
				pixel = std::min(pixel, agrandie(i + 1, j + 1) + 1);
				pixel = std::min(pixel, agrandie(i + 1, j - 1) + 1);
				agrandie(i, j) = pixel;
			}
		// conservation du centre
		for (int i = 1; i<agrandie.lireHauteur() - 1; i++)
			for (int j = 1; j<agrandie.lireLargeur() - 1; j++) {
				out(i - 1, j - 1) = agrandie(i, j);
				if (out(i - 1, j - 1) < out.lireMin())
					out.ecrireMin(out(i - 1, j - 1));
				if (out(i - 1, j - 1) > out.lireMax())
					out.ecrireMax(out(i - 1, j - 1));
			}
	}

	return out;
}

// conversion entre types
CImageNdg CImageDouble::toNdg(const std::string& methode) {

	CImageNdg out(this->lireHauteur(), this->lireLargeur());
	out.ecrireBinaire(false);
	out.choixPalette("grise");
	out.ecrireNom(this->lireNom() + "2NDG");

	if (methode.compare("defaut") == 0) {
		for (int i = 0; i < this->lireNbPixels(); i++)
			if (this->operator()(i) < 0)
				out(i) = 0;
			else
				if (this->operator()(i) > 256)
					out(i) = 255;
				else
					out(i) = (unsigned char)this->operator()(i);
	}
	else
		if (methode.compare("expansion") == 0) {
			double a = 255 / (this->lireMax() - this->lireMin());
			double b = -a*this->lireMin();

			for (int i = 0; i < this->lireNbPixels(); i++)
				out(i) = (unsigned char)(a*this->operator()(i)+b);
		}

	return(out);
}

// vecteur gradient
CImageDouble CImageDouble::vecteurGradient(const std::string& axe) {
	CImageDouble out(this->lireHauteur(), this->lireLargeur());

	if (axe.compare("norme") == 0)
	{
		out.ecrireNom(this->lireNom() + "NVG");
		double vMin = sqrt(2) * 255;
		double vMax = 0;

		CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);

		// gestion des bords et des coins
		int pix;
		agrandie(0, 0) = this->operator()(0, 0);
		agrandie(0, agrandie.lireLargeur() - 1) = this->operator()(0, this->lireLargeur() - 1);
		for (pix = 1; pix < agrandie.lireLargeur() - 1; pix++) {
			agrandie(0, pix) = this->operator()(0, pix - 1);
			agrandie(agrandie.lireHauteur() - 1, pix) = this->operator()(this->lireHauteur() - 1, pix - 1);
		}
		agrandie(agrandie.lireHauteur() - 1, 0) = this->operator()(this->lireHauteur() - 1, 0);
		agrandie(agrandie.lireHauteur() - 1, agrandie.lireLargeur() - 1) = this->operator()(this->lireHauteur() - 1, this->lireLargeur() - 1);
		for (pix = 1; pix < agrandie.lireHauteur() - 1; pix++) {
			agrandie(pix, 0) = this->operator()(pix - 1, 0);
			agrandie(pix, agrandie.lireLargeur() - 1) = this->operator()(pix - 1, this->lireLargeur() - 1);
		}

		// gestion du coeur
		for (int i = 0; i < this->lireHauteur(); i++)
			for (int j = 0; j < this->lireLargeur(); j++) {
				agrandie(i + 1, j + 1) = this->operator()(i, j);
			}

		// passage des noyaux

		for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
			for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
			{
				double vx = agrandie(i + 1, j) - agrandie(i - 1, j);
				double vy = agrandie(i, j + 1) - agrandie(i, j - 1);

				out(i - 1, j - 1) = sqrt((vx*vx) + (vy*vy));
				if (out(i - 1, j - 1) < vMin)
					vMin = out(i - 1, j - 1);
				if (out(i - 1, j - 1) > vMax)
					vMax = out(i - 1, j - 1);
			}

		out.ecrireMin(vMin);
		out.ecrireMax(vMax);
	}
	else
		if (axe.compare("angle") == 0)
		{
			out.ecrireNom(this->lireNom() + "AVG");

			CImageDouble agrandie(this->lireHauteur() + 2, this->lireLargeur() + 2);

			// gestion des bords et des coins
			int pix;
			agrandie(0, 0) = this->operator()(0, 0);
			agrandie(0, agrandie.lireLargeur() - 1) = this->operator()(0, this->lireLargeur() - 1);
			for (pix = 1; pix < agrandie.lireLargeur() - 1; pix++) {
				agrandie(0, pix) = this->operator()(0, pix - 1);
				agrandie(agrandie.lireHauteur() - 1, pix) = this->operator()(this->lireHauteur() - 1, pix - 1);
			}
			agrandie(agrandie.lireHauteur() - 1, 0) = this->operator()(this->lireHauteur() - 1, 0);
			agrandie(agrandie.lireHauteur() - 1, agrandie.lireLargeur() - 1) = this->operator()(this->lireHauteur() - 1, this->lireLargeur() - 1);
			for (pix = 1; pix < agrandie.lireHauteur() - 1; pix++) {
				agrandie(pix, 0) = this->operator()(pix - 1, 0);
				agrandie(pix, agrandie.lireLargeur() - 1) = this->operator()(pix - 1, this->lireLargeur() - 1);
			}

			// gestion du coeur
			for (int i = 0; i < this->lireHauteur(); i++)
				for (int j = 0; j < this->lireLargeur(); j++) {
					agrandie(i + 1, j + 1) = this->operator()(i, j);
				}

			// passage des noyaux

			for (int i = 1; i < agrandie.lireHauteur() - 1; i++)
				for (int j = 1; j < agrandie.lireLargeur() - 1; j++)
				{
					double vx = agrandie(i + 1, j) - agrandie(i - 1, j);
					double vy = agrandie(i, j + 1) - agrandie(i, j - 1);
					out(i - 1, j - 1) = atan2(vy, vx) * 180 / PI;
				}

			// fixer intervall -PI.. PI soit -180 .. 180 en degres
			out.ecrireMin(-180); 
			out.ecrireMax(180);
		}

	return out;
}

// filtrage : moyen ou gaussien (approches par vecteur et transposée plus rapide)
CImageDouble CImageDouble::filtrage(const std::string& methode, int N, double sigma) {

	CImageDouble out(this->lireHauteur(), this->lireLargeur());
	out.m_vMax = DBL_MIN;
	out.m_vMin = DBL_MAX;

	if (methode.compare("moyen") == 0) {
		out.m_sNom = this->lireNom() + "FMo";
		int nbBords = N / 2;

		CImageDouble agrandie(this->lireHauteur() + nbBords * 2, this->lireLargeur() + nbBords * 2);

		// gestion du coeur
		for (int i = 0; i < this->lireHauteur(); i++)
			for (int j = 0; j < this->lireLargeur(); j++) {
				agrandie(i + nbBords, j + nbBords) = this->operator()(i, j);
			}

		// gestion des bords
		for (int pix = 0; pix < agrandie.lireLargeur(); pix++) {
			for (int t = nbBords - 1; t >= 0; t--)
				agrandie(t, pix) = agrandie(nbBords, pix);
			for (int t = agrandie.lireHauteur() - 1; t >= agrandie.lireHauteur() - 1 - nbBords; t--)
				agrandie(t, pix) = agrandie(agrandie.lireHauteur() - 1 - nbBords, pix);
		}
		for (int pix = 0; pix < agrandie.lireHauteur(); pix++) {
			for (int t = nbBords - 1; t >= 0; t--)
				agrandie(pix, t) = agrandie(pix, nbBords);
			for (int t = agrandie.lireLargeur() - 1; t >= agrandie.lireLargeur() - 1 - nbBords; t--)
				agrandie(pix, t) = agrandie(pix, agrandie.lireLargeur() - 1 - nbBords);
		}

		CImageDouble agrandie2 = agrandie;

		// colonnes
		for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
			for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++) {
				double somme = 0;
				double moy = 0;

				for (int k = -nbBords; k <= nbBords; k++) {
					moy += (double)agrandie(i - k, j);
					somme += (double)1;
				}
				agrandie2(i, j) = moy / somme;
			}
		// lignes
		for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
			for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++) {
				double somme = 0;
				double moy = 0;

				for (int l = -nbBords; l <= nbBords; l++) {
					moy += (double)agrandie2(i, j - l);
					somme += (double)1;
				}
				agrandie(i, j) = (moy / somme);
			}
		// image out
		for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
			for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
			{
				out(i - nbBords, j - nbBords) = agrandie(i, j);
				if (out(i - nbBords, j - nbBords) < out.lireMin())
					out.ecrireMin(out(i - nbBords, j - nbBords));
				if (out(i - nbBords, j - nbBords) > out.lireMax())
					out.ecrireMax(out(i - nbBords, j - nbBords));
			}
	}
	else
		if (methode.compare("gaussien") == 0)
		{
			out.m_sNom = this->lireNom() + "FGa";
			// définition du noyau
			double noyau[50]; // taille maxi pour optimisation 

			double somme = 0; // normalisation
			for (int i = 0; i < N; i++)
			{
				noyau[i] = exp(-((i - N / 2)*(i - N / 2)) / (2 * sigma*sigma));
				somme += noyau[i];
			}

			// filtrage
			int nbBords = N / 2;

			CImageDouble agrandie(this->lireHauteur() + nbBords * 2, this->lireLargeur() + nbBords * 2);

			// gestion du coeur
			for (int i = 0; i < this->lireHauteur(); i++)
				for (int j = 0; j < this->lireLargeur(); j++) {
					agrandie(i + nbBords, j + nbBords) = this->operator()(i, j);
				}

			// gestion des bords
			for (int pix = 0; pix < agrandie.lireLargeur(); pix++) {
				for (int t = nbBords - 1; t >= 0; t--)
					agrandie(t, pix) = agrandie(nbBords, pix);
				for (int t = agrandie.lireHauteur() - 1; t >= agrandie.lireHauteur() - 1 - nbBords; t--)
					agrandie(t, pix) = agrandie(agrandie.lireHauteur() - 1 - nbBords, pix);
			}
			for (int pix = 0; pix < agrandie.lireHauteur(); pix++) {
				for (int t = nbBords - 1; t >= 0; t--)
					agrandie(pix, t) = agrandie(pix, nbBords);
				for (int t = agrandie.lireLargeur() - 1; t >= agrandie.lireLargeur() - 1 - nbBords; t--)
					agrandie(pix, t) = agrandie(pix, agrandie.lireLargeur() - 1 - nbBords);
			}

			CImageDouble agrandie2 = agrandie;

			// colonnes
			for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
				for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++) {
					double somme = 0;
					double moy = 0;

					for (int k = -nbBords; k <= nbBords; k++) {
						moy += (double)agrandie(i - k, j)*noyau[k + nbBords];
						somme += noyau[k + nbBords];
					}
					agrandie2(i, j) = moy / somme;
				}
			// lignes
			for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
				for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++) {
					double somme = 0;
					double moy = 0;

					for (int l = -nbBords; l <= nbBords; l++) {
						moy += (double)agrandie2(i, j - l)*noyau[l + nbBords];
						somme += noyau[l + nbBords];
					}
					agrandie(i, j) = (moy / somme);
				}
			// image out
			for (int i = nbBords; i < agrandie.lireHauteur() - nbBords; i++)
				for (int j = nbBords; j < agrandie.lireLargeur() - nbBords; j++)
				{
					out(i - nbBords, j - nbBords) = agrandie(i, j);
					if (out(i - nbBords, j - nbBords) < out.lireMin())
						out.ecrireMin(out(i - nbBords, j - nbBords));
					if (out(i - nbBords, j - nbBords) > out.lireMax())
						out.ecrireMax(out(i - nbBords, j - nbBords));
				}
		}

	return out;
}

std::vector<CImageDouble> CImageDouble::pyramide(int hauteur, int tailleFiltre, double sigma)
{
	std::vector<CImageDouble> burt;

	// propagation des min,max du niveau initial aux autres niveaux, sinon va modifier la dynamique des niveaux
	burt.resize(hauteur);
	burt.at(0) = *this;
	burt.at(0).ecrireNom(this->lireNom() + std::to_string(0));

	for (int niv = 1; niv < hauteur; niv++)
	{
		// filtrage gaussien qui améliore les résultats avant sous-échantillonage
		CImageDouble niveau = burt.at(niv - 1).filtrage("gaussien", 5, 1);
		CImageDouble inter(niveau.lireHauteur() / 2, niveau.lireLargeur() / 2);

		inter.ecrireMax(this->lireMax()); // pour ne pas changer la dynamique entre niveaux
		inter.ecrireMin(this->lireMin());
		inter.ecrireNom(this->lireNom() + std::to_string(niv));

		for (int i = 0; i < inter.lireHauteur(); i++)
			for (int j = 0; j < inter.lireLargeur(); j++)
				inter(i, j) = (niveau(2 * i, 2 * j) + niveau(2 * i + 1, 2 * j) + niveau(2 * i, 2 * j + 1) + niveau(2 * i + 1, 2 * j + 1)) / 4;

		burt.at(niv) = inter;
	}

	return burt;
}

CImageDouble CImageDouble::maxiLocaux(int N, int M)
{
	CImageDouble out(this->lireHauteur(), this->lireLargeur());
	out.m_sNom = this->lireNom() + "ML";
	out.m_vMax = this->lireMax();
	out.m_vMin = this->lireMin(); // conservation dynamique

	int ns2 = N / 2;
	int ms2 = M / 2;

	for (int i = 0; i < this->lireHauteur(); i++)
		for (int j = 0; j < this->lireLargeur(); j++) {
			int dk = std::max(0, i - ns2);
			int fk = std::min(i + ns2, this->lireHauteur() - 1);
			int dl = std::max(0, j - ms2);
			int fl = std::min(j + ms2, this->lireLargeur() - 1);

			double maxVal = this->operator()(i, j);
			bool flag = true;
			int k = dk;
			while ((k <= fk) && (flag == true)) {
				int l = dl;
				while ((l <= fl) && (flag == true)) {
					if (this->operator()(k, l) > maxVal)
						flag = false;
					l++;
				}
				k++;
			}
			if (flag == true)
				out(i, j) = this->operator()(i, j);
			else
				out(i, j) = 0;
		}

	return out;

}
