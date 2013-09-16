/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#include "Image.h"

#include <cassert>
#include <algorithm>
#include <iostream>
#include <cstring>


Image::Image() : 
	data_(0), 
	bpp_(24), 
	width_(0), 
	height_(0), 
	format_(FORMAT_RGB) {
}

Image::Image(size_t w, size_t h, size_t b) : 
	bpp_(b),
	width_(w), 
	height_(h)
{
	size_t bytesPerPixel	= bpp_ / 8;
	size_t size = width_ * height_ * bytesPerPixel;
	if (bytesPerPixel == 3)
	{
		format_ = FORMAT_RGB;
	}
	else if (bytesPerPixel == 4)
	{
		format_ = FORMAT_RGBA;
	}
	data_ = new unsigned char [size];
	memset(data_, 0, size);
}

Image::Image(unsigned long len) : 
	bpp_(0),
	width_(0), 
	height_(0)
{
	data_ = new unsigned char [len];
	memset(data_, 0, len);
}


Image::~Image() {
	delete [] data_;
}

unsigned char * Image::data() {
	return data_;
}

size_t Image::bpp() const {
	return bpp_;
}

size_t Image::width() const {
	return width_;
}

size_t Image::height() const {
	return height_;
}

Image::ImageFormat Image::format() const {
	return format_;
}

void Image::bpp(size_t bits) {
	bpp_ = bits;
}

void Image::width(size_t w) {
	width_ = w;
}

void Image::height(size_t h) {
	height_ = h;
}

unsigned char & Image::operator[] (size_t i) {
	assert(i < (width_ * height_ * (bpp_ / 8)));
	assert(data_ != 0);
	return data_[i];
}

unsigned char Image::operator[] (size_t i) const {
	assert(i < (width_ * height_ * (bpp_ / 8)));
	assert(data_ != 0);
	return data_[i];
}


void Image::copy(const boost::shared_ptr<Image> & image, size_t x, size_t y, size_t width, size_t height) {
	assert (x < width);
	assert (y < height);
	size_t stride = bpp_ / 8;
	for (unsigned int row = 0; row < height; row++) {
		unsigned char * dest = data_ + x + std::min(y + row, height - 1) * stride;
		unsigned char * src = image->data() + row * stride;
		if (x + width <= width) {
			for (size_t col = 0; col < width; col++) {
				*dest++ = *src++;
			}
		}
		else {
			size_t limit = width - x;
			for (size_t col = 0; col < limit; col++) {
				*dest++ = *src++;
			}
			*src--;
			for (size_t col = limit; col < width; col++) {
				*dest++ = *src;
			}
		}
	}
}
