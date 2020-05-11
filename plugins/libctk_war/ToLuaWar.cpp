/*
** Lua binding: ToLuaWar
*/

#include "tolua.h"

#ifndef __cplusplus
#include <stdlib.h>
#endif
#ifdef __cplusplus
 extern "C" int tolua_bnd_takeownership (lua_State* L); // from tolua_map.c
#else
 int tolua_bnd_takeownership (lua_State* L); /* from tolua_map.c */
#endif
#include <string.h>

/* Exported function */
TOLUA_API int tolua_ToLuaWar_open (lua_State* tolua_S);
LUALIB_API int luaopen_ToLuaWar (lua_State* tolua_S);

#include "PublicEnum.h"
#include "Battlefield.h"
#include "Weapon.h"
#include "Effect.h"
#include "RedArmy.h"
#include "BlueArmy.h"
#include "AllyArmy.h"
#include "Stationary.h"
#include "WarReporter.h"

/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"AllyArmy");
 tolua_usertype(tolua_S,"Serializer");
 tolua_usertype(tolua_S,"Battlefield");
 tolua_usertype(tolua_S,"Stationary");
 tolua_usertype(tolua_S,"RedArmy");
 tolua_usertype(tolua_S,"Weapon");
 tolua_usertype(tolua_S,"BlueArmy");
 tolua_usertype(tolua_S,"WarReporter");
 tolua_usertype(tolua_S,"Effect");
}

