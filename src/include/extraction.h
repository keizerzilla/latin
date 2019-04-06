/**
 * \file extraction.c
 * \author Artur Rodrigues Rocha Neto
 * \date 2019
 * \brief Diversas funções de corte para extração de atributos.
 */

#ifndef EXTRACTION_H
#define EXTRACTION_H

#include "vector3.h"
#include "cloud.h"
#include "matrix.h"
#include "plane.h"

/**
 * \brief Extrái momentos usando cortes a partir de um plano
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \param O vetor normal ao plano de corte
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_plane(struct cloud* cloud,
                                struct matrix* (*mfunc)(struct cloud*),
                                struct vector3* norm)
{
	struct cloud* par1 = cloud_empty();
	struct cloud* par2 = cloud_empty();
	struct vector3* pt = cloud_get_center(cloud);
	struct plane* plane = plane_new(norm, pt);
	
	cloud_plane_partition(cloud, plane, par1, par2);
	
	struct matrix* r1 = (*mfunc)(par1);
	struct matrix* r2 = (*mfunc)(par2);
	struct matrix* ans = matrix_concat_hor(r1, r2);
	
	cloud_free(par1);
	cloud_free(par2);
	vector3_free(pt);
	plane_free(plane);
	matrix_free(r1);
	matrix_free(r2);
	
	return ans;
}

/**
 * \brief Extrái momentos usando cortes a partir de planos recursivos
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \param O vetor normal ao plano de corte
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_recursive(struct cloud* cloud,
                                    struct matrix* (*mfunc)(struct cloud*),
                                    struct vector3* norm)
{
	struct cloud* par1 = cloud_empty();
	struct cloud* par2 = cloud_empty();
	struct vector3* pt = cloud_get_center(cloud);
	struct plane* plane = plane_new(norm, pt);
	
	cloud_plane_partition(cloud, plane, par1, par2);
	
	struct cloud* par1_fh = cloud_empty();
	struct cloud* par2_fh = cloud_empty();
	struct vector3* pt_fh = cloud_get_center(par1);
	struct plane* plane_fh = plane_new(norm, pt_fh);
	
	cloud_plane_partition(par1, plane_fh, par1_fh, par2_fh);
	
	struct matrix* r1 = matrix_concat_hor((*mfunc)(par1_fh), (*mfunc)(par2_fh));
	struct cloud* par1_sh = cloud_empty();
	struct cloud* par2_sh = cloud_empty();
	struct vector3* pt_sh = cloud_get_center(par2);
	struct plane* plane_sh = plane_new(norm, pt_sh);
	
	cloud_plane_partition(par2, plane_sh, par1_sh, par2_sh);
	
	struct matrix* r2 = matrix_concat_hor((*mfunc)(par1_sh), (*mfunc)(par2_sh));
	struct matrix* ans = matrix_concat_hor(r1, r2);
	
	plane_free(plane_fh);
	vector3_free(pt_fh);
	cloud_free(par2_fh);
	cloud_free(par1_fh);
	plane_free(plane_sh);
	vector3_free(pt_sh);
	cloud_free(par2_sh);
	cloud_free(par1_sh);
	matrix_free(r2);
	matrix_free(r1);
	plane_free(plane);
	vector3_free(pt);
	vector3_free(norm);
	cloud_free(par2);
	cloud_free(par1);
	
	return ans;
}

/**
 * \brief Extrái momentos usando cortes sagitais
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_sagittal(struct cloud* cloud,
                                   struct matrix* (*mfunc)(struct cloud*))
{
	return extraction_recursive(cloud, mfunc, vector3_new(1, 0, 0));
}

/**
 * \brief Extrái momentos usando cortes transversais
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_transversal(struct cloud* cloud,
                                      struct matrix* (*mfunc)(struct cloud*))
{
	return extraction_recursive(cloud, mfunc, vector3_new(0, 1, 0));
}

/**
 * \brief Extrái momentos usando cortes frontais
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_frontal(struct cloud* cloud,
                                  struct matrix* (*mfunc)(struct cloud*))
{
	return extraction_recursive(cloud, mfunc, vector3_new(0, 0, 1));
}

/**
 * \brief Extrái momentos usando cortes radiais
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_radial(struct cloud* cloud,
                                 struct matrix* (*mfunc)(struct cloud*))
{
	real d = 0.0f;
	//real slice = cloud_max_distance_from_center(cloud) / 2.0f;
	real slice = cloud_max_distance(cloud, cloud_min_z(cloud)) / 2.0f;
	
	struct vector3* center = cloud_get_center(cloud);
	struct cloud* sub1 = cloud_empty();
	struct cloud* sub2 = cloud_empty();
	
	for (uint i = 0; i < cloud_size(cloud); i++) {
		d = vector3_distance(&cloud->points[i], center);
		
		if (d <= slice)
			cloud_add_point_vector(sub1, &cloud->points[i]);
		else
			cloud_add_point_vector(sub2, &cloud->points[i]);
	}
	
	struct matrix* ans = matrix_concat_hor((*mfunc)(sub1), (*mfunc)(sub2));
	
	cloud_free(sub2);
	cloud_free(sub1);
	vector3_free(center);
	
	return ans;
}

/**
 * \brief Extrái momentos usando a parte superior da nuvem (já alinhada)
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_upper(struct cloud* cloud,
                                struct matrix* (*mfunc)(struct cloud*))
{
	struct vector3* norm = vector3_new(0, 1, 0);
	struct vector3* point = cloud_min_z(cloud);
	struct plane* plane = plane_new(norm, point);
	struct cloud* sub = cloud_cut_plane(cloud, plane);
	struct matrix* ans = (*mfunc)(sub);
	
	cloud_free(sub);
	plane_free(plane);
	vector3_free(point);
	vector3_free(norm);
	
	return ans;
}

/**
 * \brief Extrái momentos usando a parte inferior da nuvem (já alinhada)
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return A matrix com os momentos extraídos
 */
