// lab3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <intrin.h>
#include <string>
#include <Windows.h>
#include <xmmintrin.h>

// 4 registers: EAX, EBX, ECX, EDX
#define REGISTER_NUMBER 4
#define EAX 0
#define EBX 1
#define ECX 2
#define EDX 3

#pragma region additionalFunctions

int GetMaxSupportedProcessorFunction()
{
	int registers[REGISTER_NUMBER];
	// 1st parameter - 4 registers
	// 2nd parameter - code of implemented function
	// function with code 0 - возвращает максимальное значение обычной ф-ции,
	// которая поддерживается процессором, в регистр EAX
	__cpuid(registers, 0);

	return registers[EAX];
}

int GetMaxSupportedExtendedProcessorFunction()
{
	int registers[REGISTER_NUMBER];

	// function with code 0x80000000 - возвращает максимальное значение расширенной ф-ции,
	// которая поддерживается процессором, в регистр EAX
	__cpuid(registers, 0x80000000);

	//registers[0] - максимальное значение расширенной функции
	return registers[EAX];
}

bool IsFunctionSupported(int functionCode)
{
	int maxSupportedFunction = GetMaxSupportedProcessorFunction();
	return functionCode <= maxSupportedFunction;
}

bool IsPropertySupported(int function, int reg, int bit)
{
	int registers[REGISTER_NUMBER];
	unsigned mask = 1 << bit;
	__cpuid(registers, function);
	return (registers[reg] & mask) == mask;
}

//служебные
template <typename T>
T * GetRandomArray(int size, int min = -5, int max = 5)
{
	T * array = new T[size];
	for (int i = 0; i < size; i++)
	{
		array[i] = (T)(rand() % max + min);
	}
	return array;
}

template <typename T>
T * GetRandomArrayShift(int size)
{
	T * array = new T[size];
	for (int i = 0; i < size; i++)
	{
		array[i] = (T)(5);
	}
	return array;
}

template <typename T>
void Print(T * array, int size)
{
	for (int i = 0; i < size - 1; i++)
	{
		std::cout << array[i] << ", ";
	}
	std::cout << array[size - 1] << std::endl;
}

template <typename T>
void PrintComplexValues(T * real, T * imaginary, int size) 
{
	std::cout << "Real part: ";
	for (int i = 0; i < size - 1; i++) 
	{
		std::cout << real[i] << ", ";
	}
	std::cout << real[size - 1] << std::endl;

	std::cout << "Imaginary part: ";
	for (int i = 0; i < size - 1; i++)
	{
		std::cout << imaginary[i] << ", ";
	}
	std::cout << imaginary[size - 1] << std::endl;
}
#pragma endregion

#pragma region task1

#pragma region GetProcessorType

typedef enum
{
	INTEL,
	AMD,
	UNKNOWN
} ProcessorType;

//simple processor type info
std::string GetProcessorNameFromRegisters(int registers[REGISTER_NUMBER])
{
	char processorName[13];
	processorName[12] = 0; //устанавливаем конец строки

	size_t block = 4;
	memcpy(processorName, &registers[EBX], block);
	memcpy(processorName + block, &registers[EDX], block);
	memcpy(processorName + 2 * block, &registers[ECX], block);

	std::string value(&processorName[0]);
	return value;
}

ProcessorType GetProcessorType()
{
	std::string intelName = "GenuineIntel";
	std::string amdName = "AuthenticAMD";

	int registers[REGISTER_NUMBER];
	__cpuid(registers, 0);

	std::string processorName = GetProcessorNameFromRegisters(registers);

	ProcessorType type = UNKNOWN;

	if (processorName.compare(intelName) == 0)
	{
		type = INTEL;
	}
	else if (processorName.compare(amdName) == 0)
	{
		type = AMD;
	}

	return type;
}

std::string GetProcessorTypeName()
{
	auto processorType = GetProcessorType();
	std::string processorName;
	switch (processorType)
	{
	case INTEL:
		processorName = "Intel";
		break;
	case AMD:
		processorName = "AMD";
		break;
	default:
		processorName = "undefined";
		break;
	}
	return processorName;
}

//extended processor type info
std::string RegistersToString(int registers[REGISTER_NUMBER])
{
	size_t block = 4;

	char processorName[17]; //17 == REGISTER_NUMBER * block + 1 (нулевой символ)
	processorName[16] = 0; //устанавливаем конец строки

	for (int i = EAX; i <= EDX; i++)
	{
		memcpy(processorName + i * block, &registers[i], block);
	}

	std::string value(&processorName[0]);
	return value;
}

