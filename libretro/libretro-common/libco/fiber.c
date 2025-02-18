/*
  libco.win (2008-01-28)
  authors: Nach, byuu
  license: public domain
*/

#define LIBCO_C
#include <libco.h>
#define WINVER 0x0400
#define _WIN32_WINNT 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

static thread_local cothread_t co_active_ = 0;

static void __stdcall co_thunk(void *coentry)
{
   ((void (*)(void))coentry)();
}

cothread_t co_active(void)
{
   if(!co_active_)
   {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
      ConvertThreadToFiberEx(0, FIBER_FLAG_FLOAT_SWITCH);
#else
      ConvertThreadToFiber(0);
#endif
      co_active_ = GetCurrentFiber();
   }
   return co_active_;
}

cothread_t co_create(unsigned int heapsize, void (*coentry)(void))
{
   if(!co_active_)
   {
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
      ConvertThreadToFiberEx(0, FIBER_FLAG_FLOAT_SWITCH);
#else
      ConvertThreadToFiber(0);
#endif
      co_active_ = GetCurrentFiber();
   }

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
   return (cothread_t)CreateFiberEx(heapsize, heapsize, FIBER_FLAG_FLOAT_SWITCH, co_thunk, (void*)coentry);
#else
   return (cothread_t)CreateFiber(heapsize, co_thunk, (void*)coentry);
#endif
}

void co_delete(cothread_t cothread)
{
   DeleteFiber(cothread);
}

void co_switch(cothread_t cothread)
{
   co_active_ = cothread;
   SwitchToFiber(cothread);
}

#ifdef __cplusplus
}
#endif
