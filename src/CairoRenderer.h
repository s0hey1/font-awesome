/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once


#include "Renderer.h"

/**
 * Render a string of text into a raw image buffer using Cairo and HarfBuzz
 *
 */
class CairoRenderer : public Renderer {
	public:
		CairoRenderer(bool debug, bool gracefulEmpty, bool missing);

		boost::shared_ptr<Image> render(const Font & font, const Color & color, const std::wstring & text, const Color & emptyColor);
		void features(const std::vector<std::string> & features);

	private:
		std::vector<std::string> features_;
};