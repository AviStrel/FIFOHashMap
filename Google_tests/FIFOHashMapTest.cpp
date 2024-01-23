//
// Created by avist on 1/17/24.
//

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../sh-data-struct/FIFOHashMap/FifoHashMap.hpp"
#define MAX_ELEMENTS_TEST 10
struct FIFOHashMapTest: public ::testing::Test
{
    FIFOHashMap<int,std::string,MAX_ELEMENTS_TEST> *pFifoMap;
    std::unordered_map<int,std::string> *pRegularMap;
    std::string strArray[MAX_ELEMENTS_TEST] = {"Zero", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};

    void SetUp()
    {
        pFifoMap = new  FIFOHashMap<int,std::string,MAX_ELEMENTS_TEST>;
        pRegularMap = new std::unordered_map<int,std::string>;
    }

    void TearDown()
    {
        delete pFifoMap;
        delete pRegularMap;
    }

};

TEST_F(FIFOHashMapTest,FIFOHashMapEmpty)
{
EXPECT_TRUE(pFifoMap->empty());
}
TEST_F(FIFOHashMapTest,InsertPositive)
{
auto unorderedMapInsertResult = pRegularMap->insert({1,"One"});
auto insertResult = pFifoMap->insert({1,"One"});

EXPECT_TRUE(insertResult.second);
EXPECT_EQ(unorderedMapInsertResult.first->first,insertResult.first->first); // key test
EXPECT_EQ(unorderedMapInsertResult.first->second,insertResult.first->second);// value test
EXPECT_EQ(pRegularMap->size(),pFifoMap->size());
EXPECT_FALSE(pFifoMap->empty());
}
/**
 * @brief Test to check if second insert with the same key do not change the value.
 */
TEST_F(FIFOHashMapTest,InsertNegative)
{
auto firstInsertResult = pFifoMap->insert({1,"One"});
size_t mapSize = pFifoMap->size();
auto secondInsertResult = pFifoMap->insert({1,"two"});//Should fail because the key already exist

EXPECT_FALSE(secondInsertResult.second);
EXPECT_TRUE(secondInsertResult.first == firstInsertResult.first);

EXPECT_EQ("One", (*pFifoMap)[1]);
EXPECT_EQ(mapSize,pFifoMap->size());
EXPECT_FALSE(pFifoMap->empty());
}
TEST_F(FIFOHashMapTest,OrderOfElements)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}

/*Check the regular order*/
int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++)
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}

/*Check the reverse order*/
i = MAX_ELEMENTS_TEST - 1;
for(auto it = pFifoMap->rbegin(); it != pFifoMap->rend(); it++)
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i--;
}
}
TEST_F(FIFOHashMapTest,InsertExtraElements)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}
std::string extraStrings[MAX_ELEMENTS_TEST] = {"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};

for(int i = 0; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(MAX_ELEMENTS_TEST+i, extraStrings[i]));
EXPECT_EQ(MAX_ELEMENTS_TEST, pFifoMap->size());
if(i < MAX_ELEMENTS_TEST - 1)
{
EXPECT_EQ(i+1, pFifoMap->begin()->first);
EXPECT_EQ(strArray[i+1], pFifoMap->begin()->second);
}
}
auto iter = pFifoMap->begin();
for(int i = 0; i < MAX_ELEMENTS_TEST; i++)
{
EXPECT_EQ(i+MAX_ELEMENTS_TEST, iter->first);
EXPECT_EQ(extraStrings[i], iter->second);
iter++;
}

}

TEST_F(FIFOHashMapTest,SquareBracketsDefault)
{
EXPECT_EQ((*pRegularMap)[1], (*pFifoMap)[1]);
EXPECT_EQ("", (*pFifoMap)[1]);
EXPECT_EQ(pRegularMap->size(),pFifoMap->size());
}

TEST_F(FIFOHashMapTest,SquareBracketsAfterInsert)
{
pRegularMap->insert({1,"One"});
pFifoMap->insert({1,"One"});

EXPECT_EQ((*pRegularMap)[1], (*pFifoMap)[1]);
EXPECT_EQ("One", (*pFifoMap)[1]);
EXPECT_EQ(pRegularMap->size(),pFifoMap->size());
}

TEST_F(FIFOHashMapTest,SquareBracketsInsert)
{
(*pRegularMap)[1] = "One";
(*pFifoMap)[1] = "One";

EXPECT_EQ((*pRegularMap)[1], (*pFifoMap)[1]);
EXPECT_EQ("One", (*pFifoMap)[1]);
EXPECT_EQ(pRegularMap->size(),pFifoMap->size());
}

TEST_F(FIFOHashMapTest,SquareBracketsChangeValue)
{
pFifoMap->insert({1,"One"});
(*pFifoMap)[1] = "New One";
EXPECT_FALSE((*pFifoMap)[1] == "One");
EXPECT_EQ("New One", (*pFifoMap)[1]);
}

