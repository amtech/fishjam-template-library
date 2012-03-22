///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @file   ftlbase.h
/// @brief  Fishjam Template Library Base Header File.
/// @author fujie
/// @version 0.6 
/// @date 03/30/2008
/// @defgroup ftlbase ftl base function and class
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// ����������Ҫ�д��븴�á��������ú�ƽ̨������������

//  ���ʹ����ľ�̬������ʵ�ֹ��ܺ����Ļ����ͻ����ڵ���ʱ������Ҫ����������Ĵ��� -- ���� namespace �� inline �����ķ�ʽ���ã�
//  ʹ��cpp����ģ�棺��ʵ���ļ��ж��壺template class XpRectTemplate<char> -- �����Ϳ���������һ��ʹ��char��ģ���࣬����Link
//    �ŵ㣺������ͻ�������ģ���ʵ�ַ�ʽ��ȱ�㣺ֻ��ʹ�����޵�ģ������(�Ѿ��� .cpp �ж����)
//    ��ע�⣺memcpy �Ⱥ����൱��(�����ֽ�)�����Բ����Լ�д�ģ�����DWORD��32λ����(ע��߽�)��

#ifndef FTL_BASE_H
#define FTL_BASE_H
#pragma once

#ifndef __cplusplus
#  error FTL requires C++ compilation (use a .cpp suffix)
#endif

#include "ftlDefine.h"
#include "ftlTypes.h"

//#if defined USE_THREAD_SAFE_TRACE
#define FTLTRACE    FAST_TRACE
#define FTLTRACEEX  FAST_TRACE_EX
//#endif //USE_THREAD_SAFE_TRACE

//���÷���������ָ����־����Ͷ��ԵĻ��ƣ����û��ָ����ʹ��Ĭ��(�պ���չΪFastTrace��)
#if (!defined FTLASSERT)
#  if defined ATLTRACE
#    define FTLASSERT          ATLASSERT
#  elif defined TRACE
#    define FTLASSERT          ASSERT
#else
#   include <Windows.h>
#   include <tchar.h>
#   include <atlconv.h>
#   include <cassert>
#   define FTLASSERT          assert
#  endif
#endif 

// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the FTL include files
//#  pragma warning(disable: 4995)  //'%s': name was marked as #pragma deprecated
//#  pragma warning(disable: 4996)  //'%s': This function or variable may be unsafe.

#define STRSAFE_NO_DEPRECATE
#  include <strsafe.h>
//#pragma warning(default: 4995)
//#pragma warning(default: 4996)

#include <set>
#include <vector>
#include <map>

namespace FTL
{


    //#if (!defined FTLTRACE) || (!defined FTLASSERT)
    //#  error must define FTLTRACE and  FTLASSERT, or use ATL/MFC
    //#endif 


    //���ǵ��ֽڶ�������⣬����Ϊ 8 �ı���
    #define MAX_BUFFER_LENGTH       240

    //��ֹ��������͸�ֵ������
    #define DISABLE_COPY_AND_ASSIGNMENT(className)  \
    private:\
        className(const className& ref);\
        className& operator = (const className& ref)

    //��ÿ������ʹ�øú꣬����ʹ�ò����ඨ��Ϊ��Ʒ�����Ԫ�� -- ֧�ַ���˽�г�Ա����
    #define ENABLE_UNIT_TEST(cls) friend class cls##Tester; 

    //�ĸ��ԣ�
    //  TRACE ��ʽ��ʱ�� %ls -- WCHAR*, %hs -- CHAR*
    //  %s -- ��ԭ������� %S -- �л�UNICODE �� ANSI ��� 

    #ifdef _DEBUG
    #  define DBG_REPORT    _CrtDbgReport
    #  define DBG_BREAK     _CrtDbgBreak
    #else
    #  define DBG_REPORT    __noop
    #  define DBG_BREAK     __noop  
    #endif

