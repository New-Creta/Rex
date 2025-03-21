/*
  Dokan : user-mode file system library for Windows

  Copyright (C) 2020 - 2023 Google, Inc.
  Copyright (C) 2015 - 2019 Adrien J. <liryna.stark@gmail.com> and Maxime C. <maxime@islog.com>
  Copyright (C) 2007 - 2011 Hiroki Asakawa <info@dokan-dev.net>

  http://dokan-dev.github.io

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "dokan_fs/internal/dokan/dokan_pool.h"
#include "dokan_fs/internal/dokan/dokani.h"

VOID DispatchClose(PDOKAN_IO_EVENT IoEvent)
{
  CheckFileName(IoEvent->EventContext->Operation.Close.FileName);

  DbgPrint("###Close file handle = {}, eventID = {}, event Info = {}, filename = {}", (void*)IoEvent->DokanOpenInfo, IoEvent->DokanOpenInfo != NULL ? IoEvent->DokanOpenInfo->EventId : -1, (void*)IoEvent, rsl::to_string(rsl::wstring_view(IoEvent->EventContext->Operation.File.FileName)));

  // Driver has simply notifying us of the Close request which he has
  // already completed at this stage. Driver is not expecting us
  // to reply from this so there is no need to send an EVENT_INFORMATION.
  ReleaseDokanOpenInfo(IoEvent);
}
