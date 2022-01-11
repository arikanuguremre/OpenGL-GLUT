#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"

#define WINDOW_WIDTH  600
#define WINDOW_HEIGHT 600

#define  TIMER_PERIOD  16 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height


/* User defined Types and variables for the application */
#define FOV  30    // Field of View Angle
#define ENEMY_SPEED 2
#define PLAYER_SPEED 5

typedef struct {
	vec_t pos;
	float angle;
} player_t;

typedef struct {
	vec_t pos;
	vec_t vel;  //velocity
	bool  visibility; // visibility: true if the player sees the enemy
} enemy_t;

player_t P = { { 0, 0 }, 45 };
enemy_t  E = { { 0, 200 } };



//
// to draw circle, center at (x,y)
//  radius r
//
void circle( int x, int y, int r )
{
#define PI 3.1415
	float angle ;
	glBegin( GL_POLYGON ) ;
	for ( int i = 0 ; i < 100 ; i++ )
	{
		angle = 2*PI*i/100;
		glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
	}
	glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
	float angle;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 100; i++)
	{
		angle = 2 * PI*i / 100;
		glVertex2f(x + r*cos(angle), y + r*sin(angle));
	}
	glEnd();
}

void print(int x, int y, const char *string, void *font )
{
	int len, i ;

	glRasterPos2f( x, y );
	len = (int) strlen( string );
	for ( i =0; i<len; i++ )
	{
		glutBitmapCharacter( font, string[i]);
	}
}


// display text with variables.
void vprint(int x, int y, void *font, const char *string , ...)
{
	va_list ap;
	va_start ( ap, string );
	char str[1024] ;
	vsprintf_s( str, string, ap ) ;
	va_end(ap) ;
	
	int len, i ;
	glRasterPos2f( x, y );
	len = (int) strlen( str );
	for ( i =0; i<len; i++ )
	{
		glutBitmapCharacter( font, str[i]);
	}
}

void vprint2(int x, int y, float size, const char *string, ...) {
	va_list ap;
	va_start(ap, string);
	char str[1024];
	vsprintf_s(str, string, ap);
	va_end(ap);
	glPushMatrix();
		glTranslatef(x, y, 0);
		glScalef(size, size, 1);
		
		int len, i;
		len = (int)strlen(str);
		for (i = 0; i<len; i++)
		{
			glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
		}
	glPopMatrix();
}

void drawPlayer(player_t p) {
	vec_t vEnd = addV(p.pos, pol2rec({ 30, p.angle }));
	vec_t E1 = addV(p.pos, pol2rec({ 1000, p.angle - FOV }));
	vec_t E2 = addV(p.pos, pol2rec({1000, p.angle + FOV}));

	// Field of View Area
	glColor4f(1, 1, 0, 0.2);
	glBegin(GL_TRIANGLES);
	glVertex2f(p.pos.x, p.pos.y);
	glVertex2f(E1.x, E1.y);
	glVertex2f(E2.x, E2.y);
	glEnd();

	// View Direction Indicator
	glColor3f(1, 1, 0);
	glBegin(GL_LINES);
	glVertex2f(p.pos.x, p.pos.y);
	glVertex2f(vEnd.x, vEnd.y);
	glEnd();

	// Body of the Player Object.
	glColor3f(1, 1, 1);
	circle_wire(p.pos.x, p.pos.y, 20);

	glColor4f(0, 0, 1, 0.3);  
	circle(p.pos.x, p.pos.y, 19);
}

void drawEnemy(enemy_t e){

	glColor3f(1, 1, 1);
	circle_wire(e.pos.x, e.pos.y, 10);

	// if player sees the enemy, its color is green.
	if (E.visibility) glColor3f(0, 1, 0);
	else glColor3f(1, 0, 0);

	circle(e.pos.x, e.pos.y, 9);
}

