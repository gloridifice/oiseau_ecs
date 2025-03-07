//
// Created by Koiro on 07/03/2025.
//

#include "Table.h"

bool Signature::operator==(const Signature& rhs) const {
    return bitset == rhs.bitset;
}

bool Signature::operator!=(const Signature& rhs) const {
    return bitset != rhs.bitset;
}
