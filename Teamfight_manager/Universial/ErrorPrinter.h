#pragma once
#include "Singleton.h"

class ErrorPrinter : public Singleton<ErrorPrinter>
{
	friend Singleton<ErrorPrinter>;

private:
	ErrorPrinter() = default;
	virtual	~ErrorPrinter() = default;

	bool PrinterOn = true;
//******************************
//		true to Error check
//******************************

public:
	void isUnloadAll();
	void isUnload(const std::string path);
	void GetFailCheck(const std::string& id);
};

#define ERROR_PRINTER (ErrorPrinter::Instance())

