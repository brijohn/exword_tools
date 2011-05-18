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

#include <wx/stdpaths.h>
#include <wx/platinfo.h>
#include <wx/filename.h>

#include "LibraryInstaller.h"
#include "Resources.h"
#include "InstallWizard.h"

IMPLEMENT_APP(LibraryInstaller);


LibraryInstaller::LibraryInstaller() : m_execDone()
{
}

bool LibraryInstaller::OnInit()
{
    long ret;
    wxXmlResource::Get()->InitAllHandlers();
    InitXmlResource();
    SetAppName(wxT("exword"));
    InstallWizard *wiz = new InstallWizard(0L);
    ret = wiz->CheckDecryptFunction();
    switch (ret) {
    case DECRYPT_OK:
        wiz->RunWizard();
        break;
    case DECRYPT_NO_WINE:
        wxMessageBox(_("Wine is required on non windows systems."), _("Error"));
        break;
    case DECRYPT_NO_EXTERNAL:
        wxMessageBox(_("External decryptor not installed."), _("Error"));
        break;
    case DECRYPT_DLL_MISSING:
        wxMessageBox(_("MDSR2.dll not found."), _("Error"));
        break;
    case DECRYPT_INVALID_DLL:
        wxMessageBox(_("MDSR2.dll is invalid."), _("Error"));
        break;
    default:
        wxMessageBox(_("Unknown error."), _("Error"));
    }
    wiz->Destroy();
    return true;
}

wxString LibraryInstaller::GetUserDataDir()
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
