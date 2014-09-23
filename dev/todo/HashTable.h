#ifndef COMM_HASHTABLE_H_
#define COMM_HASHTABLE_H_


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ASSERT.h"

namespace SEC
{


/**
 * Hash 模板类，
 * 
 * @note 对于不同的数据结构 HASHELEM,
 * 只需要包含一个key数据成员，就可以使用该类来构造hash
 * 其中约定key = 0 为空的数据项
 * 
 * 
 * @author arrowgu
*/
template<class HASHELEM, class HASHKEY=uint32_t, int MAX_HASH_TIMES=100>
class HashTable
{
public:	
	HashTable();
	~HashTable();
	
	/**
	 * 初始化函数
	 * 
	 * @param [in] iShmKey hash表使用的共享内存key
	 * @param [in] iHashBase 每行hash表的元素个数
	 * @parma [in] iHashTimes hash表的行数或者叫做深度
 	 * 
	 * @note 如果使用共享内存来存储数据，调用这个函数
	 */
	bool Init(int iShmKey, int iHashBase, int iHashTimes);
	
	/**
	 * 初始化函数
	 * 
	 * @param [in] iShmKey hash表使用的共享内存key
	 * @param [in] iHashBase 每行hash表的元素个数
	 * @parma [in] iHashTimes hash表的行数或者叫做深度
	 * 
	 * @note 如果堆内存来存储数据，调用这个函数
	 */
	bool Init(int iHashBase, int iHashTimes);
	
	/**
	 * 插入数据元素
	 * 
	 * @param [in] stHashElem 数据信息
	 * 
	 * @return 返回插入是否成功
	 * 		true - 成功
	 * 		false - 失败， 如果hash表初始化成功，那么失败是因为hash冲突导致，\n
	 * 		这个时候，可以调用GetConflictElem来获取到冲突的元素，然后覆盖
	 */
	bool 		InsertElem(const HASHELEM &stHashElem);
	
	/**
	 * 查询元素
	 */
	HASHELEM* 	GetElem(HASHKEY key);	
	
	/**
	 * 获取与指定key冲突的元素
	 * 
	 * @parma [in] key 冲突的key值
	 * @parma [in] iTimesIndex 获取第几行中冲突的元素
	 * 
	 * @note 如果要想获取到所有冲突的元素
	 */
	HASHELEM*   GetConflictElem(HASHKEY key, int iTimesIndex);
	
	void DelElem(const HASHKEY key);
	void DelAllElems();

    int  GetHashBase() { return m_ulHashBase; }
    int  GetHashTimes() { return m_iHashTimes; }
	
private:		
	bool iIsPrime(int iVal);
	bool iInit(uint8_t  *pHashMem, int  iHashBase, 	int   iHashTimes);
	
	/*!获得Hash需要的内存内存大小
	 * @param [in]  iHashTimes  Hash的桶数
	 * @param [in]  iHashBase  	每个桶数最大的容量
	 *
	 * @return  需要的内存大小,字节为单位
	 * @note
	 *    该模块内部依赖 neocomm 库
	 */
	int iCalHashMemSize(int iHashTimes, int iHashBase)
	{
		return sizeof(HASHELEM)*iHashTimes*iHashBase;
	}

private:
	HASHELEM		*m_pHashMem;
	int				m_iMemSize;
	int				m_iShmKey;

	unsigned long   m_ulHashBase;	
	int				m_iHashTimes;
	unsigned long 	m_aulHashBases[MAX_HASH_TIMES]; 

