#ifndef CALC_H
#define CALC_H

#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif // RAND_MAX

#define CALC_E			2.7182818284590452353602874713527
#define CALC_PHI		1.618033988749895
#define CALC_PI			3.14159265358979323846264338327950288
#define CALC_PI_2		1.57079632679489661923132169163975144
#define CALC_DEG2RAD	CALC_PI / 180.0
#define CALC_RAD2DEG	180.0 / CALC_PI

/**
 * \var real
 * \brief Mask for the float point number most accurate available on the system
 */
typedef double real;

/**
 * \var uint
 * \brief Mask for the integer type w/o signal
 */
typedef unsigned int uint;

/**
 * \var int64
 * \brief Mask for the  64 bits integer type
 */
typedef long long int64;

/**
 * \brief Calculates the factorial of a number
 * \param n Number to calculate the factorial
 * \return n!
 */
int calc_factorial (int n);

/**
 * \brief Number of combinations N-P
 * \param n dimension 1
 * \param p dimension 2
 * \return N-P combinations
 */
int calc_combination (int n, int p);

/**
 * \brief Calculates the Kronecker Delta
 * \param i First sub-index
 * \param j Second sub-index
 * \return 1 if i==j and 0 if i!=j
 */
int calc_kronecker (int i, int j);

/**
 * \brief Calculates 3D square length
 * \param x Coordinate x
 * \param y Coordinate y
 * \param z Coordinate z
 * \return The 3D square length of the point(x,y,z)
 */
real calc_squared_length3 (real x, real y, real z);

/**
 * \brief Calculates 3D length
 * \param x Coordinate x
 * \param y Coordinate y
 * \param z Coordinate z
 * \return The 3D length of the point(x,y,z)
 */
real calc_length3 (real x, real y, real z);

/**
 * \brief Calculates 2D square length
 * \param x Coordinate x
 * \param y Coordinate y
 * \return The 2D squared length of the point(x,y)
 */
real calc_squared_length2 (real x, real y);

/**
 * \brief Calculates 2D length
 * \param x Coordinate x
 * \param y Coordinate y
 * \return The 2D length of the point(x,y)
 */
real calc_length2 (real x, real y);

/**
 * \brief Create random real numbers
 * \param max Maximun value possible
 * \return A random real number on the interval [0, max]
 */
real calc_randr (real max);

/**
 * \brief Gets the greater number out of 3 real numbers
 * \param a The first real
 * \param b The second real
 * \param c The third real
 * \return The greater value in (a, b, c)
 */
real calc_max3 (real a, real b, real c);

#endif // CALC_H