#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>
#include <locale.h>
#include <png.h>
#include <zlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define TEXT_LENGTH_LIMIT 255
#define FREETYPE_DPI 100
#define PNG_PIXELSIZE 4
#define PNG_PIXELDEPTH 8
#define FONTAWESOME_VERSION_MAJOR 1
#define FONTAWESOME_VERSION_MINOR 0
#define FONTAWESOME_VERSION_PATCH 1

//Flags
#define FONTAWESOME_FLAG_HASERROR      0x1
#define FONTAWESOME_FLAG_EMPTY         0x2
#define FONTAWESOME_FLAG_VERBOSE       0x4
#define FONTAWESOME_FLAG_GRACEFULEMPTY 0x8

typedef struct{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} pixel_t;

typedef struct{
	pixel_t *pixels;
	size_t width;
	size_t height;
} bitmap_t;

static pixel_t * pixel_at (bitmap_t * bitmap, int x, int y){
	return bitmap->pixels + bitmap->width * y + x;
}

void draw_bitmap(bitmap_t * image, FT_Bitmap * bitmap, int x, int y){
	int i, j;
	int w, h;
	int v;
	pixel_t * pixel;
	w = bitmap->width;
	h = bitmap->rows;
	for (i=0; i < w; i++){
		for (j=0; j < h; j++){
			v = bitmap->buffer[j * w + i];
			if (v != 0){
				pixel = pixel_at(image, x + i, y + j);
				pixel->r = 0;
				pixel->g = 0;
				pixel->b = 0;
				pixel->a = MAX(pixel->a, v);
			}
		}
	}
}

int show_version(){
	printf("font-awesome version %d.%d.%d\n",
			FONTAWESOME_VERSION_MAJOR,
			FONTAWESOME_VERSION_MINOR,
			FONTAWESOME_VERSION_PATCH);
	return 0;
}

int show_help(){
	printf("usage: font-awesome [flags] [-f filename] [-s fontsize]\n");
	printf("\n");
	printf("font-awesome takes text through stdin and draws a png using the specified\n");
	printf("font and size.\n");
	printf("\n");
	printf("png output goes out through stdout. debug info goes out through stderr.\n");
	printf("\n");
	printf("available flags:\n");
	printf("  -v               more verbose output\n"); 
	printf("  -h | --help      show this help\n"); 
	printf("  --version        show version\n"); 
	printf("  --gracefulempty  output 1x1 empty png when image has 0 area\n"); 
	return 0;
}

