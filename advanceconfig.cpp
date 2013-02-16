/*
    Copyright 2013 by Reza Fatahilah Shah <rshah0385@kireihana.com>
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
   
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "advanceconfig.h"
#include "ui_advanceconfig.h"

#include <KDebug>
#include <KUser>

#include "usersmodel.h"
#include "config.h"
#include "cursortheme/thememodel.h"
#include "cursortheme/sortproxymodel.h"
#include "cursortheme/cursortheme.h"

AdvanceConfig::AdvanceConfig(QWidget *parent) :
    QWidget(parent)
{
    mConfig = KSharedConfig::openConfig(SDDM_CONFIG_FILE, KConfig::SimpleConfig);
    
    configUi = new Ui::AdvanceConfig();
    configUi->setupUi(this);
    
    load();
    
    connect(configUi->userList, SIGNAL(activated(int)), SIGNAL(changed()));
    connect(configUi->haltCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->rebootCommand, SIGNAL(textChanged(QString)), SIGNAL(changed()));
    connect(configUi->cursorList, SIGNAL(activated(int)), SIGNAL(changed()));
}

AdvanceConfig::~AdvanceConfig()
{
    delete configUi;
}

void AdvanceConfig::load()
{
    //Cursor themes
    cursorModel = new CursorThemeModel(this);
    proxyCursorModel = new SortProxyModel(this);
    proxyCursorModel->setSourceModel(cursorModel);
    proxyCursorModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    proxyCursorModel->sort(NameColumn, Qt::AscendingOrder);
    
    configUi->cursorList->setModel(proxyCursorModel);
    
    //User list
    UsersModel *userModel = new UsersModel(this);
    configUi->userList->setModel(userModel);
    userModel->populate(mConfig->group("General").readEntry("MinimumUid", 1000));
    
    configUi->haltCommand->setUrl(mConfig->group("General").readEntry("HaltCommand"));
    configUi->rebootCommand->setUrl(mConfig->group("General").readEntry("RebootCommand"));
}

QVariantMap AdvanceConfig::save()
{
    QVariantMap args;
    kDebug() << "idx:" << configUi->cursorList->currentIndex();

    QModelIndex cursorIndex = cursorModel->index(configUi->cursorList->currentIndex(),0);
    if (cursorIndex.isValid()) {
        const CursorTheme *cursorTheme = proxyCursorModel->theme(cursorIndex);
        if (cursorTheme)
            args["sddm.conf/General/CursorTheme"] = cursorTheme->name();
    }

    args["sddm.conf/General/AutoUser"] = (configUi->userList->currentIndex() == 0) ? "" : configUi->userList->currentText();
    args["sddm.conf/General/HaltCommand"] = configUi->haltCommand->url().path();
    args["sddm.conf/General/RebootCommand"] = configUi->rebootCommand->url().path();
    
    return args;
}