	uint32_t			m_dwCurrTimes;
	uint32_t			m_dwCurrPos;
};

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::HashTable()
{
	m_pHashMem = NULL;
	m_pHashMem = 0;
	m_iHashTimes = 0;
	m_ulHashBase = 0;

	m_iShmKey = 0;

	m_dwCurrPos = 0;
	m_dwCurrTimes = 0;
	
	memset(m_aulHashBases, 0, sizeof(m_aulHashBases));
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::~HashTable()
{
	if (0 == m_iShmKey && m_pHashMem != NULL)
		delete m_pHashMem;
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
bool
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::Init(int iShmKey, 
		int iHashBase, int iHashTimes)
{
	if (m_pHashMem)
	{
		ASSERT(!m_pHashMem);
		return false;
	}
	
	if (iShmKey <= 0)
	{
		ASSERT(iShmKey > 0);
		return false;
	}
	if (iHashTimes > MAX_HASH_TIMES || iHashTimes <= 0 ||
		iHashBase <= 0)
	{	
		ASSERT(iHashBase > 0);
		ASSERT(iHashTimes > 0 && iHashTimes <= MAX_HASH_TIMES);
		return false;
	}

	int iShmSize = iCalHashMemSize(iHashTimes, iHashBase);
	ASSERT(iShmSize > 0);
	
	void *pShm = GetShm(iShmKey, iShmSize, 0666);
	if (!pShm)
	{
		pShm = GetShm(iShmKey, iShmSize, 0666 | IPC_CREAT);
        if (pShm)
        {
            bzero(pShm, iShmSize);
        }
	}
	if (!pShm)
	{
		ASSERT(pShm);
		return false;
	}
	
	m_iShmKey = iShmKey;
	return iInit((uint8_t*)pShm, iHashBase, iHashTimes);
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
bool
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::Init(int iHashBase, int iHashTimes)
{
	if (m_pHashMem)
	{
		ASSERT(!m_pHashMem);
		return false;
	}
	
	if (iHashTimes > MAX_HASH_TIMES || iHashTimes <= 0 ||
		iHashBase <= 0)
	{	
		ASSERT(iHashBase > 0);
		ASSERT(iHashTimes > 0 && iHashTimes <= MAX_HASH_TIMES);
		return false;
	}

	int iSize = iCalHashMemSize(iHashTimes, iHashBase);
	ASSERT(iSize > 0);

	uint8_t *pMem = new uint8_t[iSize];
	bzero(pMem, iSize);
	bool bRet = iInit(pMem, iHashBase, iHashTimes);
	if (!bRet)
	{
		delete pMem;
	}

	return bRet;
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
bool 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::iInit(uint8_t  *pHashMem, int  iHashBase, 
			 int   iHashTimes)
{
	ASSERT(pHashMem);
	ASSERT(iHashTimes > 0 && iHashTimes <= MAX_HASH_TIMES);

	int iLastPrim = iHashBase;
	for (int i = 0; i < iHashTimes; i++)
	{
		while (!iIsPrime(iLastPrim) && iLastPrim > 2)
			iLastPrim--;
		
		m_aulHashBases[i] = iLastPrim--;
	}

	m_pHashMem 	 = (HASHELEM*)pHashMem;
	m_iMemSize 	 = iCalHashMemSize(iHashTimes, iHashBase);
	m_iHashTimes = iHashTimes;
	m_ulHashBase  = iHashBase;
	
	return true;
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
HASHELEM* 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::GetElem(HASHKEY key)
{
	HASHELEM *pHashElem;
	//uint32_t dwKey = (uint32_t)key;
	int iHash;
	int i;	

	if (!m_pHashMem)
	{
		ASSERT(m_pHashMem);
		return NULL;
	}

	if (key == 0)
		return NULL;
	
	for (i = 0; i < m_iHashTimes; i++)
	{
		iHash = key % m_aulHashBases[i];
		pHashElem = m_pHashMem + m_ulHashBase*i + iHash;
		if (pHashElem->key == key)
			return pHashElem;
	}
	
	return NULL;
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
bool 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::InsertElem(const HASHELEM &stHashElem)
{
	HASHELEM *pEmptyElem = NULL;
	HASHELEM *pHashElem;
	int     iHash;
	int     i;	

	COMPILE_ASSERT(sizeof(HASHKEY) == sizeof(stHashElem.key));

	if (!m_pHashMem)
	{
		ASSERT(m_pHashMem);
		return false;
	}

    if (stHashElem.key == 0)
    {
        ASSERT(stHashElem.key != 0);
        return false;
    }

	
	for (i = 0; i < m_iHashTimes; i++)
	{
		iHash = stHashElem.key % m_aulHashBases[i];
		pHashElem = m_pHashMem + m_ulHashBase*i + iHash;
		
		if (pHashElem->key == stHashElem.key)
		{
			//overlap
			memcpy(pHashElem, &stHashElem, sizeof(stHashElem));
			return true;
		}

		if (0 == pHashElem->key && !pEmptyElem)
			pEmptyElem = pHashElem;
	}

	if (pEmptyElem)
	{
		memcpy(pEmptyElem, &stHashElem, sizeof(stHashElem));
		return true;
	}
	
	return false;
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
HASHELEM* 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::GetConflictElem(HASHKEY key, int iIndex)
{
	if (!m_pHashMem)
	{
		ASSERT(m_pHashMem);
		return false;
	}
	
	if (iIndex < 0 || iIndex >= m_iHashTimes)
	{
		ASSERT(iIndex >= 0 && iIndex < m_iHashTimes);
		return NULL;
	}

	int iHash = key % m_aulHashBases[iIndex];
	return (m_pHashMem + m_ulHashBase*iIndex + iHash);
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
void 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::DelElem(const HASHKEY key)
{
	HASHELEM *pHashElem;
	int   	iHash;
	int 	i;

	if (!m_pHashMem)
	{
		ASSERT(m_pHashMem);
		return;
	}
	
	for (i = 0; i < m_iHashTimes; i++)
	{
		iHash = key % m_aulHashBases[i];
		pHashElem = (m_pHashMem + m_ulHashBase*i + iHash);
		if (pHashElem->key == key)
		{
			memset(pHashElem, 0, sizeof(HASHELEM));
			break;
		}
	}
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
void 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::DelAllElems()
{
	if (!m_pHashMem)
	{
		ASSERT(m_pHashMem);
		return;
	}

	memset(m_pHashMem, 0, m_iMemSize);
}

template<class HASHELEM, class HASHKEY, int MAX_HASH_TIMES>
bool 
HashTable<HASHELEM, HASHKEY, MAX_HASH_TIMES>::iIsPrime(int iVal)
{
	if (iVal % 2 == 0) return false;
	
	int iEnd = (int)sqrt(iVal) + 1;
	if (iEnd > iVal/2)
		iEnd = iVal/2;
	
	for (int i = 3; i<= iEnd; i++)
	{
		if (iVal % i == 0) 
		{
			return false;
		}
	}
	
	return true;
}

}

#endif

