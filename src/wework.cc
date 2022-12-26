#include <napi.h>
#include <dlfcn.h>
#include <errno.h>
#include "WeWorkFinanceSdk_C.h"
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <map>
// #include <openssl/rsa.h>
// #include <openssl/pem.h>
// #include <alibabacloud/oss/OssClient.h>

typedef WeWorkFinanceSdk_t *newsdk_t();
typedef int Init_t(WeWorkFinanceSdk_t *, const char *, const char *);
typedef void DestroySdk_t(WeWorkFinanceSdk_t *);

typedef int GetChatData_t(WeWorkFinanceSdk_t *, unsigned long long, unsigned int, const char *, const char *, int, Slice_t *);
typedef Slice_t *NewSlice_t();
typedef void FreeSlice_t(Slice_t *);
typedef char *GetContentFromSlice_t(Slice_t *);

typedef int GetMediaData_t(WeWorkFinanceSdk_t *, const char *, const char *, const char *, const char *, int, MediaData_t *);
typedef int DecryptData_t(const char *, const char *, Slice_t *);
typedef MediaData_t *NewMediaData_t();
typedef void FreeMediaData_t(MediaData_t *);

// using namespace AlibabaCloud::OSS;

std::string val = "";
void *so_handle;
// WeWorkFinanceSdk_t *sdk;
std::string privateKey = "";

std::map<std::string, WeWorkFinanceSdk_t *> sdkMaps;

WeWorkFinanceSdk_t *GetSDK(std::string corp_id)
{
  return sdkMaps.at(corp_id);
}

void SetSDK(std::string corp_id, WeWorkFinanceSdk_t *sdk)
{
  sdkMaps[corp_id] = sdk;
}

bool CheckSDKExists(std::string corp_id)
{
  try
  {
    sdkMaps.at(corp_id);
    return true;
  }
  catch (const std::out_of_range &)
  {
    return false;
  }
}

Napi::String SetValue(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();
  val = info[0].As<Napi::String>();
  return Napi::String::New(env, val);
}

Napi::String GetValue(const Napi::CallbackInfo &info)
{
  return Napi::String::New(info.Env(), val);
}

Napi::Value InitSDK(const Napi::CallbackInfo &info)
{
  std::string corpid = info[0].As<Napi::String>();
  std::string secret = info[1].As<Napi::String>();

  if (!so_handle)
  {
    std::string libfile = info[2].As<Napi::String>();
    so_handle = dlopen(libfile.c_str(), RTLD_LAZY);
    if (!so_handle)
    {
      Napi::Error::New(info.Env(), "load sdk fail: " + std::string(dlerror())).ThrowAsJavaScriptException();
      return Napi::Number::New(info.Env(), 1);
    }
  }

  if (!CheckSDKExists(corpid))
  {
    newsdk_t *NewSdk = (newsdk_t *)dlsym(so_handle, "NewSdk");
    Init_t *Init = (Init_t *)dlsym(so_handle, "Init");

    WeWorkFinanceSdk_t *sdk = NewSdk();

    int ret = Init(sdk, corpid.c_str(), secret.c_str());
    if (ret != 0)
    {
      DestroySdk_t *DestroySdk = (DestroySdk_t *)dlsym(so_handle, "DestroySdk");
      DestroySdk(sdk);
      // printf("init sdk err ret:%d\n", ret);
      // sprintf(errmsg, "init sdk err ret:%d\n", ret);
      Napi::Error::New(info.Env(), +"init sdk err ret: " + std::to_string(ret)).ThrowAsJavaScriptException();
      return Napi::Number::New(info.Env(), 1);
      // return Napi::Number::New(info.Env(), -2);
    }
    SetSDK(corpid, sdk);
  }
  // printf("create sdk success %s", privateKey.c_str());
  return Napi::Number::New(info.Env(), 0);
}

