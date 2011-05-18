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

#ifndef LIBRARY_INSTALLER_H
#define LIBRARY_INSTALLER_H

#include <wx/app.h>
#include <wx/xrc/xmlres.h>
#include <wx/stattext.h>
#include <wx/wizard.h>
#include <wx/font.h>
#include <wx/filepicker.h>
#include <wx/checklst.h>

#include "config.h"

class LibraryInstaller : public wxApp
{
    public:
	LibraryInstaller();
        static wxString GetUserDataDir();
        virtual bool OnInit();
    public:
        wxSemaphore m_execDone;
};

DECLARE_APP(LibraryInstaller)

#endif
