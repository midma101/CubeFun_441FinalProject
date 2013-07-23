/*=========================================================================

  Program:   Visualization Toolkit
  Module:    SpecularSpheres.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This examples demonstrates the effect of specular lighting.
//
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
#include "vtkJPEGReader.h"
#include "vtkImageData.h"

#include <vtkCommand.h>
#include <vtkRendererCollection.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkPolyDataReader.h>
#include <vtkPoints.h>
#include <vtkUnsignedCharArray.h>
#include <vtkFloatArray.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>

#include <queue>
#include "DrawShapes.h"
#include <vector>

#include <vtkInteractorStyleTrackballCamera.h>

#define SAMPLE_RATE (44100)

typedef struct
{
	float left_phase;
	float right_phase;
}   
paTestData;

static paTestData data;

PaStream *stream;



  vtkSmartPointer<vtkRenderer> ren3 =
    vtkSmartPointer<vtkRenderer>::New();
int timerId, timerColor = 0, sinExplosion = 0, sqrtExplosion = 0, circlesExplosion = 0;
int diRotate, circleNumber = 0;

float rotationSpeed = 0;
float rotationLeftRight = 1.0;
float rotationUpDown = 1.0;
float circleGrowth = 0;
bool pauseRender = false, explosion = false, reverseDirection = false, contraction = false;
bool tieDye = false, diagonalRotate = false, circlesOn = false;
bool firstCircleRender = true, circleContraction = false;;
double exFactor = 0 , circleFactor = 0;
std::queue<int> colorQueue;

   GLuint circlesList, wallsDisplayList;
   
     vtkSmartPointer<vtkRenderWindow> renWin =
    vtkSmartPointer<vtkRenderWindow>::New();

	void White(void) { glColor3f(128/255.0, 128/255.0, 128/255.0); };
	void Red(void) { glColor3f(226/255.0, 58/255.0, 58/255.0); };
	void Blue(void) { glColor3f(66/255.0, 46/255.0, 243/255.0); };
	void Green(void) { glColor3f(16/255.0, 210/255.0, 62/255.0); };
	void Orange(void) { glColor3f(235/255.0, 183/255.0, 40/255.0); };


class vtk441InteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
  
   bool redLightOn = false;
   bool greenLightOn = false;
   bool blueLightOn = false;
   GLfloat specularRed[4] = {1, 0, 0, .7};
   GLfloat specularGreen[4] = {0, 1, 0, .7};
   GLfloat specularBlue[4] = {0, 0, 1, .7};
   
   
   static vtk441InteractorStyle *New();
   vtk441InteractorStyle()
   {
      shouldPick;
   }
   virtual void OnChar()
   {
     vtkRenderWindowInteractor *rwi = this->Interactor;
     char pressedChar = rwi->GetKeyCode();
     if (pressedChar == 'p')
     {
         cerr << "Should pick!" << endl;
         shouldPick = true;
         if(pauseRender == true){
         	pauseRender = false;
         }else{
         	pauseRender = true;
         }
         
     
     // r will turn on red light
     }else if(pressedChar == '7')
     {
     	cerr << "red key" << endl;
     	
     	if(redLightOn == false){
     		GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
     		glLightfv(GL_LIGHT2, GL_POSITION, light_position);
     		glLightfv(GL_LIGHT2, GL_SPECULAR, specularRed);
     		glEnable(GL_LIGHT2);
     		
     		redLightOn = true;
     		
     	}else{
     		glDisable(GL_LIGHT2);
     		redLightOn = false;
     	}
     	
     	
	 
	 // green light 
     }else if(pressedChar == 'g')
     {
     	cerr << "explosion time biyatch" << endl;
     	
     	if(explosion == false){
     		explosion = true;
     	}else{
     		explosion = false;
     	}
			
     	
     	
	 
	 // green light 
     }else if(pressedChar == '8')
     {
     	cerr << "colorQueue1" << endl;
     	//glColor3ub(128,128,128);
     	int temp = colorQueue.front();
     	colorQueue.pop();
     	colorQueue.push(temp);
     	
	 
	 // blue light 
     }else if(pressedChar == '9')
     {
         if(tieDye == true){
         	tieDye = false;
         }else{
         	tieDye = true;
         }
		
	 
	 // speed 1 
     }else if(pressedChar == '1')
     {
     	cerr << "slow down key" << endl;
     	if(rotationSpeed > 1)
     		rotationSpeed -= 1;
     	else if(rotationSpeed <= 1 && rotationSpeed > 0)
     		rotationSpeed -=.1;
     	cout << "rotation factor is " << rotationSpeed << endl; 	
	
	
	 // speed 2 
     }else if(pressedChar == '2')
     {
     	cerr << "speed up key" << endl;
     	if(rotationSpeed < 35)
     		rotationSpeed += 1;
     		
     	cout << "rotation factor is " << rotationSpeed << endl;
	 
	 // Up Rotation
     }else if(pressedChar == '4')
     {
     	cerr << "reverse direction" << endl;
     	if(reverseDirection == false){
     		reverseDirection = true;
     	}else{
     		reverseDirection = false;
     	}
     	
     	
	 // Down rotation 
     }else if(pressedChar == 's')
     {
     	cerr << "down key" << endl;
     	
	 // Right rotation 
     }else if(pressedChar == 'd')
     {
     	cerr << "diagonal rotate" << endl;
 
      	if(diagonalRotate == false){
     		diagonalRotate = true;
     	}else{
     		diagonalRotate = false;
     	}    	
     	
	  
	 // Left rotation 
     }else if(pressedChar == 'c')
     {
     	cerr << "activate circles" << endl;
      	
      	if(circlesOn == false){
     		circlesOn = true;
     	}else{
     		circlesOn = false;
     	}    	     	
     	
	  
     }
     
     vtkInteractorStyleTrackballCamera::OnChar();
   };
   virtual void OnLeftButtonDown()
   {
      if (shouldPick)
      {
          /*
          vtkRenderWindowInteractor *rwi = this->Interactor;
          vtkRenderWindow *rw = rwi->GetRenderWindow();
          int *size = rw->GetSize();
          int x = this->Interactor->GetEventPosition()[0];
          int y = this->Interactor->GetEventPosition()[1];
          vtkRenderer *ren = rwi->FindPokedRenderer(x, y);
          double pos[3];
          pos[0] = 2.0*((double)x/(double)size[0])-1;
          pos[1] = 2.0*((double)y/(double)size[1])-1;
          pos[2] = ren->GetZ(x, y);
          cerr << "Picked on " << x << ", " << y << endl;
          cerr << " = " << pos[0] << ", " << pos[1] << ", " << pos[2] << endl;
          ren->ViewToWorld(pos[0], pos[1], pos[2]);
          cerr << " converted to " << pos[0] << ", " << pos[1] << ", " << pos[2] << endl;

          shouldPick = false;
          */
      }
      //vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
   };
   
   virtual void OnTimer()
   {
   		vtkInteractorStyleTrackballCamera::OnTimer();
   		renWin->Render();
   	
   	};
  private:
   bool shouldPick;
};
vtkStandardNewMacro(vtk441InteractorStyle);



	
	
	void DrawHalfDiamond(int length, double scale, double transFactor)
	{
		glPushMatrix();
		for(int i = 0; i < 4; i++){
			if(tieDye == true){
				if(i == 0){
					Green();
				}else if (i == 1){
					Red();
				}else if (i == 2){
					Blue();
				}else{
					Orange();
				}
			}
			glRotatef(90, 0, 10, 0);
			DrawQuarterDiamond(length, scale, transFactor);
		}
		glPopMatrix();
	}
	
	void DrawDiamond(int length, double scale, double transFactor)
	{
		
		glPushMatrix();
		DrawHalfDiamond(length, scale, transFactor);
		glRotatef(180, scale, 0, 0);
		DrawHalfDiamond(length, scale, transFactor);
		glPopMatrix();
	}


