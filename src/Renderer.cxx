/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Renderer.h"
#include "Image.h"

Renderer::Renderer(bool debug) :
	debug_(debug) {

}

Renderer::~Renderer() {

}

boost::shared_ptr<Image> Renderer::render(const Font & font, const Color & color, const std::wstring & text) {
	Font::Range range = font.size(text);
	boost::shared_ptr<Image> image;
	Font::Vector pen;

	if (debug_) {
		std::cout << "Font Size [" << range.size_.first << "x" << range.size_.second << "]" << std::endl;
	}

	image.reset(new Image(range.size_.first, range.size_.second, 32));

	size_t penDPI = font.penDPI();
	pen.first = (0 - range.min_.first) * penDPI;
	pen.second = range.max_.second * penDPI;

	size_t length = text.length();
	for (size_t index = 0; index < length; ++index) {
		Font::Glyph glyph = font.glyph(text[index], pen);

		// if glyph is empty & fix missing is enabled then draw empty glyph
		if (glyph.empty_) {

		}
		else {
			blit(image, glyph, color);
		}

		pen.first += glyph.advance_.first;
		pen.second += glyph.advance_.second;
	}
	return image;
}

void Renderer::blit(const boost::shared_ptr<Image> & image, const Font::Glyph & glyph, const Color & color) {
	size_t width  		= glyph.size_.first;
	size_t height 		= glyph.size_.second;
	size_t x 	  		= glyph.position_.first;
	size_t canvasWidth  = image->width();
	size_t canvasHeight = image->height();
	size_t y 			= canvasHeight - glyph.position_.second;
	const size_t bpp 	= 4;
	size_t pixel;
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			if (glyph.bitmap_[j * width + i] != 0) {
				// x = x + i; y = y + j
				// ((y + j) * width * 4) + ((x + i) * 4);
				pixel = ((y + j) * width * bpp) + ((x + i) * bpp);
				(*image)[pixel] 	 = color.red();
				(*image)[pixel + 1]  = color.green();
				(*image)[pixel + 2]  = color.blue();
				(*image)[pixel + 3]  = color.alpha();
			}
		}
	}
}