void display()
{
	
	//
	// clear window to black
	//
	glClearColor( 0, 0 , 0 , 0 );
	glClear( GL_COLOR_BUFFER_BIT );

	drawPlayer(P);
	drawEnemy(E);
	
 
	glutSwapBuffers();


}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
	   exit(0);
    
    // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y )
{
   // exit when ESC is pressed.
   if ( key == 27 )
	   exit(0);
    
    // to refresh the window it calls display() function
   glutPostRedisplay() ;
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown( int key, int x, int y )
{
	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = true; break;
	case GLUT_KEY_DOWN: down = true; break;
	case GLUT_KEY_LEFT: left = true; break;
	case GLUT_KEY_RIGHT: right = true; break;

	}
	 // to refresh the window it calls display() function
	glutPostRedisplay() ;
}


//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp( int key, int x, int y )
{
  	// Write your codes here.
	switch (key) {
	case GLUT_KEY_UP: up = false; break;
	case GLUT_KEY_DOWN: down = false; break;
	case GLUT_KEY_LEFT: left = false; break;
	case GLUT_KEY_RIGHT: right = false; break;

	}
	 // to refresh the window it calls display() function
	glutPostRedisplay() ;
}


//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick( int button, int stat, int x, int y )
{
	// Write your codes here.


	
	 // to refresh the window it calls display() function
	glutPostRedisplay() ; 
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize( int w, int h )
{
	winWidth = w;
	winHeight = h;
	glViewport( 0,0,w,h) ;
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	display(); // refresh window.
}

void onMoveDown( int x, int y ) {
	// Write your codes here.


	
	 // to refresh the window it calls display() function	
	glutPostRedisplay() ;
}

void onMove( int x, int y ) {
	// Write your codes here.


	
	 // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) {
	 
	glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
	// Write your codes here.

	// Change orientation and/or position of the Player
	if (left) P.angle += 3;
	if (right) P.angle -= 3;
	if (up) P.pos = addV(P.pos, pol2rec({ PLAYER_SPEED, P.angle }));
	if (down) P.pos = subV(P.pos, pol2rec({ PLAYER_SPEED, P.angle }));



	// Check if the Player sees the Enemy, (visibility)
	vec_t V = pol2rec({ 1, P.angle });
	vec_t W = unitV(  subV(E.pos, P.pos) );
	E.visibility = dotP(V, W) > cos( FOV * D2R);  // E.vis:true , P sees E.

	// flee or chase
	// velocity vector from Enemy to Player
	E.vel = mulV( ENEMY_SPEED, unitV(subV(P.pos, E.pos))); 


	// if the Player sees the Enemy, it flees (gets away)
	// otherwise, it chases Player.
	vec_t prevPos = E.pos;
	if (E.visibility == true) {
		E.pos = subV(E.pos, E.vel);  // flee
	}
	else {
		E.pos = addV(E.pos, E.vel); // chase
	}

	// do not allow Enemy to get out of the game arena.
	if (E.pos.x > 290 || E.pos.x < -290 || E.pos.y > 290 || E.pos.y < -290) {
		E.pos = prevPos;
	}
	 // to refresh the window it calls display() function
	glutPostRedisplay() ; // display()

}
#endif

void Init() {
	
	// Smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	

}


void main( int argc, char *argv[] )
{

	glutInit(&argc, argv );
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow( "Template File" ) ;

	glutDisplayFunc( display ) ;
    glutReshapeFunc( onResize );
	//
	// keyboard registration
	//
	glutKeyboardFunc( onKeyDown ) ;
	glutSpecialFunc( onSpecialKeyDown ) ;

	glutKeyboardUpFunc( onKeyUp ) ;
	glutSpecialUpFunc( onSpecialKeyUp ) ;

	//
	// mouse registration
	//
    glutMouseFunc( onClick) ;
	glutMotionFunc( onMoveDown ) ;
	glutPassiveMotionFunc( onMove ) ; 
	
    #if  TIMER_ON == 1
	// timer event
	glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
	#endif

	Init();
	
	glutMainLoop();
}