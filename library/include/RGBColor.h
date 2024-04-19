#ifndef RGBCOLOR_H
#define RGBCOLOR_H

// Spikestream 0.3
#include "api.h" 

namespace spikestream {

	/*! A simple RGB color class. */
	class SPIKESTREAM_LIB_EXPORT RGBColor {
		public:
			//=======================  METHODS  ========================
			RGBColor();
			RGBColor(float red, float green, float blue);
			RGBColor(const RGBColor& rgbColor);
			RGBColor& operator=(const RGBColor& rhs);
			void set(float red, float green, float blue);

			//=======================  COLORS  =========================
			static RGBColor BLACK;

			//======================  VARIABLES  =======================
			float red;
			float green;
			float blue;
		};

}

#endif//RGBCOLOR_H

