#include <bits/stdc++.h>
//#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>

using namespace std;

typedef struct{
	float X;
	float Y;
	float Z;
}Point;

Point p[8];
int slices = 360, stacks = 360;
static GLubyte image[5][2048][1768][4];
static GLubyte linearImage[5][2048*1768*4];
unsigned int imagewidth=0, imageheight=0;
unsigned long fileSize;
static GLuint texName[5];
float T=0, Q=0;
float xg,yg,zg,xr=-1.0,yr=0.5,zr=0.5,xb,yb,zb;
int red,blue,green;
float curAngle=0.0f, animateStep=0.5f;

void Resize(GLsizei width, GLsizei height)
{
	if (height==0)
	{
		height=1;
	}
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1040.0f / 680.0f, 10, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void convertToLinearArray()
{
	int len;
	int index;
	for(index=0;index<5;index++)
	{
		len=0;
        for(unsigned int r=0; r<imageheight; r++)
        {
            for(unsigned int c=0; c<imagewidth; c++)
            {
                for(int k=2; k>=0;k--)
                {
                    linearImage[index][len++] = image[index][r][c][k];

                }
                linearImage[index][len++] = (GLubyte)255;
            }
        }
	}
}

GLUquadricObj *obj1;
int InitGL()
{
    glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	obj1 = gluNewQuadric();
	gluQuadricNormals(obj1, GLU_SMOOTH);
	gluQuadricTexture(obj1, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	/*glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);*/
	glEnable(GL_NORMALIZE);
	glEnable(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);

	float v = 2.5;
	p[0].X=-v;p[0].Y=-v;p[0].Z=-v;
	p[1].X=-v;p[1].Y=v;p[1].Z=-v;
	p[2].X=v;p[2].Y=v;p[2].Z=-v;
	p[3].X=v;p[3].Y=-v;p[3].Z=-v;

	p[4].X=-v;p[4].Y=-v;p[4].Z=v;
	p[5].X=-v;p[5].Y=v;p[5].Z=v;
	p[6].X=v;p[6].Y=v;p[6].Z=v;
	p[7].X=v;p[7].Y=-v;p[7].Z=v;

	convertToLinearArray();
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(5, texName);

    for(int i = 0; i < 5; i++)
    {
        glBindTexture(GL_TEXTURE_2D, texName[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA, imagewidth, imageheight, 0, GL_RGBA, GL_UNSIGNED_BYTE,linearImage[i]);
    }

	return 1;
}

void LightingParameters()
{
    const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 0.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0.0f };
    //const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 0.0f, 7.0f, 0.0f, 0.0f };

    const GLfloat mat_ambient[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat mat_diffuse[]    = { 1.0f, 1.0f, 1.0f, 0.0f };
    //const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 50.0f };

    glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    //glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//Add directed light red
    GLfloat lightColor2[] = {0.7f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
	xr = -1.0;
	yr = 0.5;
	zr = -0.5;
    GLfloat lightPos2[] = {xr, yr, zr, 0.0f};
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor2);
    glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);

	//Add directed light green
    GLfloat lightColor3[] = {0.2f, 0.7f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos3[] = {1.0f, 0.5f, 0.5f, 0.0f};
    glLightfv(GL_LIGHT3, GL_DIFFUSE, lightColor3);
    glLightfv(GL_LIGHT3, GL_POSITION, lightPos3);

	//Add directed light blue
    GLfloat lightColor4[] = {0.2f, 0.2f, 0.9f, 1.0f}; //Color (0.5, 0.2, 0.2)
    //Coming from the direction (-1, 0.5, 0.5)
    GLfloat lightPos4[] = {0.0f, 0.0f, -0.5f, 0.0f};
    glLightfv(GL_LIGHT4, GL_DIFFUSE, lightColor4);
    glLightfv(GL_LIGHT4, GL_POSITION, lightPos4);

    /*GLfloat spot_direction[] = {0.0, -1.0, 0.0};
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
    glLightf (GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);*/

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
    //glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
}

int compare_float(float f1, float f2)
 {
  float precision = 0.00001;
  if (((f1 - precision) < f2) &&
      ((f1 + precision) > f2))
   {
    return 1;
   }
  else
   {
    return 0;
   }
 }
float X = 0, Y = 0, Z = -20;
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	  case 27:
			exit(0);
      case '1':
        X = X + 1;
        break;
      case '2':
        X = X - 1;
        break;
      case '3':
        Y = Y + 1;
        break;
      case '4':
        Y = Y - 1;
        break;
      case '5':
        if(Z < 12.5) Z = Z + 1;
        break;
      case '6':
        Z = Z - 1;
        break;
      case 'z':
        X = 0, Y = 0, Z = -20;
        break;
      case 'a':
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        //glEnable(GL_LIGHT1);
        LightingParameters();
        break;
      case 's':
        glDisable(GL_LIGHT0);
        break;
	  case 'r':
	    red=1;
        glEnable(GL_COLOR_MATERIAL);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT2);
        LightingParameters();
        break;
      case 't':
        glDisable(GL_LIGHT2);
        break;
  	  case 'g':
	    glEnable(GL_COLOR_MATERIAL);
	    glEnable(GL_LIGHTING);
	    glEnable(GL_LIGHT3);
	    LightingParameters();
	    break;
	  case 'h':
	    glDisable(GL_LIGHT3);
	    break;
	  case 'b':
  	    glEnable(GL_COLOR_MATERIAL);
  	    glEnable(GL_LIGHTING);
  	    glEnable(GL_LIGHT4);
  	    LightingParameters();
  	    break;
  	  case 'n':
  	    glDisable(GL_LIGHT4);
  	    break;

      case 'd':
        glDisable(GL_LIGHTING);
        break;

    case 'i':
        if(red)
        {
            glEnable(GL_COLOR_MATERIAL);
            glEnable(GL_LIGHTING);
            glEnable(GL_LIGHT2);
            const GLfloat light_ambient[]  = { 0.5f, 0.5f, 0.5f, 0.0f };
            const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 0.0f };
            const GLfloat light_position[] = { 0.0f, 7.0f, 0.0f, 0.0f };

            const GLfloat mat_ambient[]    = { 1.0f, 1.0f, 1.0f, 1.0f };
            const GLfloat mat_diffuse[]    = { 1.0f, 1.0f, 1.0f, 0.0f };
            const GLfloat high_shininess[] = { 50.0f };

            glLightfv(GL_LIGHT1, GL_AMBIENT,  light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
            glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            GLfloat lightColor2[] = {0.7f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.2, 0.2)

            cout<<xr<<" "<<zr<<endl;
            if(xr>=-1.0 && xr<0 && zr>=-0.5 && zr<2.0)
            {
                cout<<"in1"<<endl;
				zr+=0.5;
				if(compare_float(xr,-0.2)) xr = 0.0;
				else
                	xr+=0.2;

			}
			else if(xr>-1.0 && xr<=0 && zr>=-3.0 && zr<-0.5) {
                cout<<"in2"<<endl;
                zr+=0.5;
                xr-=0.2;
            }
            else if(xr>=0 && xr<1.0 && zr<= 2.0 && zr>-0.5)
            {
                cout<<"in3"<<endl;
                zr-=0.5;
                xr+=0.2;
            }
            else if(xr>0 && xr<=1.0 && zr>-3.0 && zr<=-0.5)
            {
                cout<<"in4"<<endl;
                zr-=0.5;
				if(compare_float(xr,0.2)) xr = 0.0;
				else
                	xr-=0.2;
            }

            GLfloat lightPos2[] = {xr, yr, zr, 0.0f};
            glLightfv(GL_LIGHT2, GL_DIFFUSE, lightColor2);
            glLightfv(GL_LIGHT2, GL_POSITION, lightPos2);
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
            glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);

            glutPostRedisplay();

        }
        break;
        case 'o':
  	    glDisable(GL_LIGHT0);
  		glDisable(GL_LIGHT1);
  		glDisable(GL_LIGHT2);
  		glDisable(GL_LIGHT3);
  		glDisable(GL_LIGHT4);
  	    break;
    }
    glutPostRedisplay();
}


