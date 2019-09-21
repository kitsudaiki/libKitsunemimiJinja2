/**
 *  @file    jinja2_items.cpp
 *
 *  @author  Tobias Anker
 *  Contact: tobias.anker@kitsunemimi.moe
 *
 *  MIT License
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

Jinja2Item::~Jinja2Item() {}

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

IfItem::~IfItem() {}

//===================================================================
// ForLoopItem
//===================================================================
ForLoopItem::ForLoopItem() {type = FOR_ITEM;}

ForLoopItem::~ForLoopItem() {}

}  // namespace Jinja2
}  // namespace Kitsune
