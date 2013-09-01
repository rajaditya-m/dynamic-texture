#ifndef __DYNAMICTEXTURES_IMAGEMANIP_H_
#define __DYNAMICTEXTURES_IMAGEMANIP_H_

#include <iostream>

namespace DynamicTextures
{
	void readPPMImage(char* fName,int* height,int* width) 
	{
		FILE* in = fopen(fName, "r"); 

		int tht,twt;

		GLubyte readImage[1028][768][4]; 
		GLubyte texImg[256][256][3];

	  	int  ccv; 
	  	char header[100]; 
	  	fscanf(in, "%s %d %d %d", header, &twt, &tht, &ccv); 

	  	printf("%s %d %d %d\n", header, twt, tht, ccv);
	  	int r, g, b; 

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
	     	

	  	for (int i=0; i<256; i++)
	  	{
	    	for ( int j=0; j<256; j++)
	    	{
      			if (i<tht && j <twt)
      			{
					texImg[i][j][0] = readImage[i][j][0]; 
					texImg[i][j][1] = readImage[i][j][1];
					texImg[i][j][2] = readImage[i][j][2];
					//texImage[i][j][3] = 255; 
      			}
      			else 
      			{
      				texImg[i][j][0] = 0; 
					texImg[i][j][1] = 0; 
					texImg[i][j][2] = 0; 
					//texImage[i][j][3] = 255; 
      			}
	    	}
	  	}
	  
	  	fclose(in); 
	}

}

#endif