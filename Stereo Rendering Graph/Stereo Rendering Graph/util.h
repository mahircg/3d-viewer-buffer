
//This header includes the helper functions for the custom allocator and presenter.All other helper functions and macros can be declared here to improve code visibility.
#ifndef _UTIL_HH__
#define _UTIL_HH__

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
    return hr; } while(0)

#endif // _UTIL_HH__