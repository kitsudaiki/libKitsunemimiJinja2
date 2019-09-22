/**
 *  @file    jinja2_items.cpp
 *
 *  @author  Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright MIT License
*/

#include <jinja2_items.h>

namespace Kitsune
{
namespace Jinja2
{

//===================================================================
// Jinja2Item
//===================================================================
Jinja2Item::Jinja2Item() {}

Jinja2Item::~Jinja2Item()
{
    if(next != nullptr) {
        delete next;
    }
}

Jinja2Item::ItemType Jinja2Item::getType() const
{
    return type;
}

//===================================================================
// TextItem
//===================================================================
TextItem::TextItem() {type = TEXT_ITEM;}

TextItem::~TextItem() {}

//===================================================================
// ReplaceItem
//===================================================================
ReplaceItem::ReplaceItem() {type = REPLACE_ITEM;}

ReplaceItem::~ReplaceItem() {}

//===================================================================
// IfItem
//===================================================================
IfItem::IfItem() {type = IF_ITEM;}

IfItem::~IfItem()
{
    if(ifChild != nullptr) {
        delete ifChild;
    }
    if(elseChild != nullptr) {
        delete elseChild;
    }
}

//===================================================================
// ForLoopItem
//===================================================================
ForLoopItem::ForLoopItem() {type = FOR_ITEM;}

ForLoopItem::~ForLoopItem()
{
    if(forChild != nullptr) {
        delete forChild;
    }
}

}  // namespace Jinja2
}  // namespace Kitsune
