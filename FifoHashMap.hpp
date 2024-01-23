//
// Created by avist on 1/12/2024.
//

#ifndef FIFOHashMap_FIFOHashMap_HPP
#define FIFOHashMap_FIFOHashMap_HPP

#include <unordered_map>
#include <ostream>

/**
 *FIFO Hash Map

The FIFO (First-In-First-Out) Hash Map is a data structure that combines the characteristics of a hash map and a queue to manage a fixed number of elements efficiently.
 It is designed to maintain a specified maximum number of elements, denoted as 'N', and follows a strict order of insertion.

Key Features:

 Hash Map Structure:

The underlying structure of the FIFO Hash Map is a hash map, allowing for fast key-based access to values.
Each element in the map is represented by a key-value pair.

 Limited Capacity:

The FIFO Hash Map enforces a maximum capacity, denoted by 'N', to control the number of elements it can store.
Once the maximum capacity is reached, the map automatically removes the oldest element when a new element is inserted.

 Order of Insertion:

The order in which elements are inserted is preserved, and retrieval follows the FIFO principle.
The first element inserted is the first to be removed when the maximum capacity is reached.

 Automatic Eviction:

When attempting to insert a new element beyond the maximum capacity, the FIFO Hash Map automatically evicts the oldest element.
This eviction ensures that the map always maintains a fixed number of elements, preventing unbounded growth.

 Usage Scenarios:

Well-suited for scenarios where there is a need to maintain a sliding window of recent data.
Useful in situations where historical data needs to be retained up to a certain limit.

 #include "FIFOHashMap.hpp"

FIFOHashMap<int, std::string, 5> fifoMap;  // Create a FIFO Hash Map with a maximum capacity of 5

fifoMap.insert({1, "one"});
fifoMap.insert({2, "two"});
fifoMap.insert({3, "three"});
fifoMap.insert({4, "four"});
fifoMap.insert({5, "five"});

// After reaching the maximum capacity, inserting a new element will automatically evict the oldest one.

fifoMap.insert({6, "six"});  // Evicts the oldest element (1, "one") and inserts the new element.

// The order of elements now is: (2, "two"), (3, "three"), (4, "four"), (5, "five"), (6, "six")

 The FIFO Hash Map provides an efficient and controlled way to manage a fixed-size collection of data while preserving the order of insertion.
 It combines the benefits of hash-based access with the simplicity of a first-in-first-out mechanism.
 */

template<class K, class V>
class FIFOHashMapCell
{
    template <class KK,class VV, size_t N, class Hash, class KeyEqual, class Allocator>
    friend class FIFOHashMap;
    K m_key;
    V m_value;
    std::pair<const K, V&> m_keyValPair = {m_key, m_value};
    FIFOHashMapCell<K,V> *m_next = nullptr;
    FIFOHashMapCell<K,V> *m_prev = nullptr;

public:
    FIFOHashMapCell() : m_key{}, m_value{} {}
    FIFOHashMapCell(const K &key, const V &val) : m_key(key), m_value(val)  {}
    explicit FIFOHashMapCell(const std::pair<const K, V> &pair): m_key(pair.first), m_value(pair.second){}
    explicit FIFOHashMapCell(std::pair<const K, V> &&pair): m_key(pair.first), m_value(pair.second){}
    FIFOHashMapCell(const FIFOHashMapCell & cell): m_key{cell.m_key}, m_value{cell.m_value}{}
    FIFOHashMapCell( FIFOHashMapCell && cell) noexcept : m_key{cell.m_key}, m_value{cell.m_value}{}
    FIFOHashMapCell& operator=(const FIFOHashMapCell &other)
    {
        if(this == &other)
        {
            return *this;
        }

        m_key = other.m_key;
        m_value = other.m_value;
        return *this;
    }
    FIFOHashMapCell& operator=(FIFOHashMapCell &&other)noexcept
    {
        m_key = other.m_key;
        m_value = other.m_value;
        return *this;
    }

    K getKey() const;

    V getValue() const;

    FIFOHashMapCell *getNext() const;

