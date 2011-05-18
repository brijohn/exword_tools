/* ExwordLibraryInstaller - Unpacks and installs ex-word add-on libraries
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

#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/platinfo.h>

#include "LibraryInstaller.h"
#include "UnpackThread.h"
#include "libunshield.h"

static char mainKey[] = "1357913564208642";

DEFINE_LOCAL_EVENT_TYPE(myEVT_INSTALL)

UnpackThread::UnpackThread(InstallWizard *wizard)
        : wxThread()
{
    m_wizard = wizard;
}

void UnpackThread::OnExit()
{
}

void UnpackThread::FireEvent(wxString text, int id)
{
    wxCommandEvent event(myEVT_INSTALL, id);
    event.SetString(text);
    wxPostEvent(m_wizard, event);
}


int UnpackThread::DecryptKey(const char* keyFile, char* key)
{
    wxFile file;
    if (!file.Open(wxString::FromAscii(keyFile).c_str()))
        return 1;
    if (file.Read(key, 32) < 32)
        return 1;
    return DecryptFile(mainKey, 16, key, 32);
}

int UnpackThread::DecryptFile(char *key, unsigned long key_length, char *data, unsigned long length)
{
    int ret;
    char *ddata = new char[length];
    ret = m_wizard->InitDecrypt(key, key_length);
    if (ret == 0) {
        m_wizard->DoDecrypt(data, length, ddata);
        memmove(data, ddata, length);
    }
    delete[] ddata;
    m_wizard->FinDecrypt();
    return ret;
}

void UnpackThread::DecryptFiles(wxString keyFile, wxString installBase)
{
    wxOperatingSystemId id = wxPlatformInfo::Get().GetOperatingSystemId();
    if (id & wxOS_WINDOWS) {
        int ret;
        char key[33];
        memset(key, 0, 33);
        ret = DecryptKey(keyFile.utf8_str().data(), key);
        if (ret == 0) {
            char *buffer;
            wxFileOffset size;
            wxDir dir;
            wxFile file;
            wxString filename;
            wxString fullpath;
            if (dir.Open(installBase)) {
                bool cont = dir.GetFirst(&filename, wxEmptyString, wxDIR_FILES);
                while (cont) {
                    fullpath = installBase + filename;
                    if (file.Open(fullpath.c_str(), wxFile::read_write)) {
                        size = file.Length();
                        buffer = new char[size];
                        if (file.Read(buffer, size) == size) {
                            if (DecryptFile(key, 32, buffer, size) == 0) {
                                file.Seek(0);
                                file.Write(buffer, size);
                            }
                        }
                        delete [] buffer;
                    }
                    file.Close();
                    cont = dir.GetNext(&filename);
                }
            }
        }
    } else {
        wxString cmd;
        cmd.Printf(wxT("wine %s %s %s"), m_wizard->GetDecrypterPath().c_str(), keyFile.c_str(), installBase.c_str());
        FireEvent(cmd, myID_DECRYPT);
        wxGetApp().m_execDone.Wait();
    }
}

void *UnpackThread::Entry()
{
    wxString text;
    wxString keyFile;
    wxString installBase;
    UnshieldFileGroup *group;
    unsigned int g_count, i, j, k;
    char dict_name[6];
    char type[3];
    wxArrayString names = m_wizard->GetSelectedDictionaries();
    g_count = unshield_file_group_count(m_wizard->GetCab());
    for (i = 0; i < names.Count(); ++i) {
        for (j = 0; j < g_count; ++j) {
            group = unshield_file_group_get(m_wizard->GetCab(), j);
            int ret = sscanf(group->name, "\x8e\xab\x8f\x91User1 %5s %2s", dict_name, type);
            if (ret >= 1 && strcmp(dict_name, names[i].utf8_str().data()) == 0) {
                if (ret == 2 && strcmp(type, "DB") == 0) {
                    installBase.Printf(wxT("%s%c%s%c"), LibraryInstaller::GetUserDataDir().c_str(),
                                       wxFileName::GetPathSeparator(), names[i].c_str(),
                                       wxFileName::GetPathSeparator());
                    if (wxFileName::Mkdir(installBase, 0777, wxPATH_MKDIR_FULL)) {
                        text.Printf(wxT("Installing %s\n"), names[i].c_str());
                        FireEvent(text, myID_WRITETEXT);
                        for (k = group->first_file; k <= group->last_file; ++k ) {
                            if (unshield_file_is_valid(m_wizard->GetCab(), k)) {
                                wxString filename = installBase + wxString(unshield_file_name(m_wizard->GetCab(), k),  wxConvUTF8);
                                if (unshield_file_save (m_wizard->GetCab(), k, filename.utf8_str().data())) {
                                    text.Printf(wxT("Unpacking file %s...OK\n"), wxString(unshield_file_name(m_wizard->GetCab(), k),  wxConvUTF8).c_str());
                                } else {
                                    text.Printf(wxT("Unpacking file %s...Failed\n"), wxString(unshield_file_name(m_wizard->GetCab(), k),  wxConvUTF8).c_str());
                                }
                                FireEvent(text, myID_WRITETEXT);
                            }
                        }
                    } else {
                        text.Printf(wxT("Installing %s...Unable to create directory\n"), names[i].c_str());
                        FireEvent(text, myID_WRITETEXT);
                    }
                } else if (ret == 1) {
                    for (k = group->first_file; k <= group->last_file; ++k ) {
                        if (strcmp(unshield_file_name(m_wizard->GetCab(), k), "ExwordLib.bin") == 0) {
                            keyFile = wxFileName::CreateTempFileName(names[i]);
                            unshield_file_save (m_wizard->GetCab(), k, keyFile.utf8_str().data());
                        }
                    }
                }
            }
        }
        DecryptFiles(keyFile, installBase);
    }
    FireEvent(wxT(""), myID_FINISHED);
    return NULL;
}