std::string GetExtendedProcessorType() {
	int registers[REGISTER_NUMBER];
	std::string extendedProcessorType = "";

	for (unsigned uCommand = 0x80000002; uCommand < 0x80000005; uCommand++) {
		__cpuid(registers, uCommand);
		extendedProcessorType += RegistersToString(registers);
	}

	return extendedProcessorType;
}
#pragma endregion

#pragma region SIMDSupportDefining

typedef enum
{
	AVX2,
	AVX,
	SSE42,
	SSE41,
	SSSE3,
	SSE3,
	SSE2,
	SSE
} CommandType;

bool IsCommandTypeSupported(CommandType type)
{
	bool isSupported = false;
	if (IsFunctionSupported(1))
	{
		switch (type)
		{
		case AVX2:
			if (IsFunctionSupported(7))
			{
				isSupported = IsPropertySupported(7, EBX, 5);
			}
			break;
		case AVX:
			isSupported = IsPropertySupported(1, ECX, 28);
			break;
		case SSE42:
			isSupported = IsPropertySupported(1, ECX, 20);
			break;
		case SSE41:
			isSupported = IsPropertySupported(1, ECX, 19);
			break;
		case SSSE3:
			isSupported = IsPropertySupported(1, ECX, 9);
			break;
		case SSE3:
			isSupported = IsPropertySupported(1, ECX, 0);
			break;
		case SSE2:
			isSupported = IsPropertySupported(1, EBX, 26);
			break;
		case SSE:
			isSupported = IsPropertySupported(1, EBX, 25);
			break;
		}
	}
	return isSupported;
}

bool AVX2OSSupport() {
	//check whether XSAVE, XRESTORE commandes are supported
	bool isOsAvx2Support = IsPropertySupported(1, ECX, 26);
	if (isOsAvx2Support)
	{
		//check if XGETBV command supported
		isOsAvx2Support = IsPropertySupported(1, ECX, 27);
		if (isOsAvx2Support)
		{
			isOsAvx2Support = (_xgetbv(_XCR_XFEATURE_ENABLED_MASK) & 4) != 0; //эта строка с методы
		}
	}
	return isOsAvx2Support;
}

CommandType GetMaxProcessorSupportedInstructions()
{
	CommandType commandType = SSE;
	for (int i = AVX2; i <= SSE; i++)
	{
		if (IsCommandTypeSupported((CommandType)i))
		{
			commandType = CommandType(i);
			break;
		}
	}
	return commandType;
}

std::string ToString(CommandType type)
{
	std::string value = "";
	switch (type)
	{
	case AVX2:
		value = "AVX2";
		break;
	case AVX:
		value = "AVX";
		break;
	case SSE42:
		value = "SSE42";
		break;
	case SSE41:
		value = "SSE41";
		break;
	case SSSE3:
		value = "SSSE3";
		break;
	case SSE3:
		value = "SSE3";
		break;
	case SSE2:
		value = "SSE2";
		break;
	case SSE:
		value = "SSE";
	}

	return value;

}

#pragma endregion

#pragma endregion

#pragma region task2
//add arrays abs values: z[i] = |a[i]| + |b[i]|
template <typename T>
T * Add(T* first, T* second, int size) 
{
	T * result = new T[size];

	for (int i = 0; i < size; i++) 
	{
		result[i] = (first[i] >= 0) ? first[i] : - first[i];
		result[i] += (second[i] >= 0) ? second[i] : - second[i];
	}

	return result;
}

//using sse commands
__int8 * AddSSE(__int8 * first, __int8 * second, int size)
{
	__int8 * result = new __int8[size];
	__m128i * firstSSE = (__m128i *)first;
	__m128i * secondSSE = (__m128i *)second;
	__m128i * resultSSE = (__m128i *)result;
	__m128i tempSecondAbsValue;

	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128i);
	for (int i = 0; i < sizeSSE; i++)
	{
		resultSSE[i] = _mm_abs_epi8(firstSSE[i]);
		tempSecondAbsValue = _mm_abs_epi8(secondSSE[i]);
		resultSSE[i] = _mm_add_epi8(resultSSE[i], tempSecondAbsValue);
	}
	return result;
}

