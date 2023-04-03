////////////////////////// TP10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "prod.h"

int lireProchaineCommande() // pour lire l'int contenu dans nextFact
{
	FILE *f;
	int N;
	f = fopen("nextFact", "r");
	fread(&N, sizeof(int), 1, f);
	fclose(f);
	// printf("\n--->lu N=%d",N);
	return N;
}

void convertirNenChaine4(int N, char *N4) // convertit l'int N en une chaine de 4 caracteres
{										  // si N=1234 alors N4 sera égal à "1234"
	int m, c, d, u;
	char cm, cc, cd, cu;
	m = N / 1000;
	cm = 48 + m;
	N %= 1000;
	c = N / 100;
	cc = 48 + c;
	N %= 100;
	d = N / 10;
	cd = 48 + d;
	u = N % 10;
	cu = 48 + u;
	N4[0] = cm;
	N4[1] = cc;
	N4[2] = cd;
	N4[3] = cu;
	N4[4] = '\0';
}

void lireCommande(FILE *commande, char *nombreCommande)
{
	int serieCommande, quantiteCommande;
	int serieProduit;
	char nomProduit[50];
	float prix;
	float total = 0;
	char client[50];
	FILE *factureFile;
	FILE *produits;

	char facture[100] = "factures/facture";
	strcat(facture, nombreCommande);
	strcat(facture, ".txt");
	factureFile = fopen(facture, "w");

	fscanf(commande, "%s", client);
	fprintf(factureFile, "Client : %s\n", client);

	produits = fopen("produits.txt", "r");

	do
	{
		fscanf(commande, "%d %d", &serieCommande, &quantiteCommande);

		rewind(produits);
		do
		{
			fscanf(produits, "%d %s %f", &serieProduit, nomProduit, &prix);
		} while (!feof(produits) && serieProduit != serieCommande);

		fprintf(factureFile, "%d %s (PU=%f€) :: %.2f€\n", quantiteCommande, nomProduit, prix, prix * quantiteCommande);

		total += prix * quantiteCommande;

	} while (!feof(commande));

	fprintf(factureFile, "\tTOTAL = %.2f€", total);
	fclose(factureFile);
	fclose(produits);
	return;
}

void lireCommande2(FILE *commande, char *nombreCommande)
{
	int serieCommande, quantiteCommande;
	int serieProduit, seriestock, quantitestock;
	int i = 0 ,taille;
	char nomProduit[50];
	float prix;
	float total = 0;
	char client[50];
	FILE *factureFile;
	FILE *produits;
	FILE *stock;
	FILE *alerte;
	T_TableauDeProduits tab;
	T_Produit prod;

	stock = fopen("stock.txt", "r");
	while (!feof(stock))
	{
		fscanf(stock, "%d %d", &seriestock, &quantitestock);
		prod.quantite = quantitestock;
		prod.reference = seriestock;
		tab[i] = prod;
		i++;
	}
	fclose(stock);
	taille=i;

	char facture[100] = "factures/facture";
	strcat(facture, nombreCommande);
	strcat(facture, ".txt");
	factureFile = fopen(facture, "w");
	fscanf(commande, "%s", client);
	fprintf(factureFile, "Client : %s\n", client);

	produits = fopen("produits.txt", "r");
	stock = fopen("stock.txt", "w");
	alerte = fopen("alert.txt", "a");
	do
	{
		fscanf(commande, "%d %d", &serieCommande, &quantiteCommande);

		rewind(produits);
		do
		{
			fscanf(produits, "%d %s %f", &serieProduit, nomProduit, &prix);
		} while (!feof(produits) && serieProduit != serieCommande);

		i = 0;
		while (tab[i].reference != serieCommande)
			i++;
		if (tab[i].quantite > quantiteCommande)
			tab[i].quantite -= quantiteCommande;
		else
		{
			quantiteCommande = tab[i].quantite;
			tab[i].quantite = 0;
			
			fprintf(alerte, "%d \n", tab[i].reference);
		}

		fprintf(factureFile, "%d %s (PU=%f€) :: %.2f€\n", quantiteCommande, nomProduit, prix, prix * quantiteCommande);

		total += prix * quantiteCommande;

	} while (!feof(commande));
	i = 0;
	while (i!=taille-1)
	{
		fprintf(stock, "%d %d \n", tab[i].reference, tab[i].quantite);
		i++;
	}
	fprintf(stock, "%d %d", tab[i].reference, tab[i].quantite);

	fprintf(factureFile, "\tTOTAL = %.2f€", total);
	fclose(factureFile);
	fclose(stock);
	fclose(alerte);
	fclose(produits);
	return;
}

