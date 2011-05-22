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

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/file.h>
#include <wx/dir.h>

#include "ExwordDevice.h"
#include "Dictionary.h"


static char key1[17] =
	"\x42\x72\xb7\xb5\x9e\x30\x83\x45\xc3\xb5\x41\x53\x71\xc4\x95\x00";

static const char *admini_list[] = {
	"admini.inf",
	"adminikr.inf",
	"adminicn.inf",
	"adminide.inf",
	"adminies.inf",
	"adminifr.inf",
	"adminiru.inf",
	NULL
};

void put_file(char * filename, uint32_t transferred, uint32_t length, void *data)
{
    ExwordCallback **cb = (ExwordCallback**)data;
    if (*cb != NULL)
        (*cb)->PutFile(wxString::FromUTF8(filename), transferred, length);
}

void get_file(char * filename, uint32_t transferred, uint32_t length, void *data)
{
    ExwordCallback **cb = (ExwordCallback**)data;
    if (*cb != NULL)
        (*cb)->GetFile(wxString::FromUTF8(filename), transferred, length);
}

Exword::Exword()
{
    m_connected = false;
    m_device = NULL;
    m_callback = NULL;
    m_mode = LIBRARY;
    m_region = JAPANESE;
    m_storage = INTERNAL;
}

Exword::Exword(ExwordMode mode, ExwordRegion region)
{
    Connect(mode, region);
}

Exword::~Exword()
{
    Disconnect();
    if (m_callback != NULL)
        delete m_callback;
}

bool Exword::Connect(ExwordMode mode, ExwordRegion region)
{
    unsigned short options = 0;
    m_connected = false;
    switch(mode) {
    case LIBRARY:
        options |= OPEN_LIBRARY;
        break;
    case TEXT:
        options |= OPEN_TEXT;
        break;
    case CD:
        options |= OPEN_CD;
    }
    switch(region) {
    case JAPANESE:
        options |= LOCALE_JA;
        break;
    case CHINESE:
        options |= LOCALE_CN;
        break;
    case KOREAN:
        options |= LOCALE_KR;
        break;
    case GERMAN:
        options |= LOCALE_DE;
        break;
    case SPANISH:
        options |= LOCALE_ES;
        break;
    case FRENCH:
        options |= LOCALE_FR;
        break;
    case RUSSIAN:
        options |= LOCALE_RU;
        break;
    }
    m_device = exword_open2(options);
    if (m_device) {
        if (exword_connect(m_device) != 0x20) {
            exword_close(m_device);
            m_device = NULL;
        } else {
            exword_setpath(m_device, (uint8_t*)INTERNAL_MEM, 0);
            exword_register_callbacks(m_device, get_file, put_file, &m_callback);
            m_connected = true;
            m_mode = mode;
            m_region = region;
            m_storage = INTERNAL;
        }
    }
    return m_connected;
}

void Exword::Disconnect()
{
    if (IsConnected()) {
        exword_register_callbacks(m_device, NULL, NULL, NULL);
        exword_disconnect(m_device);
        exword_close(m_device);
        m_device = NULL;
        m_connected = false;
    }
}

bool Exword::IsConnected()
{
    return m_connected;
}

wxMemoryBuffer Exword::Reset(wxString user)
{
    wxMemoryBuffer key(20);
    exword_authinfo_t ai;
    exword_userid_t u;

    if (!IsConnected())
        return key;

    memset(&ai, 0, sizeof(exword_authinfo_t));
    memset(&u, 0, sizeof(exword_userid_t));
    memcpy(ai.blk1, "FFFFFFFFFFFFFFFF", 16);
    strncpy((char*)ai.blk2, user.utf8_str().data(), 24);
    strncpy(u.name, user.utf8_str().data(), 16);
    exword_setpath(m_device, (uint8_t*)INTERNAL_MEM, 0);
    exword_authinfo(m_device, &ai);
    exword_userid(m_device, u);
    key.AppendData(ai.challenge, 20);
    return key;
}

bool Exword::Authenticate(wxString user, wxMemoryBuffer& key)
{
    bool success = false;
    int rsp;
    uint16_t count;
    exword_dirent_t *entries;
    exword_authchallenge_t c;
    exword_authinfo_t ai;
    exword_userid_t u;

    if (!IsConnected())
        return success;

    memcpy(c.challenge, key.GetData(), 20);
    memcpy(ai.blk1, "FFFFFFFFFFFFFFFF", 16);
    strncpy((char*)ai.blk2, user.utf8_str().data(), 24);
    strncpy(u.name, user.utf8_str().data(), 16);
    exword_setpath(m_device, (uint8_t*)INTERNAL_MEM, 0);
    rsp = exword_authchallenge(m_device, c);
    if (rsp == 0x20) {
        exword_setpath(m_device, (uint8_t*)ROOT, 0);
        exword_list(m_device, &entries, &count);
        for (int i = 0; i < count; i++) {
            if (strcmp((const char*)entries[i].name, "_SD_00") == 0) {
                exword_setpath(m_device, (uint8_t*)SD_CARD, 0);
                rsp = exword_authchallenge(m_device, c);
                if (rsp != 0x20)
                    exword_authinfo(m_device, &ai);
             }
        }
        exword_free_list(entries);
        exword_userid(m_device, u);
        success = true;
    }
    return success;
}

