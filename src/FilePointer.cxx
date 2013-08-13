/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */


#include "FilePointer.h"
#include "FontAwesomeException.h"

FilePointer::FilePointer(Direction dir, const std::string & filename) {
	if (filename.length() == 0) {
		closeable_ = false;
		if (dir == DIRECTION_OUT) {
			fp_ = stdout;
		}
		else if (dir == DIRECTION_IN) {
			fp_ = stdin;
		}
	}
	else {
		closeable_ = true;
		if ((fp_ = fopen(filename.c_str(), "wb")) == 0) {
			throw FontAwesomeException("Failed opening file for writing");
		}		
	}
}

FilePointer::~FilePointer() {
	if (closeable_) {
		fclose(fp_);
	}
}

FILE * FilePointer::ref() {
	return fp_;
}

void FilePointer::mode(const std::string & mode) {
	freopen(NULL, mode.c_str(), fp_);
}