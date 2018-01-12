#pragma once

#ifdef __DEBUG
#define __DEBUG_TRACE(...) fprintf(fout, "file[%s]line[%u]func[%s]::",__FILE__,__LINE__,__func__);fprintf(fout,__VA_ARGS__)
#else
#define __DEBUG_TRACE(...)
#endif