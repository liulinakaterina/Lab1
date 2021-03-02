#pragma once
#include <tchar.h>
#include <windows.h>

#ifdef _MSC_VER
#define STRICT
#define _WIN32_WINNT 0x0500
#endif

#ifdef _MSC_VER
#include <stdlib.h>
#else
#include <dir.h>
#endif

class FileProvider
{
	typedef struct {
		HANDLE h;
		HANDLE hMap;
		PVOID pointer;
		DWORD size;
	} FILE_MAP;

	enum Operation {
		Delete,
		Add
	};

public:
	FileProvider();
	~FileProvider();
	void ReadKeys();
	void ReadText();
	void ReadLogs();
	void WriteKeys(TCHAR * message, DWORD size);
	void WriteText(TCHAR * message, DWORD size);
	void SearchData();
	
private:
	TCHAR defaultDirectory[40];
	TCHAR fileWithPatternsPath[40];
	TCHAR fileWithTextPath[40];
	TCHAR logFilePath[40];

	FILE_MAP logFile;
	FILE_MAP textFile;
	FILE_MAP patternsFile;

	void ClearFile(FILE_MAP * file);
	void CloseFile(FILE_MAP * mappedFile);
	void CreateDefaultDirectory();
	bool DirectoryExists(TCHAR * directoryName);
	bool ExistsFile(FILE_MAP * file);
	bool IsFileOpen(FILE_MAP * file);
	void FindTextByKey(PBYTE key, DWORD size);
	bool OpenFile(TCHAR * path, FILE_MAP * mappedFile);
	void OpenFiles();
	void ReadFile(FILE_MAP * file);
	PBYTE SetPointer(FILE_MAP * file, int number);
	void InitFile(FILE_MAP * file);
	void Write(FILE_MAP * file, TCHAR * message, DWORD size);
	void WriteLogs(TCHAR * message, DWORD size);
	void UpdateIndexData(FILE_MAP * file, Operation operation);
	void ClearBuffer(BYTE * buffer, DWORD size);
	bool IsEqual(BYTE * str1, BYTE * str2);
};

