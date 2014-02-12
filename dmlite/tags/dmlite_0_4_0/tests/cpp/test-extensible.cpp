#include <algorithm>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cstring>
#include <dmlite/cpp/utils/extensible.h>

#include "dmlite/cpp/inode.h"

#define REMOVE_SPACES(str) str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());

class TestExtensible: public CppUnit::TestFixture {
  public:
    void testRegular()
    {
      dmlite::Extensible ext;
      
      // Empty
      CPPUNIT_ASSERT_EQUAL(false, ext.hasField("something"));
      
      // Put a string, get a string
      ext["something"] = std::string("else");
      CPPUNIT_ASSERT_EQUAL(true, ext.hasField("something"));
      CPPUNIT_ASSERT_EQUAL(std::string("else"), ext.getString("something"));
      CPPUNIT_ASSERT_EQUAL((size_t)1, ext.size());
    }
    
    void testStrings()
    {
      dmlite::Extensible ext;
      
      // Put a const char*, get a string
      const char* cvar = "else";
      ext["something"] =  cvar;
      CPPUNIT_ASSERT_EQUAL(std::string("else"), ext.getString("something"));
      
      // Put a char*, get a string
      const char* cvar2 = "value";
      char var[sizeof(cvar2)];
      std::strcpy(var, cvar2);
      ext["second"] = static_cast<char*>(var);
      CPPUNIT_ASSERT_EQUAL(std::string("value"), ext.getString("second"));
    }
    
    void testSerialize()
    {
      dmlite::Extensible ext;
      const char* cvar = "old-style";
      
      ext["string"]   = std::string("careful-with-\\scape\"seque/nces\x04");
      ext["c-string"] = cvar;
      ext["int"]      = 42;
      ext["unsigned"] = 7u;
      ext["float"]    = 5.2f;
      ext["bool"]     = true;
      
      std::string expected("{\
\"bool\": true,\
\"c-string\": \"old-style\",\
\"float\": 5.2,\
\"int\": 42,\
\"string\": \"careful-with-\\\\scape\\\"seque\\/nces\\u0004\",\
\"unsigned\": 7\
}");
      
      std::string got = ext.serialize();
      
      // Remove spaces, since they are not relevant, before comparison
      REMOVE_SPACES(expected);
      REMOVE_SPACES(got);
      CPPUNIT_ASSERT_EQUAL(expected, got);
    }
    
    void testDeserialize()
    {
      dmlite::Extensible ext;
      
      std::string json("{\
\"string\": \"none-\\\"quote-\\/\",\
\"int\": 42,\
\"unsigned\": 7,\
\"float\": 5.2,\
\"bool\": true\
}");
      
      ext.deserialize(json);
      
      CPPUNIT_ASSERT_EQUAL(std::string("none-\"quote-/"), ext.getString("string"));
      CPPUNIT_ASSERT_EQUAL(42l,   ext.getLong("int"));
      CPPUNIT_ASSERT_EQUAL(7lu,   ext.getUnsigned("unsigned"));
      CPPUNIT_ASSERT_EQUAL(5.2,   ext.getDouble("float"));
      CPPUNIT_ASSERT_EQUAL(true,  ext.getBool("bool"));
    }
    
    void testNested()
    {
      dmlite::Extensible ext;
      dmlite::Extensible nested;
      
      nested["int"] = 0;
      nested["string"] = std::string("else");
      
      ext["string"] = std::string("something");
      ext["nested"] = nested;
      
      // Try to extract
      dmlite::Extensible test = ext.getExtensible("nested");
      
      CPPUNIT_ASSERT_EQUAL(std::string("else"), test.getString("string"));
      CPPUNIT_ASSERT_EQUAL(0l, test.getLong("int"));
      
      // Try serialization
      std::string expected("{\
\"nested\": {\
    \"int\": 0,\
    \"string\": \"else\"\
  },\
\"string\": \"something\"\
}");
      std::string got = ext.serialize();
      
      REMOVE_SPACES(expected);
      REMOVE_SPACES(got);
      
      CPPUNIT_ASSERT_EQUAL(expected, got);
      
      // Try deserialization
      std::string json("{\
\"nested\": {\
    \"int\": 55,\
    \"string\": \"other-string\",\
    \"nested2\": {\"int\": 42}\
  },\
\"string\": \"new-thing\"\
}");
      
      ext.clear();
      ext.deserialize(json);
      
      CPPUNIT_ASSERT_EQUAL(std::string("new-thing"), ext.getString("string"));
      
      nested = ext.getExtensible("nested");
      
      CPPUNIT_ASSERT_EQUAL(std::string("other-string"), nested.getString("string"));
      CPPUNIT_ASSERT_EQUAL(55l, nested.getLong("int"));
      
      dmlite::Extensible nested2 = nested.getExtensible("nested2");
      
      CPPUNIT_ASSERT_EQUAL(42l, nested2.getLong("int"));
    }
    
    void testArray()
    {
      dmlite::Extensible ext;
      std::vector<boost::any> array;
      
      array.push_back(55);
      array.push_back(std::string("string"));
      
      ext["array"] = array;
      
      // Check values
      std::vector<boost::any> check = ext.getVector("array");
      
      CPPUNIT_ASSERT_EQUAL((size_t)2, check.size());
      CPPUNIT_ASSERT_EQUAL(55l,
                           dmlite::Extensible::anyToLong(check[0]));
      CPPUNIT_ASSERT_EQUAL(std::string("string"),
                           dmlite::Extensible::anyToString(check[1]));
      
      // Check serialization
      std::string expected("{\
\"array\": [ 55, \"string\" ]\
}");
      std::string got = ext.serialize();
      
      
      REMOVE_SPACES(expected);
      REMOVE_SPACES(got);
      
      CPPUNIT_ASSERT_EQUAL(expected, got);
      
      // Check deserialization
      std::string json("{\
\"array\": [ \"initial\", 465 ]\
}");
      
      ext.clear();
      ext.deserialize(json);
      
      array = ext.getVector("array");
      
      CPPUNIT_ASSERT_EQUAL((size_t)2, array.size());
      CPPUNIT_ASSERT_EQUAL(std::string("initial"),
                           dmlite::Extensible::anyToString(array[0]));
      CPPUNIT_ASSERT_EQUAL(465l, dmlite::Extensible::anyToLong(array[1]));
    }
   
    CPPUNIT_TEST_SUITE(TestExtensible);
    CPPUNIT_TEST(testRegular);
    CPPUNIT_TEST(testStrings);
    CPPUNIT_TEST(testSerialize);
    CPPUNIT_TEST(testDeserialize);
    CPPUNIT_TEST(testNested);
    CPPUNIT_TEST(testArray);
    CPPUNIT_TEST_SUITE_END();
};

CPPUNIT_TEST_SUITE_REGISTRATION(TestExtensible);

int main(int argc, char **argv)
{
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest( registry.makeTest() );
  return runner.run()?0:1;
}
