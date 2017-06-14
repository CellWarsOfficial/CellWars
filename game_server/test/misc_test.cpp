#include "misc_test.hpp"
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <constants.hpp>

using namespace std;

int fails = 0;
int tests = 0;

int math_tests()
{
  fails = 0;
  tests = 0;
  // TODO tests go here
  //test binary_to_num
  string test1 = "1101";
  string test2 = "00001101";
  string test3 = "11000011010011111";
  string test4 = "0000100110011010";
  string test5 = "100110011010";
  tests++;
  if(13 != binary_to_num(test1)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  tests++;
  if(13 != binary_to_num(test2)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  tests++;
  if(99999 != binary_to_num(test3)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  tests++;
  if(2458 != binary_to_num(test4)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  tests++;
  if(2458 != binary_to_num(test5)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  //test num_to_binary
  int test6 = 70;
  int test7 = 63;
  int test8 = 0;
  int test9 = 00111;
  int test10 = 111;
  tests++;
  if("1000110"(test6)){
    fprintf(stderr, "TEST FAIL: num_to_binary.\n"); 
    fails++;
  }
  tests++;
  if("111111" != num_to_binary(test7)){
    fprintf(stderr, "TEST FAIL: num_to_binary.\n"); 
    fails++;
  }
  tests++;
  if("0" != num_to_binary(test8)){
    fprintf(stderr, "TEST FAIL: num_to_binary.\n"); 
    fails++;
  }
  tests++;
  if("7" != num_to_binary(test9)){
    fprintf(stderr, "TEST FAIL: num_to_binary.\n"); 
    fails++;
  }
  tests++;
  if("7" != num_to_binary(test10)){
    fprintf(stderr, "TEST FAIL: num_to_binary.\n"); 
    fails++;
  }
  tests++;
  if(2458 != binary_to_num(test5)){
    fprintf(stderr, "TEST FAIL: binary_to_num.\n"); 
    fails++;
  }
  fprintf(stderr, "%d/%d tests passed - math\n", tests - fails, tests);
  return fails;
}

int strings_tests()
{
  fails = 0;
  tests = 0;
  // in tests
  tests++;
  if(!in(' ', STR_WHITE)){fails++; fprintf(stderr, "In failing long\n");}
  tests++;
  if(!in('\f', STR_WHITE)){fails++; fprintf(stderr, "In failing long\n");}
  tests++;
  if(!in('\n', STR_WHITE)){fails++; fprintf(stderr, "In failing long\n");}
  tests++;
  if(!in('\r', STR_WHITE)){fails++; fprintf(stderr, "In failing long\n");}
  tests++;
  if(!in('\t', STR_WHITE)){fails++; fprintf(stderr, "In failing long\n");}
  tests++;
  if(!in('\v', STR_WHITE)){fails++; fprintf(stderr, "In failing\n");}
  tests++;
  if(in(' ', "")){fails++; fprintf(stderr, "In failing empty\n");}
  tests++;
  if(in(' ', "asdefaersdvkdjlvc")){fails++; fprintf(stderr, "In failing not\n");}
  tests++;
  if(!in('A', "AAAAAAAAAAAAAAAAAAAAA")){fails++; fprintf(stderr, "In failing many\n");}
  tests++;
  if(!in('A', "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWA")){fails++; fprintf(stderr, "In failing long\n");}
  // skip_ws tests
  tests++;
  if(*(skip_ws("          qwe")) != 'q'){fails++; fprintf(stderr, "skipped ws failing\n");}
  tests++;
  if(*(skip_ws("          \n\n\n")) != (char)0){fails++; fprintf(stderr, "skipped ws failing around sentinel\n");}
  tests++;
  if(*(skip_ws("")) != (char)0){fails++; fprintf(stderr, "skipped ws failing for empty\n");}
  tests++;
  if(*(skip_ws("asd")) != 'a'){fails++; fprintf(stderr, "skipped ws when not required\n");}
  // string_seek tests
  tests++;
  if(*(string_seek("asdqwerty", "asd")) != 'q'){fails++; fprintf(stderr, "mismatch at asd\n");}
  tests++;
  if(*(string_seek("key: MAGIC_KEY", "key:")) != 'M'){fails++; fprintf(stderr, "mismatch at MAGIC\n");}
  tests++;
  if(*(string_seek("kkey: MAGICKA_KEY", "key:")) != 'M'){fails++; fprintf(stderr, "mismatch at MAGICKA\n");}
  tests++;
  if(*(string_seek("kkkkkkkkkkkkkey: MAGICA_KEY", "key:")) != 'M'){fails++; fprintf(stderr, "mismatch at MAGICA\n");}
  tests++;
  if(*(string_seek("key:             MADOKA_KEY", "key:")) != 'M'){fails++; fprintf(stderr, "mismatch at MADOKA\n");}
  tests++;
  if(*(string_seek("kkey:        MAGIKA_KEY", "key:")) != 'M'){fails++; fprintf(stderr, "mismatch at kkey\n");}
  tests++;
  if(*(string_seek("kkkkkkkkkkkkkey:       AGICKA_KEY", "key:")) != 'A'){fails++; fprintf(stderr, "mismatch at AGICKA\n");}
  tests++;
  if(string_seek("qwertyuiop", "key") != NULL){fails++; fprintf(stderr, "mismatch at never\n");}
  tests++;
  if(string_seek("kekeroni macaroni.", "key") != NULL){fails++; fprintf(stderr, "mismatch at kek\n");}
  tests++;
  if(*(string_seek("   lorem ipsum", "lorem")) != 'i'){fails++; fprintf(stderr, "mismatch at lorem\n");}
  tests++;
  if(*(string_seek("   lorem ipsum", "lore")) != 'm'){fails++; fprintf(stderr, "mismatch at lore\n");}
  tests++;
  if(string_seek("", "key") != NULL){fails++; fprintf(stderr, "mismatch at null\n");}
  // string_get_next_token tests
  string tok;
  tests++;
  if((tok = string_get_next_token("qwert", "ert")).compare("qw")){fails++; fprintf(stderr, "mismatch token qwe1\n got \"%s\" expected qw\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token("qwert", "tr")).compare("qwe")){fails++; fprintf(stderr, "mismatch token qwe2\n got \"%s\" expected qwe\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token("   ", STR_WHITE)).compare("")){fails++; fprintf(stderr, "mismatch token empty ws\n got \"%s\" expected nothing\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token("asdqwe   ", STR_WHITE)).compare("asdqwe")){fails++; fprintf(stderr, "mismatch token full ws\n got \"%s\" expected asdqwe\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token("asdqwetyi", STR_WHITE)).compare("asdqwetyi")){fails++; fprintf(stderr, "mismatch token no separator\n got \"%s\" expected asdqwetyi\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token(STR_WHITE, "")).compare(STR_WHITE)){fails++; fprintf(stderr, "mismatch token null separator\n got \"%s\" expected whitespace\n", tok.c_str());}
  tests++;
  if((tok = string_get_next_token("", "")).compare("")){fails++; fprintf(stderr, "mismatch token empty insanity\n got \"%s\" expected nothing\n", tok.c_str());}
  // combination tests
  tests++;
  if((tok = string_get_next_token(string_seek("HTTP1/1 GET /\nws_key:    asdqwemihai", "ws_key:"), STR_WHITE)).compare("asdqwemihai")){fails++; fprintf(stderr, "failed key detection\n");}
  tests++;
  if(string_get_next_token
              (string_seek
                (string_seek
                  ("HTTP1/1 GET /index.html?x=1&y=2&t=3\nws_key:    ????????"
                  , "?")
                , "x=")
              , " \n\t\f\r\v&")
     .compare("1"))
  {fails++; fprintf(stderr, "failed argument parsing for x(first)\n");}
  tests++;
  if(string_get_next_token
              (string_seek
                (string_seek
                  ("HTTP1/1 GET /index.html?x=1&y=2&t=3\nws_key:    ????????"
                  , "?")
                , "y=")
              , " \n\t\f\r\v&")
     .compare("2"))
  {fails++; fprintf(stderr, "failed argument parsing for y(second)\n");}
  tests++;
  if(string_get_next_token
              (string_seek
                (string_seek
                  ("HTTP1/1 GET /index.html?x=1&y=2&t=3\nws_key:    ????????"
                  , "?")
                , "t=")
              , " \n\t\f\r\v&")
     .compare("3"))
  {fails++; fprintf(stderr, "failed argument parsing for t(third)\n");}
  tests++;
  if(string_get_next_token
              (string_seek
                (string_seek
                  ("HTTP1/1 GET /index.html?x=1&y=2&t=3\nws_key:    ????????"
                  , "?")
                , "k=")
              , " \n\t\f\r\v&")
     .compare(""))
  {fails++; fprintf(stderr, "failed argument parsing for t(third)\n");}
  // null propagation testing
  tests++;
  if(in('U', NULL)){fails++; fprintf(stderr, "Null propagation failing for in\n");}
  tests++;
  if(skip_ws(NULL)){fails++; fprintf(stderr, "Null propagation failing for skip_ws\n");}
  tests++;
  if(string_seek(NULL, "qwe")){fails++; fprintf(stderr, "Null propagation failing for seek(origin)\n");}
  tests++;
  if(string_seek("qwe", NULL)){fails++; fprintf(stderr, "Null propagation failing for seek(target)\n");}
  tests++;
  if(string_seek(NULL, NULL)){fails++; fprintf(stderr, "Null propagation failing for seek(both)\n");}
  tests++;
  if(string_get_next_token(FIXED_STRING, NULL).compare(FIXED_STRING)){fails++; fprintf(stderr, "Null propagation failing for tokenizer when it should work\n");}
  tests++;
  if(string_get_next_token(NULL, FIXED_STRING).compare("")){fails++; fprintf(stderr, "Null propagation failing for tokenizer when it should fail\n");}
  // results and finish
  fprintf(stderr, "%d/%d tests passed - strings\n", tests - fails, tests);
  return fails;
}

int main()
{
  return math_tests() + strings_tests();
}
