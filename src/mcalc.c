/**
 * \file mcalc.c
 * \author Artur Rodrigues Rocha Neto
 * \date 2018
 * \brief Interface de programa para extração de momentos
 */

#include <time.h>
#include <unistd.h>
#include "include/extraction.h"
#include "include/hu.h"
#include "include/zernike.h"
#include "include/legendre.h"
#include "include/chebyshev.h"
#include "include/matrix.h"
#include "include/cloud.h"
#include "include/util.h"
#include "include/spheric.h"
#include "include/golden.h"
#include "include/tutu.h"

#define HUTUTU			"hututu"
#define HU1980			"hu1980"
#define HUSIQ			"husiq"
#define HUSUPER			"husuper"
#define ZERNIKE			"zernike"
#define LEGENDRE		"legendre"
#define CHEBYSHEV		"chebyshev"
#define SPHERIC			"spheric"
#define GOLDEN			"golden"
#define TUTU			"tutu"

#define CUT_WHOLE		"w"
#define CUT_SAGITTAL	"s"
#define CUT_TRANSVERSAL	"t"
#define CUT_FRONTAL		"f"
#define CUT_RADIAL		"r"
#define CUT_UPPER		"u"
#define CUT_LOWER		"l"
#define CUT_7			"7"
#define CUT_6			"6"
#define CUT_4			"4"
#define CUT_MANHATTAN	"m"

/**
 * \brief Exibe mensagem ao usuário informando como usar o extrator de momentos
 */
void extraction_help()
{
	printf("mcalc: Calculador de Momentos 3D\n");
	printf("autor: Artur Rodrigues Rocha Neto (UFC/LATIN/INTERFACES)\n");
    printf("faltando argumentos! obrigatorios: [ -m | -i | -o | -c ]\n");
    
    printf(" -m: momento usado para extracao de atributos\n");
    printf("     > hututu\n");
    printf("     > husiq\n");
    printf("     > hu1980\n");
    printf("     > husuper\n");
    printf("     > zernike\n");
    printf("     > legendre\n");
    printf("     > chebyshev\n");
    printf("     > spheric\n");
    printf("     > golden\n");
    printf("     > tutu\n");
    
    printf(" -i: nuvem de entrada no formato XYZ\n");
    printf("     > ../data/bunny.xyz, face666.xyz, ~/bs/bs001.xyz, etc\n");
    
    printf(" -o: arquivo aonde os momentos serao salvos\n");
    printf("     > path para arquivo texto\n");
    printf("     > stdout para saida padrão (normalmente console)\n");
    
    printf(" -c: tipo de corte\n");
    printf("     > w: toda\n");
    printf("     > s: sagital\n");
    printf("     > t: transversal\n");
    printf("     > f: frontal\n");
    printf("     > r: radial\n");
    printf("     > u: parte superior\n");
    printf("     > l: parte inferior\n");
    printf("     > 7: corte em 7 segmentos\n");
    printf("     > 6: corte em 6 segmentos\n");
    printf("     > 4: corte em 4 segmentos\n");
    printf("     > m: corte manhattan\n");
    
    printf("EX1: mcalc -m hu_1980 -i ../data/cloud1.xyz -o hu1.txt -c t\n");
    printf("EX2: mcalc -m legendre -i ../dataset/bunny.xyz -o stdout -c w\n\n");
}

/**
 * \brief Função principal: lê parâmetros de linha de comando e efetua extração
 * \param argc Número de parâmetros passados pela linha de comando
 * \param argv Parâmetros passados por linha de comando
 */
int main(int argc, char** argv)
{
	char* moment = NULL;
    char* input = NULL;
    char* output = NULL;
    char* cut = NULL;
	
    int opt;
    while ((opt = getopt(argc, argv, "m:i:o:c:")) != -1) {
        switch (opt) {
            case 'm':
                moment = optarg;
                break;
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'c':
                cut = optarg;
                break;
            default:
                abort();
        }
    }
	
    if (moment == NULL || input == NULL || output == NULL || cut == NULL) {
        extraction_help();
        return 1;
    }
	
    struct matrix* (*mfunc)(struct cloud*) = &hu_cloud_moments_hututu;
    if (!strcmp(moment, HUTUTU))
        mfunc = &hu_cloud_moments_hututu;
    else if (!strcmp(moment, HU1980))
        mfunc = &hu_cloud_moments_hu1980;
    else if (!strcmp(moment, HUSIQ))
        mfunc = &hu_cloud_moments_husiq;
    else if (!strcmp(moment, HUSUPER))
        mfunc = &hu_superset;
    else if (!strcmp(moment, LEGENDRE))
        mfunc = &legendre_cloud_moments;
    else if (!strcmp(moment, CHEBYSHEV))
        mfunc = &chebyshev_cloud_moments;
    else if (!strcmp(moment, ZERNIKE))
        mfunc = &zernike_cloud_moments;
    else if (!strcmp(moment, SPHERIC))
        mfunc = &spheric_cloud_moments;
    else if (!strcmp(moment, GOLDEN))
        mfunc = &golden_cloud_moments;
    else if (!strcmp(moment, TUTU))
        mfunc = &tutu_cloud_moments;
    else
    	mfunc = &hu_cloud_moments_hututu;
	
    struct cloud* cloud = cloud_load_xyz(input);
    if (input == NULL) {
        util_seg("abortando");
        exit(1);
    }
	
	struct matrix* results = NULL;
	if (!strcmp(cut, CUT_WHOLE))
		results = (*mfunc)(cloud);
	else if (!strcmp(cut, CUT_SAGITTAL))
		results = extraction_sagittal(cloud, mfunc);
	else if (!strcmp(cut, CUT_TRANSVERSAL))
		results = extraction_transversal(cloud, mfunc);
	else if (!strcmp(cut, CUT_FRONTAL))
		results = extraction_frontal(cloud, mfunc);
	else if (!strcmp(cut, CUT_RADIAL))
		results = extraction_radial(cloud, mfunc);
	else if (!strcmp(cut, CUT_UPPER))
		results = extraction_upper(cloud, mfunc);
	else if (!strcmp(cut, CUT_LOWER))
		results = extraction_lower(cloud, mfunc);
	else if (!strcmp(cut, CUT_7))
		results = extraction_7(cloud, mfunc);
	else if (!strcmp(cut, CUT_6))
		results = extraction_6(cloud, mfunc);
	else if (!strcmp(cut, CUT_4))
		results = extraction_4(cloud, mfunc);
	else if (!strcmp(cut, CUT_MANHATTAN))
		results = extraction_manhattan(cloud, mfunc);
	else
		results = (*mfunc)(cloud);
	
	if (!strcmp(output, "stdout")) {
		matrix_debug(results, stdout);
	} else {
		matrix_save_to_file(results, output);
	}
	
    matrix_free(results);
    cloud_free(cloud);
    
    return 0;
}

