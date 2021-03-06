#include "pch.h"
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#include <chrono>
#include <omp.h>
#include "Matrix.h"

///task 1
///convert time_t to SystemTime
#pragma region task1

void UnixTimeToSystemTime(__out PSYSTEMTIME systemTime)
{
	//get UnixTime
	time_t unixTime = time(NULL);//returns number of second intervals since January 1, 1970 UTC
	LONGLONG offset = 116444736000000000; //100-nanoseconds from Jan 1, 1601 to Jan 1, 1970
	DWORD nanosecondsInSecond = 10000000;
	LONGLONG time = Int32x32To64(unixTime, nanosecondsInSecond) + offset;

	//unixTime to FILETIME
	FILETIME fileTime; //number of 100-nanosecond intervals since January 1, 1601 UTC
	fileTime.dwLowDateTime = (DWORD)time;
	fileTime.dwHighDateTime = time >> 32;

	//FILETIME to SYSTEMTIME
	FileTimeToSystemTime((LPFILETIME)&fileTime, systemTime);
}

#pragma endregion

///task 2
///define time accuracy
#pragma region task2

///time() -returns calendar system time in seconds
time_t GetTimeAccuracy() 
{
	time_t currentTime, previousTime;
	time_t timeDifference = 0;

	while(timeDifference == 0.0) 
	{
		previousTime = time(NULL);
		currentTime = time(NULL);
		timeDifference = currentTime - previousTime;
	}

	return timeDifference;
}

///clock() - returns time in milliseconds from the programm start
clock_t GetClockAccuracy() 
{
	clock_t start, stop;
	clock_t difference = 0;
	while (difference == 0.0) 
	{
		start = clock();
		stop = clock();
		difference = stop - start;
	}
	return difference;
}

DWORD GetSystemTimeAsFileTimeAccuracy() 
{
	//FILETIME keeps time in 100 nanoseconds
	FILETIME prevoius, current;
	DWORD differenceInTicks = 0;
	while (differenceInTicks == 0) {
		GetSystemTimeAsFileTime(&prevoius);
		GetSystemTimeAsFileTime(&current);
		differenceInTicks = current.dwLowDateTime - prevoius.dwLowDateTime;
	}
	return differenceInTicks;
}

///GetSystemTimePreciseAsFileTime(PFILETIME) - returns system time the highest possible level of precision(<1us)
DWORD GetSystemTimePreciseAsFileTimeAccuracy()
{
	//FILETIME keeps time in 100 nanoseconds
	FILETIME prevoius, current;
	DWORD differenceInTicks = 0;
	while (differenceInTicks == 0) {
		GetSystemTimePreciseAsFileTime(&prevoius);
		GetSystemTimePreciseAsFileTime(&current);
		differenceInTicks = current.dwLowDateTime - prevoius.dwLowDateTime;
	}
	return differenceInTicks;
}

///GetTickCount() - returns time in milliseconds from the programm start
DWORD GetTickCountAccuracy() 
{
	DWORD prevoius, current;
	DWORD difference = 0;
	while (difference == 0) 
	{
		prevoius = GetTickCount();
		current = GetTickCount();
		difference = current - prevoius;
	}
	return difference;
}

///__asm rdtsc - кол-во тактов CPU
DWORD GetRdtscAccuracy() 
{
	DWORD difference = 0;
	while (difference == 0)
	{
		__asm rdtsc;
		__asm mov[difference], eax;
		__asm rdtsc;
		__asm sub eax, [difference];
		__asm mov[difference], eax;
	}
	return difference;
}
///QueryPerformanceCounter - returns текущее значение точного счетчика высокого разрешения, в тиках
DWORD GetQueryPerformanceCounterAccuracy() 
{
	LARGE_INTEGER previous, current;
	DWORD difference = 0;
	while (difference == 0)
	{
		QueryPerformanceCounter(__out &previous);
		QueryPerformanceCounter(__out &current);
		difference = current.LowPart - previous.LowPart;
	}

	return difference;
}

/// class chrono - keeps durations, time and moments and additional functions to work with time
DWORD GetChronoAccuracy()
{
	std::chrono::time_point<std::chrono::system_clock> previous, current;
	DWORD difference = 0;
	while (difference == 0)
	{
		previous = std::chrono::system_clock::now();
		current = std::chrono::system_clock::now();
		difference = std::chrono::duration_cast<std::chrono::nanoseconds> (current - previous).count();
	}

	return difference;
}

