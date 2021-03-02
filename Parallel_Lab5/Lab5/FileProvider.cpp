#include "pch.h"
#include "FileProvider.h"
#include <tchar.h>
#include <windows.h>

#define AREA 1048576

FileProvider::FileProvider()
{
	this->CreateDefaultDirectory();
	_stprintf(this->fileWithPatternsPath, _T("%s//%s"), this->defaultDirectory, _T("patterns.txt"));
	_stprintf(this->fileWithTextPath, _T("%s//%s"), this->defaultDirectory, _T("text.txt"));
	_stprintf(this->logFilePath, _T("%s//%s"), this->defaultDirectory, _T("log.txt"));

	this->OpenFiles();

	if (!this->ExistsFile(&(this->textFile)))
	{
		this->InitFile(&(this->textFile));
	}
	if (!this->ExistsFile(&(this->patternsFile)))
	{
		this->InitFile(&(this->patternsFile));
	}

	this->ClearFile(&(this->logFile));
	
	this->CloseFile(&(this->textFile));
	this->CloseFile(&(this->logFile));
	this->CloseFile(&(this->patternsFile));
}

#pragma region  fileSystemManagement

void FileProvider::CreateDefaultDirectory() 
{
	_stprintf(this->defaultDirectory, _T("Default"));
	if (!DirectoryExists(this->defaultDirectory))
		CreateDirectory(this->defaultDirectory, NULL);
}

bool FileProvider::DirectoryExists(TCHAR * directoryName)
{
	DWORD type = GetFileAttributesA((LPCSTR)directoryName);
	//something is wrong with the path
	if (type == INVALID_FILE_ATTRIBUTES)
		return false;
	// this is a directory
	if (type & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	// this is not a directory
	return false;
}

bool FileProvider::ExistsFile(FILE_MAP * file)
{
	bool existsFile = file->size > 0;
	return existsFile;
}

bool FileProvider::IsFileOpen(FILE_MAP * file)
{
	bool isFileOpen = file->pointer != NULL &&
		file->hMap != INVALID_HANDLE_VALUE &&
		file->h != INVALID_HANDLE_VALUE;

	return isFileOpen;
}

bool FileProvider::OpenFile(TCHAR * path, FILE_MAP * mappedFile)
{
	HANDLE file = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_ALWAYS, 0, 0);
	if (file != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize = GetFileSize(file, nullptr);
		if (dwFileSize != INVALID_FILE_SIZE) {
			HANDLE hMap = CreateFileMapping(file, 0, PAGE_READWRITE, 0, AREA, 0);
			DWORD p = GetLastError();
			if (hMap != INVALID_HANDLE_VALUE) {
				PVOID pointer = MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
				if (pointer != NULL) {
					mappedFile->h = file;
					mappedFile->hMap = hMap;
					mappedFile->pointer = pointer;
					mappedFile->size = dwFileSize;
					return true;
				}
				else {
					CloseHandle(hMap);
				}
			}
		}
		else {
			CloseHandle(file);
		}
	}
	_tprintf(_T("The file %s is not found.\n"), path);
	return false;
}

void FileProvider::OpenFiles()
{
	OpenFile(this->fileWithPatternsPath, &(this->patternsFile));
	OpenFile(this->logFilePath, &(this->logFile));
	OpenFile(this->fileWithTextPath, &(this->textFile));
}

void FileProvider::CloseFile(FILE_MAP * mappedFile)
{
	if (mappedFile->pointer != NULL)
		UnmapViewOfFile(mappedFile->pointer);
	if (mappedFile->hMap != INVALID_HANDLE_VALUE)
		CloseHandle(mappedFile->hMap);
	if (mappedFile->h != INVALID_HANDLE_VALUE) {
		LONG filePointer = 0;
		SetFilePointer(mappedFile->h, mappedFile->size, &filePointer, FILE_BEGIN);
		SetEndOfFile(mappedFile->h);
		CloseHandle(mappedFile->h);
	}
}

void FileProvider::ClearFile(FILE_MAP * file)
{
	if(IsFileOpen(file))
	{
		file->size = 0;
		InitFile(file);
	}
}

#pragma endregion

FileProvider::~FileProvider()
{

}

