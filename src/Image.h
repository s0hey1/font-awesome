/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <boost/shared_ptr.hpp>

/**
 * Raw image data container
 */
class Image {
	public:
		/**
		 * The format of the image (the number of channels)
		 */
		typedef enum {
			FORMAT_RGB = 3,
			FORMAT_RGBA = 4
		} ImageFormat;

		Image();
		/**
		 * Constructor
		 * @param w image width
		 * @param h image height
		 * @param b bits per pixel
		 */
		Image(size_t w, size_t h, size_t b);
		/**
		 * Constructor
		 * @param len length of the image data in bytes
		 */
		Image(unsigned long len);
		virtual ~Image();

		/**
		 * Get the image data
		 * @return a pointer to the image data
		 */
		unsigned char * data();
		/**
		 * Get the number of bits per pixel in the image
		 * @return number of bits per pixel
		 */
		size_t bpp() const;
		/**
		 * Get the image width
		 * @return the image width
		 */
		size_t width() const;
		/**
		 * Get the image height
		 * @return the image height
		 */
		size_t height() const;
		/**
		 * Set the number of bits per pixel in the image
		 * @param bits the number of bits per pixel
		 */
		void bpp(size_t bits);
		/**
		 * Set the width of the image
		 * @param w the image width
		 */
		void width(size_t w);
		/**
		 * Set the height of the image
		 * @param h the image height
		 */
		void height(size_t h);

		ImageFormat format() const;

		unsigned char & operator[] (size_t i);
		unsigned char operator[] (size_t i) const;

		/**
		 * Copy a portion of another image into this image
		 */
		void copy(const boost::shared_ptr<Image> & image, size_t x, size_t y, size_t width, size_t height);

	private:
		unsigned char * data_;
		size_t 			bpp_;
		size_t 			width_;
		size_t 			height_;
		ImageFormat 	format_;
};