    FIFOHashMapCell *getPrev() const;

    friend std::ostream &operator<<(std::ostream &os, const FIFOHashMapCell &cell) {
        os << "(" << cell.m_key <<  " , " << cell.m_value << ")";
        return os;
    }

    ~FIFOHashMapCell() = default;
};

/**
 *
 * @tparam K - key
 * @tparam V - Value
 * @tparam N - The size of the array.
 * @tparam Hash - Hash function.
 * @tparam KeyEqual
 * @tparam Allocator
 */
template<
        class K,
        class V,
        size_t N,
        class Allocator = std::allocator<std::pair<const K,  FIFOHashMapCell<K,V>>>,
        class Hash = std::hash<K>,
        class KeyEqual = std::equal_to<K>
>
class FIFOHashMap {

    class FIFOHashMapQueue {
        friend class FIFOHashMap;
        FIFOHashMapCell<K,V> *m_pHead = nullptr;
        FIFOHashMapCell<K,V> *m_pTail = nullptr;

    public:
        FIFOHashMapQueue() = default;
        ~FIFOHashMapQueue() = default;

        void addLast( FIFOHashMapCell<K,V>* pCell);
        void removeLast();
        void removeFirst();
        void removeCell( FIFOHashMapCell<K,V> *cell);

    };


    class FIFOHashMapIterator {
        friend class FIFOHashMap;
        using unordered_map =std::unordered_map<K,FIFOHashMapCell<K,V>,Hash, KeyEqual, Allocator>;

    private:
        FIFOHashMapCell<K,V> *m_pCurrCell;
        unordered_map* m_pMap;
        bool m_isReverse;

    public:
        FIFOHashMapIterator(FIFOHashMapCell<K,V> *pCellPtr, unordered_map *pMap, bool reverse = false);
        ~FIFOHashMapIterator() = default;
        std::pair<const K, V&>& operator*();
        std::pair<const K, V&>* operator->();
        FIFOHashMapIterator& operator++();
        FIFOHashMapIterator& operator++(int);
        FIFOHashMapIterator& operator--();
        FIFOHashMapIterator& operator--(int);
        bool operator==(const FIFOHashMapIterator &other);
        bool operator!=(const FIFOHashMapIterator &other);

    };

    /*Members*/
    std::unordered_map<K, FIFOHashMapCell<K,V>,Hash, KeyEqual, Allocator> m_map;
    FIFOHashMapQueue m_queue;
    size_t m_size;

    /*Methods*/
public:
    FIFOHashMap();
    FIFOHashMap(std::initializer_list<std::pair<const K, V>> initList);

    ~FIFOHashMap() = default;


    using value_type = std::pair<const K, V>;

    /*Insert methods*/
    /**
     * @brief Insert new element to the hash map. If the key is already exist - do nothing.
     * @param value - (Key,Value) pair.
     * @return A std::pair consisting of an iterator to the inserted element
     * (or to the element that prevented the insertion) and a bool denoting whether the insertion took place
     * (true if insertion happened, false if it did not).
     */
    std::pair<FIFOHashMapIterator, bool> insert(const value_type& value);
    std::pair<FIFOHashMapIterator,bool> insert( value_type&& value );

    /*Erase methods*/
    /**
     * @brief Removes the element at pos.
     * @param pos - iterator to the element to remove.
     * @return Iterator following the last removed element.
     */
    FIFOHashMapIterator erase( FIFOHashMapIterator pos );
    /**
     * @brief  Removes the element (if one exists) with the key equivalent to key.
     * @param key
     * @return Number of elements removed (0 or 1).
     */
    size_t erase( const K& key );

    /**
     * @brief Finds an element with key equivalent to key.
     * @param key - key value of the element to search for
     * @return An iterator to the requested element. If no such element is found,
     * past-the-end (see end()) iterator is returned.
     *
     */
    FIFOHashMapIterator find(const K& key);

