#ifndef COMMANDLINE_H
#define COMMANDLINE_H


/* Forward declarations */
struct CommandLine_t;

#define CommandLine_MaxNbOfArgs               (20)
#define CommandLine_MaxLengthOfKeyWord        (30)

#if 1
#define CommandLine_New()              CommandLine_t::New()
#define CommandLine_Create(argc,argv)  CommandLine_t::Create(argc,argv)
#define CommandLine_Delete(cmd)        (cmd)->Delete()
#else
#define CommandLine_Create(argc,argv)  (new CommandLine_t(argc,argv))
#define CommandLine_Delete(cmd)        (delete (CommandLine_t*) cmd)
#endif


#define CommandLine_GetNbOfArgs(cmd)           ((cmd)->GetNbOfArgs())
#define CommandLine_GetArg(cmd)                ((cmd)->GetArg())

#define CommandLine_SetNbOfArgs(cmd,argc)      ((cmd)->SetNbOfArgs(argc))
#define CommandLine_SetArg(cmd,argv)           ((cmd)->SetArg(argv))

#define CommandLine_Set(cmd,...)               ((cmd)->Set(__VA_ARGS__))


#include <stdio.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <sstream>

struct CommandLine_t {
  private:
  int    _argc ;
  char** _argv ;

  public:
  size_t GetCapacity(){return(CommandLine_MaxNbOfArgs);}
  CommandLine_t();
  CommandLine_t(int,char**);
  ~CommandLine_t();

  /* The getters */
  int GetNbOfArgs(){return(_argc);}
  char** GetArg(){return(_argv);}

  /* The setters */
  void SetNbOfArgs(int argc){_argc = argc;}
  void SetArg(char** argv){_argv = argv;}

  template<typename... Args>
  void Set(Args...);

  void Set(std::vector<std::string>& args) {
    Set(static_cast<std::vector<std::string> const&>(args));
  }
  void Set(std::vector<std::string> const& args) {
    size_t argc = args.size();

    if(_argc+argc > GetCapacity()) {
      throw std::length_error("CommandLine_t: maximum nb of arguments reached");
    }

    for(size_t i = 0 ; i < argc ; i++) {
      size_t len = args[i].length() + 1;

      if(len > CommandLine_MaxLengthOfKeyWord) {
        throw std::length_error("CommandLine_t: maximum length of keyword reached");
      }

      strcpy(_argv[_argc+i],args[i].c_str());
    }

    _argc += argc;
  }

  void Set(char* args) {
    Set(static_cast<char const*>(args));
  }
  void Set(char const* args) {
    auto split = [](char const* s) {
      std::stringstream ss(s);
      std::vector<std::string> v;
      std::string token;
      while(ss >> token) v.push_back(token);
      return v;
    };

    Set(split(args));
  }

  void Set(int const& argc,char** argv) {
    Set(argc,static_cast<char const* const*>(argv));
  }
  void Set(int const& argc,char const* const* argv) {
    for(int i = 0 ; i < argc ; i++) {
      Set((char const*)argv[i]);
    }
  }

  static CommandLine_t* New();
  static CommandLine_t* Create(int,char**);
  void Delete();
} ;


#include "Mry.h"

  inline CommandLine_t::CommandLine_t() : _argc(0) {
    size_t n    = CommandLine_MaxNbOfArgs;
    size_t len  = CommandLine_MaxLengthOfKeyWord + 1;

    _argv = (char**) Mry_New(char*,n);

    for(size_t i = 0 ; i < n ; i++) {
      _argv[i] = (char*) Mry_New(char,len);
    }
  }

  inline CommandLine_t::CommandLine_t(int argc,char** argv) : CommandLine_t() {Set(argc,argv);}

  inline CommandLine_t::~CommandLine_t() {
    if(_argv) {
      size_t n = GetCapacity();

      for(size_t i = 0 ; i < n ; i++) {
        if(_argv[i]) {
          Mry_Free(_argv[i]);
          _argv[i] = nullptr;
        }
      }
      Mry_Free(_argv);
      _argv = nullptr;
    }
  }

  inline CommandLine_t* CommandLine_t::New() {
    CommandLine_t* cmd = (CommandLine_t*) Mry_New(CommandLine_t);

    CommandLine_SetNbOfArgs(cmd,0);

    {
      size_t len  = CommandLine_MaxLengthOfKeyWord + 1;
      size_t n    = CommandLine_MaxNbOfArgs;
      char** argv = (char**) Mry_New(char*,n);

      for(size_t i = 0 ; i < n ; i++) {
        argv[i] = (char*) Mry_New(char,len);
      }

      CommandLine_SetArg(cmd,argv);
    }
    
    return(cmd) ;
  }

  inline CommandLine_t* CommandLine_t::Create(int argc,char** argv) {
    CommandLine_t* cmd = CommandLine_New();

    CommandLine_Set(cmd,argc,argv);
    
    return(cmd) ;
  }

  inline void CommandLine_t::Delete() {
    if(_argv) {
      size_t n = GetCapacity();

      for(size_t i = 0 ; i < n ; i++) {
        if(_argv[i]) {
          Mry_Free(_argv[i]);
          _argv[i] = nullptr;
        }
      }

      Mry_Free(_argv);
      _argv = nullptr;
    }
  }


#endif