__int16 * AddSSE(__int16 * first, __int16 * second, int size)
{
	__int16 * result = new __int16[size];
	__m128i * firstSSE = (__m128i *)first;
	__m128i * secondSSE = (__m128i *)second;
	__m128i * resultSSE = (__m128i *)result;
	__m128i tempSecondAbsValue;

	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128i);
	for (int i = 0; i < sizeSSE; i++)
	{
		resultSSE[i] = _mm_abs_epi16(firstSSE[i]);
		tempSecondAbsValue = _mm_abs_epi16(secondSSE[i]);
		resultSSE[i] = _mm_add_epi16(resultSSE[i], tempSecondAbsValue);
	}
	return result;
}

int * AddSSE(int * first, int * second, int size) 
{
	int * result = new int[size];
	__m128i * firstSSE = (__m128i *)first;
	__m128i * secondSSE = (__m128i *)second;
	__m128i * resultSSE = (__m128i *)result;
	__m128i tempSecondAbsValue;

	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128i);
	for (int i = 0; i < sizeSSE; i++)
	{
		resultSSE[i] = _mm_abs_epi32(firstSSE[i]);
		tempSecondAbsValue = _mm_abs_epi32(secondSSE[i]);
		resultSSE[i] = _mm_add_epi32(resultSSE[i], tempSecondAbsValue);
	}
	return result;
}

__int64 * AddSSE(__int64 * first, __int64 * second, int size)
{
	__int64 * result = new __int64[size];
	__m128i * firstSSE = (__m128i *)first;
	__m128i * secondSSE = (__m128i *)second;
	__m128i * resultSSE = (__m128i *)result;

	__m128i comperison, andValue, andNotValue, difference;
	__m128i zero = _mm_setzero_si128();

	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128i);
	for (int i = 0; i < sizeSSE; i++)
	{
		//get module |a|
		comperison = _mm_cmpeq_epi64(zero, firstSSE[i]); //returns fff or 0
		andValue = _mm_and_si128(comperison, firstSSE[i]);
		andNotValue = _mm_andnot_si128(comperison, firstSSE[i]);
		difference = _mm_sub_epi64(zero, andNotValue);
		resultSSE[i] = _mm_add_epi64(andValue, difference);

		//get module |b|
		comperison = _mm_cmpeq_epi64(zero, secondSSE[i]); //returns fff or 0
		andValue = _mm_and_si128(comperison, secondSSE[i]);
		andNotValue = _mm_andnot_si128(comperison, secondSSE[i]);
		difference = _mm_sub_epi64(zero, andNotValue);
		resultSSE[i] = _mm_add_epi64(resultSSE[i], difference);
		resultSSE[i] = _mm_add_epi64(resultSSE[i], andValue); //тут ответ * (-1)
		resultSSE[i] = _mm_sub_epi64(zero, resultSSE[i]);

		// AVX-512 _mm_abs_epi64
		//resultSSE[i] = _mm_abs_epi64(firstSSE[i]);
		//tempSecondAbsValue = _mm_abs_epi64(secondSSE[i]);
		//resultSSE[i] = _mm_add_epi64(resultSSE[i], tempSecondAbsValue);
	}
	return result;
}

float * AddSSE(float * first, float * second, int size)
{
	float * result = new float[size];
	
	__m128 * firstSSE = (__m128 *)first;
	__m128 * secondSSE = (__m128 *)second;
	__m128 * resultSSE = (__m128 *)result;
	__m128 comperison, andValue, andNotValue, difference;
	__m128 zero = _mm_setzero_ps();


	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128);

	for (int i = 0; i < sizeSSE; i++)
	{
		//get module |a|
		comperison = _mm_cmple_ps(zero, firstSSE[i]); //returns 0 or fff
		andValue = _mm_and_ps(comperison, firstSSE[i]);
		andNotValue = _mm_andnot_ps(comperison, firstSSE[i]);
		difference = _mm_sub_ps(zero, andNotValue);
		resultSSE[i] = _mm_add_ps(andValue, difference);

		//get module |b|
		comperison = _mm_cmple_ps(zero, secondSSE[i]); //returns 0 or fff
		andValue = _mm_and_ps(comperison, secondSSE[i]);
		andNotValue = _mm_andnot_ps(comperison, secondSSE[i]);
		difference = _mm_sub_ps(zero, andNotValue);
		resultSSE[i] = _mm_add_ps(resultSSE[i], difference);
		resultSSE[i] = _mm_add_ps(resultSSE[i], andValue);
	}
	return result;
}

