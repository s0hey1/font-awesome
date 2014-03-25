/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "ImageWriter.h"

/**
 * Write raw image data to PNG format
 */
class ANSIWriter : public ImageWriter {
	public:
		ANSIWriter();
		~ANSIWriter();

		virtual bool write(FilePointer & file, const boost::shared_ptr<Image> & img, bool flip = false);
};
