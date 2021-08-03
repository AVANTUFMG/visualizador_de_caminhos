#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include "Pathfinding.h"
#include <cmath>

// ----------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------
void display();
void specialKeys();

//funcoes para converter valores X e Y em metros para coordenadas no plano cartesiano da malha
double conversor_X(double lat_drone, double X) {
    double x_coord = lat_drone + X * 0.000009;
    return x_coord;
}
double conversor_Y(double lon_drone, double Y) {
    double y_coord = lon_drone + Y * 0.000009;
    return y_coord;
}

//Vetor que recebera os pontos do pathfinding
vector <Point> N;

// ----------------------------------------------------------
// Global Variables
// ----------------------------------------------------------
double rotate_y = 0;
double rotate_x = 0;
double rotate_z = 0;

// ----------------------------------------------------------
// display() Callback function
// ----------------------------------------------------------
void display() {

    //  Clear screen and Z-buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset transformations
    glLoadIdentity();

    // Other Transformations
    // glTranslatef( 0.1, 0.0, 0.0 );      // Not included
    // glRotatef( 180, 0.0, 1.0, 0.0 );    // Not included

    // Rotate when user changes rotate_x and rotate_y
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
    glRotatef(rotate_z, 0.0, 0.0, 1.0);

    // Other Transformations
    // glScalef( 2.0, 2.0, 0.0 );          // Not included

    //Multi-colored side - FRONT
    glPointSize(3.5f);
    glBegin(GL_POINTS);

    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0,0,0);      // P1 is blue

    for (Point it : N) {
        if (it.get_obs() == 1) {
            glColor3f(1.0, 0.0, 0.0);        
            glVertex3f(it.get_x()/120, it.get_y()/120, it.get_z() /120);      // P1 is red
        }
        else{
            glColor3f(0.0, 1.0, 0.0);
            glVertex3f(it.get_x() / 120, it.get_y() / 120, it.get_z() / 120);      // P1 is green
        }
    }
    glEnd();

    glFlush();
    glutSwapBuffers();

}

// ----------------------------------------------------------
// specialKeys() Callback Function
// ----------------------------------------------------------
void specialKeys(int key, int x, int y) {

    //  Right arrow - increase rotation by 5 degree
    if (key == GLUT_KEY_RIGHT)
        rotate_y += 5;

    //  Left arrow - decrease rotation by 5 degree
    else if (key == GLUT_KEY_LEFT)
        rotate_y -= 5;

    else if (key == GLUT_KEY_UP)
        rotate_x += 5;

    else if (key == GLUT_KEY_DOWN)
        rotate_x -= 5;

    else if (key == GLUT_KEY_PAGE_UP)
        rotate_z += 5;

    else if (key == GLUT_KEY_PAGE_DOWN)
        rotate_z -= 5;

    //  Request display update
    glutPostRedisplay();

}

// ----------------------------------------------------------
// main() function
// ----------------------------------------------------------
int main(int argc, char* argv[]) {
    //------ PARTE DO MAIN DE TESTE PARA PATHFINDING PRECISÃO DE +-8 METROS------ //
    cout.precision(10); //apenas aumentando a precisao do cout

    /*
    Sobre a malha default: *DESATUALIZADA PEDIR A CONTI PRA ATUALIZAR SE NECESSÁRIO*
    O tamanho da malha é definido em Point.cpp pelos valores de N_lat, N_lon e N_H
    A malha default colocamos N_lat = 10, N_lon = 10, N_H = 4, e terá (10*2 + 1)*(10*2 + 1)*(4*2 + 1) = 3969 pontos que formam a malha
    O espaçamento entre cada pontos é 15 metros, logo a malha em 2D corresponde à (10*2 + 1)*15*(10*2 + 1)*15 = 99225 m²
    Em 3D: 99225*(4*2 + 1)*15 = 13395375 m³
    Nesse caso, 315 m de largura, 315 m de comprimento e 135 m  de altura
    Válido lembrar que o drone, está sempre no centro desse paralelepípedo
    */


    //Mude para os valores desejados
    //Os valores X,Y,Z do drone sao sempre 0,0,0
    double latitude_drone = -19.871875;
    double longitude_drone = -43.970220;
    double altura_drone = 60;

    //insira os valores desejados de x,y em metros sendo que -600 < x,y < 600. Insira também a quantidade de obstáculos desejados
    double x_metros = -45;
    double y_metros = 75;
    double z_metros = 15;
    int quantidade_obstaculo = 0;

    //--------------------------------------------------------------------------------------------------------------------------

    double latitude_destino = conversor_X(latitude_drone, x_metros); // Essa latitude equivale ao X = 600, Extremo "esquerdo"
    double longitude_destino = conversor_Y(longitude_drone, y_metros); // Essa longitude equivale ao Y = 600, Extrema "cima"
    double altura_destino = z_metros; // Não mude a altura, já que está testando apenas 2D

    N = pathfinding_Astar(latitude_drone, longitude_drone, altura_drone, latitude_destino, longitude_destino, altura_destino, quantidade_obstaculo);
    //vector <Point> N = pathfinding_3D(latitude_drone, longitude_drone, altura_drone, latitude_destino, longitude_destino, altura_destino, quantidade_obstaculo);
 
    //Funcao que imprime quais pontos o drone vai passar
    cout << "imprimindo o caminho" << endl;
    for (int i = 0; i < N.size(); i++) {
        if (N[i].get_obs() == 1) {
            cout << "Obstaculo em: \n";
            N[i].print_point();
            cout << endl;
        }
        else {
            N[i].print_point();
            cout << endl;
        }
    }


    //  Initialize GLUT and process user parameters
    glutInit(&argc, argv);

    //  Request double buffered true color window with Z-buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutCreateWindow("Rota 3D");

    //  Enable Z-buffer depth test
    glEnable(GL_DEPTH_TEST);

    // Callback functions
    glutDisplayFunc(display);
    glutSpecialFunc(specialKeys);

    //  Pass control to GLUT for events
    glutMainLoop();

    //  Return to OS
    
    return 0;

}