    #ifdef FTL_DEBUG
    #  define REPORT_ERROR_INFO(c, e, x) \
         do{ \
             c _err(e);\
             USES_CONVERSION;\
             FAST_TRACE_EX(FTL::tlError, TEXT("%s(%d) :\t Error!!! Reason = 0x%08x(%d,%s),Code:\"%s\" \n"),\
               TEXT(__FILE__),__LINE__, e, e, _err.GetConvertedInfo(),TEXT(#x));\
             (1 != DBG_REPORT(_CRT_ASSERT, __FILE__, __LINE__, NULL, T2CA(_err.GetConvertedInfo()))) || \
               (DBG_BREAK(), 0);\
         }while(0)
    #else //Not Define FTL_DEBUG
    #  define REPORT_ERROR_INFO(c, e, x) __noop
    #endif //FTL_DEBUG


    //�Լ����� HRESULT ����ֵ -- MAKE_HRESULT(sev,fac,code)
    //  SEVERITY_XXX -- ���س̶�(0:�ɹ���1:����)
    //  FACILITY_XXX -- �豸��Ϣ(��������ͣ��� RPC��SECURITY��WIN32 ��)
    //����ͨ�� HRESULT_SEVERITY��HRESULT_FACILITY��HRESULT_CODE �ֱ��ȡ��Ӧ��ֵ

    #ifdef FTL_DEBUG  //���԰汾ʱ����ȡ���ж� LastError��Ϊ�˷�ֹ REPORT_ERROR_INFO ��LastError����Ӱ�죬��Ҫ��������
    # define API_ASSERT(x)\
        if(FALSE == (x))\
        {\
            DWORD dwLastError = GetLastError();\
            REPORT_ERROR_INFO(FTL::CFAPIErrorInfo, dwLastError,x);\
            SetLastError(dwLastError);\
        }

    # define API_VERIFY(x)   \
        bRet = (x);\
        if(FALSE == bRet)\
        {\
            DWORD dwLastError = GetLastError();\
            REPORT_ERROR_INFO(FTL::CFAPIErrorInfo, dwLastError,x);\
            SetLastError(dwLastError);\
        }
    # define API_VERIFY_EXCEPT1(x,e1)\
        bRet = (x);\
        if(FALSE == bRet)\
        {\
            DWORD dwLastError = GetLastError();\
            if(dwLastError != e1)\
            {\
                REPORT_ERROR_INFO(FTL::CFAPIErrorInfo, dwLastError,x);\
            }\
            SetLastError(dwLastError);\
        }
    #else //û�ж��� FTL_DEBUG ��ʱ�� -- ������ GetLastError/SetLastError �ĵ���
    # define API_ASSERT(x)  
    # define API_VERIFY(x)   \
        bRet = (x);
    # define API_VERIFY_EXCEPT1(x,e1)\
        bRet = (x);
    #endif //FTL_DEBUG

    //������� E_FAIL������֧�� ISupportErrorInfo �Ļ�����Ҫȡ��Rich Error ������Ϣ
    # define COM_VERIFY(x)   \
        hr = (x);\
        if(S_OK != hr)\
        {\
            REPORT_ERROR_INFO(FTL::CFComErrorInfo, hr,x);\
        }

    # define COM_VERIFY_EXCEPT1(x,h1) \
        hr = (x);\
        if(S_OK != hr && (h1)!= hr)\
        {\
            REPORT_ERROR_INFO(FTL::CFComErrorInfo, hr,x);\
        }

    # define COM_VERIFY_EXCEPT2(x,h1,h2) \
        hr = (x);\
        if(S_OK != hr && (h1)!= hr && (h2) != hr)\
        {\
            REPORT_ERROR_INFO(FTL::CFComErrorInfo, hr,x);\
        }

    # define REG_VERIFY(x)   \
        lRet = (x);\
        if(ERROR_SUCCESS != lRet)\
        {\
            REPORT_ERROR_INFO(FTL::CFAPIErrorInfo, lRet,x);\
        }

    # define REG_VERIFY_EXCEPT1(x,ret1)   \
        lRet = (x);\
        if(ERROR_SUCCESS != lRet && (ret1) != lRet )\
        {\
            REPORT_ERROR_INFO(FTL::CFAPIErrorInfo, lRet,x);\
        }

    ////////////////////////////////////////////////////////////////////////////////////////

    #ifndef _countof
    # define _countof(arr) (sizeof(arr) / sizeof(arr[0]))
    #endif

    #ifndef tstring
    #  if defined UNICODE 
    #     define tstring wstring
    #  else
    #     define tstring string
    #  endif 
    #endif


    //! @code SAFE_CLOSE_HANDLE(m_hFile,INVALID_HANDLE_VALUE); ע�� NULL �� INVALID_HANDLE_VALUE
    #ifndef SAFE_CLOSE_HANDLE
    #  ifdef FTL_DEBUG
    #    define SAFE_CLOSE_HANDLE(h,v) if((v) != (h)) { BOOL oldbRet = bRet; API_VERIFY(::CloseHandle((h))); (h) = (v); bRet = oldbRet; }
    #  else
    #    define SAFE_CLOSE_HANDLE(h,v) if((v) != (h)) { ::CloseHandle((h)); (h) = (v); bRet = bRet; }
    #  endif
    #endif

    #ifndef SAFE_CLOSE_REG
    #  ifdef FTL_DEBUG
    #    define SAFE_CLOSE_REG(h) if(NULL != (h)) { BOOL oldbRet = bRet; API_VERIFY(ERROR_SUCCESS == ::RegCloseKey((h))); (h) = NULL; bRet = oldbRet; }
    #  else
    #    define SAFE_CLOSE_REG(h) if(NULL != (h)) { ::RegCloseKey((h)); (h) = NULL; }
    #  endif
    #endif

    #ifndef SAFE_DELETE
    #  define SAFE_DELETE(p) if( NULL != (p) ){ delete (p); (p) = NULL; }
    #endif

    #ifndef SAFE_DELETE_ARRAY
    #  define SAFE_DELETE_ARRAY(p) if( NULL != (p) ){ delete [] (p); (p) = NULL; }
    #endif

    #ifndef SAFE_FREE
    #  define SAFE_FREE(p)   if(NULL != (p)) { free((p)); (p) = NULL;}
    #endif

    #ifndef SAFE_LOCAL_FREE
    #  define SAFE_LOCAL_FREE(p) if(NULL != (p)) { ::LocalFree((p)); (p) = NULL; }
    #endif

    #ifndef SAFE_HEAP_FREE
    #  define SAFE_HEAP_FREE(p) if(NULL != (p)) { ::HeapFree(GetProcessHeap(),0,(p)); (p) = NULL; }
    #endif

    #ifndef SAFE_FREE_BSTR
    #  define SAFE_FREE_BSTR(s) if(NULL != (s)){ ::SysFreeString((s)); (s) = NULL; }
    #endif

    /* ����һ�ְ�ȫRelease�ķ����� ���Ƶ��� Delete
    template<class T> 
    void Release(T t)
    {
        if( t )
        {
            t->Release();
            t = 0;
        }
    }
    */
    #ifndef SAFE_RELEASE
    #  define SAFE_RELEASE(p)  if( NULL != ((p)) ){ (p)->Release(); (p) = NULL; }
    #endif 

    #ifndef SAFE_COTASKMEMFREE
    #  define SAFE_COTASKMEMFREE(p) if(NULL != (p)){ ::CoTaskMemFree((p)); (p) = NULL; }
    #endif

    #ifndef SAFE_FREE_LIBRARY
    #  define SAFE_FREE_LIBRARY(p) if(NULL != (p)){ ::FreeLibrary(static_cast<HMODULE>(p)); (p) = NULL; }
    #endif


    ////////////////////////////////////////////////////////////////////////////////////////
    //f is CFStringFormater
    //v is combine value(such as GetStyle return value), 
    //c is check type, such as WS_VISIBLE
    //d is append string, such as "," or "|"
    #ifndef HANDLE_COMBINATION_VALUE_TO_STRING
    #define HANDLE_COMBINATION_VALUE_TO_STRING(f,v,c, d) \
        if(((v) & (c)) == (c))\
        {\
            f.AppendFormat(TEXT("%s%s"), TEXT(#c), d);\
            v &= ~c;\
        }
    #endif 

    #ifndef HANDLE_CASE_TO_STRING
    # define HANDLE_CASE_TO_STRING(buf,len,c)\
        case (c):\
        StringCchCopy(buf,len,TEXT(#c));\
        break;
    #endif

    #ifndef HANDLE_CASE_RETURN_STRING
    # define HANDLE_CASE_RETURN_STRING(c) \
        case (c):\
        return TEXT(#c);
    #endif 

    #define FTL_MIN(a,b)                (((a) < (b)) ? (a) : (b))
    #define FTL_MAX(a,b)                (((a) > (b)) ? (a) : (b))
    #define FTL_ABS(a)		            (((a) < 0) ? -(a) : (a))
    #define FTL_SIGN(a)		            (((a) > 0) ? 1 : (((a) < 0) ? -1 : 0))
    #define FTL_INRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))


    #define CHECK_POINTER_RETURN_IF_FAIL(p)    \
        if(NULL == (p))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            return;\
        }

    #define CHECK_POINTER_RETURN_VALUE_IF_FAIL(p,r)    \
        if(NULL == p)\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            return r;\
        }

    #define CHECK_POINTER_WRITABLE_RETURN_IF_FAIL(p)    \
        if(NULL == p || ::IsBadWritePtr(p, sizeof(DWORD_PTR)))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid writable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, sizeof(DWORD_PTR)));\
            return;\
        }

    #define CHECK_POINTER_WRITABLE_RETURN_VALUE_IF_FAIL(p,r)    \
        if(NULL == p || ::IsBadWritePtr(p, sizeof(DWORD_PTR)))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid writable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, sizeof(DWORD_PTR)));\
            return r;\
        }

    #define CHECK_POINTER_READABLE_RETURN_IF_FAIL(p)    \
        if(NULL == p || ::IsBadReadPtr(p, sizeof(DWORD_PTR)))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid readable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, sizeof(DWORD_PTR)));\
            return;\
        }

    #define CHECK_POINTER_READABLE_RETURN_VALUE_IF_FAIL(p,r)    \
        if(NULL == p || ::IsBadReadPtr(p, sizeof(DWORD_PTR)))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid readable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, sizeof(DWORD_PTR)));\
            return r;\
        }

    #define CHECK_POINTER_WRITABLE_DATA_RETURN_IF_FAIL(p,len)    \
        if(NULL == p || ::IsBadWritePtr(p, len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len writable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, len));\
            return;\
        }

    #define CHECK_POINTER_WRITABLE_DATA_RETURN_VALUE_IF_FAIL(p,len,r)    \
        if(NULL == p || ::IsBadWritePtr(p, len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len writable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, len));\
            return r;\
        }

    #define CHECK_POINTER_READABLE_DATA_RETURN_IF_FAIL(p,len)    \
        if(NULL == p || ::IsBadReadPtr(p, len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len readable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, len));\
            return;\
        }

    #define CHECK_POINTER_READABLE_DATA_RETURN_VALUE_IF_FAIL(p,len,r)    \
        if(NULL == p || ::IsBadReadPtr(p, len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len readable pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, len));\
            return r;\
        }

    #define CHECK_POINTER_READ_WRIATE_ABLE_DATA_RETURN_IF_FAIL(p,len)    \
        if(NULL == p || ::IsBadReadPtr(p, len) || ::IsBadWritePtr(p,len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len read/write able pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, len));\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, len));\
            return;\
        }

    #define CHECK_POINTER_READ_WRIATE_ABLE_DATA_RETURN_VALUE_IF_FAIL(p,len,r)    \
        if(NULL == p || ::IsBadReadPtr(p, len))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid %d len read/write able pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p,len);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadReadPtr(p, len));\
            FTLASSERT(FALSE ==::IsBadWritePtr(p, len));\
            return r;\
        }

    #define CHECK_POINTER_ISSTRING_PTR_RETURN_IF_FAIL(p,r)    \
        if(NULL == p || ::IsBadStringPtr(p, INFINITE))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid string pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadStringPtr(p, INFINITE));\
            return;\
        }

    #define CHECK_POINTER_ISSTRING_PTR_RETURN_VALUE_IF_FAIL(p,r)    \
        if(NULL == p || ::IsBadStringPtr(p, INFINITE))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid string pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadStringPtr(p, INFINITE));\
            return r;\
        }

    #define CHECK_POINTER_CODE_RETURN_IF_FAIL(p)    \
        if(NULL == p || ::IsBadCodePtr(p))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid code pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadCodePtr(p));\
            return;\
        }

    #define CHECK_POINTER_CODE_RETURN_VALUE_IF_FAIL(p,r)    \
        if(NULL == p || ::IsBadCodePtr(p))\
        {\
            FTLTRACEEX(FTL::tlWarning, TEXT("%s(%d) : Warning!!! %s(0x%p) is not a valid code pointer\n"),TEXT(__FILE__),__LINE__, TEXT(#p),p);\
            FTLASSERT(NULL != p);\
            FTLASSERT(FALSE ==::IsBadCodePtr(p));\
            return r;\
        }
}

namespace FTL
{
    template<typename T>
    BOOL IsSameNumber(const T& expected, const T& actual, const T& delta);

    FTLEXPORT template<typename TBase, typename INFO_TYPE,LONG bufLen = MAX_BUFFER_LENGTH>
    class CFConvertInfoT
    {
        DISABLE_COPY_AND_ASSIGNMENT(CFConvertInfoT);
    public:
        FTLINLINE explicit CFConvertInfoT(INFO_TYPE info);
        FTLINLINE virtual ~CFConvertInfoT();
        FTLINLINE LPCTSTR GetConvertedInfo();
        FTLINLINE const INFO_TYPE GetInfo() const;
        FTLINLINE void SetInfo(INFO_TYPE info);
    public:
        FTLINLINE virtual LPCTSTR ConvertInfo() = 0;
    protected:
        INFO_TYPE   m_Info;
        TCHAR       m_bufInfo[bufLen];
    };

    FTLEXPORT class CFAPIErrorInfo : public CFConvertInfoT<CFAPIErrorInfo,DWORD>
    {
        DISABLE_COPY_AND_ASSIGNMENT(CFAPIErrorInfo);
    public:
        FTLINLINE explicit CFAPIErrorInfo(DWORD dwError);
        FTLINLINE virtual LPCTSTR ConvertInfo();
    };

    FTLEXPORT class CFComErrorInfo : public CFConvertInfoT<CFComErrorInfo,HRESULT>
    {
        DISABLE_COPY_AND_ASSIGNMENT(CFComErrorInfo);
    public:
        FTLINLINE explicit CFComErrorInfo(HRESULT hr);
        FTLINLINE virtual LPCTSTR ConvertInfo();
    protected:
        FTLINLINE LPCTSTR GetErrorFacility(HRESULT hr, LPTSTR pszFacility,DWORD dwLength);
    };

    //! ����Ĺ�����Ҫ�������ʱ�ڴ棬����Ĺ����з����ڴ棬�������ͷ�
    //! �Ƿ��������С���ڴ���Ƭ��ʹ��Pool�Ż���
    enum MemoryAllocType
    {
        matNew,             //ʹ��new���䣬ʹ��delete�ͷţ�Ϊ�˷����������ʹֻ����һ����Ҳʹ�����鷽ʽ
        //matAlloc,           //ʹ��
        //matMalloca,         //ʹ��_malloca��ջ�Ϸ���
    };

    template <typename T, MemoryAllocType allocType  = matNew>
    class CFTempMemoryHolder
    {
    public:
        FTLINLINE CFTempMemoryHolder(DWORD dwCount);
        FTLINLINE ~CFTempMemoryHolder();
        //FTLINLINE T* ReAlloc(DWORD dwCount);
        FTLINLINE T* Detatch(); 
        FTLINLINE DWORD GetCount() const;
        FTLINLINE operator T*() const;
    protected:
        T*              m_pMemory;
        DWORD           m_dwCount;
    };

    //! �ַ�����ʽ�������Ը��ݴ���ĸ�ʽ���ַ������Զ�����������ʱ�ͷŷ�����ڴ�
    class CFStringFormater
    {
        DISABLE_COPY_AND_ASSIGNMENT(CFStringFormater);
    public:
        FTLINLINE CFStringFormater(DWORD dwInitAllocLength = MAX_BUFFER_LENGTH);
        FTLINLINE virtual ~CFStringFormater();
        FTLINLINE VOID Reset();
        FTLINLINE HRESULT __cdecl Format(LPCTSTR lpszFormat, ...);
        FTLINLINE HRESULT __cdecl FormatV(LPCTSTR lpszFormat, va_list argList);
        FTLINLINE HRESULT __cdecl AppendFormat(LPCTSTR lpszFormat, ...);
        FTLINLINE HRESULT __cdecl AppendFormatV(LPCTSTR lpszFormat, va_list argList);
        FTLINLINE operator LPCTSTR() const
        {
            return m_pBuf;
        }
        FTLINLINE LPCTSTR GetString() const;
        FTLINLINE LONG  GetStringLength() const;
        FTLINLINE LPTSTR Detach();
    protected:
        LPTSTR  m_pBuf;
        DWORD   m_dwTotalSpaceSize;
        const DWORD m_dwInitAllocLength;
    };

    // The typedef for the debugging output function.  Note that this matches OutputDebugString.
    typedef VOID (WINAPI *PFNDEBUGOUT)(LPCTSTR lpOutputString) ;

    enum TraceLevel
    {
        //linux��syslog ����־�ֳ�7���ȼ���info < notice < warning < error < crit(ical)< alert < panic
        tlDetail,
        tlInfo,
        tlTrace ,
        tlWarning,
        tlError,
        tlEnd,              //������ǣ���������ö�ٸ�������Ҫֱ��ʹ��
    };

    typedef struct FAST_TRACE_OPTIONS
    {
        FAST_TRACE_OPTIONS()
        {
            bWriteThrough   = FALSE;
            bWriteToFile    = FALSE;
            bDoTimings      = TRUE;
            traceThreshold  = tlTrace;
            pDebugOut       = OutputDebugString;
        }
        BOOL        bWriteToFile;           //�Ƿ�д���ļ���Ĭ������¹ر�
        BOOL        bWriteThrough;          //���д���ļ��Ļ����Ƿ�ֱ��д��(Ч�ʵͣ������ᶪ����Ϣ)
        BOOL        bDoTimings;             //�Ƿ�д�뵱ʱ��ʱ��
        TraceLevel  traceThreshold;         //�����������ֵ -- ֻ�д��ڵ��ڸ���ֵ�Ĳ��������Ĭ��Ϊ tlTrace
        PFNDEBUGOUT pDebugOut;              //��չ�������ʽ��Ĭ��Ϊ OutputDebugString;
    } FAST_TRACE_OPTIONS , * LPFAST_TRACE_OPTIONS ;
    

    //Ĭ���� HMODULE_CUSTOM �� tlTrace �������
#ifdef FTL_DEBUG
#   define FAST_TRACE      FTL::CFFastTrace::GetInstance().WriteLogInfo
#   define FAST_TRACE_EX   FTL::CFFastTrace::GetInstance().WriteLogInfoEx
#else
#   define FAST_TRACE      __noop
#   define FAST_TRACE_EX   __noop
#endif

    #define FTFILESIG 'TSAF'
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// �����ܵ���־�����࣬�������̵߳���־�����ܵ������������Ŀ�ĵ�(�ļ�)�У��鿴ʱ�ٽ���ͳһ
    /// ������־�����ʹ�ã�������ʹ��ģ�沢�� inline �Ļ����Ƿ����ļ���С���ٶ���ɺܴ��Ӱ�죿 
    /// Ҳ����ʹ�� #if(DEBUGLEVEL & BASIC_DEBUG)  #endif �ķ�ʽ������־�ȼ�
    /// -- ʵ�ʴ����������ȡ�������� DLL��Lib ģ����
    /// TODO
    ///   1.�����Ѿ��˳����߳���Ҫ��һ���Ĵ��� -- �� m_AllFileWriters ���Ƴ����������Խ��Խ��
    ///     CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD...);
    ///   2.����ʹ�õ�����д�߳�
    class CFFastTrace
    {
    public:
        typedef struct tagFTFILEHEADER
        {
            DWORD dwSig ;// The signature.  See the FTFILESIG define above.
            BOOL  bIsUnicode;  //������ǰ�Ƿ���ʹ��Unicode��ʽд��
            LONG  lItemCount ;// The total number of traces written to the file.
            LONG  lPID ;// The process ID for this file.  (Used to ensure a file matches it's siblings).
            LONG  lTID ;// The thread ID for this file.
        } FTFILEHEADER , * LPFTFILEHEADER ;

        typedef struct tagFTDATA
        {
            ULONG       lSeqNum ;// The sequence number for this trace.
            FILETIME    stTime ;
            HMODULE     module;
            TraceLevel  level;
            LONG        nTraceInfoLen; //pszTraceInfo �ĳ��ȣ�Ŀǰ������ pszTraceInfo �ַ�������+1(������β��NULL,���˷ѿռ�)������TCHAR
            LPCTSTR     pszTraceInfo;
        } FTDATA , * LPFTDATA ;
    public:
        FTLINLINE static CFFastTrace& GetInstance();
        
        //���ÿ��Խ�����־���������(�����)�͵ȼ�(���ڸõȼ������)
        FTLINLINE BOOL CheckLevel(TraceLevel level);
        FTLINLINE BOOL SetTraceOptions(LPFAST_TRACE_OPTIONS pOptions);
        FTLINLINE BOOL GetTraceOptions(LPFAST_TRACE_OPTIONS pOptions);
        //FTLINLINE BOOL FlushAllFiles();
        //FTLINLINE BOOL SnapAllFiles();
        //Ĭ��ʹ�� tlTrace �������
        FTLINLINE void /*__cdecl*/ WriteLogInfo(const LPCTSTR lpszFormat,...);
        FTLINLINE void /*__cdecl*/ WriteLogInfoEx(TraceLevel level,const LPCTSTR lpszFormat,...);
    protected:
        class CFTFileWriter// : public CFTargetWriter<HANDLE>
        {
        public:
            FTLINLINE CFTFileWriter();
            FTLINLINE ~CFTFileWriter();
            FTLINLINE BOOL Initialize ( HANDLE hFile , DWORD dwTID );
            FTLINLINE BOOL WriteData ( LPFTDATA pData ) ;
            FTLINLINE BOOL Flush ();
            FTLINLINE BOOL SnapFile ( );
            FTLINLINE BOOL Close();
        protected:
            HANDLE m_hFile ;    // The actual file handle.
            LONG   m_lWrites ;  // The total number of writes to the file.
            DWORD  m_dwTID ;    // The thread this file wraps.
            FTLINLINE BOOL WriteHeader () ;
        };
    protected:
        DWORD                   m_dwTLSSlot;            //�����ֲ߳̾������Slotֵ
        ULONG                   m_SequenceNumber;       //���к�
        FAST_TRACE_OPTIONS      m_Options;
        CRITICAL_SECTION        m_CsLock;               //�ڸ���Option��ʱ���л���
        typedef std::set<CFTFileWriter*>            AllFileWriterArrayType;
        typedef AllFileWriterArrayType::iterator    AllFileWriterArrayIterator;
        typedef std::pair<AllFileWriterArrayIterator , bool > AllFileWriterArrayPair;
        AllFileWriterArrayType  m_AllFileWriters;

        //FAST_TRACE_MODULE_INFO  m_AllTraceModuleInfos[MAX_MODULE_INFO_COUNT];
        //LONG                    m_CurrentModuleCount;
        //typedef std::map<HMODULE,FAST_TRACE_MODULE_INFO>    AllTraceModuleInfoType;
        //typedef std::pair<AllTraceModuleInfoType::iterator , bool > AllTraceModuleInfoPair;
        //AllTraceModuleInfoType   m_AllTraceModuleInfos;

        FTLINLINE CFTFileWriter* GetCurrentThreadFile();
        FTLINLINE HANDLE CreateRawThreadFile( DWORD dwTID );
        FTLINLINE BOOL BuildThreadFileName( LPTSTR szFileNameBuff , size_t iBuffLen , DWORD dwTID );
        FTLINLINE BOOL AddFileToArray(CFTFileWriter* pFileWriter);
        FTLINLINE BOOL RemoveFileFromArray(CFTFileWriter* pFileWriter);
        FTLINLINE BOOL CloseAllFileWriters();
        FTLINLINE void InternalWriteLogData(TraceLevel level, LPCTSTR pStrInfo);
    private:
        FTLINLINE CFFastTrace(LPCTSTR pszKey, HMODULE hModule);
        FTLINLINE ~CFFastTrace();
    };

    //�����ܵļ�ʱ���������ڼ��㻨�ѵ�ʱ��(����㿽��ʱ���ٶȹ���) -- ֧����ͣ/����
    #define NANOSECOND_PER_MILLISECOND  (1000 * 1000)
    #define MILLISECOND_PER_SECOND      (1000)
    #define NANOSECOND_PER_SECOND       (NANOSECOND_PER_MILLISECOND * MILLISECOND_PER_SECOND)

    enum RunningStatus
    {
        rsStopped,
        rsRunning,
        rsPaused
    };

    class CFElapseCounter
    {
        DISABLE_COPY_AND_ASSIGNMENT(CFElapseCounter);
    public:
        FTLINLINE CFElapseCounter();
        FTLINLINE ~CFElapseCounter();
        FTLINLINE BOOL Start();
        FTLINLINE BOOL Pause();
        FTLINLINE BOOL Resume();
        FTLINLINE BOOL Stop();
        FTLINLINE RunningStatus GetStatus() const;

        //! ����״̬����ȡ��ǰʱ�䵽��ʼ��ʱ��
        //! ��ͣ״̬����ȡ��ͣ����ʱ�䵽��ʼ��ʱ��
        //! ֹͣ״̬����ȡ��������ʼ��ʱ��
        FTLINLINE LONGLONG GetElapseTime(); //���ص�λ������(NS)
    private:
        LARGE_INTEGER   m_Frequency;
        LARGE_INTEGER   m_StartTime;
        LARGE_INTEGER   m_PauseTime;
        LARGE_INTEGER   m_StopTime;
        RunningStatus   m_Status;
    };

#ifdef __cplusplus
    extern "C"
#endif
    void * _ReturnAddress(void);
#   pragma intrinsic(_ReturnAddress)

#ifndef DEFAULT_BLOCK_TRACE_THRESHOLD    //Ĭ�ϵ�׷����ֵΪ100����
#  define DEFAULT_BLOCK_TRACE_THRESHOLD  (100)
#endif 

#ifndef MAX_TRACE_INDICATE_LEVEL        //Ĭ�����׷��50��
#  define MAX_TRACE_INDICATE_LEVEL    (50)
#endif 

#pragma message( "  MAX_TRACE_INDICATE_LEVEL = " QQUOTE(MAX_TRACE_INDICATE_LEVEL) )

#ifdef FTL_DEBUG
    //ע��:һ����Ҫ�� FUNCTION_BLOCK_INIT �����һ�� FUNCTION_BLOCK_TRACE(0),����Ҫע��һ��Ҫ�������ɵ���ʱ elaplse ��������������
    //  ����FUNCTION_BLOCK_UNINIT, �������Ա���KB118816 �����Ƶ���� new/delete  BlockElapseInfo ��Ӧ
#  define FUNCTION_BLOCK_INIT()     FTL::CFBlockElapse::Init()

    //CFBlockElapse JOIN_TWO(elapse,__LINE__) (TEXT(__FILE__),__LINE__,TEXT(__FUNCTION__),FTL::_ReturnAddress(),(minElapse))
    // #pragma TODO(�˴���д�������⣬�޷������к�����Ψһ���� -- "JOIN_TWO" ��֧�ִ������Ĺ���)
#  define FUNCTION_BLOCK_TRACE(minElapse) \
    FTL::CFBlockElapse JOIN_TWO(elapse,__LINE__) (TEXT(__FILE__),__LINE__,TEXT(__FUNCTION__),FTL::_ReturnAddress(),(minElapse))
#  define FUNCTION_BLOCK_NAME_TRACE(blockName,minElapse) \
    FTL::CFBlockElapse JOIN_TWO(elapse,__LINE__) (TEXT(__FILE__),__LINE__,blockName,FTL::_ReturnAddress(),minElapse)

#  define FUNCTION_BLOCK_UNINIT()   FTL::CFBlockElapse::UnInit()
#else
#  define  FUNCTION_BLOCK_INIT();                           __noop
#  define  FUNCTION_BLOCK_TRACE(minElapse)                  __noop
#  define  FUNCTION_BLOCK_NAME_TRACE(blockName,minElapse)   __noop
#  define  FUNCTION_BLOCK_UNINIT();                         __noop
#endif


#ifdef FTL_DEBUG        //����BlockElapse�����Ĵ�����ʱ�䣬��ˣ�ֻ��Debug״̬������
    //! ���ٳ���������߼���Ч��
    //! @code FUNCTION_BLOCK_TRACE(1000) -- ָ���Ĵ������Ҫ��1000������ִ���꣬�����ӡ��־
    //! ����ÿ���̶߳����Լ��� s_Indent/s_bufIndicate(��̬ȫ���ֲ߳̾�����)�����������������̰߳�ȫ��
    //! Bug��
    //!   1.����� EXE + DLL �ķ�ʽ�����߳�ͬʱ����Exe + Dll ʱ������Ϊ�ǲ�ͬ���̡߳�
    //!   2.��� DLL ��ʹ��__declspec(thread) �����̱߳��ش洢�����ͻ���Ӧ�ó��������ʽ���ӵ� DLL��
    //!     ����ͻ���Ӧ�ó�����ʽ���ӵ� DLL���� LoadLibrary �ĵ��ý�����ɹ����ش� DLL������access violation (AV)���� 
    //!     http://support.microsoft.com/kb/118816/en-us
    //!     ͨ�� Dumpbin.exe �鿴��Thread Storage Directory���Ƿ�Ϊ0������֪��DLL�Ƿ��� ��̬ȫ���ֲ߳̾�������
    //!   ��ô�������TLS+FileMap��
    //!   �����ʽ��ʹ��TLS�Ƚ��KB118816�����⣬�����ڵ�һ��ʱ���䣬���һ��ʱ�ͷţ������Ҫ���߳�
    //!     ��ʼ��ʱ�����һ�� FUNCTION_BLOCK_TRACE(0)����֤���߳̽���ǰindent������0��
    class CFBlockElapse
    {
    public:
        static FTLINLINE BOOL Init();
        static FTLINLINE VOID UnInit();
        //ʹ�ú�����Ϊ�ж��Ƿ�ʱ
        FTLINLINE CFBlockElapse(LPCTSTR pszFileName,DWORD line, 
            LPCTSTR pBlockName, LPVOID pReturnAddr, DWORD MinElapse = 0);
        FTLINLINE ~CFBlockElapse(void);
    private:
        struct BlockElapseInfo
        {
            LONG   indent;
            TCHAR  bufIndicate[MAX_TRACE_INDICATE_LEVEL + 1];//��������NULL��ռ�Ŀռ�
        };
        static DWORD  s_dwTLSIndex;
        const TCHAR* m_pszFileName;
        const TCHAR* m_pszBlkName;
        const LPVOID m_pReturnAdr;
        const DWORD  m_Line;
        const DWORD m_MinElapse;
        DWORD m_StartTime;
    };
#endif //FTL_DEBUG

    //ע�⣺Ŀǰ����SideBySide��ע�⣺���಻���̰߳�ȫ��
    class CModulesHolder  
    {
    public:
        FTLINLINE CModulesHolder();
        FTLINLINE ~CModulesHolder();
        FTLINLINE void Erase(HMODULE hModule);
        FTLINLINE bool Find(HMODULE hModule);
        FTLINLINE bool Insert(HMODULE hModule);
    private:
        typedef std::set<HMODULE>   ModeulesHolderType;
        typedef ModeulesHolderType::iterator ModeulesHolderIterator;
        typedef std::pair<ModeulesHolderIterator , bool > ModeulesHolderPair;
        std::set<HMODULE> m_allModules;
    };

    //Function
    //��ʾMessageǰ��ȡ��ǰ����Ļ����
    //HWND hWnd = (HWND)(AfxGetApp()->m_pMainWnd) ;
    //if ( FALSE == ::IsWindowVisible ( hWnd ) )
    //{
    //    hWnd = ::GetActiveWindow ( ) ;
    //}
    FTLINLINE int __cdecl FormatMessageBox(HWND hWnd, LPCTSTR lpCaption, UINT uType, LPCTSTR lpszFormat, ...);

} //namespace FTL

#endif// FTL_BASE_H

#ifndef USE_EXPORT
#  include "ftlbase.hpp"
#endif