class vtk441Mapper : public vtkOpenGLPolyDataMapper
{
  protected:

   bool   initialized;

  public:
   vtk441Mapper()
   {
     initialized = false;
   }
    
   void
   RemoveVTKOpenGLStateSideEffects()
   {
     float Info[4] = { 0, 0, 0, 1 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Info);
     float ambient[4] = { 1,1, 1, 1.0 };
     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
     float diffuse[4] = { 1, 1, 1, 1.0 };
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
     float specular[4] = { 5, 1, 1, 1.0 };
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
   }


   void SetupLight(void)
   {
       glEnable(GL_LIGHTING);
       glEnable(GL_LIGHT0);
       GLfloat diffuse0[4] = { 0.6, 0.6, 0.6, 1 };
       GLfloat ambient0[4] = { 0.2, 0.2, 0.2, 1 };
       GLfloat specular0[4] = { 0.0, 0.0, 0.0, 1 };
       GLfloat pos0[4] = { 0, .707, 0.707, 0 };
       glLightfv(GL_LIGHT0, GL_POSITION, pos0);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
       glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);
       glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);
       glEnable(GL_LIGHT1);
       GLfloat pos1[4] = { .707, -.707, 0 };
       glLightfv(GL_LIGHT1, GL_POSITION, pos1);
       glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
       glLightfv(GL_LIGHT1, GL_AMBIENT, ambient0);
       glLightfv(GL_LIGHT1, GL_SPECULAR, specular0);
       glDisable(GL_LIGHT2);
       glDisable(GL_LIGHT3);
       glDisable(GL_LIGHT5);
       glDisable(GL_LIGHT6);
       glDisable(GL_LIGHT7);
   }
};

