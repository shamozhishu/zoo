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
#include "Entity.h"
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
 tolua_usertype(tolua_S,"RedArmy");
 tolua_usertype(tolua_S,"BlueArmy");
 tolua_usertype(tolua_S,"WarReporter");
 tolua_usertype(tolua_S,"Effect");
 tolua_usertype(tolua_S,"Entity");
 tolua_usertype(tolua_S,"Weapon");
 tolua_usertype(tolua_S,"Stationary");
 tolua_usertype(tolua_S,"AllyArmy");
}

/* method: setPos of class  Entity */
static int tolua_ToLuaWar_Entity_setPos00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Entity",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnumber(tolua_S,3,0,&tolua_err) || 
 !tolua_isnumber(tolua_S,4,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Entity* self = (Entity*)  tolua_tousertype(tolua_S,1,0);
  double x = ((double)  tolua_tonumber(tolua_S,2,0));
  double y = ((double)  tolua_tonumber(tolua_S,3,0));
  double z = ((double)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setPos'",NULL);
#endif
 {
  self->setPos(x,y,z);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setPos'.",&tolua_err);
 return 0;
#endif
}

/* method: setRot of class  Entity */
static int tolua_ToLuaWar_Entity_setRot00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"Entity",0,&tolua_err) || 
 !tolua_isnumber(tolua_S,2,0,&tolua_err) || 
 !tolua_isnumber(tolua_S,3,0,&tolua_err) || 
 !tolua_isnumber(tolua_S,4,0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  Entity* self = (Entity*)  tolua_tousertype(tolua_S,1,0);
  float h = ((float)  tolua_tonumber(tolua_S,2,0));
  float p = ((float)  tolua_tonumber(tolua_S,3,0));
  float r = ((float)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'setRot'",NULL);
#endif
 {
  self->setRot(h,p,r);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'setRot'.",&tolua_err);
 return 0;
#endif
}

/* method: getX of class  Entity */
static int tolua_ToLuaWar_Entity_getX00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getX'",NULL);
#endif
 {
  double tolua_ret = (double)  self->getX();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getX'.",&tolua_err);
 return 0;
#endif
}

/* method: getY of class  Entity */
static int tolua_ToLuaWar_Entity_getY00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getY'",NULL);
#endif
 {
  double tolua_ret = (double)  self->getY();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getY'.",&tolua_err);
 return 0;
#endif
}

/* method: getZ of class  Entity */
static int tolua_ToLuaWar_Entity_getZ00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getZ'",NULL);
#endif
 {
  double tolua_ret = (double)  self->getZ();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getZ'.",&tolua_err);
 return 0;
#endif
}

/* method: getH of class  Entity */
static int tolua_ToLuaWar_Entity_getH00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getH'",NULL);
#endif
 {
  float tolua_ret = (float)  self->getH();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getH'.",&tolua_err);
 return 0;
#endif
}

/* method: getP of class  Entity */
static int tolua_ToLuaWar_Entity_getP00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getP'",NULL);
#endif
 {
  float tolua_ret = (float)  self->getP();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getP'.",&tolua_err);
 return 0;
#endif
}

/* method: getR of class  Entity */
static int tolua_ToLuaWar_Entity_getR00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"const Entity",0,&tolua_err) || 
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  const Entity* self = (const Entity*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getR'",NULL);
#endif
 {
  float tolua_ret = (float)  self->getR();
 tolua_pushnumber(tolua_S,(lua_Number)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getR'.",&tolua_err);
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
 tolua_cclass(tolua_S,"Entity","Entity","",NULL);
 tolua_beginmodule(tolua_S,"Entity");
 tolua_function(tolua_S,"setPos",tolua_ToLuaWar_Entity_setPos00);
 tolua_function(tolua_S,"setRot",tolua_ToLuaWar_Entity_setRot00);
 tolua_function(tolua_S,"getX",tolua_ToLuaWar_Entity_getX00);
 tolua_function(tolua_S,"getY",tolua_ToLuaWar_Entity_getY00);
 tolua_function(tolua_S,"getZ",tolua_ToLuaWar_Entity_getZ00);
 tolua_function(tolua_S,"getH",tolua_ToLuaWar_Entity_getH00);
 tolua_function(tolua_S,"getP",tolua_ToLuaWar_Entity_getP00);
 tolua_function(tolua_S,"getR",tolua_ToLuaWar_Entity_getR00);
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Weapon","Weapon","Entity",NULL);
 tolua_beginmodule(tolua_S,"Weapon");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Effect","Effect","Entity",NULL);
 tolua_beginmodule(tolua_S,"Effect");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"RedArmy","RedArmy","Entity",NULL);
 tolua_beginmodule(tolua_S,"RedArmy");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"BlueArmy","BlueArmy","Entity",NULL);
 tolua_beginmodule(tolua_S,"BlueArmy");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"AllyArmy","AllyArmy","Entity",NULL);
 tolua_beginmodule(tolua_S,"AllyArmy");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"Stationary","Stationary","Entity",NULL);
 tolua_beginmodule(tolua_S,"Stationary");
 tolua_endmodule(tolua_S);
 tolua_cclass(tolua_S,"WarReporter","WarReporter","Entity",NULL);
 tolua_beginmodule(tolua_S,"WarReporter");
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
