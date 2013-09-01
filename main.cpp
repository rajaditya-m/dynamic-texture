#include "Oracle.h"
#include "pointRep.h"
#include "imageManip.h"
//#include <iostream>


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

//Texture Stuff
GLubyte* texImg;
int texHeight;
int texWidth;

int idx = 0;

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

		  	tempTex.x = angle/(2*M_PI);
		  	tempTex.y = tempPoint.z/height;

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

	InitCylinder(4,2,2,20,20);

	//Texture Binding Stuff
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,texWidth,texHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texImg);
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

	texImg = DynamicTextures::readASCIIPPMImage("soupLabel-P3.ppm",&texHeight,&texWidth);

	init();

	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(display);
	glutKeyboardFunc(kbdFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);

	glutMainLoop();
	return 0;
}