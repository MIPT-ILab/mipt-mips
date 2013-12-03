
#define MY_ASSERT(condition, message) {if (condition == false) {cerr << "ERROR: " << message << "\n"; abort();}};
