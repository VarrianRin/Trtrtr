#include <iostream>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <stdlib.h>

const int INF = -1;
const int ENOUGHATTEMPTS = 1000;
const double tolerance = 1e-17;

//! InputNumbers\n
//! @param [in]  text -  welcoming text
//! @param [in]  nCoeffs - number of coefficients
//! @param [out] data - index of array

double* InputNumbers(const char text[], int nCoeffs) {

    assert(text != NULL);

    int nAttempts = 0;
    double* data = (double*) calloc (nCoeffs, sizeof(data[-1]));
    assert(data != NULL);
    printf("%s\n", text);

    for (int i = 0; i < nCoeffs; i++) {

        printf("%d/%d : ", i + 1, nCoeffs);

        if(!scanf("%lg", &data[i])) {

            fflush(stdin);
            printf("%s\n", text);
            nAttempts++;

            if (nAttempts >= ENOUGHATTEMPTS)
                return data;

            i--;
        }
    }
    return data;
}

//! iszero\n
//! @param [in] number
//! @param [out] 1 if number = 0; 0 otherwise\n

bool iszero(double number) {

	if (fabs(number) < tolerance) {
		return 1;
	}
	return 0;
}

//! SolveSquare\n
//! solves ax^2 + bx + c = 0\n
//! @param [in]  a - coefficient
//! @param [in]  b - coefficient
//! @param [in]  c - coefficient
//! @param [out] n_roots - number of roots

int SolveSquare(double a, double b, double c, double* x1, double* x2) {

	assert(std::isfinite(a));
	assert(std::isfinite(b));
	assert(std::isfinite(c));
	assert(x1 != NULL);
	assert(x2 != NULL);
	assert(x1 != x2);
	double D = NAN;

	if (a != 0) {
		D = (b * b) - (4 * a * c);

		if (!iszero(D) && D > 0) {
			D = sqrt(D);
			*x1 = (-b + D) / (2 * a);
			*x2 = (-b - D) / (2 * a);
			return 2;
		}
		else if (iszero(D)) {
			*x1 = -b / (2 * a);
			return 1;
		}
		else {
			return 0;
		}
	}
	else if (b != 0) {
		*x1 = -c / b;
		return 1;
	}
	else if (c == 0) {
		return INF;
	}
	else if (c != 0) {
		return 0;
	}
	else {
		return -2;
	}
}

//! main\n
//! @param [in]  a - coefficient
//! @param [in]  b - coefficient
//! @param [in]  c - coefficient
//! @param [out] n_roots - number of roots
//! @param [out] x1 - first root
//! @param [out] x2 - second root

int main() {

    double* coeffs = NULL;
    double x1 = NAN, x2 = NAN;
	int n_roots = 0;
	coeffs = InputNumbers("input numbers", 3);
    n_roots = SolveSquare(*coeffs, *(coeffs + 1), *(coeffs + 2), &x1, &x2);

    switch (n_roots) {

		case INF:
			printf("Any numbers\n");
			break;

		case 0:
			printf("no roots\n");
			break;

		case 1:
			printf("1 root\n"
                   "x = %lg\n", x1);
			break;

		case 2:
			printf("2 roots\n"
                   "x1 = %lg\n"

                   "x2 = %lg\n", x1, x2);
			break;

		default:
			printf("Error\n");
			break;
    }

    free(coeffs);

    return 0;
}