Napi::Value GetChatDataMethod(const Napi::CallbackInfo &info)
{
  bool lossless0;
  bool lossless1;
  bool lossless2;

  WeWorkFinanceSdk_t *sdk = GetSDK(info[0].As<Napi::String>());

  uint64_t iSeq = info[1].As<Napi::BigInt>().Uint64Value(&lossless0);    // 从指定的seq开始拉取消息，注意的是返回的消息从seq+1开始返回，seq为之前接口返回的最大seq值。首次使用请使用seq:0
  uint64_t iLimit = info[2].As<Napi::BigInt>().Uint64Value(&lossless1);  // 一次拉取的消息条数，最大值1000条，超过1000条会返回错误
  uint64_t timeout = info[3].As<Napi::BigInt>().Uint64Value(&lossless2); // 超时时间，单位秒
  NewSlice_t *NewSlice = (NewSlice_t *)dlsym(so_handle, "NewSlice");
  FreeSlice_t *FreeSlice = (FreeSlice_t *)dlsym(so_handle, "FreeSlice");
  GetContentFromSlice_t *GetContentFromSlice = (GetContentFromSlice_t *)dlsym(so_handle, "GetContentFromSlice");

  Slice_t *chatDatas = NewSlice();
  GetChatData_t *GetChatData = (GetChatData_t *)dlsym(so_handle, "GetChatData");
  int ret = GetChatData(sdk, iSeq, iLimit, "", "", timeout, chatDatas);
  if (ret != 0)
  {
    FreeSlice(chatDatas);
    // printf("GetChatData err ret:%d\n", ret);
    // return Napi::Number::New(info.Env(), -3);
    // char errmsg[100];
    // sprintf(errmsg, "GetChatData error ret:%d\n", ret);
    Napi::Error::New(info.Env(), "GetChatData error ret: " + std::to_string(ret)).ThrowAsJavaScriptException();
    return Napi::Number::New(info.Env(), 1);
  }
  // printf("GetChatData len:%d data:%s\n", chatDatas->len, chatDatas->buf);
  // Napi::Value content = Napi::String::New(info.Env(), chatDatas->buf);
  Napi::Value content = Napi::String::New(info.Env(), GetContentFromSlice(chatDatas));
  FreeSlice(chatDatas);
  return content;
}

Napi::String DecryptDataMethod(const Napi::CallbackInfo &info)
{
  std::string randomKey = info[0].As<Napi::String>();
  std::string encryptMsg = info[1].As<Napi::String>();

  NewSlice_t *NewSlice = (NewSlice_t *)dlsym(so_handle, "NewSlice");
  FreeSlice_t *FreeSlice = (FreeSlice_t *)dlsym(so_handle, "FreeSlice");
  Slice_t *Msgs = NewSlice();
  DecryptData_t *DecryptData = (DecryptData_t *)dlsym(so_handle, "DecryptData");
  GetContentFromSlice_t *GetContentFromSlice = (GetContentFromSlice_t *)dlsym(so_handle, "GetContentFromSlice");

  int ret = DecryptData(randomKey.c_str(), encryptMsg.c_str(), Msgs);
  if (ret != 0)
  {
    FreeSlice(Msgs);
    // char errmsg[100];
    // sprintf(errmsg, "DecryptData error ret:%d\n", ret);
    Napi::Error::New(info.Env(), "DecryptData error ret: " + std::to_string(ret)).ThrowAsJavaScriptException();
    return Napi::String::New(info.Env(), "DecryptData error ret: " + std::to_string(ret));
    // printf("DecryptData err ret:%d\n", ret);
    // return Napi::String::New(info.Env(), "-3");
  }
  Napi::String res = Napi::String::New(info.Env(), GetContentFromSlice(Msgs));
  FreeSlice(Msgs);
  return res;
}

