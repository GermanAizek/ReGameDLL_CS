/*
*
*    This program is free software; you can redistribute it and/or modify it
*    under the terms of the GNU General Public License as published by the
*    Free Software Foundation; either version 2 of the License, or (at
*    your option) any later version.
*
*    This program is distributed in the hope that it will be useful, but
*    WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*    In addition, as a special exception, the author gives permission to
*    link the code of this program with the Half-Life Game Engine ("HL
*    Engine") and Modified Game Libraries ("MODs") developed by Valve,
*    L.L.C ("Valve").  You must obey the GNU General Public License in all
*    respects for all of the code used other than the HL Engine and MODs
*    from Valve.  If you modify this file, you may extend this exception
*    to your version of the file, but you are not obligated to do so.  If
*    you do not wish to do so, delete this exception statement from your
*    version.
*
*/

#pragma once

#include "utlmemory.h"

#include <algorithm>

template<class T>
class CUtlVector
{
public:
	typedef T ElemType_t;

	// constructor, destructor
	CUtlVector(int growSize = 0, int initSize = 0);
	CUtlVector(T *pMemory, int numElements);
	~CUtlVector();

	// features C++11 ranged based for
	T *begin() { return &m_Memory[0]; }
	T *end()   { return &m_Memory[m_Size]; }

	T const *begin() const { return &m_Memory[0]; }
	T const *end()   const { return &m_Memory[m_Size]; }

	// Copy the array.
	CUtlVector<T> &operator=(const CUtlVector<T> &other);

	// element access
	T &operator[](int i);
	T const &operator[](int i) const;
	T &Element(int i);
	T const &Element(int i) const;

	// Gets the base address (can change when adding elements!)
	T *Base();
	T const *Base() const;

	// Returns the number of elements in the vector
	// SIZE IS DEPRECATED!
	int Count() const;
	int Size() const;	// don't use me!

	// Is element index valid?
	bool IsValidIndex(int i) const;
	static int InvalidIndex(void);

	// Adds an element, uses default constructor
	int AddToHead();
	int AddToTail();
	int InsertBefore(int elem);
	int InsertAfter(int elem);

	// Adds an element, uses copy constructor
	int AddToHead(T const &src);
	int AddToTail(T const &src);
	int InsertBefore(int elem, T const &src);
	int InsertAfter(int elem, T const &src);

	// Adds multiple elements, uses default constructor
	int AddMultipleToHead(int num);
	int AddMultipleToTail(int num, const T *pToCopy = nullptr);
	int InsertMultipleBefore(int elem, int num, const T *pToCopy = nullptr);	// If pToCopy is set, then it's an array of length 'num' and
	int InsertMultipleAfter(int elem, int num);

	// Calls RemoveAll() then AddMultipleToTail.
	void SetSize(int size);
	void SetCount(int count);

	// Calls SetSize and copies each element.
	void CopyArray(T const *pArray, int size);

	// Add the specified array to the tail.
	int AddVectorToTail(CUtlVector<T> const &src);

	// Finds an element (element needs operator== defined)
	int Find(T const &src) const;

	bool HasElement(T const &src);

	// Makes sure we have enough memory allocated to store a requested # of elements
	void EnsureCapacity(int num);

	// Makes sure we have at least this many elements
	void EnsureCount(int num);

	// Element removal
	void FastRemove(int elem);				// doesn't preserve order
	void Remove(int elem);					// preserves order, shifts elements
	void FindAndRemove(T const &src);		// removes first occurrence of src, preserves order, shifts elements
	void RemoveMultiple(int elem, int num);	// preserves order, shifts elements
	void RemoveAll();						// doesn't deallocate memory

	// Memory deallocation
	void Purge();

	// Purges the list and calls delete on each element in it.
	void PurgeAndDeleteElements();
	void PurgeAndDeleteArrays();

	// Set the size by which it grows when it needs to allocate more memory.
	void SetGrowSize(int size);

