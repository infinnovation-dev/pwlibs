/*=======================================================================
 * pwlibs - Libraries used by the PiWall video wall
 * Copyright (C) 2013  Colin Hogben <colin@piwall.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *-----------------------------------------------------------------------
 *	Support abstract inerfaces
 *=======================================================================*/
#ifndef INC_pwinterface_h
#define INC_pwinterface_h

/* Check in interface implements method */
#define PW_CAN(_i,_m) ((_i) && ((const char *)&((_i)->methods->_m) - (const char *)((_i)->methods)) < (_i)->methods->size && (_i)->methods->_m)

/* Call methid in interface */
#define PW_CALL(_i,_m,...) (((_i)->methods->_m)((_i)->priv, __VA_ARGS__))

#endif /* INC_pwinterface_h */