double GetOmp_get_wtimeAccuracy()
{
	double previous, current;
	double difference = 0.0;
	while (difference == 0.0)
	{
		previous = omp_get_wtime();
		current = omp_get_wtime();
		difference = current - previous;
	}

	return difference;
}

#pragma endregion

///task 3
///використання функцій __rdtsc та QueryPerformanceCounter для визначення часу 
///додавання масиву чисел розміром 1000 елементів
#pragma region task3
int array1000 [1000];
int ARRAY1000_SIZE = 1000;

void Fill(int * array, int size) 
{
	for (int i = 0; i < size; i++) 
	{
		array[i] = rand() % 20 + 1;
	}
}

DWORD GetSum(int * array, int size) 
{
	DWORD sum = 0;
	for (int i = 0; i < size; i++) 
	{
		sum += array[i];
	}
	return sum;
}

DWORD GetRdtscTimeArrayAdd(int * array, int size) 
{
	DWORD difference = 0;
	__asm rdtsc;
	__asm mov[difference], eax;
	DWORD sum = GetSum(array, size);
	__asm rdtsc;
	__asm sub eax, [difference];
	__asm mov[difference], eax;
	_tprintf(_T("Ignore sum: %d\n"), sum);
	return difference;
}

DWORD GetQueryPerformanceCounterTimeArrayAdd(int * array, int size) 
{
	LARGE_INTEGER previous, current;
	DWORD difference = 0;
	QueryPerformanceCounter(__out &previous);
	DWORD sum = GetSum(array, size);
	QueryPerformanceCounter(__out &current);
	difference = current.LowPart - previous.LowPart;
	_tprintf(_T("Ignore sum: %d\n"), sum); 
	return difference;
}

#pragma endregion

///task4
///масиву чисел розміром 100000, 200000, 300000 елементів використайте абсолютний та відносний вимір часу 
#pragma region task4
int array100000 [100000];
int array200000 [200000];
int array300000 [300000];

int ARRAY100000_SIZE = 100000;
int ARRAY200000_SIZE = 200000;
int ARRAY300000_SIZE = 300000;

//absolute time measures by function omp_get_wtime() in seconds
double GetAbsoluteTimeAdd(int * array, int size) 
{
	double previous, current;
	double difference = 0.0;

	previous = omp_get_wtime();
	DWORD sum = GetSum(array, size);
	current = omp_get_wtime();
	difference = current - previous;

	_tprintf(_T("Ignore sum: %d\n"), sum);

	return difference;
}

//relative time measures by function GetTickCount
DWORD GetCountOfLoops(int * array, int size, DWORD milliseconds)
{
	DWORD start, current, sum;
	DWORD difference = 0;
	DWORD loopsCount = 0;
	start = GetTickCount();

	while (difference < milliseconds)
	{
		current = GetTickCount();
		sum = GetSum(array, size);
		loopsCount++;
		difference = current - start;
	}

	_tprintf(_T("Ignore sum: %d\n"), sum);

	return loopsCount;
}

#pragma endregion

///task5
#pragma region task5

void InitMatrix(__int8 ** array, int size)
{
	for (int i = 0; i < size; i++)
	{
		array[i] = new __int8[size];
	}
}

void FillMatrix(__int8 ** array, int size)
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			array[i][j] = rand() % 5 + 1;
		}
	}
}

void MultiplyMatrix(__int8 ** left, __int8 ** right, int size1, int size2, __out __int8 ** result)
{
	int size_result = (size1 < size2) ? size1 : size2;

	for (int i = 0; i < size_result; i++)
	{
		for (int j = 0; j < size_result; j++)
		{
			result[i][j] = 0;
			for (int k = 0; k < size_result; k++)
			{
				result[i][j] += left[i][k] * right[k][j];
			}
		}
	}
}

clock_t GetTimeMultiplyObjects(int size) 
{
	clock_t previous, current;
	//init matrix
	Matrix * m1 = new Matrix(size);
	Matrix * m2 = new Matrix(size);
	//fill matrix
	m1->Fill();
	m2->Fill();
	previous = clock();
	//multiply matrix
	Matrix result = *m1 * *m2;
	current = clock();
	result.Print();
	return current - previous;
}

