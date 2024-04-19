#ifndef NETWORKVIEWER_H
#define NETWORKVIEWER_H

// SpikeStream 0.3
#include "api.h"

//SpikeStream includes
#include "Box.h"

//Qt includes
#include <qgl.h>
#include <QHash>

//class GLUquadricObj; // patch
//#include "C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\gl\GLU.h"
#include "gl\GLU.h"

//#include <QOpenGLWidget>

 
namespace spikestream {

	/*! Draws the current network in 3D using OpenGL. */
	class SPIKESTREAM_APP_LIB_EXPORT NetworkViewer : public QGLWidget  {
			// obsolete https://doc.qt.io/qt-5/qglwidget.html
			// QOpenGLWidget
			// https://doc.qt.io/qt-5/qtgui-openglwindow-example.html
		Q_OBJECT

		public:
			NetworkViewer(QWidget* parent);
			~NetworkViewer();


		public slots:
			void refresh();
			void reset();


		protected:
			//Methods inherited from QGLWidget
			void initializeGL();
			void paintGL();
			void resizeGL(int width, int height);

			//Methods inherited from QWidget
			void mouseDoubleClickEvent (QMouseEvent * event );
			void mousePressEvent(QMouseEvent* event); // Context menu
			void mouseReleaseEvent(QMouseEvent* event); // Context menu


		private slots:
			void moveBackward();
			void moveForward();
			void moveUp();
			void moveDown();
			void moveLeft();
			void moveRight();
			void refreshConnections();
			void refreshNeurons();
			void refreshNeuronGroups();
			void resetView();
			void rotateUp();
			void rotateDown();
			void rotateLeft();
			void rotateRight();
			void viewClippingVolume_Horizontal(Box& clipVolume);
			void viewClippingVolume_Vertical(Box& clipVolume);


		private:
			//=======================  VARIABLES  ========================
			/*! Reference to the display list for the axes.*/
			GLuint axesDisplayList;

			/*! Determines whether the stored display list should be used or a new one generated.*/
			bool useAxesDisplayList;


			/*! Reference to the display list for the neuron groups.*/
			GLuint neuronGroupsDisplayList;

			/*! Controls whether the neuron groups display list should be used or a new one generated. */
			GLuint useNeuronGroupsDisplayList;


			/*! Reference to the display list for the neurons.*/
			GLuint neuronsDisplayList;

			/*! Controls whether the neuron display list should be used or a new one generated. */
			GLuint useNeuronsDisplayList;

			/*! Reference to the display list for the connections.*/
			GLuint connectionsDisplayList;

			/*! Controls whether the connections display list should be used or a new one generated. */
			GLuint useConnectionsDisplayList;

			/*! Reference to the display list for drawing spheres. */
			GLuint sphereDisplayList;

			/*! Cancels render.*/
			bool cancelRender;

			/*! During full render, do not want to start render or resize, so record
			whether paintGL() or resizeGL() have been called and then call them
			again at the end of the render. newTempScreen width and height are the
			latest values of the screen width and height if resizeGL() has been called. */
			bool paintGLSkipped;

			/*! During render, do not want to start render or resize, so record
			whether paintGL() or resizeGL() have been called and then call them
			again at the end of the render. newTempScreen width and height are the
			latest values of the screen width and height if resizeGL() has been called. */
			bool resizeGLSkipped;

			/*! Angle in DEGREES for OpenGL perspective. Controls how much of scene is clipped.*/
			GLfloat perspective_angle;

			/*! For OpenGL perspective. Controls how much of scene is clipped.*/
			GLfloat perspective_near;

			/*! For OpenGL perspective. Controls how much of scene is clipped.*/
			GLfloat perspective_far;

			/*! Amount that scene is rotated around X axis.*/
			GLfloat sceneRotateX;

			/*! Amount that scene is rotated around Z axis.*/
			GLfloat sceneRotateZ;

			/*! CameraMatrix holds the location and coordinate system of the camera.*/
			GLfloat cameraMatrix [16];

			/*! Used for rotating camera matrix.*/
			GLfloat rotationMatrix [16];

			/*! Default view in which all neuron groups should be visible.*/
			Box defaultClippingVol;

			/*! Screen width is stored here when resizeGL() is cancelled because
			render is in progress */
			int newTempScreenWidth;

			/*! Screen height is stored here when resizeGL() is cancelled because
			render is in progress */
			int newTempScreenHeight;

			/*! Object used to draw a sphere */
			GLUquadricObj* gluSphereObj;

			/*! Object used to draw a cone */
			GLUquadricObj* gluConeObj;

			/*! Factor by which weight is multiplied to obtain thickness */
			float weightRadiusFactor;

			/*! Minimum thickness of a connection */
			float minimumConnectionRadius;

			/*! Quality of the cones used to render a weighted connection */
			unsigned connectionQuality;

			/*! Map holding IDs of all neurons with a connection to them.
				Used when connection mode is enabled */
			//QHash<unsigned, float> connectedNeuronMap;
			// might not to be neccessary to extend to delays as only weights seems to be used
			QHash<unsigned, QPair<float,float>> connectedNeuronMap;

			//======================  METHODS  ===========================
			void checkOpenGLErrors();
			void disableFullRender();
			void drawAxes();
			void drawConnections();
			void drawNeurons();
			void drawNeuronGroups();  
			void drawSphere(float xPos, float yPos, float zPos);
			void drawWeightedConnection(float x1, float y1, float z1, float x2, float y2, float z2, float weight, unsigned weightRenderMode);
			void drawDelayedConnection(float x1, float y1, float z1, float x2, float y2, float z2, float weight);
			void fillRotationMatrix(float angle, float x, float y, float z);
			unsigned int getSelectedNeuron(GLuint selectBuffer[], int hitCount, int bufferSize);
			void initialiseCameraParameters();
			void initialiseFullRender();
			void loadDefaultClippingVolume();
			void positionCamera();
			void rotateVector(GLfloat x, GLfloat y, GLfloat z, GLfloat result[]);
			void rotateXAxis(float angle);
			void rotateZAxis(float angle);
			void setZoomLevel();
			void zoomDefaultView();
			void zoomAboveNeuronGroup(unsigned int neuronGroupID);
			void zoomToNeuronGroup(unsigned int neuronGroupID);
	};

}

#endif//NETWORKVIEWER_H
