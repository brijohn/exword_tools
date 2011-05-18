/* ExwordTools - Main class used for communicating with Exword dictionaries
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

#ifndef EXWORDDEVICE_H
#define EXWORDDEVICE_H

#include <exword.h>

#include "Dictionary.h"

enum ExwordMode {
    LIBRARY = 1,
    CD,
    TEXT,
};

enum ExwordRegion {
    JAPANESE = 1,
    CHINESE,
    KOREAN,
    GERMAN,
    SPANISH,
    FRENCH,
    RUSSIAN,
};

enum ExwordStorage {
    INTERNAL = 1,
    SD,
};

class Capacity {
    public:
        Capacity() {m_free = 0; m_total = 0; };
        Capacity(unsigned long total, unsigned long free) { m_free = free; m_total = total; };
        unsigned long GetTotal() { return m_total; };
        unsigned long GetFree() { return m_free; };
    private:
        unsigned long m_total;
        unsigned long m_free;
};

class ExwordCallback {
    public:
        ExwordCallback(void *data) { m_data = data; };
        virtual void PutFile(wxString filename, unsigned long transferred, unsigned long length) {};
        virtual void GetFile(wxString filename, unsigned long transferred, unsigned long length) {};
    protected:
        void *m_data;
};

class Exword {
    public:
        Exword();
        Exword(ExwordMode mode, ExwordRegion region = JAPANESE);
        ~Exword();
        static wxString GetUserDataDir();
        bool Connect(ExwordMode mode, ExwordRegion region = JAPANESE);
        void Disconnect();
        bool IsConnected();
        wxMemoryBuffer Reset(wxString user);
        bool Authenticate(wxString user, wxMemoryBuffer& key);
        DictionaryInfo GetDictionaryInfo(wxString id);
        DictionaryArray GetLocalDictionaries();
        DictionaryArray GetRemoteDictionaries();
        bool IsSdInserted();
        wxArrayString List(wxString path, wxString pattern = wxT("*"));
        Capacity GetCapacity();
        bool UploadFile(wxFileName filename);
        bool DeleteFile(wxString filename);
        bool InstallDictionary(LocalDictionary *dict);
        bool RemoveDictionary(RemoteDictionary *dict);
        bool SetStorage(ExwordStorage storage);
        void SetFileCallback(ExwordCallback *cb);
    private:
        void ReadAdmini(wxMemoryBuffer& buffer);
        wxString GetStoragePath();
    private:
        exword_t *m_device;
        bool m_connected;
        ExwordCallback *m_callback;
        ExwordStorage m_storage;
        ExwordMode m_mode;
        ExwordRegion m_region;
};

#endif