int main(int argc, char** argv){

	char *argv_filename = NULL;
	int argv_fontsize = 32;

	wchar_t text[TEXT_LENGTH_LIMIT];
	wchar_t c;
	int text_length = 0;
	int i, j;
	int _l, _t;
	int minx, miny, maxx, maxy;
	int width, height;
	int offset;
	int flags = 0;

	FT_Library library;
	FT_Face face;
	FT_Vector pen;
	FT_GlyphSlot slot;
	bitmap_t image;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row;
	png_text png_infotext;
	pixel_t * pixel;

	setlocale(LC_ALL, "");

	for (i=1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (strcmp(argv[i], "--version") == 0){
				return show_version();
				break;
			}
			if (strcmp(argv[i], "--gracefulempty") == 0){
				flags |= FONTAWESOME_FLAG_GRACEFULEMPTY;
			}
			if ((strcmp(argv[i], "--help") == 0) |
					(strcmp(argv[i], "-h") == 0)){
				return show_help();
			}
			switch (argv[i][1]){
				case 'f':
					argv_filename = argv[++i];
					break;
				case 's':
					argv_fontsize = atoi(argv[++i]);
					break;
				case 'v':
					flags |= FONTAWESOME_FLAG_VERBOSE;
					break;
			}
		}
	}

	if (argv_filename == NULL){
		fprintf(stderr, "no file (use -f flag)\n");
		return 1;
	}

	if (flags & FONTAWESOME_FLAG_VERBOSE){
		fprintf(stderr, "wchar_t is %zu\n", sizeof(wchar_t));
		fprintf(stderr, "stdin codepoints are as follows:\n");
	}

	while(1){
		c = fgetwc(stdin);
		if (c == 10){ /* line feed */
			continue;
		}
		if (c == WEOF){
			break;
		}
		text[text_length] = c;
		if (flags & FONTAWESOME_FLAG_VERBOSE){
			fprintf(stderr, "%d: 0x%X (%lc)\n", text_length, (wint_t)c, (wint_t)c);
		}
		text_length++;
	}

	if (FT_Init_FreeType(&library)){
		fprintf(stderr, "error initializing freetype\n");
		flags |= FONTAWESOME_FLAG_HASERROR;
		goto GOTO_ERROR_INIT_FREETYPE;
	}

	if (FT_New_Face(library, argv_filename, 0, &face)){
		fprintf(stderr, "error loading font\n");
		flags |= FONTAWESOME_FLAG_HASERROR;
		goto GOTO_ERROR_NEW_FACE;
	}

	if (FT_Set_Char_Size(face, 0, argv_fontsize * 64, FREETYPE_DPI, FREETYPE_DPI)){
		fprintf(stderr, "error setting char_size\n");
		flags |= FONTAWESOME_FLAG_HASERROR;
		goto GOTO_ERROR_SET_CHAR_SIZE;
	}
	slot = face->glyph;

	pen.x = 0;
	pen.y = 0;

	minx = 0;
	miny = 0;
	maxx = 0;
	maxy = 0;
	for (i = 0; i < text_length; i++){
		FT_Set_Transform(face, NULL, &pen);
		FT_Load_Char(face, text[i], FT_LOAD_RENDER);
		_l = slot->bitmap_left;
		_t = 0 - slot->bitmap_top;
		minx = MIN(minx, _l);
		miny = MIN(miny, _t);
		maxx = MAX(maxx, _l + slot->bitmap.width);
		maxy = MAX(maxy, _t + slot->bitmap.rows);
		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}

	width = maxx - minx;
	height = maxy - miny;

	if ((width == 0) | (height == 0)){
		flags |= FONTAWESOME_FLAG_HASERROR;
		flags |= FONTAWESOME_FLAG_EMPTY;

		if (flags | FONTAWESOME_FLAG_GRACEFULEMPTY){
			if (flags | FONTAWESOME_FLAG_VERBOSE){
				fprintf(stderr, "Empty image, returning 1x1 empty png\n");
				goto GOTO_ERROR_FT;
			}
		}else{
			goto GOTO_ERROR_FT;
		}
	}

	pen.x = (0 - minx) * 64;
	pen.y = maxy * 64;

	image.width = (flags | FONTAWESOME_FLAG_EMPTY) ? 1: width;
	image.height = (flags | FONTAWESOME_FLAG_EMPTY) ? 1 :  height;
	image.pixels = calloc (sizeof(pixel_t), image.width * image.height);

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, stdout);

	png_set_IHDR(png_ptr, info_ptr, image.width, image.height,
		PNG_PIXELDEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_infotext.compression = PNG_TEXT_COMPRESSION_NONE;
	png_infotext.key = "Title";
	png_infotext.text = "Love, COLOURlovers";
	png_set_text(png_ptr, info_ptr, &png_infotext, 1);
	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);

	row = (png_bytep) malloc(sizeof(png_byte) * PNG_PIXELSIZE * image.width);
	if (flags | FONTAWESOME_FLAG_EMPTY){
		memset(row, 0, sizeof(row));
		png_write_row(png_ptr, row);
	}else{
		for (i = 0; i < text_length; i++){
			FT_Set_Transform(face, NULL, &pen);
			FT_Load_Char(face, text[i], FT_LOAD_RENDER);
			draw_bitmap(&image, &slot->bitmap, slot->bitmap_left, height - slot->bitmap_top);
			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
		}

		for (i=0; i<height; i++){
			memset(row, 0, sizeof(row));
			for (j=0; j<width; j++){
				offset = j * PNG_PIXELSIZE;
				pixel = pixel_at(&image, j, i);
				row[offset] = pixel->r;
				row[offset + 1] = pixel->g;
				row[offset + 2] = pixel->b;
				row[offset + 3] = pixel->a;
			}
			png_write_row(png_ptr, row);
		}
	}
	if (row != NULL) free(row);

	png_write_end(png_ptr, NULL);

	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);

	GOTO_ERROR_FT:
	GOTO_ERROR_SET_CHAR_SIZE:
	FT_Done_Face(face);
	GOTO_ERROR_NEW_FACE:
	FT_Done_FreeType(library);
	GOTO_ERROR_INIT_FREETYPE:

	return (flags & FONTAWESOME_FLAG_HASERROR) ? 1 : 0;
}