class vtk441MapperPart3 : public vtk441Mapper
{
 public:
   static vtk441MapperPart3 *New();
   
   GLuint displayList;
   bool   initialized;


   vtk441MapperPart3()
   {
     initialized = false;

   }


			
	
	


   	
	virtual void RenderPiece(vtkRenderer *ren, vtkActor *act)
	{
		/*
		vtkPNGReader *rdr = vtkPNGReader::New();
		rdr->SetFileName("cube_face.png");
		rdr->Update();
		vtkImageData *img = rdr->GetOutput();
		int dims[3];
		img->GetDimensions(dims);
		unsigned char *buffer = (unsigned char *) img->GetScalarPointer(0,0,0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dims[0], dims[1], 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		
		glEnable(GL_TEXTURE_2D);
		*/
		
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);
		glEnable(GL_BLEND);
		
	
		if(initialized == false){
			
			
			wallsDisplayList = glGenLists(1);
			glNewList(wallsDisplayList, GL_COMPILE_AND_EXECUTE);
			
   	   		

			glPushMatrix();
			glBegin(GL_QUADS);
				
				glDepthMask(GL_FALSE);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
							
				// Back Wall
				//glColor3b(0,0,0);
				glColor3ub(0, 0, 0);
				glVertex3f(-18,-18,-25);
				glVertex3f(18, -18, -25);
				glVertex3f(18, 18, -25);
				glVertex3f(-18, 18, -25);
				
				// Side Walls
				//glColor3ub(0,0,0);
				glVertex3f(-18,-18,50);
				glVertex3f(-18,-18,-50);
				glVertex3f(-18,18,-50);
				glVertex3f(-18,18,50);
				
				glVertex3f(18,-18,-50);
				glVertex3f(18,-18,50);
				glVertex3f(18,18,50);
				glVertex3f(18,18,-50);
				
				// Top and Bottom Walls
				//glColor3ub(0, 0, 0);
				glVertex3f(-18,18,-25);
				glVertex3f(18,18,-25);
				glVertex3f(18,18,50);
				glVertex3f(-18,18,50);
				
				glVertex3f(-18,-18,-25);
				glVertex3f(18,-18,-25);
				glVertex3f(18,-18,50);
				glVertex3f(-18,-18,50);
				
				
			glEnd();
			glPopMatrix();
			

			
			glEndList();
			
			
			
			RemoveVTKOpenGLStateSideEffects();
			SetupLight();

			//glDepthMask(GL_FALSE);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_DST_ALPHA);
		
			circlesList = glGenLists(1);
   	   		glNewList(circlesList, GL_COMPILE);
   	   		


			glMatrixMode(GL_MODELVIEW);
			
			glPushMatrix();
			DrawSphere();
			
			glPopMatrix();


			glEndList();
			
			colorQueue.push(1); colorQueue.push(2); colorQueue.push(3); 
			colorQueue.push(4); colorQueue.push(5);
			
			initialized = true;
			
		}else{
			
			if(pauseRender == true){
				return;
			}
			
			//glCallList(wallsDisplayList);
			
			glPushMatrix();

			if(explosion == true && contraction == false){
				exFactor += sqrt(sqrtExplosion);
				sqrtExplosion++;
				if(sqrtExplosion >= 40){
					contraction = true;
					//sqrtExplosion = 0;
					//exFactor = 0;
				}
			}else if(explosion == true && contraction == true){
				exFactor -= sqrt(sqrtExplosion);
				sqrtExplosion--;
				if(sqrtExplosion <= 5){
					contraction = false;
					sqrtExplosion = 0;
					exFactor = 0;
				}
			}
			
			
			glRotatef(timerId, 0, 1, 0);
			
			//if(diagonalRotate == true)
				glRotatef(diRotate, 0, 0, 1);
			
			
			switch(colorQueue.front()){
				case 1:
					White();
					break;
				case 2:
					Red();
					break;
				case 3:
					Green();
					break;
				case 4:
					Blue();
					break;
				case 5:
					Orange();
					break;
			}
				
				
			DrawDiamond(12, .55, exFactor/180);
			glPopMatrix();
			
			
			 
			if(reverseDirection == true){
				timerId -= 1*rotationSpeed;
				if(diagonalRotate == true){
					diRotate -= 1*rotationSpeed;
				}
			}else{
				timerId += 1*rotationSpeed;
				
				if(diagonalRotate == true){
					diRotate += 1*rotationSpeed;
				}
			}
			
			
			
			if(circlesOn == true){
				
				glPushMatrix();
				
				glTranslatef(-15, -13, 0);
				
				if(circleNumber % 4 == 1){
					glTranslatef(30, 26, 0);
				}else if(circleNumber % 4 == 2){
					
					glTranslatef(0, 26, 0);
				}else if(circleNumber % 4 == 3){
					glTranslatef(30, 0, 0);
				}
				
				
				
				
				if(circleContraction == false){
					circleFactor += sqrt(circleGrowth)/15.0;
					//circleFactor += sin(circleGrowth/(M_PI*180));
					circleGrowth++;
					if(circleGrowth >= 23){
						circleContraction = true;
					}
				}else if(circleContraction == true){
					circleFactor -= sqrt(circleGrowth)/15.0;
					//circleFactor -= sin(circleGrowth/(M_PI*180))*2.0;
					circleGrowth--;
					if(circleGrowth <= 5){
						circleContraction = false;
						circleGrowth = 0;
						circleFactor = 0;
						circleNumber++;
					}
				}
				
			switch(colorQueue.front()){
				case 1:
					Orange();
					break;
				case 2:
					Green();
					break;
				case 3:
					Red();
					break;
				case 4:
					White();
					break;
				case 5:
					Blue();
					break;
			}				
			
			
				//DrawSphere();
				
				
				glScalef(circleFactor, circleFactor, circleFactor);
				glCallList(circlesList);
				
				
				
				
				glPopMatrix();
			
			}
			
			
			
		}
		
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);
		glDisable(GL_BLEND);
		//glDisable(GL_TEXTURE_2D);
	}
};

