/*
 * Plugin system functions
 *
 * $Id: plugin.inc,v 1.2 2001/09/27 20:33:16 darope Exp $
 *
 */
#if defined _plugin_included
  #endinput
#endif
#define _plugin_included


native plugin_register(Cmd[],Function[]);
native plugin_message(Msg[]);
native plugin_setdesc(Desc[]);
native plugin_setname(Name[]);
native plugin_setversion(Version[]);
