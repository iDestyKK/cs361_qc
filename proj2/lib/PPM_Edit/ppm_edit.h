/*
 * PPM_Edit Library (C Version)
 *
 * Version 0.1.0 (Last Updated: 2016-07-20)
 *
 * Description:
 *     Implements a custom data structure made to handle images. This structure
 *     relies on CN_SGrids to operate. As they are practically for this kind of
 *     structure exclusively. You can manipulate colours, import, and export
 *     PPM files flawlessly. It even supports PGM and PBM files.
 *
 *     Changelog of this library is located at the bottom of this file.
 *
 * Author:
 *     Clara Van Nguyen
 *
 * For documentation and details on every function in this library, visit the following URL:
 * http://web.eecs.utk.edu/~ssmit285/lib/ppm_edit/c/index.html
 */

#ifndef __PPM_EDIT_C_HAN__
#define __PPM_EDIT_C_HAN__

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Typedef
typedef unsigned int  cnpe_uint;
typedef unsigned char cnpe_byte;

//Colour Struct
typedef struct ppm_rgb {
	cnpe_byte R, G, B;
} PPM_RGB;

//PPM Image Structure
typedef struct ppm {
	//Dimensions
	cnpe_uint width,
	          height,
	          range;

	struct ppm_rgb* img;
} *PPM;

//Functions
//Initialiser
PPM ppm_init();
PPM ppm_init_size(cnpe_uint, cnpe_uint);

//Destructor
void ppm_free(PPM);

//File Management
void ppm_open(PPM, const char*);

//Get
cnpe_uint ppm_get_width (PPM);
cnpe_uint ppm_get_height(PPM);
cnpe_uint ppm_get_size  (PPM);
PPM_RGB*  ppm_at        (PPM, cnpe_uint, cnpe_uint);
PPM_RGB*  ppm_data      (PPM);
cnpe_byte ppm_empty     (PPM);

//Loading Functions
void _open_pbm_ascii (PPM, FILE *);
void _open_pgm_ascii (PPM, FILE *);
void _open_ppm_ascii (PPM, FILE *);
void _open_pbm_binary(PPM, FILE *);
void _open_pgm_binary(PPM, FILE *);
void _open_ppm_binary(PPM, FILE *);


#endif

/****************************************\
    * ** ***    CHANGELOG    *** ** *
\****************************************/

/*
	2016-07-20 (0.1.0)
	  - Started writing PPM_Edit (C Version)
*/
