/* ExwordTextLoader - Thread for handling file uploads
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

#include <wx/filename.h>

#include "UploadThread.h"

class UploadCallback : public ExwordCallback {
    public:
        UploadCallback(void *data) : ExwordCallback(data) {};
        virtual void PutFile(wxString filename, unsigned long transferred, unsigned long length) {
            UploadThread *thread = (UploadThread*)m_data;
            unsigned long percent = ((float)transferred / (float)length) * 100;
            thread->FireEvent(filename, percent, myID_UPDATE);
        };
};


void *UploadThread::Action()
{
    wxArrayString *files = (wxArrayString*)m_data;
    if (files) {
        FireEvent(wxT(""), 0, myID_START);
        wxThread::Sleep(500);
        m_exword->SetFileCallback(new UploadCallback(this));
        for (unsigned int i = 0; i < files->GetCount(); ++i) {
            wxFileName filename((*files)[i]);
            if (filename.GetExt().IsSameAs(wxT("txt"), false)) {
                if (filename.IsFileReadable()) {
                    m_exword->UploadFile(filename);
                }
            }
        }
        m_exword->SetFileCallback(NULL);
        wxThread::Sleep(500);
        FireEvent(wxT(""), 0, myID_FINISH);
    }
    return NULL;
}

UploadThread::~UploadThread()
{
    wxArrayString *files = (wxArrayString*)m_data;
    if (files)
        delete files;
}