double * AddSSE(double * first, double * second, int size)
{
	double * result = new double[size];

	__m128d * firstSSE = (__m128d *)first;
	__m128d * secondSSE = (__m128d *)second;
	__m128d * resultSSE = (__m128d *)result;
	__m128d comperison, andValue, andNotValue, difference;
	__m128d zero = _mm_setzero_pd();


	int sizeSSE = size * sizeof(first[0]) / sizeof(__m128d);

	for (int i = 0; i < sizeSSE; i++)
	{
		//get module |a|
		comperison = _mm_cmple_pd(zero, firstSSE[i]); //returns 0 or fff
		andValue = _mm_and_pd(comperison, firstSSE[i]);
		andNotValue = _mm_andnot_pd(comperison, firstSSE[i]);
		difference = _mm_sub_pd(zero, andNotValue);
		resultSSE[i] = _mm_add_pd(andValue, difference);

		//get module |b|
		comperison = _mm_cmple_pd(zero, secondSSE[i]); //returns 0 or fff
		andValue = _mm_and_pd(comperison, secondSSE[i]);
		andNotValue = _mm_andnot_pd(comperison, secondSSE[i]);
		difference = _mm_sub_pd(zero, andNotValue);
		resultSSE[i] = _mm_add_pd(resultSSE[i], difference);
		resultSSE[i] = _mm_add_pd(resultSSE[i], andValue);
		
	}
	return result;
}

