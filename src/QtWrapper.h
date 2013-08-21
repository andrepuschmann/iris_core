/**
 * \file QtWrapper.h
 * \version 1.0
 *
 * \section COPYRIGHT
 *
 * Copyright 2012-2013 The Iris Project Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution
 * and at http://www.softwareradiosystems.com/iris/copyright.html.
 *
 * \section LICENSE
 *
 * This file is part of the Iris Project.
 *
 * Iris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Iris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 * \section DESCRIPTION
 *
 * Wrapper class for QT.
 * This class allows us to use QT from Iris if the Qt and Qwt
 * libraries have been found on the system. Qt requires that any
 * graphical widgets should run in the main application thread.
 * Call run() from the main application thread to create the
 * QApplication and enter the main GUI event loop. Call quit()
 * from any thread to make the main application thread exit the
 * event loop.
 * If the Qt or Qwt libraries have not been found on the system,
 * QtWrapper is simply a dummy class with empty functions.
 */

#ifndef QTWRAPPER_H
#define QTWRAPPER_H

#ifdef HAVE_QT

#include <qapplication.h>
#include <boost/thread/thread.hpp>

class QtWrapper
  : QObject
{
  Q_OBJECT

public:
  QtWrapper()
    :exited_(false)
  {}

  /// Create the QApplication and enter the GUI event loop.
  void run()
  {
    int argc = 1;
    char* argv[] = { const_cast<char *>("Iris"), NULL };
    QApplication a(argc, argv);
    connect( this, SIGNAL( quitQt() ),
             &a, SLOT( quit() ));
    connect( this, SIGNAL( quitQtBlocking() ),
             &a, SLOT( quit() ),
             Qt::BlockingQueuedConnection );
    a.exec();
    exited_ = true;
  }

  /// Use a Qt signal to tell the main thread to exit the GUI event loop.
  void quit()
  {
    if(exited_)
      emit quitQt();
    else
      emit quitQtBlocking();
  }

signals:
  void quitQt();
  void quitQtBlocking();

private:
  bool exited_;
};

#else  // HAVE_QT

class QtWrapper
{
public:
  QtWrapper(){}
  void run(){}
  void quit(){}
};

#endif // HAVE_QT


#endif // QTWRAPPER_H
