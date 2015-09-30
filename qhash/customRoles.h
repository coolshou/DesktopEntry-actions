#ifndef CUSTOMROLES_H
#define CUSTOMROLES_H

#include <QString>

enum MyDataRoles {
   MyMinimumRole = Qt::UserRole  + 1,
   MyMaximumRole = MyMinimumRole + 1,
   MyHashThreadRole = MyMaximumRole + 1,
   MyCheckModeRole = MyHashThreadRole +1,
   MyHashModeRole =MyCheckModeRole+1,
};

//config key
#define CFG_KEY_HashAlg "hashAlg"
#define CFG_KEY_HashUppercase "hashUppercase"

#endif // CUSTOMROLES_H