void drawCylinder(float ty, int base, int top, int height)
{
    glPushMatrix();
    glTranslated(0,ty,0);
    glRotated(-90,1,0,0);
    GLUquadricObj *obj = gluNewQuadric();
    gluQuadricDrawStyle(obj, GLU_FILL);
    gluQuadricNormals(obj, GLU_SMOOTH);
    gluQuadricTexture(obj,1);
    gluCylinder(obj, base, top, height, 360, 360);
    glPopMatrix();
}

void drawline(float a, float b, float c, float a1, float b1, float c1)
{
    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    glVertex3f(a, b, c);
    glVertex3f(a1, b1, c1);
    glEnd();
}

void DrawGLScene()
{
    /*const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    const double a = t*90.0;
	static float i=0;
	i+=1;*/

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
    gluLookAt(X, Y, Z-25, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

    //glRotatef(a,1.0,1.0,1.0);
    //glTranslated(0, 0 ,-40);
    //glRotated(-130,1,0,0);
    //glRotated(30,0,1,0);

    drawline(0, 4.5, 0, 0, 8.5, 0);
    drawline(-6.7, 7, 0, 1.5, 7, 0);
    drawline(7, 7, 0, 1.5, 7, 0);
    drawline(0, 7, 1.5, 0, 7, -7);
    drawline(0, 7, 1.5, 0, 7, 6.7);

    glColor3f(1,1,1);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glBindTexture(GL_TEXTURE_2D, texName[0]);
        drawCylinder(4.6,7.8,7,7);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
        glTranslated(0, 7, 0);
        glColor3f(1, 1, 1);
        glutSolidSphere(1.5,slices,stacks);
    glPopMatrix();

    /*glPushMatrix();
        glTranslated(7, -2, 0);
        glColor3f(1, 0, 0);
        glutSolidSphere(2,slices,stacks);
    glPopMatrix();

    glPushMatrix();
        glTranslated(9, 7, 0);
        glColor3f(1, 1, 1);
        glutSolidSphere(2,slices,stacks);
    glPopMatrix();*/

    glPushMatrix();
	//glLoadIdentity();
	glTranslated(0,2,0);
    glEnable(GL_TEXTURE_2D);

	//sets texturing environment parameter
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glBindTexture(GL_TEXTURE_2D, texName[4]);
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(-8.5, -11, -7);
        glTexCoord2f(0.0, 1.0); glVertex3f(8.5, -11, -7);
        glTexCoord2f(1.0, 1.0); glVertex3f(8.5, -11, 7);
        glTexCoord2f(1.0, 0.0); glVertex3f(-8.5, -11, 7);
	glEnd();
    glBegin(GL_QUADS);
        glNormal3f(0.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(-8.5, -14, -7);
        glTexCoord2f(0.0, 1.0); glVertex3f(8.5, -14, -7);
        glTexCoord2f(1.0, 1.0); glVertex3f(8.5, -14, 7);
        glTexCoord2f(1.0, 0.0); glVertex3f(-8.5, -14, 7);
	glEnd();
    glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(-8.5, -14, 7);
        glTexCoord2f(0.0, 1.0); glVertex3f(8.5, -14, 7);
        glTexCoord2f(1.0, 1.0); glVertex3f(8.5, -11, 7);
        glTexCoord2f(1.0, 0.0); glVertex3f(-8.5, -11, 7);
	glEnd();
	glBegin(GL_QUADS);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(-8.5, -14, -7);
        glTexCoord2f(0.0, 1.0); glVertex3f(8.5, -14, -7);
        glTexCoord2f(1.0, 1.0); glVertex3f(8.5, -11, -7);
        glTexCoord2f(1.0, 0.0); glVertex3f(-8.5, -11, -7);
	glEnd();
	glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(-8.5, -14, 7);
        glTexCoord2f(0.0, 1.0); glVertex3f(-8.5, -14, -7);
        glTexCoord2f(1.0, 1.0); glVertex3f(-8.5, -11, -7);
        glTexCoord2f(1.0, 0.0); glVertex3f(-8.5, -11, 7);
	glEnd();
	glBegin(GL_QUADS);
        glNormal3f(1.0f, 0.0f, 0.0f);
        glTexCoord2f(0.0, 0.0); glVertex3f(8.5, -14, 7);
        glTexCoord2f(0.0, 1.0); glVertex3f(8.5, -14, -7);
        glTexCoord2f(1.0, 1.0); glVertex3f(8.5, -11, -7);
        glTexCoord2f(1.0, 0.0); glVertex3f(8.5, -11, 7);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

        //drawline(0, 2.5, 0, 0, 3.5, 0);
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glPushMatrix();
    glEnable(GL_TEXTURE_2D);
        glTranslated(0,1.75,0);
        glRotatef(90,1.0f,0.0f,0.0f);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glBindTexture(GL_TEXTURE_2D, texName[1]);
        gluQuadricDrawStyle(obj1, GLU_FILL);
        gluQuadricNormals(obj1, GLU_SMOOTH);
        gluQuadricTexture(obj1,1);
        gluSphere(obj1,3.14,20,20);
        //glutSolidSphere(4.7,160,160);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
        glTranslated(0,-4.5,0);
        glRotatef(90,1.0f,0.0f,0.0f);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

        glBindTexture(GL_TEXTURE_2D, texName[2]);
        gluQuadricDrawStyle(obj1, GLU_FILL);
        gluQuadricNormals(obj1, GLU_SMOOTH);
        gluQuadricTexture(obj1,1);
        gluSphere(obj1,4,20,20);
        //glutSolidSphere(4.7,160,160);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
        glBindTexture(GL_TEXTURE_2D, texName[3]);
        drawCylinder(-9,3,1,1.5);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

	glutSwapBuffers();
}

void Update(int value)
{
	T += 10.0f;
	Q += 20.5f;

	if (T > 360)
        T -= 360;

	if (Q > 360)
		Q -= 360;

	glutPostRedisplay();
	glutTimerFunc(25, Update, 0);
}

int loadImage(char *f_name,int index)
{
	FILE *fp;
	unsigned char blueValue, redValue, greenValue;
	unsigned int r,c;
	if(!(fp = fopen(f_name, "rb")))
	{
		printf("File Open Error\n");
		return 0;
	}
	fseek(fp,2,0);
	fread(&fileSize,4,1,fp);
	fseek(fp,18,0);

	fread(&imagewidth,4,1,fp);
	fseek(fp,22,0);
	fread(&imageheight,4,1,fp);

	fseek(fp,54,0);
	for(r=0; r<imageheight; r++)
	{
		for(c=0; c<imagewidth; c++)
		{
			fread(&blueValue, sizeof(char), 1, fp);
			fread(&greenValue, sizeof(char), 1, fp);
			fread(&redValue , sizeof(char), 1, fp);

			image[index][r][c][0] = (GLubyte)blueValue;
			image[index][r][c][1] = (GLubyte)greenValue;
			image[index][r][c][2] = (GLubyte)redValue;
			image[index][r][c][3] = (GLubyte)255;
		}
	}
	fclose(fp);
	return 1;
}

int  main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    char file_name[100];

    strcpy(file_name, "lamp46.bmp");
	if(!loadImage(file_name,0))
	{
		printf("\n0File Error\n");
		return 0;
	}

	strcpy(file_name, "lamp47.bmp");

	if(!loadImage(file_name,1))
	{
		printf("\n1File Error\n");
		return 0;
	}

	strcpy(file_name, "lamp13.bmp");
	//strcpy(file_name, "earth.bmp");
	if(!loadImage(file_name,2))
	{
		printf("\n2File Error\n");
		return 0;
	}

	strcpy(file_name, "lamp41.bmp");
	if(!loadImage(file_name,3))
	{
		printf("\n3File Error\n");
		return 0;
	}

	strcpy(file_name, "picc5.bmp");
	if(!loadImage(file_name,4))
	{
		printf("\n4File Error\n");
		return 0;
	}

    glutInitWindowSize(1040, 680);
    glutInitWindowPosition(100,0);
    glutCreateWindow("Table Lamp");

    InitGL();
    glutReshapeFunc(Resize);
    glutDisplayFunc(DrawGLScene);
    glutTimerFunc(250, Update, 0);
    glutKeyboardFunc(handleKeypress);
	//glutSpecialFunc(handleKeypress);
    //glClearColor(0.6,0.8,0.7,1);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glutMainLoop();

    return 0;
}
