/* ExwordLibrary - Thread class for installing and removing add-ons
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


#include "Dictionary.h"
#include "InstallThread.h"

class InstallCallback : public ExwordCallback {
    public:
        InstallCallback(void *data) : ExwordCallback(data) {};
        virtual void PutFile(wxString filename, unsigned long transferred, unsigned long length) {
            InstallThread *thread = (InstallThread*)m_data;
            unsigned long percent = ((float)transferred / (float)length) * 100;
            thread->FireEvent(filename, percent, myID_UPDATE);
        };
};

void *InstallThread::Action()
{
    LocalDictionary *dict = dynamic_cast<LocalDictionary*>((Dictionary*)m_data);
    if (dict) {
        FireEvent(wxT(""), 0, myID_START);
        m_exword->SetFileCallback(new InstallCallback(this));
        m_exword->InstallDictionary(dict);
        m_exword->SetFileCallback(NULL);
        FireEvent(wxT(""), 0, myID_FINISH);
    }
    return NULL;
}

void *RemoveThread::Action()
{
    wxString message;
    RemoteDictionary *dict = dynamic_cast<RemoteDictionary*>((Dictionary*)m_data);
    if (dict) {
        message.Printf(_("Removing %s..."), dict->GetName().c_str());
        FireEvent(message, 0, myID_START);
        m_exword->RemoveDictionary(dict);
        FireEvent(wxT(""), 0, myID_FINISH);
    }
    return NULL;
}
