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

#ifndef INSTALLWIZARD_H
#define INSTALLWIZARD_H

#include <wx/wx.h>
#include <wx/wizard.h>
#include <wx/dynlib.h>

#include "Resources.h"
#include "libunshield.h"

void setControlEnable(int id, bool state);
void setControlShow(int id, bool show);

enum {
    DECRYPT_OK = 1,
    DECRYPT_NO_EXTERNAL,
    DECRYPT_DLL_MISSING,
    DECRYPT_INVALID_DLL,
    DECRYPT_NO_WINE = 255,
};

typedef int (*DecryptInit)(char*, unsigned long);
typedef void (*DecryptFin)(void);
typedef void (*Decrypt)(char* src, unsigned long size, char *dest);

class InstallWizard : public InstallWizard_Base
{
    DECLARE_EVENT_TABLE()
    public:
        InstallWizard(wxFrame* parent);
        ~InstallWizard();
        bool RunWizard();
        void OnThreadWriteText(wxCommandEvent& event);
        void OnThreadDecrypt(wxCommandEvent& event);
        void OnThreadFinished(wxCommandEvent& event);
        void OnChangePage(wxWizardEvent& event);
        void OnDisplay(wxWizardEvent& event);
        Unshield *GetCab() const;
        wxArrayString GetSelectedDictionaries();
        int CheckDecryptFunction();
        int InitDecrypt(char *key, unsigned long length);
        void FinDecrypt();
        void DoDecrypt(char *src, unsigned long size, char *dest);
        wxString GetDecrypterPath();
        bool OpenCab();
    private:
        unsigned int m_state;
        wxArrayString m_dict_names;
        wxDynamicLibrary * m_dynlib;
        DecryptInit m_decryptInit;
        Decrypt m_decrypt;
        DecryptFin m_decryptFin;
        Unshield * m_cab;
};

#endif // INSTALLWIZARD_H
