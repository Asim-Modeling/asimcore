/* 
 * Copyright (c) 2014, Intel Corporation
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright 
 *   notice, this list of conditions and the following disclaimer in the 
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the Intel Corporation nor the names of its 
 *   contributors may be used to endorse or promote products derived from 
 *   this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRALDB_LOGMGR_H
#define _DRALDB_LOGMGR_H

// QT Library
#include <qstring.h>
#include <qfile.h>
#include <q3textstream.h>
#include <qdatetime.h>

/**
  * @brief
  * This class conains the log of the database.
  *
  * @description
  * This class is used to store a global log of an application that
  * uses this database. Typical file access methods are defined to
  * allow an ease use of this class.
  *
  * @author Federico Ardanaz
  * @date started at 2002-07-15
  * @version 0.1
  */
class LogMgr
{
    public:
        static LogMgr* getInstance();
        static LogMgr* getInstance(QString filename);
        static void destroy();

    public:
        inline void addLog(QString msg);
        inline void flush();
        void clearLogFile();
        inline QTime getLastTimestamp();
        bool changeFileName(QString newfilename);

    protected:
        LogMgr(QString filename);
        ~LogMgr();

    private:
        QString  fileName; // Name of the log file.
        QFile*   fileObject; // Pointer to the file descriptor.
        bool ok; // 
        QTime lastTimestamp; // Last time a comment was inserted.

    private:
        static LogMgr* _myInstance;
};

/**
 * Returns the value of the lastTimestamp variable.
 *
 * @return last time stamp.
 */
QTime
LogMgr::getLastTimestamp()
{
    return lastTimestamp;
}

/**
 * Flushes the content of the log file.
 *
 * @return void.
 */
void
LogMgr::flush()
{
    if (!ok)
    {
        return;
    }
    fileObject->flush();
}

/**
 * Adds a new string to the log file.
 *
 * @return void.
 */
void
LogMgr::addLog(QString msg)
{
    if (!ok){
        return;
    }

    Q3TextStream stream(fileObject);
    stream.setEncoding(Q3TextStream::Latin1);

    QTime now = QTime::currentTime();
    QString nowstr = now.toString ("hh:mm:ss");
    stream << "[" << nowstr << "] " << msg << "\n";
    lastTimestamp = QTime::currentTime();
}

#endif
