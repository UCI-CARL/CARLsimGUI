#ifndef POINT3D_H
#define POINT3D_H

//Qt includes
#include <QString>

// Spikestream 0.3
#include "api.h" 


namespace spikestream{

	/*! Information about a three-dimensional point. */
    class SPIKESTREAM_LIB_EXPORT Point3D {
		public:
			Point3D();
			Point3D(float xPos, float yPos, float zPos);
			Point3D(const Point3D& point);
			virtual ~Point3D();
			Point3D& operator=(const Point3D& point);
			bool operator==(const Point3D& point) const;
			bool operator!=(const Point3D& point) const;
			float distance(const Point3D& point) const;
			float getXPos() const { return xPos; }
			float getYPos() const { return yPos; }
			float getZPos() const { return zPos; }
			void setXPos(float xPos) { this->xPos = xPos; }
			void setYPos(float yPos) { this->yPos = yPos; }
			void setZPos(float zPos) { this->zPos = zPos; }
			QString toString() const;
			void translate(float dx, float dy, float dz);

		private:
			//=================  VARIABLES  ==================
			float xPos;
			float yPos;
			float zPos;
		};

}

#endif//POINT3D_H


