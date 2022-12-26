#include "WeWorkFinanceSdk_C.h"
#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <map>
using std::map;
using std::string;

typedef WeWorkFinanceSdk_t *newsdk_t();
typedef int Init_t(WeWorkFinanceSdk_t *, const char *, const char *);
typedef void DestroySdk_t(WeWorkFinanceSdk_t *);

typedef int GetChatData_t(WeWorkFinanceSdk_t *, unsigned long long, unsigned int, const char *, const char *, int, Slice_t *);
typedef Slice_t *NewSlice_t();
typedef void FreeSlice_t(Slice_t *);

typedef int GetMediaData_t(WeWorkFinanceSdk_t *, const char *, const char *, const char *, const char *, int, MediaData_t *);
typedef int DecryptData_t(const char *, const char *, Slice_t *);
typedef MediaData_t *NewMediaData_t();
typedef void FreeMediaData_t(MediaData_t *);

map<string, WeWorkFinanceSdk_t *> maps;

int main()
{
    void *so_handle = dlopen("../lib/libWeWorkFinanceSdk_C.so", RTLD_LAZY);
    newsdk_t *newsdk_fn = (newsdk_t *)dlsym(so_handle, "NewSdk");
    maps["aaa"] = newsdk_fn();
    maps["bbb"] = newsdk_fn();
}