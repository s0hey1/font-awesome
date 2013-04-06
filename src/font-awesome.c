#include <stdio.h>
#include <stdlib.h>
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
#define FREETYPE_PEN_DPI 64
#define PNG_PIXELSIZE 4
#define PNG_PIXELDEPTH 8
#define FONTAWESOME_VERSION_MAJOR 1
#define FONTAWESOME_VERSION_MINOR 3
#define FONTAWESOME_VERSION_PATCH 0

//Flags
#define FONTAWESOME_FLAG_HASERROR      0x1
#define FONTAWESOME_FLAG_EMPTY         0x2
#define FONTAWESOME_FLAG_VERBOSE       0x4
#define FONTAWESOME_FLAG_GRACEFULEMPTY 0x8
#define FONTAWESOME_FLAG_AS_CODEPOINTS 0x10
#define FONTAWESOME_FLAG_FIX_MISSING_MISSING 0x20

//Globals
int LimitWidth = 0;
int LimitHeight = 0;

static void draw_pixel(png_byte * pixel, int r, int g, int b, int a){
	pixel[0] = r;
	pixel[1] = g;
	pixel[2] = b;
	pixel[3] = a;
}
static png_byte * pixel_at (png_bytep * rows, int x, int y){
	if (x >= LimitWidth || y >= LimitHeight){
		x = 0;
		y = 0;
	}
	png_byte * row;
	row = rows[y];
	return &row[x * PNG_PIXELSIZE];
}

struct rgba {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct rgba hex_to_rgba(char * hex_string) {
	unsigned long color = strtol(hex_string, NULL, 16);
	struct rgba rgba_color;

	rgba_color.r = (color >> 24) & 0xFF;
	rgba_color.g = (color >> 16) & 0xFF;
	rgba_color.b = (color >> 8) & 0xFF;
	rgba_color.a = (color) & 0xFF;

