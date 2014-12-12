//--------------------------------------------------------------------------------------
//
// Author:      QUALCOMM, Advanced Content Group - Adreno SDK
//
//               Copyright (c) 2013 QUALCOMM Technologies, Inc. 
//                         All Rights Reserved. 
//                      QUALCOMM Proprietary/GTDR
//--------------------------------------------------------------------------------------

#include "FrmPlatform.h"
#include "Settings.h"
Settings::Settings() {

	technique = TECHNIQUE_ARRAY;
	
	shadows = SHADOWS_HARD;
	
	flickering = FLICKERING_EXACT;
	
}

Settings::~Settings() {
	

}

/*
 */
void Settings::destroy() {

}

/*
 */
void Settings::update() {
	
	
}

void Settings::render(int num_triangles) {
	
	/*App *app = App::get();
	
	fps_l->setText(String::format("FPS: %.0f\nMTPS: %.0f",app->getFps(),num_triangles * app->getFps() / 1000000.0f));
	
	gui->enable(app->getWidth(),app->getHeight());
	gui->render(app->getMouseX(),app->getMouseY(),app->getMouseButton(),0);
	gui->disable();*/
}

/*
 */


/*
 */
int Settings::getTechnique() const {
	return technique;
}

int Settings::getShadows() const {
	return shadows;
}

int Settings::getFlickering() const {
	return flickering;
}
