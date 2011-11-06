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

#ifndef DICTIONARYLISTCTRL_H
#define DICTIONARYLISTCTRL_H

#include <wx/wx.h>
#include <wx/listctrl.h>

#include "Dictionary.h"

class DictionaryListCtrl : public wxListCtrl {
    public:
        enum ListType { wxREMOTE, wxLOCAL };
    public:
        DictionaryListCtrl() {};
        DictionaryListCtrl(wxWindow *parent,
                           const wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style)
                : wxListCtrl(parent, id, pos, size, style) {};

        void SetDictionaries(DictionaryArray items);
        void ClearDictionaries();
        void SetType(ListType t);
        void SetColumnWidths(int lastWidth);
        Dictionary* GetSelectedDictionary();
        bool DictionaryExists(wxString id);
        virtual wxString OnGetItemText(long item, long column) const;

    private:
        ListType m_type;
        wxString DisplaySize(long item) const;
        DictionaryArray m_items;
        DECLARE_DYNAMIC_CLASS(DictionaryListCtrl);

};

#endif