	return rgba_color;
}

int check_hex_string(char * hex_string) {
	if (strlen(hex_string) != 8) return 1;

	int i, c;
	for (i = 0; i < 8; i++) {
		c = hex_string[i];

		if (!(
			(c >= 48 && c <= 57) // 0-9
			||
			(c >= 65 && c <= 70) // A-F
			||
			(c >= 97 && c <= 102) // a-f
		)) return 1;
	}

	return 0;
}

void blit_bitmap(png_bytep * rows, FT_Bitmap * bitmap, int x, int y, struct rgba text_color){
	int i, j;
	int w, h;
	int v;
	png_byte * pixel;
	w = bitmap->width;
	h = bitmap->rows;
	for (i=0; i < w; i++){
		for (j=0; j < h; j++){
			v = bitmap->buffer[j * w + i];
			if (v != 0){
				pixel = pixel_at(rows, x + i, y + j);
				draw_pixel(pixel, text_color.r, text_color.g, text_color.b, MIN(text_color.a, v));
			}
		}
	}
}

void draw_empty_glyph(png_bytep * rows, int x, int y, int w, int h, struct rgba text_color){
	int i;
	h--;
	w--;
	if (h > 4){
		y += 2;
		h -= 4;
	}
	if (w > 4){
		x += 2;
		w -= 4;
	}
	png_byte * pixel;
	for (i=0; i < h; i++){
		pixel = pixel_at(rows, x, y + i);
		draw_pixel(pixel, text_color.r, text_color.g, text_color.b, text_color.a);
		pixel = pixel_at(rows, x + w, y + i);
		draw_pixel(pixel, text_color.r, text_color.g, text_color.b, text_color.a);
	}
	for (i=0; i < w; i++){
		pixel = pixel_at(rows, x + i, y);
		draw_pixel(pixel, text_color.r, text_color.g, text_color.b, text_color.a);
		pixel = pixel_at(rows, x + i, y + h);
		draw_pixel(pixel, text_color.r, text_color.g, text_color.b, text_color.a);
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
	printf("  --fix0glyph      draw box if glyph 0 (missing glyph) has no substance\n");
	printf("  --text-color     text color (in RGBA hex)\n");
	return 0;
}

int main(int argc, char** argv){

	char *argv_filename = NULL;
	int argv_fontsize = 32;

	char buffer[256];
	wchar_t text[TEXT_LENGTH_LIMIT];
	wchar_t c;
	int text_length = 0;
	int i;
	int _l, _t;
	int minx, miny, maxx, maxy;
	int width, height;
	int flags = 0;
	char *argv_text_color = NULL;

	struct rgba text_color;
	text_color.r = 0x00;
	text_color.g = 0x00;
	text_color.b = 0x00;
	text_color.a = 0xFF;

	FT_UInt	index;

	FT_Library library;
	FT_Face face;
	FT_Vector pen;
	FT_GlyphSlot slot;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep * rows;
	png_text png_infotext;

	setlocale(LC_ALL, "");
	freopen(NULL, "rb", stdin);

	for (i=1; i < argc; i++){
		if (argv[i][0] == '-'){
			if (strcmp(argv[i], "--version") == 0){
				return show_version();
				break;
			}
			if (strcmp(argv[i], "--gracefulempty") == 0){
				flags |= FONTAWESOME_FLAG_GRACEFULEMPTY;
			}
			if (strcmp(argv[i], "--as-codepoints") == 0){
				flags |= FONTAWESOME_FLAG_AS_CODEPOINTS;
			}
			if (strcmp(argv[i], "--fix0glyph") == 0){
				flags |= FONTAWESOME_FLAG_FIX_MISSING_MISSING;
			}
			if ((strcmp(argv[i], "--help") == 0) |
					(strcmp(argv[i], "-h") == 0)){
				return show_help();
			}
			if (strcmp(argv[i], "--text-color") == 0){
				argv_text_color = argv[++i];
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
		fprintf(stderr, "int is %zu\n", sizeof(int));
		fprintf(stderr, "png_bytep is %zu\n", sizeof(png_bytep));
		fprintf(stderr, "png_byte is %zu\n", sizeof(png_byte));
		fprintf(stderr, "stdin codepoints are as follows:\n");
	}

	if (argv_text_color != NULL) {
		if (check_hex_string(argv_text_color)) {
			fprintf(stderr, "Invalid --text-color: %s\n", argv_text_color);
			return 1;
		}

		text_color = hex_to_rgba(argv_text_color);
	}

	while(1){
		if (flags & FONTAWESOME_FLAG_AS_CODEPOINTS){
			// Take input as series of long integars
			if (fscanf(stdin, "%s", buffer) != 1){
				break;
			}
			c = atoi(buffer);
		}else{
			// Take input from stdin as in (locale-aware)
			c = fgetwc(stdin);
		}
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

	if (FT_Set_Char_Size(face, 0, argv_fontsize * FREETYPE_PEN_DPI, FREETYPE_DPI, FREETYPE_DPI)){
		fprintf(stderr, "error setting char_size\n");
		flags |= FONTAWESOME_FLAG_HASERROR;
		goto GOTO_ERROR_SET_CHAR_SIZE;
	}
	slot = face->glyph;

	pen.x = 0;
	pen.y = 0;

	// Examine missing glyph (glyph 0)
	if (flags & FONTAWESOME_FLAG_FIX_MISSING_MISSING){
		FT_Load_Glyph(face, 0, FT_LOAD_RENDER);
		_t = 0 + slot->bitmap_top;
		if (_t == 0){
			if (flags & FONTAWESOME_FLAG_VERBOSE){
				fprintf(stderr, "Missing glyph has no substance. Will hack in box.\n");
			}
		} else {
			if (flags & FONTAWESOME_FLAG_VERBOSE){
				fprintf(stderr, "Missing glyph exists. Unsetting flag.\n");
			}
			flags &= ~FONTAWESOME_FLAG_FIX_MISSING_MISSING;
		}
	}

	// Measure glyphs first, to see how big of a canvas we need
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
	maxx = MAX(maxx, pen.x/FREETYPE_PEN_DPI);
	maxy = MAX(maxy, pen.y/FREETYPE_PEN_DPI);

	width = maxx - minx;
	height = maxy - miny;

	if ((width == 0) | (height == 0)){
		flags |= FONTAWESOME_FLAG_HASERROR;
		flags |= FONTAWESOME_FLAG_EMPTY;

		if (flags & FONTAWESOME_FLAG_GRACEFULEMPTY){
			if (flags & FONTAWESOME_FLAG_VERBOSE){
				fprintf(stderr, "Empty image, returning 1x1 empty png\n");
			}
		}else{
			goto GOTO_ERROR_FT;
		}
	}

	// Glyph measurement is done
	pen.x = (0 - minx) * FREETYPE_PEN_DPI;
	pen.y = maxy * FREETYPE_PEN_DPI;

	width = (flags & FONTAWESOME_FLAG_EMPTY) ? 1: width;
	height = (flags & FONTAWESOME_FLAG_EMPTY) ? 1 : height;
	LimitWidth = width;
	LimitHeight = height;

	// Create image datastructure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	png_init_io(png_ptr, stdout);

	png_set_IHDR(png_ptr, info_ptr, width, height,
		PNG_PIXELDEPTH, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_infotext.compression = PNG_TEXT_COMPRESSION_NONE;
	png_infotext.key = "Title";
	png_infotext.text = "Creative Market";
	png_set_text(png_ptr, info_ptr, &png_infotext, 1);
	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);

	rows = (png_bytep *) malloc(sizeof(png_bytep) * height);
	for (i = 0; i < height; i++){
		rows[i] = (png_byte*) calloc(PNG_PIXELSIZE, width);
	}

	if (flags & FONTAWESOME_FLAG_EMPTY){
		// pass
	}else{
		// Actual drawing happens here
		for (i = 0; i < text_length; i++){
			index = FT_Get_Char_Index(face, text[i]);
			FT_Set_Transform(face, NULL, &pen);
			FT_Load_Char(face, text[i], FT_LOAD_RENDER);
			if ((index == 0) && (flags & FONTAWESOME_FLAG_FIX_MISSING_MISSING)) {
				draw_empty_glyph(rows, (int)pen.x/FREETYPE_PEN_DPI, 0, (int)slot->advance.x/FREETYPE_PEN_DPI, height, text_color);
			} else {
				blit_bitmap(rows, &slot->bitmap, slot->bitmap_left, height - slot->bitmap_top, text_color);
			}
			pen.x += slot->advance.x;
			pen.y += slot->advance.y;
		}
	}
	for (i = 0; i < height; i++){
		png_write_row(png_ptr, rows[i]);
		free(rows[i]);
	}

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