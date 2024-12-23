#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "include/search.h"

using namespace std;
using namespace testing;

TEST(CleanToken, BothEndsPunctuation) {
  ASSERT_THAT(cleanToken(".hello."), StrEq("hello"));
  ASSERT_THAT(cleanToken("...hello..."), StrEq("hello"));
  ASSERT_THAT(cleanToken(".\"!?hello.\"?"), StrEq("hello"));
  ASSERT_THAT(cleanToken("...?!hello?!..."), StrEq("hello"));
}


TEST(CleanToken, MidPunctuation) {
  ASSERT_THAT(cleanToken("hello,world"), StrEq("hello,world"));
  ASSERT_THAT(cleanToken("hello.world"), StrEq("hello.world"));
  ASSERT_THAT(cleanToken("!hello,world!"), StrEq("hello,world"));
  ASSERT_THAT(cleanToken(".\"!?hello.world.\"!?"), StrEq("hello.world"));
  ASSERT_THAT(cleanToken("they're"), StrEq("they're"));
  ASSERT_THAT(cleanToken(".they're."), StrEq("they're"));
  ASSERT_THAT(cleanToken("...hello-world..."), StrEq("hello-world"));
}

TEST(CleanToken, MixedPunct) {
  ASSERT_THAT(cleanToken("123!HELLO,WORLD!678"), StrEq("123!hello,world!678"));
  ASSERT_THAT(cleanToken(",,,HElLo-WoRLd,,,"), StrEq("hello-world"));
  ASSERT_THAT(cleanToken(".hello.hi.hey."), StrEq("hello.hi.hey"));
  ASSERT_THAT(cleanToken(".HELLO,WORLD."), StrEq("hello,world"));
  ASSERT_THAT(cleanToken(".\"!?HeLLo.WoRLD.\"!?"), StrEq("hello.world"));

}
TEST(CleanToken, PunctInMidAndStart) {
  ASSERT_THAT(cleanToken(".HELLO.world"), StrEq("hello.world"));
  ASSERT_THAT(cleanToken("...HELLO...WORLD"), StrEq("hello...world"));
}

TEST(CleanToken, PunctInMidAndEnd) {
  ASSERT_THAT(cleanToken("HELLO.world."), StrEq("hello.world"));
  ASSERT_THAT(cleanToken("HELLO...WORLD..."), StrEq("hello...world"));

}