Napi::Value GetMediaDataMethod(const Napi::CallbackInfo &info)
{
  WeWorkFinanceSdk_t *sdk = GetSDK(info[0].As<Napi::String>());
  std::string sdkfileid = info[1].As<Napi::String>();
  std::string fileName = info[2].As<Napi::String>();

  // 拉取媒体文件
  std::string index;
  uint64_t timeout = strtoul("60", NULL, 10);
  int isfinish = 0;

  GetMediaData_t *getmediadata_fn = (GetMediaData_t *)dlsym(so_handle, "GetMediaData");
  NewMediaData_t *newmediadata_fn = (NewMediaData_t *)dlsym(so_handle, "NewMediaData");
  FreeMediaData_t *freemediadata_fn = (FreeMediaData_t *)dlsym(so_handle, "FreeMediaData");

  // 媒体文件每次拉取的最大size为512k，因此超过512k的文件需要分片拉取。若该文件未拉取完整，mediaData中的is_finish会返回0，同时mediaData中的outindexbuf会返回下次拉取需要传入GetMediaData的indexbuf。
  // indexbuf一般格式如右侧所示，”Range:bytes=524288-1048575“，表示这次拉取的是从524288到1048575的分片。单个文件首次拉取填写的indexbuf为空字符串，拉取后续分片时直接填入上次返回的indexbuf即可。
  while (isfinish == 0)
  {
    // 每次使用GetMediaData拉取存档前需要调用NewMediaData获取一个mediaData，在使用完mediaData中数据后，还需要调用FreeMediaData释放。
    MediaData_t *mediaData = newmediadata_fn();
    int ret = getmediadata_fn(sdk, index.c_str(), sdkfileid.c_str(), NULL, NULL, timeout, mediaData);
    if (ret != 0)
    {
      // 单个分片拉取失败建议重试拉取该分片，避免从头开始拉取。
      freemediadata_fn(mediaData);
      printf("GetMediaData err ret:%d\n", ret);
      Napi::Error::New(info.Env(), "GetMediaData err ret: " + std::to_string(ret)).ThrowAsJavaScriptException();
      return Napi::Number::New(info.Env(), -1);
      // return -1;
    }
    // printf("content size:%d isfin:%d outindex:%s\n", mediaData->data_len, mediaData->is_finish, mediaData->outindexbuf);

    // 大于512k的文件会分片拉取，此处需要使用追加写，避免后面的分片覆盖之前的数据。
    //  snprintf(file, sdkfileid.size(), "%s", sdkfileid.c_str());
    FILE *fp = fopen(fileName.c_str(), "ab+");
    // printf("filename:%s \n", file);
    if (NULL == fp)
    {
      // errNum = errno;
      freemediadata_fn(mediaData);
      Napi::Error::New(info.Env(), "Open file Error: " + std::to_string(errno)).ThrowAsJavaScriptException();
      // printf("open file err\n");
      return Napi::String::New(info.Env(), "fail");
    }

    fwrite(mediaData->data, mediaData->data_len, 1, fp);
    fclose(fp);

    // 获取下次拉取需要使用的indexbuf
    index.assign(std::string(mediaData->outindexbuf));
    isfinish = mediaData->is_finish;
    freemediadata_fn(mediaData);
  }

  return Napi::String::New(info.Env(), fileName);
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set(Napi::String::New(env, "SetVal"),
              Napi::Function::New(env, SetValue));

  exports.Set(Napi::String::New(env, "GetVal"),
              Napi::Function::New(env, GetValue));

  exports.Set(Napi::String::New(env, "init"),
              Napi::Function::New(env, InitSDK));

  exports.Set(Napi::String::New(env, "getChatData"),
              Napi::Function::New(env, GetChatDataMethod));

  exports.Set(Napi::String::New(env, "decryptData"),
              Napi::Function::New(env, DecryptDataMethod));

  exports.Set(Napi::String::New(env, "getMediaData"),
              Napi::Function::New(env, GetMediaDataMethod));

  return exports;
}

NODE_API_MODULE(wework, Init)