void FileProvider::ReadFile(FILE_MAP * file)
{
	BYTE Buffer[100];

	PBYTE filePointer = (PBYTE)(file->pointer);
	DWORD recordsCount = 0;

	memcpy(&recordsCount, filePointer, sizeof(DWORD));
	filePointer += sizeof(DWORD);

	for (int i = 0; i < recordsCount; i++)
	{
		DWORD currentRecord = 0, recordSize = 0;
		//get message size
		memcpy(&recordSize, filePointer, sizeof(DWORD)); 
		filePointer += sizeof(DWORD);
		//get message number
		memcpy(&currentRecord, (PBYTE)filePointer, sizeof(DWORD));
		filePointer += sizeof(DWORD);
		//get message
		memcpy(&(Buffer), (PBYTE)filePointer, recordSize);
		filePointer += recordSize;
		//print message
		_tprintf(_T("%d - "), currentRecord);
		_tprintf((TCHAR *)(Buffer));
		_tprintf(_T("\n"));
	}
}

PBYTE FileProvider::SetPointer(FILE_MAP * file, int number)
{
	PBYTE pointer = NULL;
	if (IsFileOpen(file)) 
	{
		pointer = (PBYTE)file->pointer;
		DWORD statementCount = 0;
		memcpy(&statementCount, pointer, sizeof(DWORD));
		pointer += sizeof(DWORD);

		for (int i = 0; i < number; i++)
		{
			DWORD currentSize = 0;
			memcpy(&currentSize, pointer, sizeof(DWORD));
			pointer += 2 * sizeof(DWORD);

			pointer += currentSize;
		}
	}
	return pointer;
}

void FileProvider::ReadKeys()
{
	this->OpenFile(this->fileWithPatternsPath, &(this->patternsFile));
	if (this->IsFileOpen(&(this->patternsFile)))
	{
		ReadFile(&(this->patternsFile));
		this->CloseFile(&(this->patternsFile));
	}
	else
	{ 
		_tprintf(_T("The file with keys is invalid. Delete all data and try to restart the programm."));
	}
}

void FileProvider::ReadText()
{
	this->OpenFile(this->fileWithTextPath, &(this->textFile));
	if (this->IsFileOpen(&(this->textFile)))
	{
		ReadFile(&(this->textFile));
		this->CloseFile(&(this->textFile));
	}
	else
	{
		_tprintf(_T("The file with text is invalid. Delete all data and try to restart the programm."));
	}
}

void FileProvider::ReadLogs()
{
	this->OpenFile(this->logFilePath, &(this->logFile));
	if (this->IsFileOpen(&(this->logFile)))
	{
		ReadFile(&(this->logFile));
		this->CloseFile(&(this->logFile));
	}
	else
	{
		_tprintf(_T("The log file is invalid. Delete all data and try to restart the programm."));
	}
}

void FileProvider::Write(FILE_MAP * file, TCHAR * message, DWORD messageSize)
{
	if (IsFileOpen(file))
	{
		DWORD messageSizeInBytes = messageSize * sizeof(TCHAR);
		PBYTE pointer = (PBYTE)(file->pointer);

		//set pointer to the end of file
		pointer += file->size;

		//write message size
		memcpy(pointer, &messageSizeInBytes, sizeof(DWORD));
		pointer += sizeof(DWORD);
		//write message number
		memcpy(pointer, (DWORD *)(file->pointer), sizeof(DWORD));
		pointer += sizeof(DWORD);
		//write message
		memcpy(pointer, message, messageSizeInBytes);

		file->size += 2 * sizeof(DWORD) + messageSizeInBytes; //message size info (DWORD) + message number + message size

		this->UpdateIndexData(file, Add);
	}
}

void FileProvider::WriteKeys(TCHAR * message, DWORD messageSize)
{
	this->OpenFile(this->fileWithPatternsPath, &(this->patternsFile));
	if (this->IsFileOpen(&(this->patternsFile)))
	{
		this->Write(&(this->patternsFile), message, messageSize);
		this->CloseFile(&(this->patternsFile));
	}
	else
	{
		_tprintf(_T("The file with keys is invalid. The data wasn't written in the file."));
	}
}

void FileProvider::WriteText(TCHAR * message, DWORD messageSize)
{
	this->OpenFile(this->fileWithTextPath, &(this->textFile));
	if (this->IsFileOpen(&(this->textFile)))
	{
		this->Write(&(this->textFile), message, messageSize);
		this->CloseFile(&(this->textFile));
	}
	else
	{
		_tprintf(_T("The file with text is invalid. The data wasn't written in the file."));
	}
}

void FileProvider::WriteLogs(TCHAR * message, DWORD messageSize)
{
	this->OpenFile(this->logFilePath, &(this->logFile));
	if (this->IsFileOpen(&(this->logFile)))
	{
		this->Write(&(this->logFile), message, messageSize);
		this->CloseFile(&(this->logFile));
	}
	else
	{
		_tprintf(_T("The log file is invalid. The data wasn't written in the file."));
	}
}