TEST(GatherTokens, LeadingTrailingSpaces) {
  string text = "   hello   world   ";
  set<string> expected = {"hello", "world"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, MixedCaseAndSpaces) {
  string text = "WE ARE done with the CS EXAM";
  set<string> expected = {"we", "are", "done", "with", "the", "cs", "exam"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(GatherTokens, MultipleSpacesInMid) {
  string text = "how    was   your   day";
  set<string> expected = {"how", "was", "your", "day"};

  EXPECT_THAT(gatherTokens(text), ContainerEq(expected))
      << "text=\"" << text << "\"";
}

TEST(BuildIndex, TinyTxt) {
  string filename = "data/tiny.txt";
  map<string, set<string>> expectedIndex = {
      {"eggs", {"www.shoppinglist.com"}},
      {"milk", {"www.shoppinglist.com"}},
      {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
      {"bread", {"www.shoppinglist.com"}},
      {"cheese", {"www.shoppinglist.com"}},
      {"red", {"www.rainbow.org", "www.dr.seuss.net"}},
      {"gre-en", {"www.rainbow.org"}},  
      {"orange", {"www.rainbow.org"}},
      {"yellow", {"www.rainbow.org"}},
      {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
      {"indigo", {"www.rainbow.org"}},
      {"violet", {"www.rainbow.org"}},
      {"one", {"www.dr.seuss.net"}},
      {"two", {"www.dr.seuss.net"}},
      {"i'm", {"www.bigbadwolf.com"}},  
      {"not", {"www.bigbadwolf.com"}},
      {"trying", {"www.bigbadwolf.com"}},
      {"to", {"www.bigbadwolf.com"}},
      {"eat", {"www.bigbadwolf.com"}},
      {"you", {"www.bigbadwolf.com"}}
  };
  map<string, set<string>> studentIndex;
  int studentNumProcessed = buildIndex(filename, studentIndex);

  string indexTestFeedback =
      "buildIndex(\"" + filename + "\", ...) index incorrect\n";
  EXPECT_THAT(studentIndex, ContainerEq(expectedIndex)) << indexTestFeedback;

  string retTestFeedback =
      "buildIndex(\"" + filename + "\", ...) return value incorrect\n";
  EXPECT_THAT(studentNumProcessed, Eq(4)) << retTestFeedback;
}

TEST(BuildIndex, FileNotFound) {
  string filename = "data/nonexistent_file.txt";  
  map<string, set<string>> studentIndex;

  int studentNumProcessed = buildIndex(filename, studentIndex);

  string indexTestFeedback =
      "buildIndex(\"" + filename + "\", ...) should leave index empty for nonexistent file\n";
  EXPECT_TRUE(studentIndex.empty()) << indexTestFeedback;

  string retTestFeedback =
      "buildIndex(\"" + filename + "\", ...) should return -1 when file is not found\n";
  EXPECT_THAT(studentNumProcessed, Eq(0)) << retTestFeedback;  
}
map<string, set<string>> INDEX = {
    {"hello", {"example.com", "uic.edu"}},
    {"there", {"example.com"}},
    {"according", {"uic.edu"}},
    {"to", {"uic.edu"}},
    {"all", {"example.com", "uic.edu", "random.org"}},
    {"known", {"uic.edu"}},
    {"laws", {"random.org"}},
    {"of", {"random.org"}},
    {"aviation", {"random.org"}},
    {"a", {"uic.edu", "random.org"}},
};

TEST(FindQueryMatches, Difference) {
  set<string> expected;

  expected = {"example.com", "uic.edu"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -laws"), ContainerEq(expected));

  expected = {"random.org"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -hello"), ContainerEq(expected));

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -of -a"), ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "known -to"), ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "to -all -a"), ContainerEq(expected));
}

TEST(FindQueryMatches, LongCombinedQueries) {
  set<string> expected;

  expected = {"example.com"};
  EXPECT_THAT(findQueryMatches(INDEX, "there laws aviation -to +hello"),
              ContainerEq(expected));

  expected = {};
  EXPECT_THAT(findQueryMatches(INDEX, "all -all +all"), ContainerEq(expected));

  expected = {"example.com", "uic.edu", "random.org"};
  EXPECT_THAT(findQueryMatches(INDEX, "all -all +all all"),
              ContainerEq(expected));
}

TEST(FindQueryMatches, FirstQueryTermNotInIndex) {
  map<string, set<string>> index = {
      {"apple", {"url1", "url2"}},
      {"banana", {"url2", "url3"}},
      {"cherry", {"url1", "url3"}}
  };
  string query = "nonexistent fruit";
  
  set<string> expected = {};
  EXPECT_THAT(findQueryMatches(index, query), ContainerEq(expected));
}

TEST(FindQueryMatches, LaterQueryTermNotInIndex) {
  map<string, set<string>> index = {
      {"apple", {"url1", "url2"}},
      {"banana", {"url2", "url3"}},
      {"cherry", {"url1", "url3"}}
  };
  string query = "apple +nonexistent";
  
  set<string> expected = {};
  EXPECT_THAT(findQueryMatches(index, query), ContainerEq(expected));
}

TEST(FindQueryMatches, LaterQueryTermWithMinusNotInIndex) {
  map<string, set<string>> index = {
      {"apple", {"url1", "url2"}},
      {"banana", {"url2", "url3"}},
      {"cherry", {"url1", "url3"}}
  };
  string query = "apple -nonexistent";
  
  set<string> expected = {"url1", "url2"};
  EXPECT_THAT(findQueryMatches(index, query), ContainerEq(expected));
}