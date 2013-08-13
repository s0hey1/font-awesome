/**
 * font-awesome (c) creativemarket.com
 * Author: Josh Farr <josh@creativemarket.com>
 */

#pragma once

#include <string>
#include <cstdio>

/**
 * Simple representation of a file
 * This is a simple wrapper for 'FILE *' that makes it easy for us to generically write out to either stdout or a regular file
 */
class FilePointer {
	public:
		typedef enum Direction {
			DIRECTION_IN,
			DIRECTION_OUT
		} Direction;

		FilePointer(Direction dir, const std::string & filename = "");
		~FilePointer();

		FILE * ref();
		void mode(const std::string & mode);

	private:
		bool closeable_;
		FILE * fp_;
};