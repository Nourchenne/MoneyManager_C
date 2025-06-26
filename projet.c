#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int idUtilisateur;
    int cin;
    char nom[30];
    char prenom[30];
    char email[50];
    int telephone;
    char mdp[30];
    char role[10];
} Utilisateur;

typedef struct {
    int idCompte;
    int idUtilisateur;
    char rib[24];
    float solde;
} Compte;

int verifierUtilisateur(const char *cin, const char *mdp, char *role, int *idUtilisateur) {
    FILE *f = fopen("utilisateurs.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier utilisateurs.dat.\n");
        return 0;
    }

    Utilisateur user;
    char userCin[15];
    while (fscanf(f, "%d %d %s %s %s %d %s %s", &user.idUtilisateur, &user.cin, user.nom, user.prenom, user.email, &user.telephone, user.mdp, user.role) == 8) {
        sprintf(userCin, "%d", user.cin);
        if (strcmp(cin, userCin) == 0 && strcmp(mdp, user.mdp) == 0) {
            strcpy(role, user.role);
            *idUtilisateur = user.idUtilisateur;
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}


void ajouterUtilisateur() {
    FILE *f = fopen("utilisateurs.dat", "a");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier utilisateurs.dat.\n");
        exit(1);
    }

    Utilisateur user;
    printf("Saisir les informations de l'utilisateur :\n");
    printf("ID : ");
    scanf("%d", &user.idUtilisateur);
    printf("CIN : ");
    scanf("%d", &user.cin);
    printf("Nom : ");
    scanf("%s", user.nom);
    printf("Prenom : ");
    scanf("%s", user.prenom);
    printf("Email : ");
    scanf("%s", user.email);
    printf("Telephone : ");
    scanf("%d", &user.telephone);
    printf("Mot de passe : ");
    scanf("%s", user.mdp);
    printf("Role (admin/user) : ");
    scanf("%s", user.role);

    fprintf(f, "%d %d %s %s %s %d %s %s\n", user.idUtilisateur, user.cin, user.nom, user.prenom, user.email, user.telephone, user.mdp, user.role);
    fclose(f);
    printf("Utilisateur ajoute avec succes.\n");
}