void FileProvider::SearchData()
{
	this->OpenFile(this->fileWithPatternsPath, &(this->patternsFile));

	if (IsFileOpen(&(this->patternsFile))) 
	{
		DWORD keysCount = 0;

		PBYTE patternsPointer = (PBYTE)this->patternsFile.pointer;
		memcpy(&keysCount, patternsPointer, sizeof(DWORD));

		for (int i = 0; i < keysCount; i++)
		{
			BYTE buffer[100];
			PBYTE pointer = this->SetPointer(&(this->patternsFile), i);

			DWORD patternSize = 0;
			PBYTE pattern = (PBYTE)buffer;

			memcpy(&patternSize, pointer, sizeof(DWORD));
			pointer += sizeof(DWORD) + sizeof(DWORD); //skip message size info + messsage number

			memcpy(pattern, pointer, patternSize);

			this->FindTextByKey(pattern, patternSize);
			this->ClearBuffer(&buffer[0], 100);
		}

		this->CloseFile(&(this->patternsFile));
	}
}

void FileProvider::FindTextByKey(BYTE * key, DWORD size)
{
	BYTE buffer[100];
	this->OpenFile(this->fileWithTextPath, &(this->textFile));

	if (IsFileOpen(&(this->textFile)))
	{
		DWORD index = -1;
		DWORD statementCount = 0;

		PBYTE textPointer = (PBYTE)this->textFile.pointer;
		memcpy(&statementCount, textPointer, sizeof(DWORD));

		for (int i = 0; i < statementCount; i++)
		{
			BYTE buffer[100];

			PBYTE pointer = this->SetPointer(&(this->textFile), i);
			DWORD currentStatement = 0, textSize = 0;

			memcpy(&textSize, pointer, sizeof(DWORD));
			pointer += sizeof(DWORD);

			memcpy(&currentStatement, pointer, sizeof(DWORD));
			pointer += sizeof(DWORD);

			memcpy((PBYTE)buffer, pointer, textSize);

			bool isEqual = this->IsEqual(key, buffer);

			if (isEqual)
			{
				index = currentStatement;
			}
		}

		PBYTE result = &(buffer[size]);
		for (int i = 0; i < size; i++)
		{
			buffer[i] = (BYTE)key[i];
		}

		if ((int)index < 0)
		{
			_stprintf((TCHAR *)result, _T(" is NOT FOUND."));
			this->WriteLogs((TCHAR *)buffer, size + 14);
			
		}
		else
		{
			_stprintf((TCHAR *)result, _T(" is FOUND at index %d"), index);
			DWORD messageSize = (size + 19) + sizeof(DWORD) * 3;
			this->WriteLogs((TCHAR *)buffer, messageSize);
		}

		this->CloseFile(&(this->textFile));
	}
}

void FileProvider::InitFile(FILE_MAP * file)
{
	if (IsFileOpen(file))
	{
		DWORD count = 0;
		PBYTE pointer = (PBYTE)file->pointer;
		memcpy(pointer, &count, sizeof(DWORD));
		file->size += sizeof(DWORD);
	}
}

void FileProvider::UpdateIndexData(FILE_MAP * file, Operation operation)
{
	if (IsFileOpen(&(this->patternsFile)))
	{
		DWORD previousNumberOfRecords = 0;
		memcpy(&previousNumberOfRecords, file->pointer, sizeof(DWORD));
		switch (operation)
		{
		case Delete:
			previousNumberOfRecords--;
			break;
		case Add:
			previousNumberOfRecords++;
			break;
		}

		if (previousNumberOfRecords >= 0)
		{
			PBYTE pointer = (PBYTE)file->pointer;
			memcpy(pointer, &previousNumberOfRecords, sizeof(DWORD));
		}
	}
}

void FileProvider::ClearBuffer(BYTE * buffer, DWORD size) 
{
	for (DWORD i = 0; i < size; i++) 
	{
		buffer[i] = '\0';
	}
}

bool FileProvider::IsEqual(BYTE * str1, BYTE * str2)
{
	bool isEqual = true;
	int i = 0;
	for (i = 0; str1[i] != '\0' || str2[i] != '\0'; i++)
	{
		isEqual = str1[i] == str2[i];
		if (!isEqual)
			break;
	}

	return isEqual && str1[i] == str2[i];
}