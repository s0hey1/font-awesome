/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "ImageWriter.h"

/**
 * Write raw image data to PNG format
 */
class PNGWriter : public ImageWriter {
	public:
		PNGWriter();
		~PNGWriter();

		virtual bool write(FilePointer & file, const boost::shared_ptr<Image> & img);
};
