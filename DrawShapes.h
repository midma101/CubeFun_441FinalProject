#ifndef DRAW_SHAPES_H
#define DRAW_SHAPES_H

#include "vtkSmartPointer.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkInteractorStyle.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkLight.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "vtkPNGReader.h"
#include "vtkJPEGReader.h"
#include "vtkImageData.h"

#include "portaudio.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <math.h>

#include <vector>

#include <vtkInteractorStyleTrackballCamera.h>


class Triangle
{
  public:
      double         X[3];
      double         Y[3];
      double         Z[3];
};


   void DrawCylinder()
   {
       int nfacets = 30;
       glBegin(GL_TRIANGLES);
       for (int i = 0 ; i < nfacets ; i++)
       {
           double angle = 3.14159*2.0*i/nfacets;
           double nextAngle = (i == nfacets-1 ? 0 : 3.14159*2.0*(i+1)/nfacets);
           glNormal3f(0,0,1);
           glVertex3f(0, 0, 1);
           glVertex3f(cos(angle), sin(angle), 1);
           glVertex3f(cos(nextAngle), sin(nextAngle), 1);
           glNormal3f(0,0,-1);
           glVertex3f(0, 0, 0);
           glVertex3f(cos(angle), sin(angle), 0);
           glVertex3f(cos(nextAngle), sin(nextAngle), 0);
       }
       glEnd();
       glBegin(GL_QUADS);
       for (int i = 0 ; i < nfacets ; i++)
       {
           double angle = 3.14159*2.0*i/nfacets;
           double nextAngle = (i == nfacets-1 ? 0 : 3.14159*2.0*(i+1)/nfacets);
           glNormal3f(cos(angle), sin(angle), 0);
           glVertex3f(cos(angle), sin(angle), 1);
           glVertex3f(cos(angle), sin(angle), 0);
           glNormal3f(cos(nextAngle), sin(nextAngle), 0);
           glVertex3f(cos(nextAngle), sin(nextAngle), 0);
           glVertex3f(cos(nextAngle), sin(nextAngle), 1);
       }
       glEnd();
   }
   std::vector<Triangle> SplitTriangle(std::vector<Triangle> &list)
   {
       std::vector<Triangle> output(4*list.size());
       for (unsigned int i = 0 ; i < list.size() ; i++)
       {
           double mid1[3], mid2[3], mid3[3];
           mid1[0] = (list[i].X[0]+list[i].X[1])/2;
           mid1[1] = (list[i].Y[0]+list[i].Y[1])/2;
           mid1[2] = (list[i].Z[0]+list[i].Z[1])/2;
           mid2[0] = (list[i].X[1]+list[i].X[2])/2;
           mid2[1] = (list[i].Y[1]+list[i].Y[2])/2;
           mid2[2] = (list[i].Z[1]+list[i].Z[2])/2;
           mid3[0] = (list[i].X[0]+list[i].X[2])/2;
           mid3[1] = (list[i].Y[0]+list[i].Y[2])/2;
           mid3[2] = (list[i].Z[0]+list[i].Z[2])/2;
           output[4*i+0].X[0] = list[i].X[0];
           output[4*i+0].Y[0] = list[i].Y[0];
           output[4*i+0].Z[0] = list[i].Z[0];
           output[4*i+0].X[1] = mid1[0];
           output[4*i+0].Y[1] = mid1[1];
           output[4*i+0].Z[1] = mid1[2];
           output[4*i+0].X[2] = mid3[0];
           output[4*i+0].Y[2] = mid3[1];
           output[4*i+0].Z[2] = mid3[2];
           output[4*i+1].X[0] = list[i].X[1];
           output[4*i+1].Y[0] = list[i].Y[1];
           output[4*i+1].Z[0] = list[i].Z[1];
           output[4*i+1].X[1] = mid2[0];
           output[4*i+1].Y[1] = mid2[1];
           output[4*i+1].Z[1] = mid2[2];
           output[4*i+1].X[2] = mid1[0];
           output[4*i+1].Y[2] = mid1[1];
           output[4*i+1].Z[2] = mid1[2];
           output[4*i+2].X[0] = list[i].X[2];
           output[4*i+2].Y[0] = list[i].Y[2];
           output[4*i+2].Z[0] = list[i].Z[2];
           output[4*i+2].X[1] = mid3[0];
           output[4*i+2].Y[1] = mid3[1];
           output[4*i+2].Z[1] = mid3[2];
           output[4*i+2].X[2] = mid2[0];
           output[4*i+2].Y[2] = mid2[1];
           output[4*i+2].Z[2] = mid2[2];
           output[4*i+3].X[0] = mid1[0];
           output[4*i+3].Y[0] = mid1[1];
           output[4*i+3].Z[0] = mid1[2];
           output[4*i+3].X[1] = mid2[0];
           output[4*i+3].Y[1] = mid2[1];
           output[4*i+3].Z[1] = mid2[2];
           output[4*i+3].X[2] = mid3[0];
           output[4*i+3].Y[2] = mid3[1];
           output[4*i+3].Z[2] = mid3[2];
       }
       return output;
   }
	void DrawSphere()
		{
		int recursionLevel = 3;
		Triangle t;
		t.X[0] = 1;
		t.Y[0] = 0;
		t.Z[0] = 0;
		t.X[1] = 0;
		t.Y[1] = 1;
		t.Z[1] = 0;
		t.X[2] = 0;
		t.Y[2] = 0;
		t.Z[2] = 1;
		std::vector<Triangle> list;
		list.push_back(t);
		
		for (int r = 0 ; r < recursionLevel ; r++)
		{
			list = SplitTriangle(list);
		}

		// really draw `
		for (int octent = 0 ; octent < 8 ; octent++)
		{
			glPushMatrix();
			glRotatef(90*(octent%4), 1, 0, 0);
			
			if (octent >= 4)
				glRotatef(180, 0, 0, 1);
			
			glBegin(GL_TRIANGLES);
			
			for (unsigned int i = 0 ; i < list.size() ; i++)
			{
			
				for (int j = 0 ; j < 3 ; j++)
				{
				double ptMag = sqrt(list[i].X[j]*list[i].X[j]+
				    list[i].Y[j]*list[i].Y[j]+
				    list[i].Z[j]*list[i].Z[j]);
					glNormal3f(list[i].X[j]/ptMag, list[i].Y[j]/ptMag, list[i].Z[j]/ptMag);
					glVertex3f(list[i].X[j]/ptMag, list[i].Y[j]/ptMag, list[i].Z[j]/ptMag);
				}
			
			}
			glEnd();
			glPopMatrix();
		}
	}


	
	void DrawCubeFace(void)
	{
		
		glPushMatrix();
		glBegin(GL_QUADS);
		
		// Draw Face
		//glTexCoord2f(0, 0);
		glVertex3f(0, 0, 0);
		//glTexCoord2f(1, 0);
		glVertex3f(1, 0, 0);
		//glTexCoord2f(1, 1);
		glVertex3f(1, 1, 0);
		//glTexCoord2f(0, 1);
		glVertex3f(0, 1, 0);
		
		glEnd();
		glPopMatrix();
		
	
	}
	
	void DrawCube(double scale)
	{
		// Draw Front face
		glPushMatrix();
		glScalef(scale, scale, scale);
		DrawCubeFace();
		// Draw opposing face
		glPushMatrix();
		glTranslatef(0, 0, 1);
		glRotatef(180, 1, 1, 0);
		DrawCubeFace();
		glPopMatrix();
		
		glRotatef(90, 2, 0, 0);
		// Draw Bottom face
		DrawCubeFace();
		// Draw opposing face
		glPushMatrix();
		
		glTranslatef(0, 0, -1);
		glRotatef(180, 1, 1, 0);
		DrawCubeFace();
		glPopMatrix();
		
		
		glRotatef(90, 0, 2, 0);
		// Draw right face
		DrawCubeFace();
		
		glPushMatrix();
		
		glTranslatef(0, 0, 1);
		glRotatef(180, 1, 1, 0);
		DrawCubeFace();
		glPopMatrix();
		
		
		glPopMatrix();
   		
	}

	
	void DrawRowOfCubes(int length, double scale, double transFactor)
	{	
		glPushMatrix();
		
		for(int i = 0; i < length; i++){
			DrawCube(scale);
			glTranslatef(scale + transFactor, 0, 0);
		}
		
		glPopMatrix();
	}
	
	void DrawQuarterDiamond(int length, double scale, double transFactor)
	{
		glPushMatrix();
		for(int i = length; i > 0; i--){
			glPushMatrix();
			for(int j = i; j > 0; j--){
				DrawRowOfCubes(j, scale, transFactor);
				glTranslatef(0, 0, scale);
			}
			glPopMatrix();
			glTranslatef(0, scale + transFactor, 0);
		}
		glPopMatrix();
	}
	

	

	
	
	double SineParameterize(int curFrame, int nFrames, int ramp)
	{
		int nNonRamp = nFrames-2*ramp;
		double height = 1./(nNonRamp + 4*ramp/M_PI);
		if (curFrame < ramp)
		{
			double factor = 2*height*ramp/M_PI;
			double eval = cos(M_PI/2*((double)curFrame)/ramp);
			return (1.-eval)*factor;
		}
		else if (curFrame > nFrames-ramp)
		{
			int amount_left = nFrames-curFrame;
			double factor = 2*height*ramp/M_PI;
			double eval =cos(M_PI/2*((double)amount_left/ramp));
			return 1. - (1-eval)*factor;
		}
		double amount_in_quad = ((double)curFrame-ramp);
		double quad_part = amount_in_quad*height;
		double curve_part = height*(2*ramp)/M_PI;
		return quad_part+curve_part;
	}
	

#endif