TEST_F(FIFOHashMapTest,SquareBracketsLValue)
{
int x = 1;
(*pFifoMap)[x] = "One";
EXPECT_EQ("One", (*pFifoMap)[1]);

}

TEST_F(FIFOHashMapTest,FindAfterInsert)
{
pFifoMap->insert({1,"One"});
(*pFifoMap)[2] = "Two";

auto findResult = pFifoMap->find(1);
EXPECT_TRUE(pFifoMap->end() != findResult);
EXPECT_EQ("One" , findResult->second);

findResult = pFifoMap->find(2);
EXPECT_TRUE(pFifoMap->end() != findResult);
EXPECT_EQ("Two" , findResult->second);

findResult = pFifoMap->find(3);
EXPECT_TRUE(pFifoMap->end() == findResult);
}

TEST_F(FIFOHashMapTest,EraseNotExist)
{
auto result = pFifoMap->erase(1);
EXPECT_EQ(0, result);

pFifoMap->insert({1,"One"});
result = pFifoMap->erase(2);
EXPECT_EQ(0, result);

}
TEST_F(FIFOHashMapTest,EraseAfterInsert)
{
auto insertResultIter = pFifoMap->insert({1,"One"}).first;
auto eraseResultIter = pFifoMap->erase(insertResultIter);
EXPECT_TRUE(pFifoMap->end() == eraseResultIter);
EXPECT_TRUE(pFifoMap->empty());

(*pFifoMap)[1] = "One";
auto removedElements = pFifoMap->erase(1);
EXPECT_EQ(1, removedElements);
EXPECT_TRUE(pFifoMap->empty());

}
TEST_F(FIFOHashMapTest,EraseSeveralElements)
{
auto insertResultIter = pFifoMap->insert({1,"One"}).first;
(*pFifoMap)[2] = "Two";
pFifoMap->insert(std::make_pair(3, "Three"));
(*pFifoMap)[4] = "Four";


size_t oldSize = pFifoMap->size();

/*Remove the first element by iterator
 * Should return an iterator to the next element.
 * */
auto eraseResultIter = pFifoMap->erase(insertResultIter);
EXPECT_EQ(oldSize-1, pFifoMap->size());
EXPECT_EQ(eraseResultIter->first, 2);
EXPECT_EQ(eraseResultIter->second, (*pFifoMap)[2]);

oldSize = pFifoMap->size();

/*Remove a middle element by key
 * Should return the number of elements that were removed
 * */
auto eraseResultSize = pFifoMap->erase(3);
EXPECT_EQ(1, eraseResultSize);
EXPECT_EQ(oldSize-1, pFifoMap->size());

oldSize = pFifoMap->size();
/*Insert a new element and remove it
 * Should return an iterator to the new last element.
 * */
insertResultIter = pFifoMap->insert({1,"One"}).first;
oldSize = pFifoMap->size();

eraseResultIter = pFifoMap->erase(insertResultIter);
EXPECT_EQ(oldSize-1, pFifoMap->size());
EXPECT_EQ(eraseResultIter->first, 4);
EXPECT_EQ(eraseResultIter->second, (*pFifoMap)[4]);

}


TEST_F(FIFOHashMapTest,FindAfterErase)
{
pFifoMap->insert({1,"One"});
(*pFifoMap)[2] = "Two";

auto findResult = pFifoMap->find(1);
EXPECT_TRUE(pFifoMap->end() != findResult);
EXPECT_TRUE("One" == findResult->second);

findResult = pFifoMap->find(2);
EXPECT_TRUE(pFifoMap->end() != findResult);
EXPECT_TRUE("Two" == findResult->second);

pFifoMap->erase(findResult); //Erase by iterator
findResult = pFifoMap->find(2);
//Shouldn't find 2 because it was erased.
EXPECT_TRUE(pFifoMap->end() == findResult);

//Should still find 1 because it wasn't erased.
findResult = pFifoMap->find(1);
EXPECT_TRUE(pFifoMap->end() != findResult);
EXPECT_TRUE("One" == findResult->second);

pFifoMap->erase(1); //Erase by key.
findResult = pFifoMap->find(1);

EXPECT_TRUE(pFifoMap->end() == findResult);

}

TEST_F(FIFOHashMapTest,PopSingleElement)
{
(*pFifoMap)[1] = "one";
pFifoMap->pop();
EXPECT_TRUE(pFifoMap->empty());
}

TEST_F(FIFOHashMapTest,PopAllElements)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}
size_t oldSize = pFifoMap->size();

for(int i = 0; i < MAX_ELEMENTS_TEST-1; i++)
{
pFifoMap->pop();

EXPECT_EQ(oldSize -1, pFifoMap->size());
EXPECT_EQ(strArray[i+1], pFifoMap->begin()->second);

oldSize = pFifoMap->size();
}
pFifoMap->pop();

EXPECT_EQ(oldSize -1, pFifoMap->size());
EXPECT_TRUE(pFifoMap->empty());
}

TEST_F(FIFOHashMapTest,MoveNonExistingElementToTail)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}