    /*Value access*/
    /**
     * @brief Returns a reference to the value that is mapped to a key equivalent to key,
     * performing an insertion if such key does not already exist.
     * @param key - the key of the element to find
     * @return A reference to the mapped value of the new element if no element with key key existed.
     * Otherwise, a reference to the mapped value of the existing element whose key is equivalent to key.
     */
    V& operator[]( const K& key );
    /**
     * @brief Returns a reference to the value that is mapped to a key equivalent to key,
     * performing an insertion if such key does not already exist.
     * @param key - the key of the element to find
     * @return A reference to the mapped value of the new element if no element with key key existed.
     * Otherwise, a reference to the mapped value of the existing element whose key is equivalent to key.
     */
    V& operator[]( K&& key );

    /**
     * @brief Popping the first element in the queue
     */
    void pop();

    /**
     * @brief Move the element to the end of the queue if existed. If not - do nothing
     * @param key
     */
    void moveElementToTail(const K& key);

    /**
     * @brief Move the element to the beginning of the queue if existed. If not - do nothing
     * @param key
     */
    void moveElementToHead(const K& key);

    size_t size() const
    {
        return m_size;
    }

    bool empty() const
    {
        return (m_size == 0);
    }

    bool isFull() const;

    /**
     *
     * @return Iterator to the first element.
     */
    FIFOHashMapIterator begin()
    {
        return FIFOHashMapIterator{m_queue.m_pHead, &m_map};
    }
    /**
     *
     * @return Iterator to the element following the last element.
     */
    FIFOHashMapIterator end()
    {
        return FIFOHashMapIterator{nullptr, &m_map};
    }
    /**
     *
     * @return Returns a reverse iterator to the first element of the reversed queue.
     * It corresponds to the last element of the non-reversed queue.
     * If the vector is queue, the returned iterator is equal to rend().
     */
    FIFOHashMapIterator rbegin()
    {
        return FIFOHashMapIterator{m_queue.m_pTail, &m_map, true};
    }
    /**
     *
     * @return Returns a reverse iterator to the element following the last element of the reversed vector.
     * It corresponds to the element preceding the first element of the non-reversed queue.
     * This element acts as a placeholder, attempting to access it results in undefined behavior.
     */
    FIFOHashMapIterator rend()
    {
        return FIFOHashMapIterator{nullptr, &m_map, true};
    }

};

