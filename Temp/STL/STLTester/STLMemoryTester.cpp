#include "StdAfx.h"
#include "STLMemoryTester.h"
#include <memory>
#include <iterator>
using std::tr1::shared_ptr;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void CSTLMemoryTester::test_auto_ptr()
{
    DECLARE_MYTEST_DATA_COUNT_CHECKER(ConstructCountChecker,ctDataCount, 0, __FILE__, __LINE__);
    {
        //���Գ�����Χ���Զ��ͷ�
        DECLARE_MYTEST_DATA_COUNT_CHECKER(dataCountChecker,ctDataCount, 0, __FILE__, __LINE__);
        DECLARE_MYTEST_DATA_COUNT_CHECKER(constructCountChecker,ctConstructCount, 1, __FILE__, __LINE__);
        {
            std::auto_ptr<CMyTestData> pMyTestData(new CMyTestData(1));
            CPPUNIT_ASSERT(pMyTestData.get()->GetCurrentIndex() == 1);
        }//�˴��Ժ��Զ��ͷ�
    }

    {
        DECLARE_MYTEST_DATA_COUNT_CHECKER(dataCountChecker,ctDataCount, 0, __FILE__, __LINE__);
        DECLARE_MYTEST_DATA_COUNT_CHECKER(constructCountChecker,ctConstructCount, 1, __FILE__, __LINE__);
        DECLARE_MYTEST_DATA_COUNT_CHECKER(copyConstructChecker,ctCopyConstructCount, 0, __FILE__, __LINE__); //ֻ��ָ��Ŀ����������п������캯��

        //����auto_ptr���и�ֵʱ������Ȩת�� -- ��Ϊ��ֵʱ����Ȩת�Ƶ����⣬���Բ������ڱ�׼����
        std::auto_ptr<CMyTestData> pMyData(new CMyTestData(1));
        CPPUNIT_ASSERT(pMyData->GetCurrentIndex() == 1);

        std::auto_ptr<CMyTestData> pMyDataAssign = pMyData;
        CPPUNIT_ASSERT(NULL == pMyData.get()); //����Ȩת�ƺ�ԭ��������ָ���ΪNULL
    }

    {
        //����reset����
        LONG lCount = CMyTestData::GetTestDataCount();
        {
            std::auto_ptr<CMyTestData> pMyTestData(new CMyTestData(1));
            CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount + 1);
            CPPUNIT_ASSERT(pMyTestData.get()->GetCurrentIndex() == 1);

            //ʹ��reset��������ǰ�Ļ��ͷŵ��������µ�
            pMyTestData.reset(new CMyTestData(2));
            CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount + 1);
            CPPUNIT_ASSERT(pMyTestData.get()->GetCurrentIndex() == 2);

            //ʹ��reset������ͬʱ���������Ļ������ͷ�
            pMyTestData.reset();

            CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount);
            CPPUNIT_ASSERT(pMyTestData.get() == NULL);

        }
        CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount);
    }

    {
        //�����ֹ�ʹ�� release �ͷ� -- ���õ�ʹ�÷�ʽ
        LONG lCount = CMyTestData::GetTestDataCount();

        std::auto_ptr<CMyTestData> pMyTestData(new CMyTestData(1));
        CPPUNIT_ASSERT(NULL != pMyTestData.get());
        CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount + 1);
        CPPUNIT_ASSERT(pMyTestData.get()->GetCurrentIndex() == 1);

        //����release����������ǰ��ָ�룬��˱����ֹ��ͷ�
        CMyTestData* pTestData = pMyTestData.release();
        CPPUNIT_ASSERT(NULL == pMyTestData.get());
        CPPUNIT_ASSERT(NULL != pTestData);
        CPPUNIT_ASSERT(pTestData->GetCurrentIndex() == 1);
        delete pTestData;  //�����ֹ��ͷ�

        CPPUNIT_ASSERT(CMyTestData::GetTestDataCount() == lCount);
    }
}

void CSTLMemoryTester::test_contain_assign()
{
#ifdef NEED_OPERATOR_FUNC
    typedef std::vector<CMyTestData> DataContainer;
    DataContainer srcDataContainer;
    
    //����һ��ʼ��ʱ���ǹ��캯��������ʱ���ǵ��ÿ�������
    DECLARE_MYTEST_DATA_COUNT_CHECKER(ConstructCheck,ctConstructCount, 5, __FILE__, __LINE__);
    {
        for (long i = 0; i < 5; i++)
        {
            srcDataContainer.push_back(CMyTestData(i, 0));
        }

        {
            DECLARE_MYTEST_DATA_COUNT_CHECKER(ConstructCheck,ctConstructCount, 0, __FILE__, __LINE__);
            //��ֵʱ��������е�ÿһ��Ԫ�ص��ÿ������캯�� -- 
            DECLARE_MYTEST_DATA_COUNT_CHECKER(CopyConstructCheck,ctCopyConstructCount, 5, __FILE__, __LINE__);
            //����dstDataContainer���������ں��ͷ�
            DECLARE_MYTEST_DATA_COUNT_CHECKER(DestructCheck,ctDestructCount, 5, __FILE__, __LINE__);
            DataContainer dstDataContainer;
            dstDataContainer = srcDataContainer;
        }

        {
            DECLARE_MYTEST_DATA_COUNT_CHECKER(ConstructCheck,ctConstructCount, 0, __FILE__, __LINE__);
            //assignʱ��������е�ÿһ��Ԫ�ص��ÿ������캯�� -- 
            DECLARE_MYTEST_DATA_COUNT_CHECKER(CopyConstructCheck,ctCopyConstructCount, 5, __FILE__, __LINE__);
            //����dstDataContainer���������ں��ͷ�
            DECLARE_MYTEST_DATA_COUNT_CHECKER(DestructCheck,ctDestructCount, 5, __FILE__, __LINE__);
            DataContainer dstDataContainer;
            dstDataContainer.assign(srcDataContainer.begin(), srcDataContainer.end());
        }

        {
            DECLARE_MYTEST_DATA_COUNT_CHECKER(ConstructCheck,ctConstructCount, 0, __FILE__, __LINE__);
            DECLARE_MYTEST_DATA_COUNT_CHECKER(CopyConstructCheck,ctCopyConstructCount, 5, __FILE__, __LINE__);
            DECLARE_MYTEST_DATA_COUNT_CHECKER(DestructCheck,ctDestructCount, 5, __FILE__, __LINE__);

            DataContainer dstDataContainer;
            dstDataContainer.reserve(srcDataContainer.size());
            std::copy(srcDataContainer.begin(), srcDataContainer.end(), 
                std::back_insert_iterator<DataContainer>(dstDataContainer));
        }
    }
#endif
}

void CSTLMemoryTester::test_shared_ptr()
{
	//���Խ� shared_ptr ��ָ���������������������������Χ�� clear ����Զ��ͷ�
	DECLARE_MYTEST_DATA_COUNT_CHECKER(dataCountChecker,ctDataCount, 0, __FILE__, __LINE__);
	DECLARE_MYTEST_DATA_COUNT_CHECKER(constructCountChecker,ctConstructCount, 4, __FILE__, __LINE__);
	{
		typedef std::vector<std::tr1::shared_ptr<CMyTestData> >		CMyTestDataSharePtrContainer;
		CMyTestDataSharePtrContainer myTestDataSharedPtrs;
		for (int i = 0; i < 4; i++)
		{
			myTestDataSharedPtrs.push_back(shared_ptr<CMyTestData>(new CMyTestData(i)));
		}
	}//�˴��Ժ��Զ��ͷ�

}