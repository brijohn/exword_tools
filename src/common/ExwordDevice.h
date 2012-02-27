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
#include <wx/filename.h>

#include "Model.h"

class DictionaryInfo;
class DictionaryArray;
class LocalDictionary;
class RemoteDictionary;

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
    INDIAN,
    ITALIAN,
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

class DirEntry {
    public:
        DirEntry() { m_flags = 0; };
        DirEntry(exword_dirent_t *entry);
        wxString GetFilename() { return m_filename; };
        unsigned long GetFlags() { return m_flags; };
        bool IsDirectory() { return ((m_flags & 0x1) != 0); };
        bool IsUnicode() { return ((m_flags & 0x2) != 0); };
    private:
        wxString m_filename;
        unsigned long m_flags;
};

WX_DECLARE_OBJARRAY(DirEntry, DirEnts);

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_LOCAL_EVENT_TYPE(myEVT_DISCONNECT, 1)
    DECLARE_LOCAL_EVENT_TYPE(myEVT_FILE_TRANSFER, 2)
END_DECLARE_EVENT_TYPES()

class wxTransferEvent: public wxEvent
{
    public:
        wxTransferEvent(wxEventType eventType = wxEVT_NULL, int id = 0)
               : wxEvent(1, myEVT_FILE_TRANSFER) {
            m_length = m_transferred = 0;
            m_download = false;
        };
        wxTransferEvent(const wxTransferEvent& object)
            : wxEvent(object),
              m_download(object.m_download),
              m_length(object.m_length),
              m_transferred(object.m_transferred),
              m_filename(object.m_filename)
        {}

        virtual wxEvent* Clone() const { return new wxTransferEvent(*this); }
        void SetFilename(wxString filename) { m_filename = filename; }
        void SetLength(unsigned long length) { m_length = length; }
        void SetTransferred(unsigned long transferred) { m_transferred = transferred; }
        void SetDownload(bool download) { m_download = download; }
        wxString GetFilename() { return m_filename; }
        unsigned long GetLength() { return m_length; }
        unsigned long GetTransferred() { return m_transferred; }
        bool GetDownload() { return m_download; }
    private:
        bool m_download;
        wxString m_filename;
        unsigned long m_length;
        unsigned long m_transferred;
};

typedef void (wxEvtHandler::*wxTransferEventFunction)(wxTransferEvent&);

#define EVT_FILE_TRANSFER(fn) \
    DECLARE_EVENT_TABLE_ENTRY(myEVT_FILE_TRANSFER, wxID_ANY, -1, \
    (wxObjectEventFunction) (wxEventFunction)  \
    wxStaticCastEvent( wxTransferEventFunction, & fn ), (wxObject *) NULL ),

#define EVT_DISCONNECT(fn) EVT_COMMAND(wxID_ANY, myEVT_DISCONNECT, fn)


class Exword : public wxEvtHandler {
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
        DirEnts List(wxString path, wxString pattern = wxT("*"));
        Capacity GetCapacity();
        bool UploadFile(wxFileName filename);
        bool DeleteFile(wxString filename, unsigned long flags);
        bool InstallDictionary(LocalDictionary *dict);
        bool RemoveDictionary(RemoteDictionary *dict);
        bool SetStorage(ExwordStorage storage);
        void SetEventTarget(wxWindow *target);
        Model GetModel();
        void OnTimer(wxTimerEvent& event);
        friend void disconnect_event_cb(int reason, void *data);
        friend void put_file_cb(char * filename, uint32_t transferred, uint32_t length, void *data);
        friend void get_file_cb(char * filename, uint32_t transferred, uint32_t length, void *data);
    private:
        void ReadAdmini(wxMemoryBuffer& buffer);
        wxString GetStoragePath();
    private:
        exword_t *m_device;
        bool m_connected;
	wxTimer m_timer;
	wxWindow *m_eventTarget;
        ExwordStorage m_storage;
        ExwordMode m_mode;
        ExwordRegion m_region;
    DECLARE_EVENT_TABLE();
};

#endif
