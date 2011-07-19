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

#ifndef UNPACKTHREAD_H
#define UNPACKTHREAD_H

#include <wx/thread.h>
#include "InstallWizard.h"
#include "ThreadBase.h"

enum {
        myID_WRITETEXT = myID_FINISH + 1,
        myID_DECRYPT,
};

class UnpackThread : public ThreadBase
{
public:
    UnpackThread(InstallWizard *wizard);
    virtual void *Action();
    int DecryptKey(const char* keyFile, char* key);
    int DecryptFile(char *key, unsigned long key_length, char *data, unsigned long length);
    void DecryptFiles(wxString keyFile, wxString installBase);
private:
    InstallWizard *m_wizard;
};

#endif // INSTALLTHREAD_H
