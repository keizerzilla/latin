/**
 * \file shapenet.c
 * \author Artur Rodrigues Rocha Neto
 * \date 2019
 * \brief Extrator de momentos de zernike para a base shapenet
 */

#include "../pontu_core.h"
#include "../pontu_features.h"
#include "../pontu_sampling.h"

#define ZERNIKE_ODD			"zo"
#define ZERNIKE_EVEN		"ze"
#define ZERNIKE_MAG			"zm"
#define ZERNIKE_FULL		"zf"
#define SPHARMONICS_MAG		"sm"

int main(int argc, char** argv)
{
	if (argc != 3) {
		printf("! numero de parametros incorreto !\n");
		printf("uso: shapenet <arquivo_obj> <tipo_zernike>\n");
		exit(1);
	}
	
	struct cloud *cloud = cloud_load_obj(argv[1]); // Shapenet formato .obj
	if (cloud == NULL) {
		printf("! ops, kd a nuvem? !\n");
		exit(1);
	}
	
	struct matrix *moments = NULL;
	if (!strcmp(argv[2], ZERNIKE_ODD))
		moments = zernike_cloud_moments_odd(cloud);
	else if (!strcmp(argv[2], ZERNIKE_EVEN))
		moments = zernike_cloud_moments_even(cloud);
	else if (!strcmp(argv[2], ZERNIKE_MAG))
		moments = zernike_cloud_moments_mag(cloud);
	else if (!strcmp(argv[2], ZERNIKE_FULL))
		moments = zernike_cloud_moments_full(cloud);
	else if (!strcmp(argv[2], SPHARMONICS_MAG))
		moments = spharmonics_cloud_moments_mag(cloud);
	else
		moments = spharmonics_cloud_moments_full(cloud);
	
	if (moments == NULL) {
		printf("! ops, momentos vazios !\n");
		exit(1);
	}
	
	matrix_debug(moments, stdout);
	
	matrix_free(moments);
	cloud_free(cloud);
	
	return 0;
}

