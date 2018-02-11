/*!\file window.c
 *
 * \brief GL4Dummies, exemple 3D simple avec dessin d'un quadrilat�re
 * plus des transformations spaciales projection/mod�lisation
 * utulisant les fonction gl4duXXX  
 * \author Far�s BELHADJ,
 * amsi@ai.univ-paris8.fr \date February 11 2018
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fen�tre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du (futur) Vertex Array Object */
static GLuint _vao = 0;
/*!\brief identifiant du (futur) buffer de data - VBO ou Vertex Buffer Object */
static GLuint _buffer = 0;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;

/*!\brief cr�� la fen�tre d'affichage, initialise GL et les donn�es,
 * affecte les fonctions d'�v�nements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les param�tres OpenGL et les donn�es. 
 *
 * Exercice compliqu� (ne sera pas corrig�) : s�parer la vue de la
 * mod�lisation en utilisant deux matrices disjointes (par exemple
 * viewMatrix et modelMatrix). Refaire la m�me vue que celle obtenue
 * ici en n'utilisant que des rotate et translate (pas de lookAt).
 *
 * Exercice compliqu� (corrig� en 1.5) : supprimer les 6x4 sommets du
 * cube de data et mettre � la place 8 sommets repr�sentant les 8
 * sommets du cube. Mettre du Rouge-Vert-Bleu-Jaune sombres pour les 4
 * de devant et satur�es-claires pour les 4 de derri�re. Utiliser un
 * second VBO de type GL_ELEMENT_ARRAY_BUFFER et un tableau idx
 * contenant 6x4 indices des sommets du cube (avec r�p�tition) de
 * mani�re � reconstruire le m�me cube avec des LINE_LOOP mais cette
 * fois-ci en utilisant la fonction glDrawElements. Enfin, ajouter les
 * 3 axes X (en rouge), Y (en vert) et Z (en bleu).
 */
