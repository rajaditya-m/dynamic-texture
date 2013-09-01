#include "Oracle.h"
#include "pointRep.h"
//#include "imageManip.h"
#include <iostream>


int numVertices;

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;


point4 points[1000];
vec3 normals[1000];
vec2 texcoods[1000];

//some transformation params
int xform_mode = 0; 
#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2 
GLfloat x_angle=0.0,y_angle=0.0,z_angle=0.0;
int press_x, press_y; 
int release_x, release_y; 
float scale_size = 1.0;
GLfloat u_disp= 0.0,v_disp=0.0;

//some translation params
#define TRANS_D 0.1

//Uniform Locations for the ModelView and Projection Matrices in the shaders
GLuint ModelView,Projection;
GLuint texture;

//@TODO: Delete 
GLubyte readImage[1028][768][4]; 
GLubyte texImg[256][256][3];
int tht;
int twt;

int idx = 0;

void triangle(const point4& a,const point4& b,const point4& c)
{
	vec3 normal = normalize(cross(b-a,c-b));
	normals[idx] = normal; points[idx] = a; idx++;
	normals[idx] = normal; points[idx] = b; idx++;
	normals[idx] = normal; points[idx] = c; idx++;
}

point4 unit (const point4& p)
{
	float len = p.x*p.x + p.y*p.y + p.z*p.z;

	point4 t;
	if(len>DivideByZeroTolerance)
	{
		t = p/sqrt(len);
		t.w = 1.0;
	}
	return t;
}

//@TODO ::Delete 
void read_Image() 
{
  FILE* in = fopen("soupLabel-P3.ppm", "r"); 

  int  ccv; 
  char header[100]; 
  fscanf(in, "%s %d %d %d", header, &twt, &tht, &ccv); 

  printf("%s %d %d %d\n", header, twt, tht, ccv);
  int r, g, b; 

  for (int i=tht-1; i>=0; i--)
     for (int j=0; j<twt; j++)
{
      fscanf(in, "%d %d %d", &r, &g, &b); 
      readImage[i][j][0] = (GLubyte)r; 
      readImage[i][j][1] = (GLubyte)g; 
      readImage[i][j][2] = (GLubyte )b; 
      //readImage[i][j][3] = 255; 
    }

  for (int i=0; i<256; i++)
    for ( int j=0; j<256; j++) {
      if (i<tht && j <twt) {
	texImg[i][j][0] = readImage[i][j][0]; 
	texImg[i][j][1] = readImage[i][j][1];
	texImg[i][j][2] = readImage[i][j][2];
	//texImage[i][j][3] = 255; 
      }
      else {
      	texImg[i][j][0] = 0; 
	texImg[i][j][1] = 0; 
	texImg[i][j][2] = 0; 
	//texImage[i][j][3] = 255; 
      }
    }
  
  fclose(in); 
}


void InitCylinder(double height,double lRad,double tRad,int nslices, int nstacks) 
{


  	int numUniqueVertices = nslices * nstacks; 

  	DynamicTextures::PointRep* pointPool = new DynamicTextures::PointRep[numUniqueVertices];

	double taper = (lRad - tRad)/(double)(nstacks-1);
	double Dangle = (2*M_PI)/(double)(nslices-1); 

	point4 tempPoint;
	Angel::vec3 tempNormal;
	Angel::vec2 tempTex;

	for (int j =0; j<nstacks; j++)
	{
		for (int i=0; i<nslices; i++) 
		{
	      	int idx = j*nslices + i; // mesh[j][i] 

	      	double angle = Dangle * i;
		  	double aRad = lRad - (taper*j); 

		  	tempPoint.x = aRad*cos(angle);
		  	tempPoint.y = aRad*sin(angle);
		  	tempPoint.z = j*height/(double)(nstacks-1); 
		  	tempPoint.w = 1.0;

		  	tempNormal.x = aRad*cos(angle);
		  	tempNormal.y = aRad*sin(angle);
		  	tempNormal.z = 0.0;

		  	//tempTex.x = (atan2(lRad*tempPoint.y,tRad*tempPoint.x)+M_PI*0.5)/M_PI;
		  	tempTex.x = angle/(2*M_PI);
		  	tempTex.y = tempPoint.z/height;

		  	//std::cout << tempTex << "\n";

		  	pointPool[idx].point_ = tempPoint;
		  	pointPool[idx].normal_ = tempNormal;
		  	pointPool[idx].texture_ = tempTex;

	    }
	}

  	numVertices = (nstacks-1)*2*(nslices+1);
  	
  	int n = 0; 
  	for (int j =0; j<nstacks-1; j++)
  	{
  		for (int i=0; i<=nslices; i++) 
  		{
      		int mi = i % nslices;  
      		int idx = j*nslices + mi; // mesh[j][mi] 
      		int idx2 = (j+1) * nslices + mi; 
      		
      		points[n] = pointPool[idx].point_;
      		texcoods[n] = pointPool[idx].texture_;
      		normals[n++] = pointPool[idx].normal_;
      		points[n] = pointPool[idx2].point_;
      		texcoods[n] = pointPool[idx2].texture_;
      		normals[n++] = pointPool[idx2].normal_;
    	}
  	}
}