DictionaryInfo Exword::GetDictionaryInfo(wxString id)
{
    unsigned int i;
    wxMemoryBuffer admini(180);
    ReadAdmini(admini);
    char *data = (char *)admini.GetData();
    if (admini.GetDataLen() > 0) {
        for (i = 0; i < admini.GetDataLen(); i += 180) {
            if (wxString::FromAscii(data + i) == id)
                break;
        }
        if (i < admini.GetDataLen())
            return DictionaryInfo(data + i, data + i + 48, data + i + 32);
    }
    return InvalidDictionary;
}

DictionaryArray Exword::GetLocalDictionaries()
{
    DictionaryArray list;
    wxDir dir(Exword::GetUserDataDir());
    if (dir.IsOpened()) {
        wxString filename;
        bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_DIRS);
        while (cont) {
            Dictionary *local = new LocalDictionary(filename);
            if (local->Exists()) {
                list.Add(local);
            }
            cont = dir.GetNext(&filename);
        }
    }
    return list;
}

DictionaryArray Exword::GetRemoteDictionaries()
{
    DictionaryArray list;
    wxMemoryBuffer admini(180);
    ReadAdmini(admini);
    char *data = (char *)admini.GetData();
    if (admini.GetDataLen() > 0) {
        for (unsigned int i = 0; i < admini.GetDataLen(); i += 180) {
            list.Add(new RemoteDictionary(wxString::FromAscii(data + i), this));
        }
    }
    return list;
}

bool Exword::UploadFile(wxFileName filename)
{
    char *data;
    wxFile file(filename.GetFullPath());
    bool success = false;
    if (file.IsOpened()) {
        char *data = new char[file.Length()];
        if (file.Read(data, file.Length()) == file.Length()) {
            if (exword_send_file(m_device, (char*)wxConvLocal.cWX2MB(filename.GetFullName()).data(), data, file.Length()) == 0x20)
                success = true;
        }
    }
    return success;
}

bool Exword::DeleteFile(wxString filename)
{
    int rsp;
    Model model = GetModel();
    if (m_mode == TEXT && model.GetSeries() >=5)
        rsp = exword_remove_file(m_device, (char*)wxConvLocal.cWX2MB(filename).data(), 1);
    else
        rsp = exword_remove_file(m_device, (char*)wxConvLocal.cWX2MB(filename).data(), 0);
    return (rsp == 0x20);
}

bool Exword::InstallDictionary(LocalDictionary *dict)
{
    bool success = false;
    exword_cryptkey_t ck;
    Capacity cap;
    wxArrayString files;
    int rsp;
    wxString content_path = GetStoragePath() + dict->GetId() + wxT("\\_CONTENT");
    wxString user_path = GetStoragePath() + dict->GetId() + wxT("\\_USER");
    memset(&ck, 0, sizeof(exword_cryptkey_t));
    memcpy(ck.blk1, key1, 2);
    memcpy(ck.blk1 + 10, key1 + 10, 2);
    memcpy(ck.blk2, key1 + 2, 8);
    memcpy(ck.blk2 + 8, key1 + 12, 4);
    if (dict->Exists()) {
        rsp = exword_unlock(m_device);
        rsp |= exword_cname(m_device, (char*)dict->GetName().mb_str(wxCSConv(wxT("SJIS"))).data(), (char*)dict->GetId().utf8_str().data());
        rsp |= exword_cryptkey(m_device, &ck);
        if (rsp == 0x20) {
            files = dict->GetFiles();
            exword_setpath(m_device, (uint8_t*)content_path.utf8_str().data(), 1);
            for (unsigned int i = 0; i < files.GetCount(); ++i) {
                wxFile file(files[i]);
                wxFileName filename(files[i]);
                wxString ext = filename.GetExt();
                if (file.IsOpened()) {
                    char *data = new char[file.Length()];
                    file.Read(data, file.Length());
                    if (ext == wxT("txt") ||
                        ext == wxT("bmp") ||
                        ext == wxT("htm") ||
                        ext == wxT("TXT") ||
                        ext == wxT("BMP") ||
                        ext == wxT("HTM")) {
                        crypt_data(data, file.Length(), (char *)ck.xorkey);
                    }
                    rsp |= exword_send_file(m_device, (char*)filename.GetFullName().utf8_str().data(), data, file.Length());
                    delete [] data;
                }
            }
            exword_setpath(m_device, (uint8_t*)user_path.utf8_str().data(), 1);
        }
        rsp |= exword_lock(m_device);
        success = (rsp == 0x20);
    }
    return success;
}

