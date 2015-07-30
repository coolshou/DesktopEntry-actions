#ifndef CUSTOMROLES_H
#define CUSTOMROLES_H

#include <QString>

enum MyDataRoles {
   MyMinimumRole = Qt::UserRole  + 1,
   MyMaximumRole = MyMinimumRole + 1,
   MyHashThreadRole = MyMaximumRole + 1,
};


#endif // CUSTOMROLES_H
