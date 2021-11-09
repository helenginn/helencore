// helencore
// Copyright (C) 2019 Helen Ginn
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
// Please email: vagabond @ hginn.co.uk for more details.

namespace HelenCore
{
	typedef struct
	{
		double *vals;
		double **ptrs;
		int rows;
		int cols;
	} Matrix;

	typedef struct
	{
		Matrix u;
		Matrix v;
		double *w;
	} SVD;

	typedef struct
	{
		double w;
		int idx;
	} OrderW;

	void setupMatrix(HelenCore::Matrix *mat, int x, int y = 0);
	void setupSVD(HelenCore::SVD *cc, int x, int y = 0);
	void printMatrix(HelenCore::Matrix *mat);
	void multMatrix(Matrix &mat, double *vector);
	void reorderSVD(HelenCore::SVD *cc);
	bool invertSVD(HelenCore::SVD *cc);
	void freeMatrix(HelenCore::Matrix *m);
	void freeSVD(HelenCore::SVD *cc);

	bool runSVD(HelenCore::SVD *cc);
	bool order_by_w(const OrderW &a, const OrderW &b);
};
