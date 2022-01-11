//06-StateMachine-Microwave

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  500
#define WINDOW_HEIGHT 500

#define  TIMER_PERIOD  1000 // Period for the timer.
#define  TIMER_ON     1     // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

/* GLOBAL VARIABLES and CONSTANTS */
#define  START 0
#define  RUN   1
#define  END   2
#define  OPEN  3
#define  DURATION 10

int appState = START;
bool inStartButton = false; 
int timeCounter = DURATION; 

// this is function signature (prototype)
// this is called "function declaration"
void onTimer(int v);

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

void display_background() {
   // label
   glColor3f(0, 0, 0);
   vprint(-240, 230, GLUT_BITMAP_8_BY_13, "Name Surname");
   vprint(-240, 210, GLUT_BITMAP_8_BY_13, "208997656");
   vprint(-70, 200, GLUT_BITMAP_9_BY_15, " - Homework -");

   // main container
   glColor3ub(225, 230, 255);
   glRectf(-200, -150, 200, 150);

   glColor3ub(175, 200, 240);
   glRectf(-200, 150, 200, 91);

   glColor3ub(100, 130, 180);
   glRectf(-200, -150, 200, -92);

   glLineWidth(3);
   glColor3ub(140, 170, 200);
   glBegin(GL_LINES);
   glVertex2f(-200, 90);
   glVertex2f(200, 90);
   glEnd();

   glColor3ub(90, 100, 140);
   glBegin(GL_LINES);
   glVertex2f(-200, -91);
   glVertex2f(200, -91);
   glEnd();

   glBegin(GL_LINE_LOOP);
   glVertex2f(-200, -150);
   glVertex2f(-200, 150);
   glVertex2f(200, 150);
   glVertex2f(200, -150);
   glEnd();
   glLineWidth(1);

   vprint(-30, 120, GLUT_BITMAP_9_BY_15, "MicroWave Interface v1.0");
}

void display_start() {
   // circle with a border for start button
   glColor3ub(90, 100, 140);
   circle(0, 0, 70);

   // background of the circular start button
   if (inStartButton) glColor3ub(255, 200, 200);
   else glColor3ub(255, 255, 255);
   circle(0, 0, 67);

   glColor3ub(90, 100, 140);
   vprint2(-50, -12, 0.25, "START");

   glColor3ub(255, 255, 255);
   vprint(-190, -130, GLUT_BITMAP_HELVETICA_12, "Click Start button");
}
void display_run() {
   glColor3ub(90, 100, 140);
   vprint2(-50, 0, 0.35, "00:%02d", timeCounter);

   int width = (DURATION - timeCounter) * 30; 
   glColor3ub(170, 250, 140);
   glRectf(-150, -70, -150 + width, -20);
}
void display_end() {
   glColor3ub(90, 100, 140);
   vprint2(-50, 0, 0.35, "00:00");

   glColor3ub(255, 150, 140);
   glRectf(-150, -70, 150 , -20);

   glColor3f(1, 1, 1);
   vprint(-190, -130, GLUT_BITMAP_HELVETICA_12, "DOWN Arrow to Open");
}
void display_open() {
   glColor3ub(90, 100, 140);
   vprint2(-60, 30, 0.35, "DOOR");
   vprint2(-10, -20, 0.35, "IS");
   vprint2(-60, -70, 0.35, "OPEN");

   glColor3f(1, 1, 1);
   vprint(-190, -130, GLUT_BITMAP_HELVETICA_12, "UP Arrow to Open");

}
//
// To display onto window using OpenGL commands
//
void display()
{

   glClearColor( 1, 1 , 1 , 0 );
   glClear( GL_COLOR_BUFFER_BIT );

   // common background graphics for all states.
   display_background();

   switch (appState) {
   case START: display_start(); break;
   case RUN: display_run(); break;
   case END: display_end(); break;
   case OPEN: display_open(); break;

   }
 
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

   if (key == GLUT_KEY_DOWN && appState == END) {
      appState = OPEN; 
   }

   if (key == GLUT_KEY_UP && appState == OPEN) {
      appState = START;
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
   if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
      if (appState == START && inStartButton == true) {
         appState = RUN;
         timeCounter = DURATION;
         glutTimerFunc(1000, onTimer, 0);
      }

   }

   
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


// (px, py) : coordinate of a point, p
// (cx, cy) : center point (centroid) of the circle
// R : radius of the circle
// Result: true (point is withing the circle), otherwise false.
bool checkCircle(float px, float py, float cx, float cy, float R) {
   float dx = px - cx;
   float dy = py - cy;
   float d = sqrt(dx*dx + dy*dy);
   return d <= R;
}

// When you move mouse pointer, this event handler is called automatically
void onMove( int x, int y ) {
   // Write your codes here.

   // Convert coordinate (x,y) from GLUT to OpenGL
   int mx = x - winWidth / 2;
   int my = winHeight / 2 - y;

   // modify the flag that shows if the mouse is on the button.
   // start button circle = (0, 0 ) with radius 70
   inStartButton = checkCircle(mx, my, 0, 0, 70);

   
    // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) {
   // Write your codes here.
   if (appState == RUN) {
      timeCounter--;
      if (timeCounter > 0) {
         glutTimerFunc(TIMER_PERIOD, onTimer, 0);
      }
      else {
         // time expires
         appState = END;
      }
   }
    
   
   // to refresh the window it calls display() function
   glutPostRedisplay() ; // display()

}
#endif

void Init() {
   
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
   
   Init();
   
   glutMainLoop();
}