/* method: createWeapon of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createWeapon'",NULL);
#endif
 {
  Weapon* tolua_ret = (Weapon*)  self->createWeapon(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Weapon");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createWeapon'.",&tolua_err);
 return 0;
#endif
}

/* method: getWeapon of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getWeapon'",NULL);
#endif
 {
  Weapon* tolua_ret = (Weapon*)  self->getWeapon(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Weapon");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getWeapon'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyWeapon of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyWeapon00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyWeapon'",NULL);
#endif
 {
  self->destroyWeapon(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyWeapon'.",&tolua_err);
 return 0;
#endif
}

/* method: createEffect of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createEffect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createEffect'",NULL);
#endif
 {
  Effect* tolua_ret = (Effect*)  self->createEffect(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Effect");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createEffect'.",&tolua_err);
 return 0;
#endif
}

/* method: getEffect of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getEffect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getEffect'",NULL);
#endif
 {
  Effect* tolua_ret = (Effect*)  self->getEffect(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Effect");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getEffect'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyEffect of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyEffect00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyEffect'",NULL);
#endif
 {
  self->destroyEffect(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyEffect'.",&tolua_err);
 return 0;
#endif
}

/* method: createRedArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createRedArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createRedArmy'",NULL);
#endif
 {
  RedArmy* tolua_ret = (RedArmy*)  self->createRedArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"RedArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createRedArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: getRedArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getRedArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getRedArmy'",NULL);
#endif
 {
  RedArmy* tolua_ret = (RedArmy*)  self->getRedArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"RedArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getRedArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyRedArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyRedArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyRedArmy'",NULL);
#endif
 {
  self->destroyRedArmy(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyRedArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: createBlueArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createBlueArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createBlueArmy'",NULL);
#endif
 {
  BlueArmy* tolua_ret = (BlueArmy*)  self->createBlueArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlueArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createBlueArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: getBlueArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getBlueArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getBlueArmy'",NULL);
#endif
 {
  BlueArmy* tolua_ret = (BlueArmy*)  self->getBlueArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"BlueArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getBlueArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyBlueArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyBlueArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyBlueArmy'",NULL);
#endif
 {
  self->destroyBlueArmy(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyBlueArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: createAllyArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createAllyArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createAllyArmy'",NULL);
#endif
 {
  AllyArmy* tolua_ret = (AllyArmy*)  self->createAllyArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"AllyArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createAllyArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: getAllyArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getAllyArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getAllyArmy'",NULL);
#endif
 {
  AllyArmy* tolua_ret = (AllyArmy*)  self->getAllyArmy(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"AllyArmy");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getAllyArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyAllyArmy of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyAllyArmy00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyAllyArmy'",NULL);
#endif
 {
  self->destroyAllyArmy(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyAllyArmy'.",&tolua_err);
 return 0;
#endif
}

/* method: createStationary of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createStationary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createStationary'",NULL);
#endif
 {
  Stationary* tolua_ret = (Stationary*)  self->createStationary(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Stationary");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createStationary'.",&tolua_err);
 return 0;
#endif
}

/* method: getStationary of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getStationary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getStationary'",NULL);
#endif
 {
  Stationary* tolua_ret = (Stationary*)  self->getStationary(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"Stationary");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getStationary'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyStationary of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyStationary00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyStationary'",NULL);
#endif
 {
  self->destroyStationary(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyStationary'.",&tolua_err);
 return 0;
#endif
}

/* method: createWarReporter of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_createWarReporter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'createWarReporter'",NULL);
#endif
 {
  WarReporter* tolua_ret = (WarReporter*)  self->createWarReporter(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"WarReporter");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'createWarReporter'.",&tolua_err);
 return 0;
#endif
}

/* method: getWarReporter of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_getWarReporter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getWarReporter'",NULL);
#endif
 {
  WarReporter* tolua_ret = (WarReporter*)  self->getWarReporter(id);
 tolua_pushusertype(tolua_S,(void*)tolua_ret,"WarReporter");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getWarReporter'.",&tolua_err);
 return 0;
#endif
}

/* method: destroyWarReporter of class  Battlefield */
static int tolua_ToLuaWar_Battlefield_destroyWarReporter00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Battlefield",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Battlefield* self = (Battlefield*)  tolua_tousertype(tolua_S,1,0);
  int id = ((int)  tolua_tonumber(tolua_S,2,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'destroyWarReporter'",NULL);
#endif
 {
  self->destroyWarReporter(id);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'destroyWarReporter'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  Weapon */
static int tolua_ToLuaWar_Weapon_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Weapon",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Weapon* self = (const Weapon*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  Weapon */
static int tolua_ToLuaWar_Weapon_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Weapon",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Weapon* self = (const Weapon*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  Effect */
static int tolua_ToLuaWar_Effect_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Effect",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Effect* self = (const Effect*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  Effect */
static int tolua_ToLuaWar_Effect_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Effect",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Effect* self = (const Effect*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  RedArmy */
static int tolua_ToLuaWar_RedArmy_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const RedArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const RedArmy* self = (const RedArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  RedArmy */
static int tolua_ToLuaWar_RedArmy_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const RedArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const RedArmy* self = (const RedArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  BlueArmy */
static int tolua_ToLuaWar_BlueArmy_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const BlueArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const BlueArmy* self = (const BlueArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  BlueArmy */
static int tolua_ToLuaWar_BlueArmy_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const BlueArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const BlueArmy* self = (const BlueArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  AllyArmy */
static int tolua_ToLuaWar_AllyArmy_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const AllyArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const AllyArmy* self = (const AllyArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  AllyArmy */
static int tolua_ToLuaWar_AllyArmy_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const AllyArmy",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const AllyArmy* self = (const AllyArmy*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  Stationary */
static int tolua_ToLuaWar_Stationary_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Stationary",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Stationary* self = (const Stationary*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  Stationary */
static int tolua_ToLuaWar_Stationary_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Stationary",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Stationary* self = (const Stationary*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* method: ID of class  WarReporter */
static int tolua_ToLuaWar_WarReporter_ID00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const WarReporter",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const WarReporter* self = (const WarReporter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ID'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ID();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ID'.",&tolua_err);
 return 0;
#endif
}

/* method: Kind of class  WarReporter */
static int tolua_ToLuaWar_WarReporter_Kind00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const WarReporter",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const WarReporter* self = (const WarReporter*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'Kind'",NULL);
#endif
 {
  int tolua_ret = (int)  self->Kind();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'Kind'.",&tolua_err);
 return 0;
#endif
}

/* Open lib function */
LUALIB_API int luaopen_ToLuaWar (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_cclass(tolua_S,"Battlefield","Battlefield","Serializer",NULL);
 tolua_beginmodule(tolua_S,"Battlefield");
 tolua_function(tolua_S,"createWeapon",tolua_ToLuaWar_Battlefield_createWeapon00);
 tolua_function(tolua_S,"getWeapon",tolua_ToLuaWar_Battlefield_getWeapon00);
 tolua_function(tolua_S,"destroyWeapon",tolua_ToLuaWar_Battlefield_destroyWeapon00);
 tolua_function(tolua_S,"createEffect",tolua_ToLuaWar_Battlefield_createEffect00);
 tolua_function(tolua_S,"getEffect",tolua_ToLuaWar_Battlefield_getEffect00);
 tolua_function(tolua_S,"destroyEffect",tolua_ToLuaWar_Battlefield_destroyEffect00);
 tolua_function(tolua_S,"createRedArmy",tolua_ToLuaWar_Battlefield_createRedArmy00);
 tolua_function(tolua_S,"getRedArmy",tolua_ToLuaWar_Battlefield_getRedArmy00);
 tolua_function(tolua_S,"destroyRedArmy",tolua_ToLuaWar_Battlefield_destroyRedArmy00);
 tolua_function(tolua_S,"createBlueArmy",tolua_ToLuaWar_Battlefield_createBlueArmy00);
 tolua_function(tolua_S,"getBlueArmy",tolua_ToLuaWar_Battlefield_getBlueArmy00);
 tolua_function(tolua_S,"destroyBlueArmy",tolua_ToLuaWar_Battlefield_destroyBlueArmy00);
 tolua_function(tolua_S,"createAllyArmy",tolua_ToLuaWar_Battlefield_createAllyArmy00);
 tolua_function(tolua_S,"getAllyArmy",tolua_ToLuaWar_Battlefield_getAllyArmy00);
 tolua_function(tolua_S,"destroyAllyArmy",tolua_ToLuaWar_Battlefield_destroyAllyArmy00);
 tolua_function(tolua_S,"createStationary",tolua_ToLuaWar_Battlefield_createStationary00);
 tolua_function(tolua_S,"getStationary",tolua_ToLuaWar_Battlefield_getStationary00);
 tolua_function(tolua_S,"destroyStationary",tolua_ToLuaWar_Battlefield_destroyStationary00);
 tolua_function(tolua_S,"createWarReporter",tolua_ToLuaWar_Battlefield_createWarReporter00);
 tolua_function(tolua_S,"getWarReporter",tolua_ToLuaWar_Battlefield_getWarReporter00);
 tolua_function(tolua_S,"destroyWarReporter",tolua_ToLuaWar_Battlefield_destroyWarReporter00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Weapon","Weapon","Serializer",NULL);
 tolua_beginmodule(tolua_S,"Weapon");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_Weapon_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_Weapon_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Effect","Effect","Serializer",NULL);
 tolua_beginmodule(tolua_S,"Effect");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_Effect_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_Effect_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"RedArmy","RedArmy","Serializer",NULL);
 tolua_beginmodule(tolua_S,"RedArmy");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_RedArmy_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_RedArmy_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"BlueArmy","BlueArmy","Serializer",NULL);
 tolua_beginmodule(tolua_S,"BlueArmy");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_BlueArmy_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_BlueArmy_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"AllyArmy","AllyArmy","Serializer",NULL);
 tolua_beginmodule(tolua_S,"AllyArmy");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_AllyArmy_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_AllyArmy_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Stationary","Stationary","Serializer",NULL);
 tolua_beginmodule(tolua_S,"Stationary");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_Stationary_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_Stationary_Kind00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"WarReporter","WarReporter","Serializer",NULL);
 tolua_beginmodule(tolua_S,"WarReporter");
 tolua_function(tolua_S,"ID",tolua_ToLuaWar_WarReporter_ID00);
 tolua_function(tolua_S,"Kind",tolua_ToLuaWar_WarReporter_Kind00);
 tolua_endmodule(tolua_S);
 tolua_endmodule(tolua_S);
 return 1;
}
/* Open tolua function */
TOLUA_API int tolua_ToLuaWar_open (lua_State* tolua_S)
{
 lua_pushcfunction(tolua_S, luaopen_ToLuaWar);
 lua_pushstring(tolua_S, "ToLuaWar");
 lua_call(tolua_S, 1, 0);
 return 1;
}
