#pragma once

#include <QMessageBox>
#include <string>

inline void KError(std::string msg) {
  QMessageBox msgBox;
  msgBox.setText(QString::fromStdString(msg));
  //msgBox.setInformativeText("Do you want to save your changes?");
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.setDefaultButton(QMessageBox::Ok);
  int ret = msgBox.exec();
  exit(-1);
}