vtkStandardNewMacro(vtk441MapperPart3);

/* This routine will be called by the PortAudio engine when audio is needed.
   It may called at interrupt level on some machines so don't do anything
   that could mess up the system like calling malloc() or free().
*/ 
// static int patestCallback( const void *inputBuffer, void *outputBuffer,
//                            unsigned long framesPerBuffer,
//                            const PaStreamCallbackTimeInfo* timeInfo,
//                            PaStreamCallbackFlags statusFlags,
//                            void *userData )
// {
//     /* Cast data passed through stream to our structure. */
//     paTestData *data = (paTestData*)userData; 
//     float *out = (float*)outputBuffer;
//     unsigned int i;
//     (void) inputBuffer; /* Prevent unused variable warning. */
    
    
//     for( i=0; i<framesPerBuffer; i++ )
//     {
        
//         //*out++ = data->left_phase;  // left 
//         //*out++ = data->right_phase;  // right 
//         /* Generate simple sawtooth phaser that ranges between -1.0 and 1.0. 
//         //data->left_phase += 0.01f;
//         /* When signal reaches top, drop back down. */
//         //if( data->left_phase >= 1.0f ) data->left_phase -= 2.0f;
//         /* higher pitch so we can distinguish left and right. */
//        // data->right_phase += 0.03f;
//         //if( data->right_phase >= 1.0f ) data->right_phase -= 2.0f;
        
