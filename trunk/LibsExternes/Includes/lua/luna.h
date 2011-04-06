#ifndef LUNA_H_
#define LUNA_H_

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

template <typename T> class Luna {
  typedef struct { T *pT; } userdataType;
public:
  typedef int (T::*mfp)(lua_State *L);
  typedef struct { const char *name; mfp mfunc; } RegType;

  static void Register(lua_State *L) {
    lua_newtable(L);
    int methods = lua_gettop(L);

    luaL_newmetatable(L, T::CLASS_NAME);
    int metatable = lua_gettop(L);

    // store method table in globals so that
    // scripts can add functions written in Lua.
    lua_pushstring(L, T::CLASS_NAME);
    lua_pushvalue(L, methods);
    lua_settable(L, LUA_ENVIRONINDEX);

    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);  // hide metatable from Lua getmetatable()

    lua_pushliteral(L, "__index");
    lua_pushvalue(L, methods);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, tostring_T);
    lua_settable(L, metatable);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, gc_T);
    lua_settable(L, metatable);

    lua_newtable(L);                // mt for method table
    int mt = lua_gettop(L);
    lua_pushliteral(L, "__call");
    lua_pushcfunction(L, new_T);
    lua_pushliteral(L, "new");
    lua_pushvalue(L, -2);           // dup new_T function
    lua_settable(L, methods);       // add new_T to method table
    lua_settable(L, mt);            // mt.__call = new_T
    lua_setmetatable(L, methods);

    // fill method table with methods from class T
    for (const RegType *l = T::METHODS; l->name; l++) {
    /* edited by Snaily: shouldn't it be const RegType *l ... ? */
      lua_pushstring(L, l->name);
      lua_pushlightuserdata(L, (void*)l);
      lua_pushcclosure(L, thunk, 1);
      lua_settable(L, methods);
    }

    lua_pop(L, 2);  // drop metatable and method table
  }

  // get userdata from Lua stack and return pointer to T object
  static T *check(lua_State *L, int narg, bool self) 
  {
        userdataType *ud = NULL;

        void *p = lua_touserdata(L, narg);
        bool ok = (lua_getmetatable(L, narg) != 0);
        
        lua_getfield(L, LUA_REGISTRYINDEX, T::CLASS_NAME);  /* get correct metatable */
        ok = ok && (lua_rawequal(L, -1, -2) != 0);

        if (p == NULL || !ok)
            ud = NULL;
        else
            ud = static_cast<userdataType*>(p);

        lua_pop(L, 2);  /* remove both metatables */

        if(!ud)
        {
            const char * typeName = luaL_typename(L, 1);

            lua_Debug ar;
            lua_getstack(L, 0, &ar);
            lua_getinfo(L, "n", &ar);
            luaL_where(L, 1);

            char buffer[1024];
            if(!self)
            {
                sprintf_s(buffer, sizeof(buffer), "%sbad argument #%d to '%s' (%s expected, got %s)",
                    lua_tostring(L, -1), // where
                    narg,
                    ar.name, // func name
                    T::CLASS_NAME, // expected
                    typeName); // got
            }
            else
            {
                sprintf_s(buffer, sizeof(buffer), "%sbad self/this calling method '%s' (%s expected, got %s) maybe you called the method with a '.' instead of a ':'",
                    lua_tostring(L, -1), // where
                    ar.name, // func name
                    T::CLASS_NAME, // expected
                    typeName); // got
            }
            lua_pushstring(L, buffer);
            lua_error(L);
        }
        return ud->pT;  // pointer to T object
  }

private:
  Luna();  // hide default constructor

  // member function dispatcher
  static int thunk(lua_State *L) {
    // stack has userdata, followed by method args
    T *obj = check(L, 1, true);  // get 'self', or if you prefer, 'this'
    lua_remove(L, 1);  // remove self so member function args start at index 1
    // get member function from upvalue
    RegType *l = static_cast<RegType*>(lua_touserdata(L, lua_upvalueindex(1)));
    return (obj->*(l->mfunc))(L);  // call member function
  }

  // create a new T object and
  // push onto the Lua stack a userdata containing a pointer to T object
  static int new_T(lua_State *L) {
    lua_remove(L, 1);   // use CLASS_NAME:new(), instead of CLASS_NAME.new()
    T *obj = new T(L);  // call constructor for T objects
    userdataType *ud =
      static_cast<userdataType*>(lua_newuserdata(L, sizeof(userdataType)));
    ud->pT = obj;  // store pointer to object in userdata
    luaL_getmetatable(L, T::CLASS_NAME);  // lookup metatable in Lua registry
    lua_setmetatable(L, -2);
    return 1;  // userdata containing pointer to T object
  }

  // garbage collection metamethod
  static int gc_T(lua_State *L) {
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    delete obj;  // call destructor for T objects
    return 0;
  }

  static int tostring_T (lua_State *L) {
    char buff[32];
    userdataType *ud = static_cast<userdataType*>(lua_touserdata(L, 1));
    T *obj = ud->pT;
    sprintf_s(buff, "%p", obj);
    lua_pushfstring(L, "%s (%s)", T::CLASS_NAME, buff);
    return 1;
  }
};

#endif