struct matrix* extraction_lower(struct cloud* cloud,
                                struct matrix* (*mfunc)(struct cloud*))
{
	struct vector3* norm = vector3_new(0, -1, 0);
	struct vector3* point = cloud_min_z(cloud);
	struct plane* plane = plane_new(norm, point);
	struct cloud* sub = cloud_cut_plane(cloud, plane);
	struct matrix* ans = (*mfunc)(sub);
	
	cloud_free(sub);
	plane_free(plane);
	vector3_free(point);
	vector3_free(norm);
	
	return ans;
}

/**
 * \brief Os 4 segmentos do tutu (e do iraniano)
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return 4 segmentos
 */
struct matrix* extraction_4(struct cloud* cloud,
                            struct matrix* (*mfunc)(struct cloud*))
{
	struct vector3* norm_sagit = vector3_new(1, 0, 0);
	struct vector3* center = cloud_get_center(cloud);
	struct plane* plane_sagit = plane_new(norm_sagit, center);
	struct cloud* left = cloud_empty();
	struct cloud* right = cloud_empty();
	
	cloud_plane_partition(cloud, plane_sagit, left, right);
	
	struct vector3* norm_trans = vector3_new(0, 1, 0);
	struct cloud* left_1 = cloud_empty();
	struct cloud* left_2 = cloud_empty();
	struct cloud* right_1 = cloud_empty();
	struct cloud* right_2 = cloud_empty();
	struct plane* plane_left = plane_new(norm_trans, center);
	struct plane* plane_right = plane_new(norm_trans, center);
	
	cloud_plane_partition(left, plane_left, left_1, left_2);
	cloud_plane_partition(right, plane_right, right_1, right_2);
	
	struct matrix* ans_left_1 = (*mfunc)(left_1);
	struct matrix* ans_left_2 = (*mfunc)(left_2);
	struct matrix* ans_right_1 = (*mfunc)(right_1);
	struct matrix* ans_right_2 = (*mfunc)(right_2);
	
	struct matrix* subleft = matrix_concat_hor(ans_left_1, ans_left_2);
	struct matrix* subright = matrix_concat_hor(ans_right_1, ans_right_2);
	struct matrix* ret = matrix_concat_hor(subleft, subright);
	
	vector3_free(norm_sagit);
	vector3_free(center);
	plane_free(plane_sagit);
	cloud_free(left);
	cloud_free(right);
	vector3_free(norm_trans);
	cloud_free(left_1);
	cloud_free(left_2);
	cloud_free(right_1);
	cloud_free(right_2);
	plane_free(plane_left);
	plane_free(plane_right);
	matrix_free(ans_left_1);
	matrix_free(ans_left_2);
	matrix_free(ans_right_1);
	matrix_free(ans_right_2);
	matrix_free(subleft);
	matrix_free(subright);
	
	return ret;
}

/**
 * \brief Os 6 segmentos do tutu (e do iraniano)
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return 6 segmentos
 */
struct matrix* extraction_6(struct cloud* cloud,
                            struct matrix* (*mfunc)(struct cloud*))
{
	struct vector3* norm_sagit = vector3_new(1, 0, 0);
	struct vector3* center = cloud_get_center(cloud);
	struct plane* plane_sagit = plane_new(norm_sagit, center);
	struct cloud* left = cloud_empty();
	struct cloud* right = cloud_empty();
	
	cloud_plane_partition(cloud, plane_sagit, left, right);
	
	struct vector3* norm_trans = vector3_new(0, 1, 0);
	struct cloud* left_1 = cloud_empty();
	struct cloud* left_2 = cloud_empty();
	struct cloud* right_1 = cloud_empty();
	struct cloud* right_2 = cloud_empty();
	struct plane* plane_left = plane_new(norm_trans, center);
	struct plane* plane_right = plane_new(norm_trans, center);
	