//       //  cout << "data->left is " << *out << endl;
        
//     //}
    
//     PaError err = Pa_ReadStream(&stream, outputBuffer, framesPerBuffer);
    
    
//     return 0;
// }





int main()
{

	
	// PaError err = Pa_Initialize();
	// if(err != paNoError)
	// 	printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
		
		
 	
 //    /* Open an audio I/O stream. */
 //    err = Pa_OpenDefaultStream( &stream,
 //                                0,          /* no input channels */
 //                                2,          /* stereo output */
 //                                paFloat32,  /* 32 bit floating point output */
 //                                SAMPLE_RATE,
 //                                256,         frames per buffer, i.e. the number
 //                                                   of sample frames that PortAudio will
 //                                                   request from the callback. Many apps
 //                                                   may want to use
 //                                                   paFramesPerBufferUnspecified, which
 //                                                   tells PortAudio to pick the best,
 //                                                   possibly changing, buffer size.
 //                                patestCallback, /* this is your callback function */
 //                                &data ); /*This is a pointer that will be passed to
 //                                                   your callback*/
 //    if( err != paNoError ) 
	// 	printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
		
		
	// err = Pa_StartStream( stream );
  // Dummy input so VTK pipeline mojo is happy.
  //
  vtkSmartPointer<vtkSphereSource> sphere =
    vtkSmartPointer<vtkSphereSource>::New();
  sphere->SetThetaResolution(100);
  sphere->SetPhiResolution(50);

  vtkSmartPointer<vtk441MapperPart3> win3Mapper =
    vtkSmartPointer<vtk441MapperPart3>::New();
  win3Mapper->SetInputConnection(sphere->GetOutputPort());

  vtkSmartPointer<vtkActor> win3Actor =
    vtkSmartPointer<vtkActor>::New();
  win3Actor->SetMapper(win3Mapper);




  renWin->AddRenderer(ren3);
  ren3->SetViewport(0, 0, 1, 1);

  vtkSmartPointer<vtkRenderWindowInteractor> iren =
    vtkSmartPointer<vtkRenderWindowInteractor>::New();
  iren->SetRenderWindow(renWin);
  
	vtk441InteractorStyle *style = vtk441InteractorStyle::New();

  iren->SetInteractorStyle(style);
  	//style->StartTimer();

  // Add the actors to the renderer, set the background and size.
  //
  bool doWindow3 = true;
  if (doWindow3)
      ren3->AddActor(win3Actor);
  ren3->SetBackground(0, 0, 0);
  renWin->SetSize(1000, 1000);

  // Set up the lighting.
  //
  vtkSmartPointer<vtkLight> light =
    vtkSmartPointer<vtkLight>::New();
  light->SetFocalPoint(1.875,0.6125,0);
  light->SetPosition(0.875,1.6125,1);
  ren3->AddLight(light);

  ren3->GetActiveCamera()->SetFocalPoint(0,0,0);
  ren3->GetActiveCamera()->SetPosition(0,0,70);
  ren3->GetActiveCamera()->SetViewUp(0,1,0);
  ren3->GetActiveCamera()->SetClippingRange(20, 200);
  ren3->GetActiveCamera()->SetDistance(70);
  
  // This starts the event loop and invokes an initial render.
  //
  ((vtkInteractorStyle *)iren->GetInteractorStyle())->SetAutoAdjustCameraClippingRange(0);
  iren->Initialize();
  
  iren->CreateRepeatingTimer(30);
 
  
  iren->Start();
	
	
  // err = Pa_Terminate();
  // if(err != paNoError)
  // 	printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
  return EXIT_SUCCESS;
}




