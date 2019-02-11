#pragma once


/// deal with windows structure exception
class  se_exception
{
public:
  se_exception(EXCEPTION_POINTERS* exp) 
    : code_(exp->ExceptionRecord->ExceptionCode),
    addr_(exp->ExceptionRecord->ExceptionAddress),
    exp_(exp)
  {
  }
  ~se_exception() {}
  DWORD code() const { return code_; }
  PVOID addr() const { return addr_; }
  EXCEPTION_POINTERS * exp() const { return exp_; }
  const char *name() const;
  void minidumpToFile(const std::wstring &path);
	void minidump(const std::string &threadName);
public:
  static void trans_func(unsigned int u, EXCEPTION_POINTERS* exp);

private:
  DWORD code_;
  PVOID addr_;
  EXCEPTION_POINTERS *exp_;
};

class  se_translator
{
  typedef _se_translator_function TranslatorFunction;
public:
  se_translator();
  ~se_translator();

private:
  TranslatorFunction old_; 
};
