#ifndef CUSTOMROLES_H
#define CUSTOMROLES_H

enum MyDataRoles {
   MyMinimumRole = Qt::UserRole  + 1,
   MyMaximumRole = MyMinimumRole + 1,
   MyHashThreadRole = MyMaximumRole + 1,
};

#endif // CUSTOMROLES_H