/*FIFOHashMap*/

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMap():m_map{}, m_queue{},m_size{0} {
    m_map.reserve(N);
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMap(std::initializer_list<std::pair<const K, V>> initList):m_map{}, m_queue{}, m_size{0}
{
    m_map.reserve(N);
    for (const auto &p: initList)
    {
        //Insert return a <iterator,bool> pair.
        auto resultPair = m_map.emplace(p.first,  FIFOHashMapCell<K,V>{p});
        if(resultPair.second == true)
        {
            if(m_size >= N)
            {
                pop();
            }

            m_queue.addLast(&((resultPair.first)->second));
            m_size++;
        }
    }
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
std::pair<typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator, bool>
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::insert(FIFOHashMap::value_type &&value)
{

    auto mapIter = m_map.find(value.first);
    if(mapIter != m_map.end())
    {
        /*Element exist*/
        return std::make_pair(FIFOHashMapIterator{&(mapIter->second), &m_map}, false);
    }
    /*New element*/
    if(m_size >= N)
    {
        /*If table is full - pop the first one*/
        pop();
    }

    auto resultPair = m_map.emplace(value.first,  FIFOHashMapCell<K,V>{std::move(value)});
    if(resultPair.second == true)
    {
        m_queue.addLast(&((resultPair.first)->second));
        m_size++;
        return std::make_pair(FIFOHashMapIterator{m_queue.m_pTail, &m_map}, true);
    }

    return std::make_pair(end(), false);
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
std::pair<typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator, bool>FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::
insert(const FIFOHashMap::value_type &value)
{

    auto mapIter = m_map.find(value.first);
    if(mapIter != m_map.end())
    {
        /*Element exist*/
        return std::make_pair(FIFOHashMapIterator{&(mapIter->second), &m_map}, false);
    }
    /*New element*/
    if(m_size >= N)
    {
        /*If table is full - pop the first one*/
        pop();
    }

    auto resultPair = m_map.emplace(value.first,  FIFOHashMapCell<K,V>{value});
    if(resultPair.second == true)
    {
        m_queue.addLast(&((resultPair.first)->second));
        m_size++;
        return std::make_pair(FIFOHashMapIterator{m_queue.m_pTail, &m_map}, true);
    }

    return std::make_pair(end(), false);
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
V &FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::operator[](K &&key)
{
    auto mapIter = m_map.find(key);
    if(mapIter == m_map.end())
    {//New element - need to insert.
        if(m_size >= N)
        {
            /*If table is full - pop the first one*/
            pop();
        }
        FIFOHashMapCell<K,V> &&newCell{std::move(key), V{}};
        auto emplaceResultPair = m_map.emplace(newCell.m_key,  std::move(newCell));
        mapIter = emplaceResultPair.first;
        if(emplaceResultPair.second == true)
        {//Insertion succeeded - add it to the end of the list.
            m_queue.addLast(&((emplaceResultPair.first)->second));
            m_size++;
            return mapIter->second.m_value;
        }
    }
    return mapIter->second.m_value;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
V &FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::
operator[](const K &key)
{
    auto mapIter = m_map.find(key);
    if(mapIter == m_map.end())
    {//New element - need to insert.
        if(m_size >= N)
        {
            /*If table is full - pop the first one*/
            pop();
        }

        auto emplaceResultPair = m_map.emplace(key,  FIFOHashMapCell<K,V>{key, V{}});
        mapIter = emplaceResultPair.first;
        if(emplaceResultPair.second == true)
        {//Insertion succeeded - add it to the end of the list.
            m_queue.addLast(&((emplaceResultPair.first)->second));
            m_size++;
            return mapIter->second.m_value;
        }
    }
    return mapIter->second.m_value;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMap::FIFOHashMapIterator
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::erase(FIFOHashMap::FIFOHashMapIterator pos) {
    if((pos != end()) && (m_map.erase(pos->first) != 0))
    {
        m_queue.removeCell(pos.m_pCurrCell);
        m_size--;
        if(pos.m_pCurrCell->m_next == nullptr)
        {
            //pos is the last element.
            return FIFOHashMapIterator{m_queue.m_pTail, &m_map};
        }
        return (++pos);
    }
    return end();
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
size_t FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::erase(const K &key)
{
    auto unorderedMapIter = m_map.find(key);
    if(unorderedMapIter != m_map.end())
    {
        erase(FIFOHashMapIterator{&(unorderedMapIter->second), &m_map});
        return 1;
    }
    return 0;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMap::FIFOHashMapIterator
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::find(const K &key)
{
    auto unorderedMapIter = m_map.find(key);
    if(unorderedMapIter != m_map.end())
    {
        return FIFOHashMap::FIFOHashMapIterator{&unorderedMapIter->second, &m_map};
    }
    return FIFOHashMap::FIFOHashMapIterator(nullptr, &m_map);
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::pop()
{
    K key = m_queue.m_pHead->m_key;
    m_queue.removeFirst();
    m_map.erase(key);
    m_size--;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::moveElementToTail(const K &key)
{
    auto unorderedMapIter = m_map.find(key);
    if(unorderedMapIter != m_map.end())
    {
        FIFOHashMapCell<K,V> &cell = unorderedMapIter->second;
        m_queue.removeCell(&cell);
        cell.m_next = cell.m_prev = nullptr;
        m_queue.addLast(&cell);
    }
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::moveElementToHead(const K &key)
{
    auto unorderedMapIter = m_map.find(key);
    if(unorderedMapIter != m_map.end())
    {
        FIFOHashMapCell<K,V> &cell = unorderedMapIter->second;
        m_queue.removeCell(&cell);
        cell.m_prev = nullptr;
        cell.m_next = m_queue.m_pHead;
        m_queue.m_pHead->m_prev = &cell;
        m_queue.m_pHead = &cell;

    }
}

/********************************************************************************/
/*FIFOHashMapCell*/
/********************************************************************************/
template<class K, class V>
K FIFOHashMapCell<K,V>::getKey() const {
    return m_key;
}

template<class K, class V>
V FIFOHashMapCell<K,V>::getValue() const {
    return m_value;
}

template<class K, class V>
FIFOHashMapCell<K,V> *FIFOHashMapCell<K,V>::getNext() const{
    return m_next;
}

template<class K, class V>
FIFOHashMapCell<K,V> *FIFOHashMapCell<K,V>::getPrev() const{
    return m_prev;
}

/********************************************************************************/
/*FIFOHashMapQueue*/
/********************************************************************************/

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapQueue::addLast( FIFOHashMapCell<K,V>* pCell)
{
    if(m_pHead == nullptr)
    {
        //First element
        m_pHead = m_pTail = pCell;
        return;
    }
    m_pTail->m_next = pCell;
    pCell->m_prev = m_pTail;
    m_pTail = pCell;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapQueue::removeLast()
{
    if(m_pHead == m_pTail)
    {
        m_pHead = m_pTail = nullptr;
        return;
    }
    m_pTail = m_pTail->m_prev;
    m_pTail->m_next = nullptr;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapQueue::removeFirst() {
    if(m_pHead == nullptr)
    { // Maybe need to remove
        return;
    }
    if(m_pHead == m_pTail)
    {
        m_pHead = m_pTail = nullptr;
        return;
    }
    m_pHead = m_pHead->m_next;
    m_pHead->m_prev = nullptr;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
void FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapQueue::removeCell(FIFOHashMapCell<K,V> *cell)
{
    if(cell == nullptr)
    {
        return;
    }
    FIFOHashMapCell<K,V> *previousCell = cell->m_prev;
    FIFOHashMapCell<K,V> *nextCell = cell->m_next;
    if(previousCell == nextCell)
    {
        m_pHead = m_pTail = nullptr;
        return;
    }
    if(previousCell != nullptr)
    {
        previousCell->m_next = nextCell;
    }
    else
    {
        m_pHead = nextCell;
    }

    if(nextCell != nullptr)
    {
        nextCell->m_prev = previousCell;
    }
    else
    {
        m_pTail = previousCell;
    }
}


/********************************************************************************/
/*FIFOHashMapIterator*/
/********************************************************************************/

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::FIFOHashMapIterator(FIFOHashMapCell<K,V> *pCellPtr,
                                                                                          unordered_map *pMap,
                                                                                          bool reverse):m_pCurrCell{pCellPtr}, m_pMap{pMap}, m_isReverse{reverse}
{
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
std::pair<const K, V &> &FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator*()
{
    return m_pCurrCell->m_keyValPair;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
std::pair<const K, V &> *FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator->()
{
    return &m_pCurrCell->m_keyValPair;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator& FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator++()
{
    if(m_isReverse == false)
    {
        m_pCurrCell = m_pCurrCell->getNext();
    }
    else
    {
        m_pCurrCell = m_pCurrCell->getPrev();
    }
    return *this;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator& FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator++(int) {
    FIFOHashMapIterator &result = (*this);
    ++(*this);
    return result;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator& FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator--() {
    if(m_isReverse == false)
    {
        m_pCurrCell = m_pCurrCell->getPrev();
    }
    else
    {
        m_pCurrCell = m_pCurrCell->getNext();
    }
    return *this;
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
typename FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator& FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::
operator--(int) {
    auto &result = *this;
    --(*this);
    return result;
}


template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
bool  FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::operator==(const FIFOHashMapIterator &other) {
    return (m_pCurrCell == other.m_pCurrCell) && (m_pMap == other.m_pMap);
}

template<class K, class V, size_t N, class Hash, class KeyEqual, class Allocator>
bool  FIFOHashMap<K, V, N, Hash, KeyEqual, Allocator>::FIFOHashMapIterator::operator!=(const FIFOHashMapIterator &other) {
    return (m_pCurrCell != other.m_pCurrCell) || (m_pMap != other.m_pMap);
}


#endif //FIFOHashMap_FIFOHashMap_HPP
