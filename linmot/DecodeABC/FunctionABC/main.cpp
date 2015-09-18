#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


/*int communicat(char* a,int tablAccordage[]){
    int taille1,taille2;
    double csteArduino;
    tab=new int[taille1][taille2];
    int r=a-1;
    return r;
}*/

int main()
{
    int tablAccordage[14]={1,2,3,4,5,6,7,8,9,10,11,12,13,14}; //Tableau des fréduences des notes respectives La=A, Si=B, Do=C, Ré=D, Mi=E, Fa=F, Sol=G.
    int dt=10; //durée de référence

    int i = 0; // initialisation de la ligne à lire

    ifstream fichier("C:/Elec/Qt/MoubarakS/FunctionABC/ABC1.txt");

    if (fichier){

    //Extraction des lignes du fichier ABC
        string line[50]; // définition du tableau des lignes du fichier ABC de taille 50..
        string contenu;
        while(getline(fichier,contenu)){
            line[i]=contenu;
            i=i+1;
        }

        fichier.close(); // fermeture du flux

    //Affichage de i
        cout << i <<endl;
        // la ligne 4 représente celle qui nous intéresse :: cout << line[4][j] << endl;

        int n=line[4].size();//nombres de bytes sur la lignes

    //Traitement de la partition :::: ATTENTION : on ne travaille que sur une seule ligne
        //Tableau pour arduino : arduinoTab et tableau pour engine Driver : engDriTabFreq et engDrivTabDuree {la taille des tableaux sera déinie égale à n (valeur max)}
        //double *arduinTab=new double[n];
        int *engDriTabFreq=new int[n];
        int *engDriTabDuree=new int[n];

        int nbChar =0;
        char* noteTab=new char[n];// tableau où sont stockés les caractères
        int *noteDur=new int[n];// tableau où sont stockés la durée du caractère considéré
        int *noteFreq=new int[n];// tableau où sont stockés la fréquence du caractère considéré
        int *noteInst=new int[n];// tableau où sont stockés l'instant du caractère considéré
        int *instant=new int[n];// tableau pour l'instant
        int *noteLiaison=new int[n];//1: note lié à la note suivante, 0 : non liée à la suivante

        //Initialisation des tableaux
        for (int p=0;p<n;p++){
            engDriTabDuree[p]=0;
            noteDur[p]=0;
            noteDur[p]=0;
            noteLiaison[p]=0;
            instant[p]=0;
        }

        //Différenciation des caractères
        for(int j=0;j<n;j++){

            //Majuscule
            if (line[4][j]>='A' && line[4][j]<='Z'){
                noteTab[nbChar]=line[4][j];
                // cout << "majuscule" << endl;
                 engDriTabDuree[j] = dt;
                 noteDur[nbChar]=engDriTabDuree[j];

                if (line[4][j]=='A'){
                    engDriTabFreq[j]=tablAccordage[0];
                }
                else if (line[4][j]=='B'){
                    engDriTabFreq[j]=tablAccordage[1];
                }
                else if (line[4][j]=='C'){
                    engDriTabFreq[j]=tablAccordage[2];
                }
                else if (line[4][j]=='D'){
                    engDriTabFreq[j]=tablAccordage[3];
                }
                else if (line[4][j]=='E'){
                    engDriTabFreq[j]=tablAccordage[4];
                }
                else if (line[4][j]=='F'){
                    engDriTabFreq[j]=tablAccordage[5];
                }
                else {
                    engDriTabFreq[j]=tablAccordage[6];
                }
                noteFreq[nbChar]= engDriTabFreq[j];

                //Faire les liaison
                if (nbChar>=2 && j>=1){
                    if (line[4][j-1]>='A' && line[4][j-1]<='Z'){
                        noteLiaison[nbChar-1]=1;
                    }
                    else if (line[4][j-1]>='a' && line[4][j-1]<='z'){
                        noteLiaison[nbChar-1]=1;
                    }
                    else {
                    }
                }//fin liaison

                //incrémentation de nbChar
                nbChar++;

            }

            //Minuscule
            else if (line[4][j]>='a' && line[4][j]<='z'){
                noteTab[nbChar]=line[4][j];
                //cout <<  "minuscule"<< endl;
                engDriTabDuree[j] = dt;
                noteDur[nbChar]=engDriTabDuree[j];

                if (line[4][j]=='a'){
                    engDriTabFreq[j]=tablAccordage[7];
                }
                else if (line[4][j]=='b'){
                    engDriTabFreq[j]=tablAccordage[8];
                }
                else if (line[4][j]=='c'){
                    engDriTabFreq[j]=tablAccordage[9];
                }
                else if (line[4][j]=='d'){
                    engDriTabFreq[j]=tablAccordage[10];
                }
                else if (line[4][j]=='e'){
                    engDriTabFreq[j]=tablAccordage[11];
                }
                else if (line[4][j]=='f'){
                    engDriTabFreq[j]=tablAccordage[12];
                }
                else {
                    engDriTabFreq[j]=tablAccordage[13];
                }
                noteFreq[nbChar]= engDriTabFreq[j];

                //Faire les liaison
                if (nbChar>=2 && j>=1){
                    if (line[4][j-1]>='A' && line[4][j-1]<='Z'){
                        noteLiaison[nbChar-1]=1;
                    }
                    else if (line[4][j-1]>='a' && line[4][j-1]<='z'){
                        noteLiaison[nbChar-1]=1;
                    }
                    else {
                    }
                }//fin liaison

                //incrémentation de nbChar
                nbChar++;
            }

            //Chiffre
            else if (line[4][j]!=' '){
                int x=line[4][j]-48;
                engDriTabDuree[j-1] = x*dt; // C'est la durée de la note "j-1"
                //cout << x << endl;
                noteDur[nbChar-1]=engDriTabDuree[j-1];//actualiser la durée de la note
            }

            //Espace
            else {
                engDriTabDuree[j] = dt/2; //temps de transition = dt/2, il faudra adapter la vitesse du moteur
                 //cout<<  endl;
            }

            instant[j]=instant[j-1]+engDriTabDuree[j-1];// Calcul de l'instant auquel se fait l'opération
            instant[0]=0; //initialisation de instant 0

//Calcul de l'instant du caractère
            //Majuscule
            if (line[4][j]>='A' && line[4][j]<='Z'){
                noteInst[nbChar-1]= instant[j]; //nbChar -1 car on a déjà incrémenté nbChar
            }
            //Minuscule
             else if (line[4][j]>='a' && line[4][j]<='z'){
                noteInst[nbChar-1]= instant[j]; //nbChar -1 car on a déjà incrémenté nbChar
            }
            //Pour les chiffres et les espaces, on ne fait rien
             else if (line[4][j]!=' '){
            }
            else {
            }

//On ferme la boucle for
        }

       for (int p=0;p<nbChar;p++){
            cout<<"Note: ";
            cout<<noteTab[p];
            cout<<", duree: ";
            cout<<noteDur[p];
            cout<<", instant de demarrage: ";
            cout<<noteInst[p];
            cout<<", liaison: ";
            cout<<noteLiaison[p];
            cout<<", frequence: ";
            cout<<noteFreq[p]<<endl;
        }

//Arduino


    }

    else {
        cerr << "Impossible" << endl;
    }

    return 0;
}

