//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#ifndef __SETTINGS_H__
#define __SETTINGS_H__


class Settings {
		
	public:
		
		Settings();
		virtual ~Settings();
		
		void destroy();
		
		void update();
		
		void render(int num_triangles);

		// rendering technique
		enum {
			TECHNIQUE_ATLAS = 0,
			TECHNIQUE_ARRAY,
			TECHNIQUE_ARRAY_GS,
			TECHNIQUE_ARRAY_INST,
		};
		int getTechnique() const;
		
		// rendering shadows
		enum {
			SHADOWS_HARD = 0,
			SHADOWS_PCF,
			SHADOWS_VSM,
			SHADOWS_ESM,
		};
		int getShadows() const;
		
		// rendering flickering
		enum {
			FLICKERING_NONE = 0,
			FLICKERING_EXACT,
			FLICKERING_APPROXIMATE,
			NUM_FLICKERING_MODES
		};
		int getFlickering() const;
		
		// rendering tweaks
		int getFilter() const;
		int getShowSplits() const;
		int getShowShadows() const;

		int getFlicker() const { return flickering; }
		
		// pause
		int getPause() const;

		void setTechnique(int in) { technique = in;}
		void setFlickering(int in) { flickering = in;}
		
	private:
		
		void technique_clicked();
		void shadows_clicked();
		void flickering_clicked();
		void grab_clicked();
		void quit_clicked();
		
		int technique;
		int shadows;	
		int flickering;

	
};

#endif /* __SETTINGS_H__ */
