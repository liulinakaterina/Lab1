#include "pch.h"
#include "Matrix.h"
#include <stdlib.h>
#include <tchar.h>

Matrix::Matrix(int n)
{
	this->N = n;
	this->InitMatrix();
}

Matrix::~Matrix()
{
}

void Matrix::Fill() 
{
	for (int i = 0; i < this->N; i++)
	{
		for (int j = 0; j < this->N; j++)
		{
			matrix[i][j] = rand()%5 - 1;
		}
	}
}

const Matrix operator*(const Matrix & left, const Matrix & right) 
{
	int size = (left.N < right.N) ? left.N : right.N;
	Matrix * result = new Matrix(size);
	
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			result->matrix[i][j] = 0;
			for (int k = 0; k < size; k++) 
			{
				result->matrix[i][j] += left.matrix[i][k] * right.matrix[k][j];
			}
		}
	}
	return *result;
}

void Matrix::InitMatrix() 
{
	matrix = new __int8*[this->N];
	for (int i = 0; i < this->N; i++) 
	{
		matrix[i] = new __int8[N];
	}
}

void Matrix::Print()
{
	_tprintf(_T("Size (object): %d x \nFirst element: %d\n"), this->N, this->N);
}

