// classe images segmentées, signatures

#pragma once

#ifndef _IMAGE_CLASSE_
#define _IMAGE_CLASSE_

#include <string>
#include <vector>

#include "ImageNdg.h"
#include "ImageCouleur.h"

#define EUCLIDEAN(a,b) ((a-b) * (a-b)) 
#define MAX_ITER 100

typedef struct  {
	float  moyenne;
	int    surface;
} SIGNATURE_Ndg;

typedef struct  {
	float  moyenne[3];
	int    surface;
} SIGNATURE_Couleur;

typedef struct {
	float		centreGravite_i;
	float		centreGravite_j;
	int			surface;
	std::string codeFreeman; // au sens V8
	int			premierPt_i; // premier point rencontré sens de parcours avant
	int			premierPt_j;
	int			rectEnglob_Hi; // coins supérieur gauche et inférieur droit
	int			rectEnglob_Hj;
	int			rectEnglob_Bi;
	int			rectEnglob_Bj;
	float		perimetre; // au sens V8
} SIGNATURE_Forme;

// définition classe Image Classe --> images étiquetées pour analyse objets, nuées dynamiques pour analyse régions

class CImageClasse {

	///////////////////////////////////////
	private : 
	///////////////////////////////////////

		int              m_iHauteur;
		int              m_iLargeur;
		std::string      m_sNom;
		long			 m_lNbRegions;

		unsigned long*   m_pucPixel;

	///////////////////////////////////////
	public : 
	///////////////////////////////////////
		
		// constructeurs
		_declspec(dllexport) CImageClasse(); 
		_declspec(dllexport) CImageClasse(int hauteur, int largeur); 
		_declspec(dllexport) CImageClasse(const CImageNdg& im, std::string choix = "V8"); // objets
		_declspec(dllexport) CImageClasse(const CImageClasse& in, std::string misAJour = "sans", std::string voisinage = "V8"); // re-étiquetage éventuel
		_declspec(dllexport) CImageClasse(const CImageNdg& im, int nbClusters = 2, std::string choix = "aleatoire"); // clustering 
		_declspec(dllexport) CImageClasse(const CImageCouleur& im, int nbClusters = 2, std::string choix = "aleatoire", std::string espace = "hsv", int plan = 0);

		_declspec(dllexport) CImageClasse(const CImageNdg& im, std::vector<unsigned char>* germes = NULL); // clustering contraint, germes a priori

		_declspec(dllexport) ~CImageClasse(); // destructeur

		// sauvegarde au format bmp
		// attention : cast des informations car pertes potentielles
		_declspec(dllexport) void sauvegarde(const std::string& file = ""); // sauvegarde data au format BMP avec cast des long en char

		// pouvoir accéder à un pixel par image(i)
		_declspec(dllexport) unsigned long& operator() (int i) const { 
		return m_pucPixel[i];
		}

		// pouvoir accéder à un pixel par image(i,j)
		_declspec(dllexport) unsigned long& operator() (int i, int j) const { 
		return m_pucPixel[i*m_iLargeur+j];
		}

		// opérateur copie image par imOut = imIn
		_declspec(dllexport) CImageClasse& operator=(const CImageClasse& im);

		// get et set 

		_declspec(dllexport) int lireHauteur() const { 
		return m_iHauteur;
		}

		_declspec(dllexport) int lireLargeur() const {
		return m_iLargeur;
		}

		_declspec(dllexport) int lireNbRegions() const {
		return m_lNbRegions;
		}

		_declspec(dllexport) std::string lireNom() const {
		return m_sNom;
		}

		_declspec(dllexport) int lireNbPixels() const {
		return m_iHauteur*m_iLargeur;
		}

		_declspec(dllexport) void ecrireHauteur(int hauteur) {
		m_iHauteur = hauteur;
		}

		_declspec(dllexport) void ecrireLargeur(int largeur) {
		m_iLargeur = largeur;
		}

		_declspec(dllexport) void ecrireNom(std::string nom) {
		m_sNom = nom;
		}

		_declspec(dllexport) void ecrireNbRegions(int nb) {
		m_lNbRegions = nb;
		}

		// signatures pour Image_Ndg et Image_Couleur
		_declspec(dllexport) std::vector<SIGNATURE_Ndg> signatures(const CImageNdg& img, bool enregistrementCSV = false);
		_declspec(dllexport) std::vector<SIGNATURE_Couleur> signatures(const CImageCouleur& img, bool enregistrementCSV = false);

		// affichage chaque région -> sa valeur moyenne
		_declspec(dllexport) CImageCouleur affichageMoyenne(const CImageCouleur& img, bool fondAPart = true);
		_declspec(dllexport) CImageNdg affichageMoyenne(const CImageNdg& img, bool fondAPart = true);

		// filtrage selon critères taille, bords, etc
		_declspec(dllexport) CImageClasse filtrage(const std::string& methode = "taille", int taille=50, bool miseAJour = false);

		// signatures forme pour Image_Ndg et Image_Couleur
		_declspec(dllexport) std::vector<SIGNATURE_Forme> sigComposantesConnexes(bool enregistrementCSV = false);

		// morphologie
		_declspec(dllexport) CImageClasse morphologie(const std::string& methode = "erosion", const std::string& eltStructurant = "V8");
};

#endif _IMAGE_CLASSE_