void retraitArgent() {
    FILE *f = fopen("comptes.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return;
    }

    FILE *temp = fopen("temp.dat", "w");
    if (temp == NULL) {
        printf("Erreur lors de la creation du fichier temporaire.\n");
        fclose(f);
        return;
    }

    int idCompte;
    float montant;
    int compteTrouve = 0;

    printf("Entrez l'ID du compte : ");
    scanf("%d", &idCompte);
    printf("Entrez le montant a retirer : ");
    scanf("%f", &montant);

    Compte compte;
    while (fscanf(f, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idCompte == idCompte) {
            if (compte.solde >= montant) {
                compte.solde -= montant;
                compteTrouve = 1;
                printf("Retrait reussi. Nouveau solde : %.2f\n", compte.solde);
            } else {
                printf("Solde insuffisant pour effectuer le retrait.\n");
            }
        }
        fprintf(temp, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
    }

    fclose(f);
    fclose(temp);

    if (!compteTrouve) {
        printf("Compte non trouve.\n");
        remove("temp.dat");
    } else {
        remove("comptes.dat");
        rename("temp.dat", "comptes.dat");
    }
}

void depotArgent() {
    FILE *f = fopen("comptes.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return;
    }

    FILE *temp = fopen("temp.dat", "w");
    if (temp == NULL) {
        printf("Erreur lors de la creation du fichier temporaire.\n");
        fclose(f);
        return;
    }

    int idCompte;
    float montant;
    int compteTrouve = 0;

    printf("Entrez l'ID du compte : ");
    scanf("%d", &idCompte);
    printf("Entrez le montant a deposer : ");
    scanf("%f", &montant);

    Compte compte;
    while (fscanf(f, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idCompte == idCompte) {
            compte.solde += montant;
            compteTrouve = 1;
            printf("Depot reussi. Nouveau solde : %.2f\n", compte.solde);
        }
        fprintf(temp, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
    }

    fclose(f);
    fclose(temp);

    if (!compteTrouve) {
        printf("Compte non trouve.\n");
        remove("temp.dat");
    } else {
        remove("comptes.dat");
        rename("temp.dat", "comptes.dat");
    }
}

int isAccountOwnedByUser(int idCompte, int idUtilisateur) {
    FILE *f = fopen("comptes.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return 0;
    }

    Compte compte;
    while (fscanf(f, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idCompte == idCompte && compte.idUtilisateur == idUtilisateur) {
            fclose(f);
            return 1;
        }
    }

    fclose(f);
    return 0;
}

void virementBancaire(int idUtilisateur) {
    FILE *f = fopen("comptes.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return;
    }

    FILE *temp = fopen("temp.dat", "w");
    if (temp == NULL) {
        printf("Erreur lors de la création du fichier temporaire.\n");
        fclose(f);
        return;
    }

    int idSource, idDestination;
    float montant;
    int sourceTrouve = 0, destinationTrouve = 0;
    float sourceSolde;

    printf("Entrez l'ID du compte source : ");
    scanf("%d", &idSource);
    if (!isAccountOwnedByUser(idSource, idUtilisateur)) {
        printf("Erreur : Vous ne possédez pas ce compte source.\n");
        fclose(f);
        fclose(temp);
        return;
    }

    printf("Entrez l'ID du compte destinataire : ");
    scanf("%d", &idDestination);
    printf("Entrez le montant à transférer : ");
    scanf("%f", &montant);

    Compte compte;
    while (fscanf(f, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idCompte == idSource) {
            if (compte.solde >= montant) {
                sourceSolde = compte.solde - montant;
                sourceTrouve = 1;
            } else {
                printf("Solde insuffisant dans le compte source.\n");
            }
        } else if (compte.idCompte == idDestination) {
            destinationTrouve = 1;
            if (sourceTrouve) {
                compte.solde += montant;
            }
        }


        if (compte.idCompte == idSource) {
            fprintf(temp, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, sourceSolde);
        } else {
            fprintf(temp, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
        }
    }

    fclose(f);
    fclose(temp);

    if (!sourceTrouve) {
        printf("Compte source non trouvé ou solde insuffisant.\n");
        remove("temp.dat");
    } else if (!destinationTrouve) {
        printf("Compte destinataire non trouvé.\n");
        remove("temp.dat");
    } else if (sourceTrouve && destinationTrouve) {
        remove("comptes.dat");
        rename("temp.dat", "comptes.dat");
        printf("Virement effectué avec succès.\n");
    }
}

void listerUtilisateurs() {
    FILE *f = fopen("utilisateurs.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier utilisateurs.dat.\n");
        return;
    }

    Utilisateur user;
    printf("\nListe des utilisateurs :\n");
    printf("ID\tCIN\tNom\tPrenom\tEmail\tTelephone\tRole\n");
    while (fscanf(f, "%d %d %s %s %s %d %s %s", &user.idUtilisateur, &user.cin, user.nom, user.prenom, user.email, &user.telephone, user.mdp, user.role) == 8) {
        printf("%d\t%d\t%s\t%s\t%s\t%d\t%s\n", user.idUtilisateur, user.cin, user.nom, user.prenom, user.email, user.telephone, user.role);
    }
    fclose(f);
}

void creerCompte() {
    FILE *f = fopen("comptes.dat", "a");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        exit(1);
    }

    FILE *usersFile = fopen("utilisateurs.dat", "r");
    if (usersFile == NULL) {
        printf("Erreur lors de l'ouverture du fichier utilisateurs.dat.\n");
        fclose(f);
        return;
    }

    Compte compte;
    Utilisateur user;
    int userExists = 0;

    printf("Saisir l'ID utilisateur pour le compte : ");
    scanf("%d", &compte.idUtilisateur);


    while (fscanf(usersFile, "%d %d %s %s %s %d %s %s", &user.idUtilisateur, &user.cin, user.nom, user.prenom, user.email, &user.telephone, user.mdp, user.role) == 8) {
        if (user.idUtilisateur == compte.idUtilisateur) {
            userExists = 1;
            break;
        }
    }
    fclose(usersFile);

    if (!userExists) {
        printf("Erreur : Aucun utilisateur avec cet ID n'existe.\n");
        fclose(f);
        return;
    }

    printf("Saisir l'ID du compte : ");
    scanf("%d", &compte.idCompte);
    printf("Saisir le RIB : ");
    scanf("%s", compte.rib);
    printf("Saisir le solde initial : ");
    scanf("%f", &compte.solde);

    fprintf(f, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
    fclose(f);
    printf("Compte créé avec succès pour l'utilisateur %d.\n", compte.idUtilisateur);
}

void listerComptesUtilisateur(int idUtilisateur) {
    FILE *f = fopen("comptes.dat", "r");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return;
    }

    Compte compte;
    int found = 0;

    printf("\nComptes pour l'utilisateur %d :\n", idUtilisateur);
    printf("ID Compte\tRIB\t		Solde\n");

    while (fscanf(f, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idUtilisateur == idUtilisateur) {
            printf("%d\t\t%s\t%.2f\n", compte.idCompte, compte.rib, compte.solde);
            found = 1;
        }
    }

    if (!found) {
        printf("Aucun compte trouvé pour cet utilisateur.\n");
    }

    fclose(f);
}

void supprimerUtilisateurEtComptes(int idUtilisateur) {

    FILE *usersFile = fopen("utilisateurs.dat", "r");
    FILE *tempUsers = fopen("temp_users.dat", "w");
    if (usersFile == NULL || tempUsers == NULL) {
        printf("Erreur lors de l'ouverture des fichiers utilisateurs.\n");
        return;
    }

    Utilisateur user;
    int userFound = 0;

    while (fscanf(usersFile, "%d %d %s %s %s %d %s %s", &user.idUtilisateur, &user.cin, user.nom, user.prenom, user.email, &user.telephone, user.mdp, user.role) == 8) {
        if (user.idUtilisateur == idUtilisateur) {
            userFound = 1; // Skip this user
        } else {
            fprintf(tempUsers, "%d %d %s %s %s %d %s %s\n", user.idUtilisateur, user.cin, user.nom, user.prenom, user.email, user.telephone, user.mdp, user.role);
        }
    }
    fclose(usersFile);
    fclose(tempUsers);

    if (userFound) {
        remove("utilisateurs.dat");
        rename("temp_users.dat", "utilisateurs.dat");
        printf("Utilisateur %d supprimé avec succès.\n", idUtilisateur);
    } else {
        remove("temp_users.dat");
        printf("Utilisateur %d introuvable.\n", idUtilisateur);
        return;
    }


    FILE *accountsFile = fopen("comptes.dat", "r");
    FILE *tempAccounts = fopen("temp_accounts.dat", "w");
    if (accountsFile == NULL || tempAccounts == NULL) {
        printf("Erreur lors de l'ouverture des fichiers comptes.\n");
        return;
    }

    Compte compte;
    while (fscanf(accountsFile, "%d %d %s %f", &compte.idCompte, &compte.idUtilisateur, compte.rib, &compte.solde) == 4) {
        if (compte.idUtilisateur != idUtilisateur) {
            fprintf(tempAccounts, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
        }
    }
    fclose(accountsFile);
    fclose(tempAccounts);

    remove("comptes.dat");
    rename("temp_accounts.dat", "comptes.dat");
    printf("Tous les comptes associés à l'utilisateur %d ont été supprimés.\n", idUtilisateur);
}

void creerCompteUtilisateur(int idUtilisateur) {
    FILE *f = fopen("comptes.dat", "a");
    if (f == NULL) {
        printf("Erreur lors de l'ouverture du fichier comptes.dat.\n");
        return;
    }

    Compte compte;

    printf("Saisir l'ID du compte : ");
    scanf("%d", &compte.idCompte);
    printf("Saisir le RIB : ");
    scanf("%s", compte.rib);
    printf("Saisir le solde initial : ");
    scanf("%f", &compte.solde);

    compte.idUtilisateur = idUtilisateur;

    fprintf(f, "%d %d %s %.2f\n", compte.idCompte, compte.idUtilisateur, compte.rib, compte.solde);
    fclose(f);

    printf("Compte créé avec succès pour l'utilisateur %d.\n", idUtilisateur);
}


int main() {
    int choix;
    do {
        printf("\nMenu principal :\n");
        printf("1. S'inscrire (Admin/User)\n");
        printf("2. Connexion\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                ajouterUtilisateur();
                break;
            case 2: {
    char cin[30], mdp[30], role[10];
    int idUtilisateur;
    printf("CIN : ");
    scanf("%s", cin);
    printf("Mot de passe : ");
    scanf("%s", mdp);

    if (verifierUtilisateur(cin, mdp, role, &idUtilisateur)) {
        if (strcmp(role, "admin") == 0) {

            int adminAction;
            do {
                printf("\nMenu Admin :\n");
                printf("1. Gérer les utilisateurs\n");
                printf("2. Gérer les comptes\n");
                printf("3. Supprimer un utilisateur et ses comptes\n");
                printf("0. Déconnexion\n");
                printf("Votre choix : ");
                scanf("%d", &adminAction);

                switch (adminAction) {
                    case 1:
                        listerUtilisateurs();
                        break;
                    case 2: {
                        int userId;
                        printf("Entrez l'ID utilisateur pour voir ses comptes : ");
                        scanf("%d", &userId);
                        listerComptesUtilisateur(userId);
                        break;
                    }
                    case 3: {
                        int userId;
                        printf("Entrez l'ID utilisateur à supprimer : ");
                        scanf("%d", &userId);
                        supprimerUtilisateurEtComptes(userId);
                        break;
                    }
                    case 0:
                        printf("Déconnexion réussie.\n");
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
            } while (adminAction != 0);
        } else {
            int userAction;
            do {
                printf("\nMenu Utilisateur :\n");
                printf("1. Créer un compte\n");
                printf("2. Dépôt d'argent\n");
                printf("3. Retrait d'argent\n");
                printf("4. Virement bancaire\n");
                printf("5. Voir mes comptes\n");
                printf("0. Déconnexion\n");
                printf("Votre choix : ");
                scanf("%d", &userAction);

                switch (userAction) {
                    case 1:
                        creerCompteUtilisateur(idUtilisateur);
                        break;
                    case 2:
                        depotArgent();
                        break;
                    case 3:
                        retraitArgent();
                        break;
                    case 4:
                        virementBancaire(idUtilisateur);
                        break;
                    case 5:
                        listerComptesUtilisateur(idUtilisateur);
                        break;
                    case 0:
                        printf("Déconnexion réussie.\n");
                        break;
                    default:
                        printf("Choix invalide.\n");
                }
            } while (userAction != 0);
        }
    } else {
        printf("CIN ou mot de passe incorrect.\n");
    }
    break;
}

            case 0:
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