	// sort using std:: and expecting a "<" function to be defined for the type
	void Sort();
	void Sort(bool (*pfnLessFunc)(const T &src1, const T &src2));

#if defined(_WIN32)
	void Sort(int (__cdecl *pfnCompare)(const T *, const T *));
#else
	void Sort(int (*pfnCompare)(const T *, const T *));
#endif

	// sort using std:: with a predicate. e.g. [] -> bool (const T &a, const T &b) const { return a < b; }
	template <class F>
	void SortPredicate(F &&predicate);

protected:
	// Can't copy this unless we explicitly do it!
	CUtlVector(CUtlVector const &vec) { Assert(0); }

	// Grows the vector
	void GrowVector(int num = 1);

	// Shifts elements....
	void ShiftElementsRight(int elem, int num = 1);
	void ShiftElementsLeft(int elem, int num = 1);

	// For easier access to the elements through the debugger
	void ResetDbgInfo();

	CUtlMemory<T> m_Memory;
	int m_Size;

	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T *m_pElements;
};

// For easier access to the elements through the debugger
template <class T>
inline void CUtlVector<T>::ResetDbgInfo()
{
	m_pElements = m_Memory.Base();
}

// constructor, destructor
template <class T>
inline CUtlVector<T>::CUtlVector(int growSize, int initSize) :
	m_Memory(growSize, initSize), m_Size(0)
{
	ResetDbgInfo();
}

template <class T>
inline CUtlVector<T>::CUtlVector(T *pMemory, int numElements) :
	m_Memory(pMemory, numElements), m_Size(0)
{
	ResetDbgInfo();
}

template <class T>
inline CUtlVector<T>::~CUtlVector()
{
	Purge();
}

template <class T>
inline CUtlVector<T> &CUtlVector<T>::operator=(const CUtlVector<T> &other)
{
	CopyArray(other.Base(), other.Count());
	return *this;
}

// element access
template <class T>
inline T &CUtlVector<T>::operator[](int i)
{
	DbgAssert(IsValidIndex(i));
	return m_Memory[i];
}

template <class T>
inline T const &CUtlVector<T>::operator[](int i) const
{
	DbgAssert(IsValidIndex(i));
	return m_Memory[i];
}

template <class T>
inline T &CUtlVector<T>::Element(int i)
{
	DbgAssert(IsValidIndex(i));
	return m_Memory[i];
}

template <class T>
inline T const &CUtlVector<T>::Element(int i) const
{
	DbgAssert(IsValidIndex(i));
	return m_Memory[i];
}

// Gets the base address (can change when adding elements!)
template <class T>
inline T *CUtlVector<T>::Base()
{
	return m_Memory.Base();
}

template <class T>
inline T const *CUtlVector<T>::Base() const
{
	return m_Memory.Base();
}

// Count
template <class T>
inline int CUtlVector<T>::Size() const
{
	return m_Size;
}

template <class T>
inline int CUtlVector<T>::Count() const
{
	return m_Size;
}

// Is element index valid?
template <class T>
inline bool CUtlVector<T>::IsValidIndex(int i) const
{
	return (i >= 0) && (i < m_Size);
}

// Returns in invalid index
template <class T>
inline int CUtlVector<T>::InvalidIndex(void)
{
	return -1;
}

// Grows the vector
template <class T>
void CUtlVector<T>::GrowVector(int num)
{
	if (m_Size + num - 1 >= m_Memory.NumAllocated())
	{
		m_Memory.Grow(m_Size + num - m_Memory.NumAllocated());
	}

	m_Size += num;
	ResetDbgInfo();
}

// Makes sure we have enough memory allocated to store a requested # of elements
template <class T>
void CUtlVector<T>::EnsureCapacity(int num)
{
	m_Memory.EnsureCapacity(num);
	ResetDbgInfo();
}

// Makes sure we have at least this many elements
template <class T>
void CUtlVector<T>::EnsureCount(int num)
{
	if (Count() < num)
		AddMultipleToTail(num - Count());
}