void lireLesCommandes2() // cette fonction ouvre tous les fichiers commandeXXX.txt avec XXXX démarrant à N
{
	FILE *ficCommande = NULL;
	int FINI = 0;
	int N = lireProchaineCommande(); // numero de la premiere commande qui sera lue et traitee
	char NNNN[5];
	char nomCommande[29];

	do // ce do while prend fin dès que fichier commandeXXXX.txt est absent
	{
		strcpy(nomCommande, "commandes/commande");
		convertirNenChaine4(N, NNNN);
		// printf("\n==>%s<==",NNNN);
		ficCommande = NULL;
		strcat(nomCommande, NNNN);
		strcat(nomCommande, ".txt");

		// printf("\n traitement de  %s",nomCommande);

		ficCommande = fopen(nomCommande, "rt");
		if (ficCommande != NULL)
		{ // le fichier commandeNNNN.txt existe
			printf("\n fichier %s present", nomCommande);
			lireCommande2(ficCommande, NNNN); // à vous de coder cette fonction lors de ce TP9
			fclose(ficCommande);
		}
		else
		{
			printf("\n toutes les commandes presentes ont ete traitees.\n");
			FILE *f = fopen("nextFact", "w"); // on va ecrire la valeur de N dans enxtFact
			// pour
			fwrite(&N, 1, sizeof(int), f);
			fclose(f);
			FINI = 1;
		}

		N++;
	} while (FINI == 0);
}

void lireLesCommandes() // cette fonction ouvre tous les fichiers commandeXXX.txt avec XXXX démarrant à N
{
	FILE *ficCommande = NULL;
	int FINI = 0;
	int N = lireProchaineCommande(); // numero de la premiere commande qui sera lue et traitee
	char NNNN[5];
	char nomCommande[29];

	do // ce do while prend fin dès que fichier commandeXXXX.txt est absent
	{
		strcpy(nomCommande, "commandes/commande");
		convertirNenChaine4(N, NNNN);
		// printf("\n==>%s<==",NNNN);
		ficCommande = NULL;
		strcat(nomCommande, NNNN);
		strcat(nomCommande, ".txt");

		// printf("\n traitement de  %s",nomCommande);

		ficCommande = fopen(nomCommande, "rt");
		if (ficCommande != NULL)
		{ // le fichier commandeNNNN.txt existe
			printf("\n fichier %s present", nomCommande);
			lireCommande(ficCommande, NNNN); // à vous de coder cette fonction lors de ce TP9
			fclose(ficCommande);
		}
		else
		{
			printf("\n toutes les commandes presentes ont ete traitees.");
			FILE *f = fopen("nextFact", "w"); // on va ecrire la valeur de N dans enxtFact
			// pour
			fwrite(&N, 1, sizeof(int), f);
			fclose(f);
			FINI = 1;
		}

		N++;
	} while (FINI == 0);
}

int main()
{
	// creation d un fichier d'un seul int nommé nextFact et contenant l'int 1
	//  code à utiliser pour réinitialiser nextFact à 1 si besoin au cours du TP

	FILE *f;
	int N = 1;
	f = fopen("nextFact", "w");
	fwrite(&N, 1, sizeof(int), f);
	fclose(f);

	// PARTIE 1 du TP : sans Gestion de stock
	// lireLesCommandes(); // lecture de tous les fichiers commandeXXX.txt (fichiers non traités jusqu'ici)

	// PARTIE 2 du TP : avec Gestion de stock
	// copiez coller votre travail précédent puis modifiez le
	lireLesCommandes2();

	return 0;
}
