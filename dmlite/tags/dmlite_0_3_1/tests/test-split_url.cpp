#include <assert.h>
#include <dmlite/cpp/utils/dm_urls.h>
#include <iostream>
#include <string.h>



inline int Validate(const char* uri,
                    const char* scheme, const char* host, unsigned port,
                    const char* path, const char* query = NULL)
{
  Url  parsed;
  bool failed = false;

  // Log
  std::cout << "- Checking " << uri;

  parsed = dmlite::splitUrl(uri);

  // And check it is like it should
  if (parsed.port != port) {
    std::cout << std::endl
              << "\tExpected port '" << port
              << "', got '" << parsed.port << "'";
    failed = true;
  }
  if (strcmp(parsed.scheme, scheme) != 0) {
    std::cout << std::endl
              << "\tExpected scheme '" << scheme
              << "', got '" << parsed.scheme << "'";
    failed = true;
  }
  if (strcmp(parsed.host, host) != 0) {
    std::cout << std::endl
              << "\tExpected host '" << host
              << "', got '" << parsed.host << "'";
    failed = true;
  }
  if (strcmp(parsed.path, path) != 0) {
    std::cout << std::endl
              << "\tExpected path '" << path
              << "', got '" << parsed.path << "'";
    failed = true;
  }
  if (query != NULL && strcmp(parsed.query, query) != 0) {
    std::cout << std::endl
              << "\tExpected query '" << query
              << "', got '" << parsed.query << "'";
    failed = true;
  }

  if (!failed)
    std::cout << "\t[OK]" << std::endl;
  else
    std::cout << std::endl;

  return failed ? 1 : 0;
}



int main(int argc, char **argv)
{
  int r = 0;

  r += Validate("https://something.else.com:443/mypath?query",
                "https", "something.else.com", 443, "/mypath", "query");
  r += Validate("http://something.else.com:/mypath",
                "http", "something.else.com", 0, "/mypath");
  r += Validate("/path",
                "", "", 0, "/path");
  r += Validate("host:85/path",
                "", "host", 85, "/path");
  r += Validate("host:/path",
                "", "host", 0, "/path");
  r += Validate("file:///tmp/file",
                "file", "", 0, "/tmp/file");
  r += Validate("file:////tmp/file",
                "file", "", 0, "//tmp/file");
  r += Validate("host-with-hyphen.cern.ch:/scratch/file",
                "", "host-with-hyphen.cern.ch", 0, "/scratch/file");
  return r == 0;
}
