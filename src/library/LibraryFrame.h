/* ExwordLibrary - Main frame for ExwordLibrary
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

#ifndef LIBRARYFRAME_H
#define LIBRARYFRAME_H

#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>
#include <wx/listctrl.h>
#include <wx/statbox.h>
#include <wx/panel.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
#include <wx/msgdlg.h>

#include "DictionaryListCtrl.h"
#include "ProgressDialog.h"
#include "ExwordDevice.h"
#include "Resources.h"
#include "Users.h"

class LibraryFrame : public LibraryGUI
{
    private:
        ProgressDialog *m_progress;
        wxTimer m_pulser;
        Users m_users;
        Exword m_exword;
    public:
        LibraryFrame();
        ~LibraryFrame();
    private:
        ExwordRegion GetRegionFromString(wxString name);
        void UpdateStatusbar();

        void OnConnect(wxCommandEvent& event);
        void OnInternal(wxCommandEvent& event);
        void OnSDCard(wxCommandEvent& event);
        void OnInstall(wxCommandEvent& event);
        void OnRemove(wxCommandEvent& event);

        void OnThreadStart(wxCommandEvent& event);
        void OnThreadUpdate(wxCommandEvent& event);
        void OnThreadFinish(wxCommandEvent& event);
        void OnPulse(wxTimerEvent& event);
    DECLARE_EVENT_TABLE();
};

#endif