// Shifts elements
template <class T>
void CUtlVector<T>::ShiftElementsRight(int elem, int num)
{
	DbgAssert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
		memmove(&Element(elem+num), &Element(elem), numToMove * sizeof(T));
}

template <class T>
void CUtlVector<T>::ShiftElementsLeft(int elem, int num)
{
	DbgAssert(IsValidIndex(elem) || (m_Size == 0) || (num == 0));
	int numToMove = m_Size - elem - num;
	if ((numToMove > 0) && (num > 0))
	{
		memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));

#ifdef _DEBUG
		memset(&Element(m_Size-num), 0xDD, num * sizeof(T));
#endif
	}
}

// Adds an element, uses default constructor
template <class T>
inline int CUtlVector<T>::AddToHead()
{
	return InsertBefore(0);
}

template <class T>
inline int CUtlVector<T>::AddToTail()
{
	return InsertBefore(m_Size);
}

template <class T>
inline int CUtlVector<T>::InsertAfter(int elem)
{
	return InsertBefore(elem + 1);
}

template <class T>
int CUtlVector<T>::InsertBefore(int elem)
{
	// Can insert at the end
	DbgAssert((elem == Count()) || IsValidIndex(elem));

	GrowVector();
	ShiftElementsRight(elem);
	Construct(&Element(elem));
	return elem;
}

// Adds an element, uses copy constructor
template <class T>
inline int CUtlVector<T>::AddToHead(T const &src)
{
	return InsertBefore(0, src);
}

template< class T >
inline int CUtlVector<T>::AddToTail(T const &src)
{
	return InsertBefore(m_Size, src);
}

template< class T >
inline int CUtlVector<T>::InsertAfter(int elem, T const &src)
{
	return InsertBefore(elem + 1, src);
}

template< class T >
int CUtlVector<T>::InsertBefore(int elem, T const &src)
{
	// Can insert at the end
	DbgAssert((elem == Count()) || IsValidIndex(elem));

	GrowVector();
	ShiftElementsRight(elem);
	CopyConstruct(&Element(elem), src);
	return elem;
}

// Adds multiple elements, uses default constructor
template <class T>
inline int CUtlVector<T>::AddMultipleToHead(int num)
{
	return InsertMultipleBefore(0, num);
}

template <class T>
inline int CUtlVector<T>::AddMultipleToTail(int num, const T *pToCopy)
{
	return InsertMultipleBefore(m_Size, num, pToCopy);
}

template <class T>
int CUtlVector<T>::InsertMultipleAfter(int elem, int num)
{
	return InsertMultipleBefore(elem + 1, num);
}

template <class T>
void CUtlVector<T>::SetCount(int count)
{
	RemoveAll();
	AddMultipleToTail(count);
}

template <class T>
inline void CUtlVector<T>::SetSize(int size)
{
	SetCount(size);
}

template <class T>
void CUtlVector<T>::CopyArray(T const *pArray, int size)
{
	SetSize(size);
	for(int i = 0; i < size; i++)
	{
		(*this)[i] = pArray[i];
	}
}

template <class T>
int CUtlVector<T>::AddVectorToTail(CUtlVector const &src)
{
	int base = Count();

	// Make space.
	AddMultipleToTail(src.Count());

	// Copy the elements.
	for (int i = 0; i < src.Count(); i++)
	{
		(*this)[base + i] = src[i];
	}

	return base;
}

template <class T>
inline int CUtlVector<T>::InsertMultipleBefore(int elem, int num, const T *pToInsert)
{
	if (num == 0)
		return elem;

	// Can insert at the end
	DbgAssert((elem == Count()) || IsValidIndex(elem));

	GrowVector(num);
	ShiftElementsRight(elem, num);

	// Invoke default constructors
	for (int i = 0; i < num; i++)
	{
		Construct(&Element(elem+i));
	}

	// Copy stuff in?
	if (pToInsert)
	{
		for (int i = 0; i < num; i++)
		{
			Element(elem+i) = pToInsert[i];
		}
	}

	return elem;
}

