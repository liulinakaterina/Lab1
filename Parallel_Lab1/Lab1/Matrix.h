#pragma once
class Matrix
{
public:
	__int8 ** matrix;
	Matrix(int n);
	~Matrix();
	void Fill();
	void Print();
	//Matrix * operator* (Matrix * other);
	friend const Matrix operator*(const Matrix & left, const Matrix & right);

private:
	int N;
	void InitMatrix();
};