/*Move the non-existing element*/
pFifoMap->moveElementToTail(MAX_ELEMENTS_TEST);
EXPECT_EQ(MAX_ELEMENTS_TEST -1, pFifoMap->rbegin()->first);
EXPECT_EQ(strArray[MAX_ELEMENTS_TEST -1], pFifoMap->rbegin()->second);

int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}

}

TEST_F(FIFOHashMapTest,MoveFistElementToTail)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}

/*Move the first element*/
const int firstElementKey = pFifoMap->begin()->first;
pFifoMap->moveElementToTail(firstElementKey);
EXPECT_EQ(0, pFifoMap->rbegin()->first);
EXPECT_EQ(strArray[0], pFifoMap->rbegin()->second);

/*Check the rest*/
int i = firstElementKey;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
if(i != MAX_ELEMENTS_TEST-1)
{
EXPECT_EQ(++i, it->first);
EXPECT_EQ(strArray[i], it->second);
}
else
{
EXPECT_EQ(0, it->first);
EXPECT_EQ(strArray[0], it->second);
}

}

EXPECT_EQ(1, pFifoMap->begin()->first);
EXPECT_EQ(strArray[1], pFifoMap->begin()->second);

}

TEST_F(FIFOHashMapTest,MoveMiddleElementToTail)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}


/*Move from the middle*/
int middleKey = MAX_ELEMENTS_TEST/2;
pFifoMap->moveElementToTail(middleKey);
EXPECT_EQ(middleKey, pFifoMap->rbegin()->first);
EXPECT_EQ(strArray[middleKey], pFifoMap->rbegin()->second);

/*Rest of the elements*/
int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
if(i == MAX_ELEMENTS_TEST - 1)
{
EXPECT_EQ(middleKey, it->first);
EXPECT_EQ(strArray[middleKey], it->second);
}
else if(i >= middleKey)
{
EXPECT_EQ(++i, it->first);
EXPECT_EQ(strArray[i], it->second);
}
else
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}
}

}

TEST_F(FIFOHashMapTest,MoveLastElementToTail)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}

int lastKey = MAX_ELEMENTS_TEST - 1;
pFifoMap->moveElementToTail(lastKey);
EXPECT_EQ(lastKey, pFifoMap->rbegin()->first);
EXPECT_EQ(strArray[lastKey], pFifoMap->rbegin()->second);

int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}

}


TEST_F(FIFOHashMapTest,MoveNonExistingElementToHead)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}


/*Move the non-existing element*/
pFifoMap->moveElementToHead(MAX_ELEMENTS_TEST);
EXPECT_EQ(0, pFifoMap->begin()->first);
EXPECT_EQ(strArray[0], pFifoMap->begin()->second);

int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}

}

TEST_F(FIFOHashMapTest,MoveFistElementToHead)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}

/*Move the first element*/
const int firstElementKey = pFifoMap->begin()->first;
pFifoMap->moveElementToHead(firstElementKey);
EXPECT_EQ(0, pFifoMap->begin()->first);
EXPECT_EQ(strArray[0], pFifoMap->begin()->second);

//Check the rest
int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}

}

TEST_F(FIFOHashMapTest,MoveMiddleElementToHead)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}


/*Move from the middle*/
int middleKey = MAX_ELEMENTS_TEST/2;
pFifoMap->moveElementToHead(middleKey);
EXPECT_EQ(middleKey, pFifoMap->begin()->first);
EXPECT_EQ(strArray[middleKey], pFifoMap->begin()->second);

/*Rest of the elements*/
int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
if(i == 0)
{
EXPECT_EQ(middleKey, it->first);
EXPECT_EQ(strArray[middleKey], it->second);
i++;
}
else if(i <= middleKey)
{
EXPECT_EQ(i-1, it->first);
EXPECT_EQ(strArray[i-1], it->second);
i++;
}
else
{
EXPECT_EQ(i, it->first);
EXPECT_EQ(strArray[i], it->second);
i++;
}
}

}

TEST_F(FIFOHashMapTest,MoveLastElementToHead)
{
for(int i = 0 ; i < MAX_ELEMENTS_TEST; i++)
{
pFifoMap->insert(std::make_pair(i, strArray[i]));
}

int lastKey = MAX_ELEMENTS_TEST - 1;
pFifoMap->moveElementToHead(lastKey);
EXPECT_EQ(lastKey, pFifoMap->begin()->first);
EXPECT_EQ(strArray[lastKey], pFifoMap->begin()->second);

int i = 0;
for(auto it = pFifoMap->begin(); it != pFifoMap->end(); it++ )
{
if(i == 0)
{
EXPECT_EQ(MAX_ELEMENTS_TEST - (++i), pFifoMap->begin()->first);
EXPECT_EQ(strArray[MAX_ELEMENTS_TEST - i], pFifoMap->begin()->second);
}
else
{
EXPECT_EQ(i -1, it->first);
EXPECT_EQ(strArray[i-1], it->second);
i++;
}

}

}