void init()
{
	read_Image();

	InitCylinder(4,2,2,20,20);

	//Texture Binding Stuff
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,texImg);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glActiveTexture(GL_TEXTURE0);

	//Generate Vertex Array Objects
	GLuint vao;
	glGenVertexArraysAPPLE(1,&vao);
	glBindVertexArrayAPPLE(vao);

	//Generate Vertex Buffer Objects
	GLuint buffer;
	glGenBuffers(1,&buffer);
	glBindBuffer(GL_ARRAY_BUFFER,buffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(points)+sizeof(normals)+sizeof(texcoods),NULL,GL_STATIC_DRAW);

	GLintptr offset = 0;
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(points),points);
	offset += sizeof(points);
	glBufferSubData(GL_ARRAY_BUFFER,offset,sizeof(normals),normals);
	offset += sizeof(normals);
	glBufferSubData(GL_ARRAY_BUFFER,offset,sizeof(texcoods),texcoods);

	//Load shader setups
	GLuint program = InitShader("shaders/vshader_dynTex.glsl","shaders/fshader_dynTex.glsl");
	glUseProgram(program);

	//setup vertex arrays for data transmissions
	offset = 0;
	GLuint vPosition = glGetAttribLocation(program,"vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition,4,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(offset));

	offset += sizeof(points);

	GLuint vNormal = glGetAttribLocation(program,"vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal,3,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(offset));

	offset += sizeof(normals);

	GLuint vTexCoods = glGetAttribLocation(program,"vTextureCoods");
	glEnableVertexAttribArray(vTexCoods);
	glVertexAttribPointer(vTexCoods,2,GL_FLOAT,GL_FALSE,0,BUFFER_OFFSET(offset));

	glUniform1i(glGetUniformLocation(program,"texture"),0);

	point4 lightPosition(20.0,20.0,20.0,0.0);
	color4 lightAmbient(0.2,0.2,0.2,1.0);
	color4 lightDiffuse(1.0,1.0,1.0,1.0);
	color4 lightSpecular(1.0,1.0,1.0,1.0);

	color4 materialAmbient(1.0,0.0,1.0,1.0);
	color4 materialDiffuse(1.0,0.8,0.0,1.0);
	color4 materialSpecular(1.0,0.0,1.0,1.0);
	float materialShininess = 0.5;

	color4 ambientPdk = lightAmbient*materialAmbient;
	color4 diffusePdk = lightDiffuse*materialDiffuse;
	color4 specularPdk = lightSpecular*materialSpecular;

	glUniform4fv(glGetUniformLocation(program,"ambientPdk"),1,ambientPdk);
	glUniform4fv(glGetUniformLocation(program,"diffusePdk"),1,diffusePdk);
	glUniform4fv(glGetUniformLocation(program,"specularPdk"),1,specularPdk);

	glUniform4fv(glGetUniformLocation(program,"lightPos"),1,lightPosition);

	glUniform1f(glGetUniformLocation(program,"shininess"),materialShininess);

	ModelView = glGetUniformLocation(program,"modelView");
	Projection = glGetUniformLocation(program,"projection");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//Model Matrix
	Angel::mat4 model_ = mat4(1.0);
	Angel::vec3 scaleVec(scale_size,scale_size,scale_size);
	//Angel::vec3 transVec(4.0,0.0,0.0);
	Angel::vec3 transVec(u_disp,v_disp,0.0);
	Angel::mat4 scaleMat = Scale(scaleVec);
	Angel::mat4 xRotMat = RotateY(x_angle);
	Angel::mat4 yRotMat = RotateX(y_angle);
	Angel::mat4 transMat = Translate(transVec);
	model_ = scaleMat*xRotMat*yRotMat*transMat;

	//Viewing Matrix
	point4 at(0.0,0.0,0.0,1.0);
	point4 eye(0.0,5.0,3.0,1.0);
	vec4 up(0.0,0.0,1.0,0.0);
	mat4 view_ = LookAt(eye,at,up);

	mat4 modelView_ = model_*view_;
	glUniformMatrix4fv(ModelView,1,GL_TRUE,modelView_);

	glDrawArrays(GL_TRIANGLE_STRIP,0,numVertices);
	
	glutSwapBuffers();
}

void mouseFunc(int button, int state, int x, int y)
{
  if (state == GLUT_DOWN) {
    press_x = x; press_y = y; 
    if (button == GLUT_LEFT_BUTTON)
      xform_mode = XFORM_ROTATE; 
	 else if (button == GLUT_RIGHT_BUTTON) 
      xform_mode = XFORM_SCALE; 
  }
  else if (state == GLUT_UP) {
	  xform_mode = XFORM_NONE; 
  }
}

void motionFunc(int x, int y)
{
    if (xform_mode==XFORM_ROTATE) {

      x_angle += (x - press_x)/5.0; 
      if (x_angle > 180) x_angle -= 360; 
      else if (x_angle <-180) x_angle += 360; 
      press_x = x; 
	   
      y_angle += (y - press_y)/5.0; 
      if (y_angle > 180) y_angle -= 360; 
      else if (y_angle <-180) y_angle += 360; 
      press_y = y; 
     }
	else if (xform_mode == XFORM_SCALE){
      float old_size = scale_size;
      scale_size *= (1+ (y - press_y)/60.0); 
      if (scale_size <0) scale_size = old_size; 
      press_y = y; 
    }
    glutPostRedisplay(); 
}

void kbdFunc(unsigned char key,int x,int y)
{
	switch(key)
	{
		case 'q' : case 'Q' : 
			std::cout << "U:" << u_disp << "V:" << v_disp << "\n";
			exit(EXIT_SUCCESS); 
			break;
		case 'd' : case 'D' :
			u_disp = u_disp + TRANS_D; 
			glutPostRedisplay();
			break;
		case 'a' : case 'A' : 
			u_disp = u_disp - TRANS_D;
			glutPostRedisplay();
			break;
		case 'w' : case 'W' : 
			v_disp = v_disp + TRANS_D;
			glutPostRedisplay();
			break;
		case 's' : case 'S' :
			v_disp = v_disp - TRANS_D;
			glutPostRedisplay();
			break;
	}
}

void reshapeFunc(int width,int height)
{
	glViewport(0,0,width,height);

	GLfloat left = -10.0,right = 10.0;
	GLfloat top = 10.0, bottom = -10.0;
	GLfloat hither = -10.0,yon = 10.0;

	GLfloat aspectRatio = GLfloat(width)/height;

	if(aspectRatio > 1.0)
	{
		left *= aspectRatio;
		right *= aspectRatio;
	} 
	else
	{
		top /= aspectRatio;
		bottom /= aspectRatio;
	}

	mat4 projection_ = Ortho(left,right,bottom,top,hither,yon);
	glUniformMatrix4fv(Projection,1,GL_TRUE,projection_);
}

int main(int argc, char** argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH);
	glutInitWindowSize(512,512);
	glutCreateWindow("Dynamic Textures");

	glewExperimental = GL_TRUE;
	glewInit();

	init();

	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(display);
	glutKeyboardFunc(kbdFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);

	glutMainLoop();
	return 0;
}