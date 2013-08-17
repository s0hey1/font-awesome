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

boost::shared_ptr<Image> Renderer::render(const Font & font, const Color & color, const std::wstring & text) {
	Font::Range range = font.size(text);
	boost::shared_ptr<Image> image;
	bool emptyImage = false;

	if (debug_) {
		std::cout << "Font Size [" << range.size_.first << "x" << range.size_.second << "]" << std::endl;
	}

	// handle empty images
	if (range.size_.first == 0 || range.size_.second == 0) {
		if (!gracefulEmpty_) {
			throw FontAwesomeException("Empty font data!");
		}
		if (debug_) {
			std::cout << "Empty image, returning 1x1 empty png" << std::endl;
		}
		range.size_.first = 1;
		range.size_.second = 1;
		emptyImage = true;
	}

	image.reset(new Image(range.size_.first, range.size_.second, 32));

	if (emptyImage) {
		return image;
	}

	size_t penDPI = font.penDPI();
	Font::Vector pen;
	pen.first = (0 - range.min_.first) * penDPI;
	pen.second = range.max_.second * penDPI;

	size_t length = text.length();
	if (debug_) {
		std::cout << "rendering [" << length << "] characters to image [" << image->width() << "x" << image->height() << "]" << std::endl;
	}
	for (size_t index = 0; index < length; ++index) {
		Font::Glyph glyph = font.glyph(text[index], pen);

		// if glyph is empty & fix missing is enabled then draw empty glyph
		if (glyph.empty_ && missing_) {
			if (debug_) {
				std::cout << "empty glyph [" << text[index] << "] at index [" << index << "]" << std::endl;
			}
			drawRect(image, pen.first / font.penDPI(), 0, glyph.advance_.first / font.penDPI(), image->height(), color);
		}
		else {
			blit(image, glyph, color);
		}

		pen.first += glyph.advance_.first;
		pen.second += glyph.advance_.second;
	}
	return image;
}

void Renderer::drawPoint(const boost::shared_ptr<Image> & image, size_t pixel, const Color & color, unsigned char alpha) {
		(*image)[pixel] 	 = color.red();
		(*image)[pixel + 1]  = color.green();
		(*image)[pixel + 2]  = color.blue();
		(*image)[pixel + 3]  = alpha;
}

void Renderer::drawRect(const boost::shared_ptr<Image> & image, size_t x, size_t y, size_t width, size_t height, const Color & color) {
	size_t index;
	size_t pixel;
	size_t canvasWidth 	= image->width();
	const size_t bpp 	= 4;
	width--;
	height--;
	if (height > 4) {
		y += 2;
		height -= 4;
	}
	if (width > 4) {
		x += 2;
		width -= 4;
	}
	for (index = 0; index < height; index++) {
		pixel = ((y + index) * canvasWidth * bpp) + (x * bpp);
		drawPoint(image, pixel, color, color.alpha());

		pixel = ((y + index) * canvasWidth * bpp) + ((x + width) * bpp);
		drawPoint(image, pixel, color, color.alpha());
	}
	for (index = 0; index < width; index++) {
		pixel = (y * canvasWidth * bpp) + ((x + index) * bpp);
		drawPoint(image, pixel, color, color.alpha());

		pixel = ((y + height) * canvasWidth * bpp) + ((x + index) * bpp);
		drawPoint(image, pixel, color, color.alpha());
	}
}

void Renderer::blit(const boost::shared_ptr<Image> & image, const Font::Glyph & glyph, const Color & color) {
	size_t width  		= glyph.size_.first;
	size_t height 		= glyph.size_.second;
	size_t canvasWidth  = image->width();
	size_t canvasHeight = image->height();
	size_t x 	  		= glyph.position_.first;
	size_t y 			= canvasHeight - glyph.position_.second;
	const size_t bpp 	= 4;
	size_t pixel;
	size_t flipY;
	unsigned char value;
	unsigned char alpha;
	unsigned char colorAlpha = color.alpha();
	if (debug_) {
		std::cout << "glyph size [" << width << "x" << height << "] @ [" << x << "x" << y << "]" << std::endl;
	}
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height; j++) {
			value = glyph.bitmap_[j * width + i];
			if (value != 0) {
				flipY = canvasHeight - j - 1 - y; // freetype rows are backwards so flip them around
				pixel = ((flipY) * canvasWidth * bpp) + ((x + i) * bpp);
				alpha = std::min(colorAlpha, value);
				drawPoint(image, pixel, color, alpha);
			}
		}
	}
}