	cloud_plane_partition(left, plane_left, left_1, left_2);
	cloud_plane_partition(right, plane_right, right_1, right_2);
	
	struct matrix* ans_left = (*mfunc)(left);
	struct matrix* ans_right = (*mfunc)(right);
	struct matrix* ans_left_1 = (*mfunc)(left_1);
	struct matrix* ans_left_2 = (*mfunc)(left_2);
	struct matrix* ans_right_1 = (*mfunc)(right_1);
	struct matrix* ans_right_2 = (*mfunc)(right_2);
	
	struct matrix* full = matrix_concat_hor(ans_left, ans_right);
	struct matrix* subleft = matrix_concat_hor(ans_left_1, ans_left_2);
	struct matrix* subright = matrix_concat_hor(ans_right_1, ans_right_2);
	struct matrix* subs = matrix_concat_hor(subleft, subright);
	struct matrix* ret = matrix_concat_hor(full, subs);
	
	vector3_free(norm_sagit);
	vector3_free(center);
	plane_free(plane_sagit);
	cloud_free(left);
	cloud_free(right);
	vector3_free(norm_trans);
	cloud_free(left_1);
	cloud_free(left_2);
	cloud_free(right_1);
	cloud_free(right_2);
	plane_free(plane_left);
	plane_free(plane_right);
	matrix_free(ans_left);
	matrix_free(ans_right);
	matrix_free(ans_left_1);
	matrix_free(ans_left_2);
	matrix_free(ans_right_1);
	matrix_free(ans_right_2);
	matrix_free(full);
	matrix_free(subleft);
	matrix_free(subright);
	matrix_free(subs);
	
	return ret;
}

/**
 * \brief Os 7 segmentos iranianos
 * \param cloud A nuvem alvo
 * \param mfunc A função extratora de momentos
 * \return 7 segmentos
 */
struct matrix* extraction_7(struct cloud* cloud,
                            struct matrix* (*mfunc)(struct cloud*))
{
	struct vector3* norm_sagit = vector3_new(1, 0, 0); //ok
	struct vector3* center = cloud_get_center(cloud); //ok
	struct plane* plane_sagit = plane_new(norm_sagit, center); //ok
	struct cloud* left = cloud_empty(); //ok
	struct cloud* right = cloud_empty(); //ok
	
	cloud_plane_partition(cloud, plane_sagit, left, right);
	
	struct matrix* ans_left = (*mfunc)(left); //ok
	struct matrix* ans_right = (*mfunc)(right); //ok
	
	struct vector3* norm_trans = vector3_new(0, 1, 0); //ok
	struct cloud* left_1 = cloud_empty(); //ok
	struct cloud* left_2 = cloud_empty(); //ok
	struct cloud* right_1 = cloud_empty(); //ok
	struct cloud* right_2 = cloud_empty(); //ok
	struct plane* plane_left = plane_new(norm_trans, center); //ok
	struct plane* plane_right = plane_new(norm_trans, center); //ok
	
	cloud_plane_partition(left, plane_left, left_1, left_2);
	cloud_plane_partition(right, plane_right, right_1, right_2);
	
	struct matrix* ans_left_1 = (*mfunc)(left_1); //ok
	struct matrix* ans_left_2 = (*mfunc)(left_2); //ok
	struct matrix* ans_right_1 = (*mfunc)(right_1); //ok
	struct matrix* ans_right_2 = (*mfunc)(right_2); //ok
	
	struct matrix* ans_full = (*mfunc)(cloud); //ok
	struct matrix* halves = matrix_concat_hor(ans_left, ans_right); //ok
	struct matrix* subleft = matrix_concat_hor(ans_left_1, ans_left_2); //ok
	struct matrix* subright = matrix_concat_hor(ans_right_1, ans_right_2); //ok
	struct matrix* subs = matrix_concat_hor(subleft, subright); //ok
	struct matrix* tris = matrix_concat_hor(ans_full, halves); //ok
	
	struct matrix* ret = matrix_concat_hor(subs, tris);
	
	vector3_free(norm_sagit);
	vector3_free(center);
	plane_free(plane_sagit);
	cloud_free(left);
	cloud_free(right);
	matrix_free(ans_left);
	matrix_free(ans_right);
	cloud_free(left_1);
	cloud_free(left_2);
	cloud_free(right_1);
	cloud_free(right_2);
	matrix_free(ans_left_1);
	matrix_free(ans_left_2);
	matrix_free(ans_right_1);
	matrix_free(ans_right_2);
	plane_free(plane_right);
	plane_free(plane_left);
	vector3_free(norm_trans);
	matrix_free(ans_full);
	matrix_free(halves);
	matrix_free(subleft);
	matrix_free(subright);
	matrix_free(subs);
	matrix_free(tris);
	
	return ret;
}

#endif // EXTRACTION_H

