/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Renderer.h"
#include "Image.h"
#include "FontAwesomeException.h"

#include <iostream>

Renderer::Renderer(bool debug, bool gracefulEmpty, bool missing) :
	debug_(debug),
	gracefulEmpty_(gracefulEmpty),
	missing_(missing)
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