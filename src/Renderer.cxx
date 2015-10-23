/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Renderer.h"
#include "Image.h"
#include "FontAwesomeException.h"

#include <iostream>

Renderer::Renderer(bool debug, bool gracefulEmpty, bool missing, int padWidth, int padHeight) :
	debug_(debug),
	gracefulEmpty_(gracefulEmpty),
	missing_(missing),
	padWidth_(padWidth),
	padHeight_(padHeight)
{

}

Renderer::~Renderer() {

}

bool Renderer::debug() const {
	return debug_;
}

bool Renderer::gracefulEmpty() const {
	return gracefulEmpty_;
}


bool Renderer::missing() const {
	return missing_;
}


int Renderer::padWidth() const {
	return padWidth_;
}


int Renderer::padHeight() const {
	return padHeight_;
}
