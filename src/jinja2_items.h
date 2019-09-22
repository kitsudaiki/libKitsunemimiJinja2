/**
 *  @file    jinja2_items.h
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
 */

#ifndef JINJA2_ITEMS_H
#define JINJA2_ITEMS_H

#include <iostream>
#include <common_items/data_items.h>

using Kitsune::Common::DataItem;
using Kitsune::Common::DataMap;
using Kitsune::Common::DataArray;
using Kitsune::Common::DataValue;

namespace Kitsune
{
namespace Jinja2
{

//===================================================================
// SakuraItem
//===================================================================
class Jinja2Item
{
public:
    enum ItemType
    {
        UNDEFINED_ITEM = 0,
        REPLACE_ITEM = 1,
        IF_ITEM = 2,
        FOR_ITEM = 3,
        TEXT_ITEM = 4
    };

    Jinja2Item();
    virtual ~Jinja2Item();

    Jinja2Item* next = nullptr;

    ItemType getType() const;

protected:
    ItemType type = UNDEFINED_ITEM;
};

//===================================================================
// ReplaceItem
//===================================================================
class TextItem : public Jinja2Item
{
public:
    TextItem();
    ~TextItem();

    std::string text = "";
};

//===================================================================
// ReplaceItem
//===================================================================
class ReplaceItem : public Jinja2Item
{
public:
    ReplaceItem();
    ~ReplaceItem();

    DataArray iterateArray;
};

//===================================================================
// IfItem
//===================================================================
class IfItem : public Jinja2Item
{
public:
    enum compareTypes {
        EQUAL = 0,
        GREATER_EQUAL = 1,
        GREATER = 2,
        SMALLER_EQUAL = 3,
        SMALLER = 4,
        UNEQUAL = 5
    };

    IfItem();
    ~IfItem();

    DataArray leftSide;
    compareTypes ifType = EQUAL;
    DataValue rightSide;

    Jinja2Item* ifChild = nullptr;
    Jinja2Item* elseChild = nullptr;
};

//===================================================================
// ForLoopItem
//===================================================================
class ForLoopItem : public Jinja2Item
{
public:
    ForLoopItem();
    ~ForLoopItem();

    std::string tempVarName = "";
    DataArray iterateArray;

    Jinja2Item* forChild = nullptr;
};


}  // namespace Jinja2
}  // namespace Kitsune

#endif // JINJA2_ITEMS_H