bool Exword::RemoveDictionary(RemoteDictionary *dict)
{
    bool success = false;
    exword_cryptkey_t ck;
    int rsp;
    if (dict->Exists()) {
        memset(&ck, 0, sizeof(exword_cryptkey_t));
        memcpy(ck.blk1, key1, 2);
        memcpy(ck.blk1 + 10, key1 + 10, 2);
        memcpy(ck.blk2, key1 + 2, 8);
        memcpy(ck.blk2 + 8, key1 + 12, 4);
        exword_setpath(m_device, (uint8_t*)GetStoragePath().utf8_str().data(), 0);
        rsp = exword_unlock(m_device);
        rsp |= exword_cname(m_device, (char*)dict->GetName().mb_str(wxCSConv(wxT("SJIS"))).data(), (char*)dict->GetId().utf8_str().data());
        rsp |= exword_cryptkey(m_device, &ck);
        if (rsp == 0x20)
            rsp |= exword_remove_file(m_device, (char*)dict->GetId().utf8_str().data(), 0);
        rsp |= exword_lock(m_device);
        success = rsp == 0x20;
    }
    return success;
}

Capacity Exword::GetCapacity()
{
    exword_capacity_t cap;
    int rsp;
    exword_setpath(m_device, (uint8_t*)GetStoragePath().utf8_str().data(), 0);
    rsp = exword_get_capacity(m_device, &cap);
    if (rsp != 0x20)
        return Capacity();
    return Capacity(cap.total, cap.free);
}

wxArrayString Exword::List(wxString path, wxString pattern)
{
    exword_dirent_t *entries;
    uint16_t count;
    wxArrayString list;
    wxString filename;
    wxString fullpath = GetStoragePath() + path;
    if (exword_setpath(m_device, (uint8_t*)fullpath.utf8_str().data(), 0) == 0x20) {
        if (exword_list(m_device, &entries, &count) == 0x20) {
            for (int i = 0; i < count; i++) {
                if (entries[i].flags & LIST_F_UNICODE)
                    filename = wxString((const char*)entries[i].name, wxCSConv(wxT("UTF16BE")));
                else
                    filename = wxString::FromAscii((const char*)entries[i].name);
                if (wxMatchWild(pattern.Lower(), filename.Lower(), true))
                    list.Add(filename);
            }
            exword_free_list(entries);
        }
    }
    return list;
}

bool Exword::IsSdInserted()
{
    exword_dirent_t *entries;
    uint16_t count;
    bool found = false;
    if (exword_setpath(m_device, (uint8_t*)ROOT, 0) == 0x20) {
        if (exword_list(m_device, &entries, &count) == 0x20) {
            for (int i = 0; i < count; i++) {
                if (strcmp((const char*)entries[i].name, "_SD_00") == 0)
                    found = true;
            }
            exword_free_list(entries);
        }
    }
    return found;
}

void Exword::SetFileCallback(ExwordCallback* cb)
{
    if (m_callback != NULL)
        delete m_callback;
    this->m_callback = cb;
}

bool Exword::SetStorage(ExwordStorage storage)
{
    if (storage == SD && !IsSdInserted())
        return false;
    m_storage = storage;
    return true;
}


Model Exword::GetModel()
{
    Model modelInfo;
    exword_model_t model;
    memset(&model, 0, sizeof(exword_model_t));
    if (exword_get_model(m_device, &model) == 0x20) {
        modelInfo = ModelDatabase::Get()->Lookup(wxString::FromAscii(model.model),
                                                 wxString::FromAscii(model.sub_model),
                                                 wxString::FromAscii(model.ext_model));
        if (modelInfo.GetSeries() == 0) {
            if ((model.capabilities & CAP_F) &&
                (model.capabilities & CAP_C)) {
                modelInfo = Model(5);
            } else if (model.capabilities & CAP_P) {
                modelInfo = Model(4);
            } else {
                modelInfo = Model(3);
            }
        }
    }
    return modelInfo;
}

wxString Exword::GetStoragePath()
{
    if (m_storage == SD)
        return wxT("\\_SD_00\\");
    else
        return wxT("\\_INTERNAL_00\\");
}

void Exword::ReadAdmini(wxMemoryBuffer& buffer)
{
    int rsp, length;
    char *data;
    if (IsConnected()) {
        exword_setpath(m_device, (uint8_t*)GetStoragePath().utf8_str().data(), 0);
        for (int i = 0; admini_list[i] != NULL; i++) {
            rsp = exword_get_file(m_device, (char*)admini_list[i], &data, &length);
            if (rsp == 0x20 && length > 0) {
                buffer.AppendData(data, length);
                free(data);
                break;
            }
            free(data);
        }
    }
}



wxString Exword::GetUserDataDir()
{
    wxOperatingSystemId id = wxPlatformInfo::Get().GetOperatingSystemId();
    wxString dataDir;
    if (id & wxOS_UNIX) {
        if (wxGetEnv(wxString::FromAscii("XDG_DATA_HOME"), &dataDir)) {
            dataDir = dataDir + wxTheApp->GetAppName();
        } else {
            dataDir = wxFileName::GetHomeDir() + wxString::FromAscii("/.local/share/") + wxTheApp->GetAppName();
        }
        wxFileName::Mkdir(dataDir, 0777, wxPATH_MKDIR_FULL);
    } else {
        dataDir = wxStandardPaths::Get().GetUserDataDir();
    }
    return dataDir;
}