clock_t GetTimeMultiplyArrays(__int8 ** left, __int8 ** right, int size)
{
	clock_t previous, current;
	__int8 ** result = new __int8 *[size];
	InitMatrix(result, size);
	//fill matrix
	FillMatrix(left, size);
	FillMatrix(right, size);

	previous = clock();
	MultiplyMatrix(left, right, size, size, result);
	current = clock();
	result[0][6] += 7;
	_tprintf(_T("Size (array): %d x %d\nFirst element: %d\n"), size, size, (int)result[0][6] );
	return current - previous;
}

#pragma endregion

int main()
{
#pragma region task1
	_tprintf(_T("\t\tTASK 1\n\n"));

	SYSTEMTIME systemTime; //Greenwich Mean Time
	UnixTimeToSystemTime((PSYSTEMTIME)&systemTime);
	_tprintf(_T("System time: %d-%02d-%02d %02d:%02d:%02d.%03d\n"),
		systemTime.wYear,
		systemTime.wMonth,
		systemTime.wDay,
		systemTime.wHour,
		systemTime.wMinute,
		systemTime.wSecond,
		systemTime.wMilliseconds);
#pragma endregion

#pragma region task2
	_tprintf(_T("\n\t\tTASK 2\n\n"));

	//using time() --> time_t
	time_t timeDifference = GetTimeAccuracy();
	_tprintf(_T("Time: %d seconds\n"), timeDifference);

	//using clock() --> clock_t -- clocks per second
	clock_t clockDifference = GetClockAccuracy();
	_tprintf(_T("Clock: %d milliseconds\n"), clockDifference);

	//using GetSystemTimeAsFileTime()
	DWORD SystemTimeAsFileTimeDifference = GetSystemTimeAsFileTimeAccuracy();
	_tprintf(_T("SystemTimeAsFileTime: %d in 100 nanoseconds\n"), SystemTimeAsFileTimeDifference);

	//using GetSystemTimePreciseAsFileTime()
	DWORD SystemTimePreciseAsFileTimeDifference = GetSystemTimeAsFileTimeAccuracy();
	_tprintf(_T("SystemTimePreciseAsFileTime: %d in 100 nanoseconds\n"), SystemTimePreciseAsFileTimeDifference);

	//using GetTickCount()
	DWORD TickCountDifference = GetSystemTimeAsFileTimeAccuracy();
	_tprintf(_T("GetTickCount: %d milliseconds\n"), TickCountDifference);

	//using __asm rdtsc
	DWORD RdtscDifference = GetRdtscAccuracy();
	_tprintf(_T("__asm rdtsc: %d tacts\n"), RdtscDifference);

	//using QueryPerformanceCounter()
	DWORD QueryPerformanceCounterDifference = GetQueryPerformanceCounterAccuracy();
	_tprintf(_T("QueryPerformanceCounter: %d ticks\n"), QueryPerformanceCounterDifference);

	//using class chrono
	DWORD ChronoDifference = GetChronoAccuracy();
	_tprintf(_T("Chrono: %d nanoseconds\n"), ChronoDifference);

	//using omp_get_wtime
	double omp_get_wtimeDifference = GetOmp_get_wtimeAccuracy();
	_tprintf(_T("omp_get_wtime: %f seconds\n"), omp_get_wtimeDifference);
#pragma endregion

#pragma region task3
	_tprintf(_T("\n\t\tTASK 3\n\n"));

	//init array and random 1000 elements
	Fill(&array1000[0], ARRAY1000_SIZE);
	//get _rdtsc time for 1000 elements add GetQueryPerformanceCounterTimeArrayAdd
	DWORD RdtscTime = GetRdtscTimeArrayAdd(&array1000[0], ARRAY1000_SIZE);
	_tprintf(_T("Add 1000 elements (__asm rdtsc): %d tacts\n"), RdtscTime);

	//get QueryPerformanceCounter time for 1000 elements add
	DWORD QueryPerformanceCounterTime = GetQueryPerformanceCounterTimeArrayAdd(&array1000[0], ARRAY1000_SIZE);
	_tprintf(_T("Add 1000 elements (QueryPerformanceCounter): %d ticks\n"), QueryPerformanceCounterTime);
#pragma endregion

#pragma region task4
	_tprintf(_T("\n\t\tTASK 4\n\n"));

	Fill(&array100000[0], ARRAY100000_SIZE);
	Fill(&array200000[0], ARRAY200000_SIZE);
	Fill(&array300000[0], ARRAY300000_SIZE);
	int duration = 2000; //milliseconds

	//array100000
	double Absolute1 = GetAbsoluteTimeAdd(&array100000[0], ARRAY100000_SIZE);
	DWORD Relative1 = GetCountOfLoops(&array100000[0], ARRAY100000_SIZE, duration);

	_tprintf(_T("Absolute 100 000 elements (ticks): %f ticks\n"), Absolute1);
	_tprintf(_T("Relative 100 000 elements (loops): %d loops (2 sec)\n\n"), Relative1);

	//array200000
	double Absolute2 = GetAbsoluteTimeAdd(&array200000[0], ARRAY200000_SIZE);
	DWORD Relative2 = GetCountOfLoops(&array200000[0], ARRAY200000_SIZE, duration);

	_tprintf(_T("Absolute 200 000 elements (ticks): %f ticks\n"), Absolute2);
	_tprintf(_T("Relative 200 000 elements (loops): %d loops (2 sec)\n\n"), Relative2);

	//array300000
	double Absolute3 = GetAbsoluteTimeAdd(&array200000[0], ARRAY300000_SIZE);
	DWORD Relative3 = GetCountOfLoops(&array200000[0], ARRAY300000_SIZE, duration);

	_tprintf(_T("Absolute 300 000 elements (ticks): %f ticks\n"), Absolute3);
	_tprintf(_T("Relative 300 000 elements (loops): %d loops (2 sec)\n\n"), Relative3);

	//rations
	///Absolute ratio
	_tprintf(_T("Absolute:\n"));
	if (Absolute2 != 0.0) 
	{
		_tprintf(_T("\tT(200 000) / T(100 000) = %f \n"), Absolute2 / Absolute1);
		_tprintf(_T("\tT(300 000) / T(100 000) = %f \n"), Absolute3 / Absolute1);
	}
	else
	{
		_tprintf(_T("\tRatio can't be found: T(100 000) is equal to %f\n"), Absolute1);
	}

	///Relative ratio
	_tprintf(_T("Relative:\n"));
	if (Relative2 != 0.0)
	{
		_tprintf(_T("\tT(300 000) / T(100 000) = %f \n"), (double)Relative3 / (double)Relative1);
		_tprintf(_T("\tT(200 000) / T(100 000) = %f \n"), (double)Relative2 / (double)Relative1);
	}
	else
	{
		_tprintf(_T("\tRatio can't be found: T(100 000) is equal to %d\n"), Relative1);
	}
	
#pragma endregion

#pragma region task5
	_tprintf(_T("\n\t\tTASK 5\n\n"));
	//int ** array512 = new int *[512];
	__int8 ** array1024 = new __int8 *[1024];
	//int ** array2048 = new int *[2048];

	//InitMatrix(array512, 512);
	InitMatrix(array1024, 1024);
	//InitMatrix(array2048, 2048);
	//objects
	//time_t timeObjects512 = GetTimeMultiplyObjects(512);
	clock_t timeObjects1024 = GetTimeMultiplyObjects(1024);
	//time_t timeObjects2048 = GetTimeMultiplyObjects(2048);

	//arrays
	//time_t timeArrays512 = GetTimeMultiplyArrays(array512, array512, 512);
	clock_t timeArrays1024 = GetTimeMultiplyArrays(array1024, array1024, 1024);
	//time_t timeArrays2048 = GetTimeMultiplyArrays(array2048, array2048, 2048);

	_tprintf(_T("Objects:\n"));
	//_tprintf(_T("\t%dx%d multiply (ticks): %d seconds\n"), 512, 512, timeObjects512);
	_tprintf(_T("\t%dx%d multiply (ticks): %d milliseconds\n"), 1024, 1024, timeObjects1024);
	//_tprintf(_T("\t%dx%d multiply (ticks): %d seconds\n"), 2048, 2048, timeObjects2048);

	_tprintf(_T("\nArrays:\n"));
	//_tprintf(_T("\t%dx%d multiply (ticks): %d seconds\n"), 512, 512, timeArrays512);
	_tprintf(_T("\t%dx%d multiply (ticks): %d milliseconds\n"), 1024, 1024, timeArrays1024);
	//_tprintf(_T("\t%dx%d multiply (ticks): %d seconds\n"), 2048, 2048, timeArrays2048);

#pragma endregion


		return 0;
}


