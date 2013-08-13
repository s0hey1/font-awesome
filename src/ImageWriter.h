/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include "Image.h"
#include "FilePointer.h"

#include <string>
#include <boost/shared_ptr.hpp>


/**
 * Generic image writer interface
 */
class ImageWriter {
	public:	
		ImageWriter();
		virtual ~ImageWriter();

		virtual bool write(FilePointer & file, const boost::shared_ptr<Image> & img);
};