///////////////////////////////////////////////////////////////////////////////////////
//
//	FilePath.h
//	Simple wrapper for dealing with file paths, extensions, root name, etc.
//	
//	Authors: Chris Peters, Benjamin Ellinger
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

class FilePath
{
public:
	FilePath();
	FilePath(std::string file);
	FilePath& operator=(std::string file);
	void SetFilePath(std::string file);
	std::string GetFilePathWithNewExtension(std::string newExtension);

	//Extension of file which may be empty includes the period
	//such as ".png", ".txt"
	std::string Extension;
	//The root filename of the file without the extension or path
	//and in lower case For "C:\Data\FileName.txt" "filename"
	std::string FileName;
	//The path the file is locate at
	//For"C:\Data\FileName.txt" "C:\data\"
	std::string FullDirectory;
	//The full path including the filename
	//For "C:\Data\FileName.txt" "c:\data\filename.txt"
	std::string FullPath;

};