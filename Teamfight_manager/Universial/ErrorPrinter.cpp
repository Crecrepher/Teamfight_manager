#include "stdafx.h"
#include "ErrorPrinter.h"

void ErrorPrinter::isUnloadAll()
{
	if (PrinterOn)
	{
		std::cout << "flush All maps. woosh! :) " << std::endl;
	}
}

void ErrorPrinter::isUnload(const std::string path)
{
	if (PrinterOn)
	{
		std::cout << "flush the map : " << path << std::endl;
	}
}

void ErrorPrinter::GetFailCheck(const std::string& id)
{
	std::cout << "error! nullptr map of :" << id << std::endl;
}
