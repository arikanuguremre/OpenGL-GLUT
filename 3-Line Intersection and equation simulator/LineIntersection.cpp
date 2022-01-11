#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON      0    // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

/* Programmer variables and definitions */
typedef struct {
   float x, y;
} point_t;

typedef struct {
   point_t start, end;
   float A, B, C;
} line_t;

typedef struct {
   point_t center;
   float radius;
} circle_t;

typedef struct {
   bool hit;
   float dist;
   point_t nearest;
   point_t ixect;
} result_t;

circle_t circ = { {0, 0} , 50 };

line_t line;
int vertNo = 0;

line_t perp;
result_t res;

point_t mouse;

//
// to draw circle, center at (x,y)
// radius r
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

float testPoint(line_t line, point_t p) {
   float dx = line.end.x - line.start.x;
   float dy = line.end.y - line.start.y;
   // Ix = Mx + dx.t => t = (Ix - Mx) / dx
   if (dx != 0) {
      return (p.x - line.start.x) / dx;
   }
   // Iy = My + dy.t => t = (Iy - My) / dy
   return (p.y - line.start.y) / dy;
}

float distance(point_t p, point_t q) {
   return sqrt((p.x - q.x) * (p.x - q.x) + (p.y - q.y)*(p.y - q.y));
}

void calcLines() {
// *** 1) Find General Line Equation
   line.A = line.start.y - line.end.y;  // A1 = y1 - y2
   line.B = line.end.x - line.start.x;  // B1 = x2 - x1
   // C1 = A1.x1 + B1.y1
   line.C = line.A * line.start.x + line.B * line.start.y;

// *** 2) Find Perpendicular Line Equation
   perp.A = -line.B; // Ap = -B1
   perp.B = line.A;  // Bp = A1
   // Cp = Ap.xp + Bp.yp   (xp, yp) is the center of circle
   perp.C = perp.A * circ.center.x + perp.B * circ.center.y;

// *** 3) Find Intersection Point
   // xi = (B1.Cp - Bp.C1) / (B1.Ap - Bp.A1)
   // yi = (Ap.C1 - A1.Cp) / (B1.Ap - Bp.A1)
   float denom = (line.B * perp.A - perp.B * line.A);
   res.ixect.x = (line.B * perp.C - perp.B * line.C) / denom;
   res.ixect.y = (perp.A * line.C - line.A * perp.C) / denom;

// *** 4) Find the nearest point (intersection or one of the end points)
   float t = testPoint(line, res.ixect);
   if (t < 0)
      res.nearest = line.start;
   else if (t > 1.0)
      res.nearest = line.end;
   else
      res.nearest = res.ixect;

// *** 5) Find the shortest distance to the line segment
   res.dist = distance(res.nearest, circ.center);

// *** 6) Test if the circle hits the wall (line segment)
   res.hit = res.dist <= circ.radius;
}

void drawFirstPoint() {
   glColor3f(1, 1, 0);
   circle(line.start.x, line.start.y, 5);
}

void drawLine(point_t s, point_t e, float size) {
   glLineWidth(size);
   glBegin(GL_LINES);
      glVertex2f(s.x, s.y);
      glVertex2f(e.x, e.y);
   glEnd();
   glLineWidth(1.0f);
}

void drawLineSegment() {
   circle(line.end.x, line.end.y, 5);
   glColor3f(1, 1, 1);
   drawLine(line.start, line.end, 3);
}

point_t  findPoint(point_t start, point_t end, float t) {
   point_t p = { start.x + (end.x - start.x) *t, start.y + (end.y - start.y) * t };
   return p;
}

void drawMainLines() {
   point_t s = findPoint(line.start, line.end, -100);
   point_t e = findPoint(line.start, line.end, 100);
   glColor3f(0.8, 0.8, 0.8);
   glEnable(GL_LINE_STIPPLE);
   glLineStipple(1, 0x00F0);
   drawLine(s, e, 1);
   s = findPoint(circ.center, res.ixect, 100);
   e = findPoint(circ.center, res.ixect, -100);
   drawLine(s, e, 1);
   glDisable(GL_LINE_STIPPLE);
}

void drawNearestLine() {
   // to nearest point
   glColor3f(1.0, 0.1, 0.1);
   drawLine(res.nearest, circ.center, 1);
}

void drawIntersection() {
   // draw intersection
   glColor3f(0.3, 0.3, 1.0);
   drawLine(res.ixect, circ.center, 3);

   circle(res.ixect.x, res.ixect.y, 5);
   circle(circ.center.x, circ.center.y, 5);
}

void drawCircularObject() {
   glColor4f(0, 1, 0, 0.7);
   if (res.hit)
      glColor4f(1, 0, 0, 0.7);
   circle(circ.center.x, circ.center.y, circ.radius);
}

void showEquations() {
   glColor4f(0, 0, 0.5, 0.9);
   glRectf(-100, -150, -395, -295);
   glColor3f(0.3, 0.3, 1.0);
   // line Equations
   vprint(-390, -170, GLUT_BITMAP_8_BY_13,
      "Line : %.1fx + %.1fy = %.1f", line.A, line.B, line.C);
   vprint(-390, -190, GLUT_BITMAP_8_BY_13,
      "Perp : %.1fx + %.1fy = %.1f", perp.A, perp.B, perp.C);

   glColor3f(1, 0.3, 0.3);
   vprint(-390, -210, GLUT_BITMAP_8_BY_13, "Intersection : ( %.1f %.1f )",
      res.ixect.x, res.ixect.y);
   glColor3f(1, 1, 1);
   vprint(-390, -230, GLUT_BITMAP_8_BY_13, "distance = %.1f", res.dist);
}

//
// To display onto window using OpenGL commands
//
void display() {
   
   //
   // clear window to black
   //
   glClearColor( 0, 0, 0, 0 );
   glClear( GL_COLOR_BUFFER_BIT );

   //mouse coordinate
   glColor3f(1, 1, 0);
   vprint(310, 280, GLUT_BITMAP_8_BY_13, "%.0f %.0f", mouse.x, mouse.y);
  
   if (vertNo > 0) {
      drawFirstPoint();
   }
 
   if (vertNo > 1) {
      drawLineSegment();
   }

   if (vertNo > 2) {
      //draw lines passing thru line segment, and perpendicular line.
      drawMainLines();

      drawNearestLine();
      drawIntersection();
      drawCircularObject();
      showEquations();

      glColor3f(1, 1, 0);
      vprint(-390, -290, GLUT_BITMAP_8_BY_13, "Press F1 to redraw the line");
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
   if (key == GLUT_KEY_F1) {
      vertNo = 0;
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
   point_t click = { x - winWidth / 2 , winHeight / 2 - y };

   if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
      if (vertNo == 0) //starting point of the line segment
         line.start = click;
      if (vertNo == 1) { //end point of the line segment
         if (line.start.x == click.x && line.start.y == click.y)
            return; //if the same point is clicked (not a line segment; a point)
                    //   do nothing; wait for another point
         line.end = click;
      }
      if (vertNo >= 2) { //center of the circle
         circ.center = click;
         calcLines();
      }
      vertNo++;
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
   glViewport( 0, 0, w, h ) ;
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
   mouse.x = x - winWidth / 2;
   mouse.y = winHeight / 2 - y;

   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) {
    
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   // Write your codes here.

    
   
   // to refresh the window it calls display() function
   glutPostRedisplay() ; // display()

}
#endif

void Init() {
   
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
}

void main( int argc, char *argv[] ) {

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
