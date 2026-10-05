#include "BilConfig.h"

#ifdef HAVE_SUPERLU
#include <superlu/slu_ddefs.h>
#endif

#ifdef HAVE_SUPERLUMT
#error "Multithreaded SuperLU method not available"
#endif

#ifdef HAVE_SUPERLUDIST
#include <superlu_ddefs.h>
#endif
