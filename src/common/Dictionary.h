/* ExwordTools - Classes representing remote and local dictionaries
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

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <wx/wx.h>

class Exword;

class DictionaryInfo {
    public:
        DictionaryInfo(char *id, char *name, char *key);
        wxString GetId() { return m_id; };
        wxString GetName() { return m_name; };
        wxMemoryBuffer GetKey() { return m_key; };
        bool operator==(const DictionaryInfo& other);
    private:
        wxMemoryBuffer m_key;
        wxString m_name;
        wxString m_id;
};

extern DictionaryInfo InvalidDictionary;

class Dictionary {
    public:
        Dictionary(wxString id);
        wxString GetId();
        virtual wxString GetName() = 0;
        virtual unsigned long long GetSize() = 0;
        virtual bool Exists() = 0;
        virtual wxArrayString GetFiles() = 0;
        virtual unsigned long GetMinSupportedSeries() = 0;
    protected:
        wxString m_id;
};

class LocalDictionary : public Dictionary {
    public:
        LocalDictionary(wxString id);
        virtual wxString GetName();
        virtual unsigned long long GetSize();
        virtual bool Exists();
        virtual wxArrayString GetFiles();
        virtual unsigned long GetMinSupportedSeries();
    private:
        wxString m_path;
};


class RemoteDictionary : public Dictionary {
    public:
        RemoteDictionary(wxString id, Exword *device);
        virtual wxString GetName();
        virtual unsigned long long GetSize();
        virtual bool Exists();
        virtual wxArrayString GetFiles();
        virtual unsigned long GetMinSupportedSeries();
    private:
        Exword *m_device;
        wxString m_name;
        wxString m_path;
};


WX_DEFINE_ARRAY_PTR(Dictionary*, DictionaryArray);

#endif
