/* ExwordLibrary - Implements ListCtrl for local and remote dictionary lists
 *
 * Copyright (C) 2011 - Brian Johnson <brijohn@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "DictionaryListCtrl.h"

IMPLEMENT_DYNAMIC_CLASS(DictionaryListCtrl, wxListCtrl);

void DictionaryListCtrl::SetDictionaries(DictionaryArray items)
{
    m_items = items;
    SetItemCount(m_items.GetCount());
    RefreshItems(0, GetItemCount());
}

void DictionaryListCtrl::ClearDictionaries()
{
    m_items.Clear();
    SetItemCount(m_items.GetCount());
    RefreshItems(0, GetItemCount());
}

Dictionary* DictionaryListCtrl::GetSelectedDictionary()
{
    long index = this->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (index == -1)
        return NULL;
    return m_items[index];
}

wxString DictionaryListCtrl::OnGetItemText(long item, long column) const
{
    Dictionary *dict = m_items[item];
    if (column == 0) {
        return dict->GetName();
    } else if (column == 1) {
        return DisplaySize(item);
    }
    return wxT("");
}

wxString DictionaryListCtrl::DisplaySize(long item) const
{
    wxString size;
    Dictionary *dict = m_items[item];
    float sz = (float)dict->GetSize() / 1048576;
    size.Printf(wxT("%5.2f MB"), sz);
    return size;
}