template <typename T>
void СallTask2(int arraySize) 
{
	FILETIME previous, current;
	int sizeToPrint = 8;

	T * firstArray = GetRandomArray<T>(arraySize);
	T * secondArray = GetRandomArray<T>(arraySize);

	std::cout << std::endl << "arr1: ";
	Print<T>(firstArray, sizeToPrint);
	std::cout << "arr2: ";
	Print<T>(secondArray, sizeToPrint);

	//sum using usual commands
	GetSystemTimePreciseAsFileTime(&previous);
	T * res = Add<T>(firstArray, secondArray, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "usual: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<T>(res, sizeToPrint);

	//sum using simd commands
	GetSystemTimePreciseAsFileTime(&previous);
	T * resSSE = AddSSE(firstArray, secondArray, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "SSE: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<T>(resSSE, sizeToPrint);
}
#pragma endregion

#pragma region task3

template <typename T>
T * Sqrt(T * array, int size)
{
	T * result = new T[size];
	for (int i = 0; i < size; i++) 
	{
		result[i] = (T)sqrt(array[i]);
	}

	return result;
}

float * SqrtSSE(float * array, int size)
{
	float * result = new float[size];
	__m128 * arraySSE = (__m128 *)array;
	__m128 * resultSSE = (__m128 *)result;

	int sizeSSE = size * sizeof(array[0]) / sizeof(__m128);
	for (int i = 0; i < sizeSSE; i++) 
	{
		resultSSE[i] = _mm_sqrt_ps(arraySSE[i]);
	}
	return result;
}

double * SqrtSSE(double * array, int size)
{
	double * result = new double[size];
	__m128d * arraySSE = (__m128d *)array;
	__m128d * resultSSE = (__m128d *)result;

	int sizeSSE = size * sizeof(array[0]) / sizeof(__m128d);
	for (int i = 0; i < sizeSSE; i++)
	{
		resultSSE[i] = _mm_sqrt_pd(arraySSE[i]);
	}
	return result;
}

template <typename T>
void CallTask3(int arraySize) 
{
	FILETIME previous, current;
	int sizeToPrint = 8;

	T * array = GetRandomArray<T>(arraySize, 1, 17);

	std::cout << std::endl << "arr: ";
	Print<T>(array, sizeToPrint);

	//sum using usual commands
	GetSystemTimePreciseAsFileTime(&previous);
	T * res = Sqrt<T>(array, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "usual: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<T>(res, sizeToPrint);

	//sum using simd commands
	GetSystemTimePreciseAsFileTime(&previous);
	T * resSSE = SqrtSSE(array, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "SSE: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<T>(resSSE, sizeToPrint);
}
#pragma endregion

#pragma region task4

float * Multiply(float * realX, float * imaginaryX, float * realY, float * imaginaryY, int size, __out float * imaginaryResult)
{
	float * realResult = new float[size];

	for (int i = 0; i < size; i++)
	{
		realResult[i] = realX[i] * realY[i];
		realResult[i] += imaginaryX[i] * imaginaryY[i];

		imaginaryResult[i] = realX[i] * imaginaryY[i];
		imaginaryResult[i] += imaginaryX[i] * realY[i];
	}
	return realResult;
}

float * MultiplySSE(float * realX, float * imaginaryX, float * realY, float * imaginaryY, int size, __out float * imaginaryResult) 
{
	float * realResult = new float[size];
	__m128 * xRealSSE = (__m128 *)realX;
	__m128 * xImaginarySSE = (__m128 *)imaginaryX;
	__m128 * yRealSSE = (__m128 *)realY;
	__m128 * yImaginarySSE = (__m128 *)imaginaryY;
	__m128 * imaginaryResultSSE = (__m128 *)imaginaryResult;
	__m128 * realResultSSE = (__m128 *)realResult;

	int sizeSSE = size * sizeof(realX[0]) / (sizeof(__m128));
	for (int i = 0; i < sizeSSE; i++) 
	{
		realResultSSE[i] = _mm_mul_ps(xRealSSE[i], yRealSSE[i]);
		realResultSSE[i] = _mm_add_ps(realResultSSE[i], _mm_mul_ps(xImaginarySSE[i], yImaginarySSE[i]));

		imaginaryResultSSE[i] = _mm_mul_ps(xRealSSE[i], yImaginarySSE[i]);
		imaginaryResultSSE[i] = _mm_add_ps(imaginaryResultSSE[i], _mm_mul_ps(xImaginarySSE[i], yRealSSE[i]));
	}
	return realResult;
}

void CallTask4(int arraySize)
{
	FILETIME previous, current;
	int sizeToPrint = 8;

	float * xReal = GetRandomArray<float>(arraySize);
	float * xImaginary = GetRandomArray<float>(arraySize);
	float * yReal = GetRandomArray<float>(arraySize);
	float * yImaginary = GetRandomArray<float>(arraySize);
	float * imaginaryResult = new float[arraySize];

	std::cout << std::endl << "Input x: " << std::endl;
	PrintComplexValues<float>(xReal, xImaginary, sizeToPrint);
	std::cout << "Input y: " << std::endl;
	PrintComplexValues<float>(yReal, yImaginary, sizeToPrint);
	std::cout << std::endl;

	//multiply using usual commands
	GetSystemTimePreciseAsFileTime(&previous);
	float * realResult = Multiply(xReal, xImaginary, yReal, yImaginary, arraySize, __out imaginaryResult);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "usual: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	PrintComplexValues<float>(realResult, imaginaryResult, sizeToPrint);
	std::cout << std::endl;

	//multuply using simd commands
	GetSystemTimePreciseAsFileTime(&previous);
	realResult = MultiplySSE(xReal, xImaginary, yReal, yImaginary, arraySize, __out imaginaryResult);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "SSE: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	PrintComplexValues<float>(realResult, imaginaryResult, sizeToPrint);
}

#pragma endregion

#pragma region task5

__int32 * ShiftRight(__int32 * array, int size) 
{
	__int32 * result = new __int32[size];
	int mask = 1;
	int carrage = 0;

	for (int i = 0; i < size; i++) 
	{
		result[i] = (array[i] >> 1) | (carrage << 31);
		carrage = array[i] & mask;
	}
	return result;
}

__int32 * ShiftRightSSE(__int32 * array, int size) 
{
	__int32 * result = new __int32[size];
	__m128i * arraySSE = (__m128i *)array;
	__m128i * resultSSE = (__m128i *)result;

	__m128i carrage = _mm_setzero_si128();
	__m128i mask = _mm_set1_epi32(1);
	__m128i tempShiftedCarrage = _mm_setzero_si128();

	int lastBitOfPrevious128i = 0; //keeps first bit of the last 128-bit int
	int sizeSSE = size * sizeof(array[0]) / sizeof(__m128i);

	for (int i = 0; i < sizeSSE; i++)
	{
		carrage = _mm_and_si128(arraySSE[i], mask); // ....1....1....0....1 -- get all last bits of 32-bit int
		// carrage >> 32 -- values kepps in temporary array of int32 
		((__int32 *)&tempShiftedCarrage)[0] = lastBitOfPrevious128i; //....0....0....0....0 - first bit of the previous 128-bit int
		for (int j = 1; j < 4; j++) 
		{
			((__int32 *)&tempShiftedCarrage)[j] = ((__int32 *)&carrage)[j - 1]; //....0....1....1....0
			lastBitOfPrevious128i = ((__int32 *)&carrage)[j]; //1 
		}
		carrage = _mm_slli_epi32(tempShiftedCarrage, 31); //0....1.....1....0.... -- carrage == outingBits << 31
		resultSSE[i] = _mm_srai_epi32(arraySSE[i], 1); // array[i] >> 1 -- 1xxx.... => 01xxx...
		resultSSE[i] = _mm_or_si128(resultSSE[i], carrage); // array[i] || carrage ==> 0xx..0xx..0xx..0xx.. || 0...1...1...0 => 0xx..1xx..1xx..0xx...

	}
	return result;
}

void CallTask5(int arraySize)
{
	FILETIME previous, current;
	int sizeToPrint = 8;

	__int32 * array = GetRandomArrayShift<__int32>(arraySize);

	std::cout << std::endl << "Input: " << std::endl;
	Print<__int32>(array, sizeToPrint);
	std::cout << std::endl;

	//shift right using usual commands
	GetSystemTimePreciseAsFileTime(&previous);
	__int32 * result = ShiftRight(array, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "usual: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<__int32>(result, sizeToPrint);
	std::cout << std::endl;

	//multuply using simd commands
	GetSystemTimePreciseAsFileTime(&previous);
	__int32 * resultSSE = ShiftRightSSE(array, arraySize);
	GetSystemTimePreciseAsFileTime(&current);
	std::cout << "SSE: " << current.dwLowDateTime - previous.dwLowDateTime << " in 100 nanoseconds intervals" << std::endl;
	Print<__int32>(resultSSE, sizeToPrint);
}
#pragma region

int main()
{
#pragma region task1
	std::cout << "\t Task 1" << std::endl << std::endl;
	std::cout << "Processor type: "<<GetProcessorTypeName() << std::endl;
	std::cout << "Extended processor info: " << GetExtendedProcessorType() << std::endl;

	CommandType maxSupportedCommandSet = GetMaxProcessorSupportedInstructions();
	std::cout << "Max supported set of instructions (processor): " << ToString(maxSupportedCommandSet) << std::endl;
	std::cout << "AVX2 is supported by OS: " << (AVX2OSSupport() ? "true" : "false") << std::endl;
#pragma endregion

#pragma region task2
	int arraySize = 4096;
	std::cout << std::endl << "\t Task 2" << std::endl << std::endl << "Sum arrays values: z[i] = |x[i]| + |y[i]| " << std::endl;
	std::cout << "Array size: " << arraySize << std::endl << std::endl;

	std::cout << "\t __int8";
	СallTask2<__int8>(arraySize);
	std::cout << "\t __int16";
	СallTask2<__int16>(arraySize);
	std::cout << "\t __int32";
	СallTask2<int>(arraySize);
	std::cout << "\t __int64";
	СallTask2<__int64>(arraySize);
	std::cout << "\t float";
	СallTask2<float>(arraySize);
	std::cout << "\t double";
	СallTask2<double>(arraySize);
#pragma endregion

#pragma region task3
	arraySize = 64;
	std::cout << std::endl << "\t Task 3" << std::endl << std::endl << "Get sqrt" << std::endl;
	std::cout << "Array size: " << arraySize << std::endl << std::endl;

	std::cout << "\t double";
	CallTask3<double>(arraySize);

	std::cout << "\t float";
	CallTask3<float>(arraySize);
#pragma endregion

#pragma region task4

	arraySize = 64;
	std::cout << std::endl << "\t Task 4" << std::endl << std::endl << "Get sqrt" << std::endl;
	std::cout << "Array size: " << arraySize << std::endl << std::endl;

	std::cout << "\t float";
	CallTask4(arraySize);

#pragma endregion

	arraySize = 512 / sizeof(__int32);
	std::cout << std::endl << "\t Task 5" << std::endl << std::endl << "Shift right" << std::endl;
	std::cout << "Value size: " << 512 << " bits" << std::endl << std::endl;

	CallTask5(arraySize);
	return 0;
}
