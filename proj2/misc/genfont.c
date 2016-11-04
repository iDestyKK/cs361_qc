/*
 * Font Generator
 *
 * Description:
 *     Generates a font to use with graph generation. The format is simple
 *     as it contains the font width, height, and characters. Each character
 *     is stored in a map, and is then used to print out the data.
 * 
 * Author:
 *     Clara Van Nguyen
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/handy/types.h"
#include "lib/handy/stream.h"
#include "lib/handy/cnds/cn_vec.h"
#include "lib/PPM_Edit/ppm_edit.h" 

const cn_uint WIDTH  = 5;
const cn_uint HEIGHT = 5;

typedef struct cnb_char {
	char     char_val;
	cn_byte *pixel[5][5];
} CNB_CHAR;

char* cn_fstream_proceed(CN_FSTREAM stream) {
	cn_fstream_next(stream);
	return cn_fstream_get(stream);
}

void read_font(char* fname, CN_VEC vec_chars) {
	FILE *fp = fopen(fname, "r");
	
	if (fp == NULL)
		return; //You failed
	
	CN_FSTREAM fs = cn_fstream_init(fp);
	
	cn_uint width  = atoi(cn_fstream_proceed(fs)),
	        height = atoi(cn_fstream_proceed(fs)),
			a, b;

	while (cn_fstream_get(fs) != NULL) {
		CNB_CHAR _char;
		//Read in the character
		cn_fstream_next(fs);
		if (cn_fstream_get(fs) == NULL)
			break;
		_char.char_val = cn_fstream_get(fs)[0];
		
		//Read in the byte data
		for (b = 0; b < HEIGHT; b++) {
			for (a = 0; a < WIDTH; a++) {
				_char.pixel[a][b] = (cn_byte)atoi(cn_fstream_proceed(fs));
			}
		}
		cn_vec_push_back(vec_chars, &_char);
	}

	cn_fstream_free(fs);

	fclose(fp);
}

void export_font(char* fname, CN_VEC vec_chars) {
	FILE *fp = fopen(fname, "wb");
	
	if (fp == NULL)
		return; //You failed... again
	
	//Write the image information first
	cn_uint num = cn_vec_size(vec_chars);
	fwrite(&WIDTH , sizeof(cn_uint), 1, fp);
	fwrite(&HEIGHT, sizeof(cn_uint), 1, fp);
	fwrite(&num  , sizeof(cn_uint), 1, fp);

	//Write each character
	cn_uint i = 0,
	        a, b;
	for (; i < num; i++) {
		CNB_CHAR* _char = cn_vec_at(vec_chars, i);
		//fwrite(&_char->char_val, sizeof(char   ), 1             , fp);
		//fwrite(_char->pixel    , sizeof(cn_byte), WIDTH * HEIGHT, fp);
		fputc(_char->char_val, fp);
		for (b = 0; b < HEIGHT; b++) {
			for (a = 0; a < WIDTH; a++) {
				fputc(_char->pixel[a][b], fp);
			}
		}
	}
	fclose(fp);
}

void print_char_to_ppm(cnpe_uint x, cnpe_uint y, PPM img, CNB_CHAR* bitmap) {
	cn_uint a, b;
	for (b = 0; b < HEIGHT; b++) {
		for (a = 0; a < WIDTH; a++) {
			PPM_RGB* pixel = ppm_at(img, x + a, y + b);
			pixel->R = (bitmap->pixel[a][b] == 1) ? 0xFF : 0;
			pixel->G = pixel->R;
			pixel->B = pixel->R;
		}
	}
}

void export_font_preview_ppm(char* fname, CN_VEC vec_chars) {
	cn_uint _img_width  = (WIDTH + 2) * cn_vec_size(vec_chars),
			_img_height = HEIGHT + 2;

	PPM img = ppm_init_size(_img_width, _img_height);
	cn_uint i = 0;
	
	//Draw the characters on the PPM file
	for (; i < cn_vec_size(vec_chars); i++) {
		print_char_to_ppm(1 + (7 * i), 1, img, (CNB_CHAR *)cn_vec_at(vec_chars, i));
	}

	//Write the PPM data to a PPM file
	FILE* fp = fopen(fname, "wb");
	if (fp == NULL)
		return; //You failed (and wasted the processing above...)
	
	//Header
	fprintf(fp, "P6\n%d %d\n255\n", _img_width, _img_height);
	
	//Actual bytes
	fwrite(ppm_data(img), sizeof(struct ppm_rgb), ppm_get_size(img), fp);

	fclose(fp);
}

int main() {
	CN_VEC vec_chars = cn_vec_init(CNB_CHAR);
	read_font("font.dat", vec_chars);
	export_font("font.bin", vec_chars);
	export_font_preview_ppm("font.ppm", vec_chars);
}
