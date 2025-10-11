//Emescripten has trouble with 64-bit stuff; maybe this wil lfix it up.

#ifdef PEBL_EMSCRIPTEN
#define pDouble double
#define pInt int
#else
#define pDouble long double
#define pInt long int
#endif