static void init(void) {
  /* donn�es envoy�es par tranches sommet-attributs dans le VBO */
  GLfloat data[] = {
    /* 3 coordonn�es de sommets en 3D chacune suivie de sa couleur */
    -1.5f, -1.5f, 0.0f,
    1.0f, 0.0f, 0.0f, 
    1.5f, -1.5f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f,  1.5f, 0.0f,
    0.0f, 0.0f, 1.0f,
    /* 4 sommets de la face haut */
    -1, 1,  1, 0, 0, 0,
     1, 1,  1, 0, 0, 0,
     1, 1, -1, 0, 0, 0,
    -1, 1, -1, 0, 0, 0,
    /* 4 sommets de la face bas */
    -1, -1,  1, 0, 0, 0,
    -1, -1, -1, 0, 0, 0,
     1, -1, -1, 0, 0, 0,
     1, -1,  1, 0, 0, 0,
    /* 4 sommets de la face avant */
    -1, -1, 1, 0, 0, 0,
     1, -1, 1, 0, 0, 0,
     1,  1, 1, 0, 0, 0,
    -1,  1, 1, 0, 0, 0,
    /* 4 sommets de la face arri�re */
    -1, -1, -1, 0, 0, 0,
    -1,  1, -1, 0, 0, 0,
     1,  1, -1, 0, 0, 0,
     1, -1, -1, 0, 0, 0,
    /* 4 sommets de la face droite */
    1, -1, -1, 0, 0, 0,
    1,  1, -1, 0, 0, 0,
    1,  1,  1, 0, 0, 0,
    1, -1,  1, 0, 0, 0,
    /* 4 sommets de la face gauche */
    -1, -1, -1, 0, 0, 0,
    -1, -1,  1, 0, 0, 0,
    -1,  1,  1, 0, 0, 0,
    -1,  1, -1, 0, 0, 0
  };
  /* Cr�ation du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derri�re. */
  glEnable(GL_DEPTH_TEST);
  /* Cr�ation des matrices GL4Dummies, une pour la projection, une pour la mod�lisation-vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  /* on active la matrice de projection cr��e pr�c�demment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours re�oit identit� (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* d�commenter pour orthographique gl4duOrthof(-1, 1, -1, 1, 0, 100); */
  gl4duFrustumf(-1, 1, -1, 1, 2, 100);
  /* dans quelle partie de l'�cran on effectue le rendu */
  glViewport(0, 0, _wW, _wH);
  /* G�n�ration d'un identifiant de VAO */
  glGenVertexArrays(1, &_vao);
  /* Lier le VAO-machine-GL � l'identifiant VAO g�n�r� */
  glBindVertexArray(_vao);
  /* Activation des 3 premiers indices d'attribut de sommet */
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  /* G�n�ration d'un identifiant de VBO */
  glGenBuffers(1, &_buffer);
  /* Lier le VBO-machine-GL � l'identifiant VBO g�n�r� */
  glBindBuffer(GL_ARRAY_BUFFER, _buffer);
  /* Transfert des donn�es VBO */
  glBufferData(GL_ARRAY_BUFFER, sizeof data, data, GL_STATIC_DRAW);
  /* Param�trage 2 premiers indices d'attribut de sommet */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)0);  
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof *data, (const void *)(3 * sizeof *data));
  /* d�-lier le VBO et VAO */
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  /* une variable d'angle, maintenant elle passe ne degr�s */
  static GLfloat angle = 0.0f;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derri�re. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la diff�rence. Nous avons ajout�
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice mod�lisation-vue
   * cr��e dans init */
  gl4duBindMatrix("modelViewMatrix");
  /* Charger la matrice identit� dans la matrice courante (li�e) */
  gl4duLoadIdentityf();
  /* Composer la matrice courante avec une translation (voir la
   * projection perspective dans le manuel pour comprendre pourquoi
   * nous devons �loigner de -3 en z les objets dessin�s) */
  gl4duLookAtf(-3, 3, -5, 0, 0, 0, 0, 1, 0);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies, ici on int�gre pas la rotation qui vient apr�s */
  gl4duSendMatrices();
  /* Lier le VAO-machine-GL � l'identifiant VAO _vao */
  glBindVertexArray(_vao);
  /* Dessiner le VAO comme des line loop pour chaque face de 4 sommets
   * en commen�ant par l'indice 3 (+4 � chaque fois, sur 6
   * faces). Pour tester, vous pouvez remplacer tous les GL_LINE_LOOP
   * par des GL_TRIANGLE_FAN */
  glDrawArrays(GL_LINE_LOOP, 3, 4);
  glDrawArrays(GL_LINE_LOOP, 7, 4);
  glDrawArrays(GL_LINE_LOOP, 11, 4);
  glDrawArrays(GL_LINE_LOOP, 15, 4);
  glDrawArrays(GL_LINE_LOOP, 19, 4);
  glDrawArrays(GL_LINE_LOOP, 23, 4);
  /* on transmet la variable d'angle en l'int�grant dans la matrice de
   * mod�lisation-vue. Soit composer la matrice courante avec une
   * rotation "angle" autour de l'axe y (0, 1, 0) */
  gl4duRotatef(angle, 0, 1, 0);
  /* on incr�mente angle d'un 1/60 de tour soit (360� x 1/60). Tester
   * l'application en activant/d�sactivant la synchronisation
   * verticale de votre carte graphique. Que se passe-t-il ? Trouver
   * une solution pour que r�sultat soit toujours le m�me. */
  angle += (1.0f / 60.0f) * 360.0f;
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies */
  gl4duSendMatrices();
  /* Dessiner le VAO comme une bande d'un triangle avec 3 sommets commen�ant � 0 */
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
  /* d�-lier le VAO */
  glBindVertexArray(0);
  /* d�sactiver le programme shader */
  glUseProgram(0);
}
/*!\brief appel�e au moment de sortir du programme (atexit), elle
 *  lib�re les �l�ments OpenGL utilis�s.*/
static void quit(void) {
  /* suppression du VAO _vao en GPU */
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  /* suppression du VBO _buffer en GPU */
  if(_buffer)
    glDeleteBuffers(1, &_buffer);
  /* nettoyage des �l�ments utilis�s par la biblioth�que GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
