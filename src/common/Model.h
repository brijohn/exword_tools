/* ExwordTools - Class used to retreive model information
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

#ifndef MODEL_H
#define MODEL_H

#include <wx/wx.h>

class Model {
    public:
        Model(unsigned long series, wxString model = wxT("Unknown")) {
            m_series = series;
            m_model = model;
        };
        Model() {
            m_series = 0;
            m_model = wxT("Unknown");
        };
        wxString GetSeriesName() {
            if (m_series > 0)
                return wxString::Format(wxT("Dataplus %lu"), m_series);
            return wxT("Unknown");
        }
        wxString GetModel() { return m_model; };
        unsigned long GetSeries() { return m_series; };
    private:
        wxString m_model;
        unsigned long m_series;
};


class ModelDatabase {
    public:
        static ModelDatabase *Get();
        Model& Lookup(wxString main, wxString sub, wxString ext = wxT(""));
    private:
        WX_DECLARE_STRING_HASH_MAP( Model, ModelHash );
        static ModelDatabase* m_self;
        ModelHash models;
        ModelDatabase();

};


#endif
