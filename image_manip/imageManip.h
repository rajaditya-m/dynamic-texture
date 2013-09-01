#ifndef __DYNAMICTEXTURES_IMAGEMANIP_H_
#define __DYNAMICTEXTURES_IMAGEMANIP_H_

#include <iostream>

namespace DynamicTextures
{
	GLubyte* readASCIIPPMImage(char* fName,int* height,int* width) 
	{
		FILE* in = fopen(fName, "r"); 

		int tht,twt;

		GLubyte readImage[1000][1000][3]; 

	  	int  ccv; 
	  	char header[100]; 
	  	fscanf(in, "%s %d %d %d", header, &twt, &tht, &ccv); 
	  	printf("%s %d %d %d\n", header, twt, tht, ccv);
	  	int r, g, b; 

	  	*height = tht;
	  	*width = twt;

	  	GLubyte* texImg = new GLubyte[tht*twt*3];

	  	for (int i=tht-1; i>=0; i--)
	  	{
	  		for (int j=0; j<twt; j++)
			{
	      		fscanf(in, "%d %d %d", &r, &g, &b); 
	      		readImage[i][j][0] = (GLubyte)r; 
	      		readImage[i][j][1] = (GLubyte)g; 
	      		readImage[i][j][2] = (GLubyte)b; 
	      		//readImage[i][j][3] = 255; 
	    	}
	  	}
	     	
	  	for (int i=0; i<tht; i++)
	  	{
	    	for ( int j=0; j<twt; j++)
	    	{
	    		texImg[i*twt*3+j*3+0] = readImage[i][j][0];
				texImg[i*twt*3+j*3+1] = readImage[i][j][1];
				texImg[i*twt*3+j*3+2] = readImage[i][j][2];
	    	}
	  	}	  
	  	fclose(in);
	  	return texImg; 
	}

}

#endif