// Finds an element (element needs operator== defined)
template <class T>
int CUtlVector<T>::Find(T const &src) const
{
	for (int i = 0; i < Count(); i++)
	{
		if (Element(i) == src)
			return i;
	}

	return InvalidIndex();
}

template <class T>
bool CUtlVector<T>::HasElement(T const &src)
{
	return (Find(src) >= 0);
}

// Element removal
template <class T>
void CUtlVector<T>::FastRemove(int elem)
{
	DbgAssert(IsValidIndex(elem));

	Destruct(&Element(elem));
	if (m_Size > 0)
	{
		Q_memcpy(&Element(elem), &Element(m_Size - 1), sizeof(T));
		m_Size--;
	}
}

template <class T>
void CUtlVector<T>::Remove(int elem)
{
	Destruct(&Element(elem));
	ShiftElementsLeft(elem);
	m_Size--;
}

template <class T>
void CUtlVector<T>::FindAndRemove(T const &src)
{
	int elem = Find(src);
	if (elem != InvalidIndex())
	{
		Remove(elem);
	}
}

template <class T>
void CUtlVector<T>::RemoveMultiple(int elem, int num)
{
	DbgAssert(IsValidIndex(elem));
	DbgAssert(elem + num <= Count());

	for (int i = elem + num; --i >= elem;)
		Destruct(&Element(i));

	ShiftElementsLeft(elem, num);
	m_Size -= num;
}

template <class T>
void CUtlVector<T>::RemoveAll()
{
	for (int i = m_Size; --i >= 0;)
		Destruct(&Element(i));

	m_Size = 0;
}

// Memory deallocation
template <class T>
void CUtlVector<T>::Purge()
{
	RemoveAll();
	m_Memory.Purge();
	ResetDbgInfo();
}

template <class T>
inline void CUtlVector<T>::PurgeAndDeleteElements()
{
	for (int i = 0; i < m_Size; i++)
		delete Element(i);

	Purge();
}

template <class T>
inline void CUtlVector<T>::PurgeAndDeleteArrays()
{
	for (int i = 0; i < m_Size; i++)
		delete[] Element(i);

	Purge();
}

template <class T>
void CUtlVector<T>::SetGrowSize(int size)
{
	m_Memory.SetGrowSize(size);
}

// Sort methods
template <class T>
void CUtlVector<T>::Sort()
{
	std::sort(Base(), Base() + Count());
}

template <class T>
void CUtlVector<T>::Sort(bool (*pfnLessFunc)(const T &src1, const T &src2))
{
	std::sort(Base(), Base() + Count(),
		[pfnLessFunc](const T &a, const T &b) -> bool
		{
			if (&a == &b)
				return false;

			return (*pfnLessFunc)(a, b);
		});
}

#if defined(_WIN32)

template <class T>
void CUtlVector<T>::Sort(int (__cdecl *pfnCompare)(const T *, const T *))
{
	typedef int (__cdecl *QSortCompareFunc_t)(const void *, const void *);
	if (Count() <= 1)
		return;

	qsort(Base(), Count(), sizeof(T), (QSortCompareFunc_t)(pfnCompare));
}

#else // #if defined(_LINUX)

template <class T>
void CUtlVector<T>::Sort(int (*pfnCompare)(const T *, const T *))
{
	typedef int (*QSortCompareFunc_t)(const void *, const void *);
	if (Count() <= 1)
		return;

	qsort(Base(), Count(), sizeof(T), (QSortCompareFunc_t)(pfnCompare));
}
#endif // #if defined(_LINUX)

template <class T>
template <class F>
void CUtlVector<T>::SortPredicate(F &&predicate)
{
	std::sort(Base(), Base() + Count(), predicate);
}
