#include <assert.h>
#include <cstdarg>
#include <iostream>
#include <vector>
#include <dmlite/cpp/utils/urls.h>


int Validate(const std::string& path, int n, ...)
{
  std::vector<std::string> components = dmlite::Url::splitPath(path);
  va_list check;
  bool    failed = false;

  std::cout << "- Checking " << path;

  int i;
  std::vector<std::string>::const_iterator j;
  va_start(check, n);
  for (i = 0, j = components.begin(); i < n && j != components.end(); ++i, ++j) {
    const char *c = va_arg(check, const char*);
    if (*j != c) {
      failed = true;
      std::cout << std::endl << "\tExpected '" << c << "' at position " << i
                << ", got '" << *j << "'" << std::endl;
    }
  }
  va_end(check);

  if (j != components.end()) {
    failed = true;
    std::cout << std::endl << "\tThe returned list is bigger!" << std::endl;
  }
  else if (i < n) {
    failed = true;
    std::cout << std::endl << "\tThe returned list is smaller! Got "
              << components.size() << " while expecting " << n << std::endl;
  }

  if (!failed)
    std::cout << "\t[OK]" << std::endl;

  return failed ? 1 : 0;
}



int ValidateNormalization(const std::string& original, const std::string &expected)
{
  std::string got = dmlite::Url::normalizePath(original);
  
  std::cout << "- Checking normalization of " << original;
  
  if (got == expected) {
    std::cout << "\t[OK]" << std::endl;
    return 0;
  }
  else {
    std::cout << std::endl << "\t Expected '" << expected
              << "', got '" << got << "'"
              << std::endl;
    return 1;
  }
}



int main(int argc, char **argv)
{
  int r = 0;

  r += Validate("", 0);
  r += Validate("no-root", 1, "no-root");
  r += Validate("/regular", 2, "/", "regular");
  r += Validate("/two/levels", 3, "/", "two", "levels");
  r += Validate("/three/levels/without", 4, "/", "three", "levels", "without");
  r += Validate("/three/levels/slash/", 4, "/", "three", "levels", "slash");
  r += Validate("////several////slashes//////right//", 4, "/", "several", "slashes", "right");
  
  // Check normalization too
  r += ValidateNormalization("/", "/");
  r += ValidateNormalization("////", "/");
  r += ValidateNormalization("/something/else", "/something/else");
  r += ValidateNormalization("/with/slash/", "/with/slash/");
  r += ValidateNormalization("/multiple///slashes//", "/multiple/slashes/");
  r += ValidateNormalization("relative//path", "relative/path");
  
  // Check join
  std::string original = "/sample/path/file";
  std::vector<std::string> components = dmlite::Url::splitPath(original);
  std::string result = dmlite::Url::joinPath(components);
  
  if (result == original)
    std::cout << "- joinPath\t[OK]" << std::endl;
  else
    std::cout << "- joinPath: expected " << original << " got " << result << std::endl;
    
  return r;
}
